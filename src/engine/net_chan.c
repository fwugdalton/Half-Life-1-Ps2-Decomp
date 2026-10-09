#include "common.h"
#include "stdarg.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/server.h"

/* net_chan.c: canal de rede no estilo QuakeWorld, com as estatísticas de fluxo do
 * Half-Life. Compilado com -ffast-math (o clamp do Netchan_Process vira max.s/min.s). */

#define PACKET_HEADER 8
#define UDP_HEADER_SIZE 28
#define FLOW_INTERVAL 0.1f
#define FLOW_AVG (2.0f / 3.0f)

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

void NET_SendPacket(int sock, int length, void *data, netadr_t to, int unk);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
char *NET_AdrToString(netadr_t a);
void MSG_WriteLong(sizebuf_t *sb, int c);
void MSG_BeginReading(void);
int MSG_ReadLong(void);
double atof(const char *str);
int vsprintf(char *buf, const char *fmt, va_list ap);

extern float realtime;
extern netadr_t net_from;
extern sizebuf_t net_message;
extern cvar_t sv_lan;
extern int net_drop;
extern float net_rate;
extern char netchan_string[8192];

cvar_t showpackets = { "showpackets", "0" };
cvar_t showdrop = { "showdrop", "0" };
cvar_t r_netgraph = { "r_netgraph", "0" };
cvar_t netchokeloop = { "netchokeloop", "0" };

void Netchan_UpdateFlow(netchan_t *chan)
{
	int i, j;
	int start, bytes = 0;
	float faccumulatedtime = 0.0f;
	flow_t *pflow;
	flowstats_t *pstat, *pprev;

	if (!chan)
		return;

	for (i = 0; i < 2; i++)
	{
		pflow = &chan->flow[i];

		if ((realtime - pflow->nextcompute) < FLOW_INTERVAL)
			continue;

		pflow->nextcompute = realtime + FLOW_INTERVAL;

		start = pflow->current - 1;

		pprev = &pflow->stats[start & (MAX_LATENT - 1)];

		for (j = 1; j < MAX_LATENT; j++)
		{
			pstat = &pflow->stats[(start - j) & (MAX_LATENT - 1)];

			faccumulatedtime += pprev->time - pstat->time;
			bytes += pstat->size;

			pprev = pstat;
		}

		if (!faccumulatedtime)
			pflow->kbytespersec = 0.0f;
		else
			pflow->kbytespersec = (float)bytes / faccumulatedtime / 1024.0f;

		pflow->avgkbytespersec = FLOW_AVG * pflow->avgkbytespersec + (1.0f - FLOW_AVG) * pflow->kbytespersec;
	}
}

void Net_Rate_f(void)
{
	float rate;

	if (Cmd_Argc() != 2)
	{
		Con_Printf("Usage:  rate <num>\nTransmission rate (bytes/sec)\nCurrent:  %.5f\n", net_rate);
		return;
	}

	rate = atof(Cmd_Argv(1));
	if (rate == 0)
		rate = 9999.0f;

	if (rate < 500.0f || rate > 10000.0f)
	{
		Con_Printf("Rate:  Maximum %f, Minimum %f\n", 10000, 500);
		return;
	}

	net_rate = rate;
	cls[0].netchan.rate = rate;
}

void Netchan_OutOfBand(int sock, netadr_t adr, int length, byte *data, int unk)
{
	sizebuf_t send;
	byte send_buf[MAX_MSGLEN + PACKET_HEADER];

	send.data = send_buf;
	send.maxsize = sizeof(send_buf);
	send.cursize = 0;

	MSG_WriteLong(&send, -1);
	SZ_Write(&send, data, length);

	NET_SendPacket(sock, send.cursize, send.data, adr, unk);
}

void Netchan_OutOfBandPrint(int sock, netadr_t adr, int unk, char *format, ...)
{
	va_list argptr;

	va_start(argptr, format);
	vsprintf(netchan_string, format, argptr);
	va_end(argptr);

	Netchan_OutOfBand(sock, adr, strlen(netchan_string) + 1, (byte *)netchan_string, unk);
}

void Netchan_Setup(int sock, netchan_t *chan, netadr_t adr)
{
	memset(chan, 0, sizeof(*chan));

	chan->sock = sock;
	chan->remote_address = adr;
	chan->last_received = realtime;
	chan->connect_time = realtime;
	chan->message.data = chan->message_buf;
	chan->message.allowoverflow = true;
	chan->message.maxsize = sizeof(chan->message_buf);
	chan->rate = net_rate;
}

void Netchan_Transmit(netchan_t *chan, int length, byte *data, int unk)
{
	sizebuf_t send;
	byte send_buf[MAX_MSGLEN + PACKET_HEADER];
	qboolean send_reliable;
	unsigned w1, w2;
	int i, j, mask;
	float fRate;

	if (chan->message.overflowed)
	{
		chan->fatal_error = true;
		Con_Printf("%s:Outgoing message overflow\n", NET_AdrToString(chan->remote_address));
		return;
	}

	send_reliable = false;

	if (chan->incoming_acknowledged > chan->last_reliable_sequence
		&& chan->incoming_reliable_acknowledged != chan->reliable_sequence)
		send_reliable = true;

	if (!chan->reliable_length && chan->message.cursize)
	{
		memcpy(chan->reliable_buf, chan->message_buf, chan->message.cursize);
		chan->reliable_length = chan->message.cursize;
		chan->message.cursize = 0;
		chan->reliable_sequence ^= 1;
		send_reliable = true;
	}

	send.data = send_buf;
	send.maxsize = sizeof(send_buf);
	send.cursize = 0;

	w1 = chan->outgoing_sequence | (send_reliable << 31);
	w2 = chan->incoming_sequence | (chan->incoming_reliable_sequence << 31);

	chan->outgoing_sequence++;

	MSG_WriteLong(&send, w1);
	MSG_WriteLong(&send, w2);

	if (send_reliable)
	{
		SZ_Write(&send, chan->reliable_buf, chan->reliable_length);
		chan->last_reliable_sequence = chan->outgoing_sequence;
	}

	if (send.maxsize - send.cursize >= length)
		SZ_Write(&send, data, length);
	else
		Con_DPrintf("Netchan_Transmit:  Unreliable would overfow, ignoring\n");

	if (send.cursize == 10)
	{
		for (j = send.cursize; j; j--)
			;
	}

	mask = MAX_LATENT - 1;
	if (send.cursize < 16)
	{
		for (i = 0; i < 16; i++)
			MSG_WriteByte(&send, 1);
	}

	i = chan->outgoing_sequence & (MAX_LATENT - 1);
	chan->outgoing_size[i] = send.cursize;
	chan->outgoing_time[i] = realtime;

	chan->flow[FLOW_OUTGOING].stats[chan->flow[FLOW_OUTGOING].current & mask].size = send.cursize + UDP_HEADER_SIZE;
	chan->flow[FLOW_OUTGOING].stats[chan->flow[FLOW_OUTGOING].current & (MAX_LATENT - 1)].time = realtime;
	chan->flow[FLOW_OUTGOING].current++;
	Netchan_UpdateFlow(chan);

	NET_SendPacket(chan->sock, send.cursize, send.data, chan->remote_address, unk);

	if (sv.active && sv_lan.value)
		fRate = 1.0f / 9999.0f;
	else
		fRate = 1.0f / chan->rate;

	if (chan->cleartime < realtime)
		chan->cleartime = realtime + send.cursize * fRate;
	else
		chan->cleartime += send.cursize * fRate;

	if (showpackets.value)
		Con_Printf("--> s=%i(%i) a=%i(%i) %i ", chan->outgoing_sequence, send_reliable, chan->incoming_sequence, chan->incoming_reliable_sequence, send.cursize);
	if (showpackets.value)
		Con_Printf("\n");
}

void Netchan_UpdateLatency(netchan_t *chan, int latency, float rate)
{
	netlatency_t *p;
	float totallatency, totalrate;
	int i;

	p = &chan->latency[chan->good_count & (MAX_LATENCY - 1)];
	if (!p)
		return;

	p->latency = latency;
	p->rate = rate;

	if (chan->good_count < MAX_LATENCY)
	{
		chan->frame_latency = 0;
		chan->frame_rate = 0;
		return;
	}

	totallatency = 0;
	totalrate = 0;
	p = chan->latency;
	for (i = 0; i < MAX_LATENCY; p++, i++)
	{
		totallatency += p->latency;
		totalrate += p->rate;
	}

	totalrate *= (1.0f / MAX_LATENCY);
	totallatency *= (1.0f / MAX_LATENCY);
	chan->frame_latency = totallatency;
	chan->frame_rate = totalrate;
}

qboolean Netchan_Process(netchan_t *chan)
{
	unsigned sequence, sequence_ack;
	unsigned reliable_ack, reliable_message;
	float frametime;

	if (!NET_CompareAdr(net_from, chan->remote_address))
		return false;

	MSG_BeginReading();
	sequence = MSG_ReadLong();
	sequence_ack = MSG_ReadLong();

	reliable_message = sequence >> 31;
	reliable_ack = sequence_ack >> 31;

	sequence &= ~(1 << 31);
	sequence_ack &= ~(1 << 31);

	if (showpackets.value)
		Con_Printf("<-- s=%i(%i) a=%i(%i) %i\n", sequence, reliable_message, sequence_ack, reliable_ack, net_message.cursize);

	if (sequence <= (unsigned)chan->incoming_sequence)
	{
		if (showdrop.value)
			Con_Printf("%s:Out of order packet %i at %i\n", NET_AdrToString(chan->remote_address), sequence, chan->incoming_sequence);
		return false;
	}

	net_drop = sequence - (chan->incoming_sequence + 1);
	if (net_drop > 0)
	{
		chan->drop_count += 1;

		if (showdrop.value)
			Con_Printf("%s:Dropped %i packets at %i\n", NET_AdrToString(chan->remote_address), sequence - (chan->incoming_sequence + 1), sequence);
	}

	if (reliable_ack == chan->reliable_sequence)
		chan->reliable_length = 0;

	chan->incoming_sequence = sequence;
	chan->incoming_acknowledged = sequence_ack;
	chan->incoming_reliable_acknowledged = reliable_ack;
	if (reliable_message)
		chan->incoming_reliable_sequence ^= 1;

	frametime = max(realtime - chan->last_received, 0.001f);
	frametime = min(frametime, 0.5f);
	Netchan_UpdateLatency(chan, chan->outgoing_sequence - sequence_ack, 1.0f / frametime);

	chan->good_count += 1;
	chan->last_received = realtime;

	chan->flow[FLOW_INCOMING].stats[chan->flow[FLOW_INCOMING].current & (MAX_LATENT - 1)].size = net_message.cursize + UDP_HEADER_SIZE;
	chan->flow[FLOW_INCOMING].stats[chan->flow[FLOW_INCOMING].current & (MAX_LATENT - 1)].time = realtime;
	chan->flow[FLOW_INCOMING].current++;
	Netchan_UpdateFlow(chan);

	return true;
}

void Netchan_Init(void)
{
	Cvar_RegisterVariable(&showpackets);
	Cvar_RegisterVariable(&showdrop);
	Cvar_RegisterVariable(&r_netgraph);
	Cvar_RegisterVariable(&netchokeloop);
	Cmd_AddCommand("uprate", Net_Rate_f);
	net_rate = 9999.0f;
}
