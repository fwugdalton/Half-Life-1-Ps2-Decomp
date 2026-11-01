/* cl_dll/text_message.cpp: CHudTextMessage do HLSDK, versão do PS2 (HUD por jogador,
   strcpy/sprintf no lugar de safe_strcpy/safe_sprintf, sem o gViewPort). */

#include "cl_dll/hud.h"

extern "C" {
char *strcpy( char *dst, const char *src );
unsigned int strlen( const char *s );
int sprintf( char *buf, const char *fmt, ... );
}

/* .bss em asm */
extern "C" char D_003A3270[255];
extern "C" char D_003A3370[6][128];

DECLARE_MESSAGE( m_TextMessage, TextMsg );

int CHudTextMessage::Init( int clientId )
{
	HOOK_MESSAGE( TextMsg );

	gHUD[clientId].AddHudElem( this );

	Reset( clientId );

	return 1;
};

// Searches through the string for any msg names (indicated by a '#')
// any found are looked up in titles.txt and the new message substituted
// the new value is pushed into dst_buffer
char *CHudTextMessage::LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size )
{
	char *dst = dst_buffer;
	for ( char *src = (char*)msg; *src != 0 && buffer_size > 0; buffer_size-- )
	{
		if ( *src == '#' )
		{
			// cut msg name out of string
			char *wdst = D_003A3270, *word_start = src;
			for ( ++src ; *src >= 'A' && *src <= 'z'; wdst++, src++ )
			{
				*wdst = *src;
			}
			*wdst = 0;

			// lookup msg name in titles.txt
			client_textmessage_t *clmsg = TextMessageGet( D_003A3270 );
			if ( !clmsg || !(clmsg->pMessage) )
			{
				src = word_start;
				*dst = *src;
				dst++, src++;
				continue;
			}

			// copy string into message over the msg name
			for ( char *wsrc = (char*)clmsg->pMessage; *wsrc != 0; wsrc++, dst++ )
			{
				*dst = *wsrc;
			}
			*dst = 0;
		}
		else
		{
			*dst = *src;
			dst++, src++;
			*dst = 0;
		}
	}

	dst_buffer[buffer_size-1] = 0; // PS2: usa o buffer_size já decrementado
	return dst_buffer;
}

// As above, but with a local buffer (PS2: na pilha)
char *CHudTextMessage::BufferedLocaliseTextString( const char *msg )
{
	char dst_buffer[1024];
	return LocaliseTextString( msg, dst_buffer, 1024 );
}

// Simplified version of LocaliseTextString;  assumes string is only one word
char *CHudTextMessage::LookupString( const char *msg, int *msg_dest )
{
	if ( !msg )
		return "";

	// '#' character indicates this is a reference to a string in titles.txt, and not the string itself
	if ( msg[0] == '#' )
	{
		// this is a message name, so look up the real message
		client_textmessage_t *clmsg = TextMessageGet( msg+1 );

		if ( !clmsg || !(clmsg->pMessage) )
			return (char*)msg; // lookup failed, so return the original string

		if ( msg_dest )
		{
			// check to see if titles.txt info overrides msg destination
			// if clmsg->effect is less than 0, then clmsg->effect holds -1 * message_destination
			if ( clmsg->effect < 0 )  //
				*msg_dest = -clmsg->effect;
		}

		return (char*)clmsg->pMessage;
	}
	else
	{  // nothing special about this message, so just return the same string
		return (char*)msg;
	}
}

void StripEndNewlineFromString( char *str )
{
	int s = strlen( str ) - 1;
	if ( str[s] == '\n' || str[s] == '\r' )
		str[s] = 0;
}

// converts all '\r' characters to '\n', so that the engine can deal with the properly
// returns a pointer to str
char* ConvertCRtoNL( char *str )
{
	for ( char *ch = str; *ch != 0; ch++ )
		if ( *ch == '\r' )
			*ch = '\n';
	return str;
}

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

int CHudTextMessage::MsgFunc_TextMsg( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int msg_dest = READ_BYTE();

#define MSG_BUF_SIZE 128
#define szBuf D_003A3370
	char *msg_text = LookupString( READ_STRING(), &msg_dest );
	msg_text = strcpy( szBuf[0], msg_text );

	// keep reading strings and using C format strings for subsituting the strings into the localised text string
	char *sstr1 = LookupString( READ_STRING() );
	sstr1 = strcpy( szBuf[1], sstr1 );
	StripEndNewlineFromString( sstr1 );  // these strings are meant for subsitution into the main strings, so cull the automatic end newlines
	char *sstr2 = LookupString( READ_STRING() );
	sstr2 = strcpy( szBuf[2], sstr2 );
	StripEndNewlineFromString( sstr2 );
	char *sstr3 = LookupString( READ_STRING() );
	sstr3 = strcpy( szBuf[3], sstr3 );
	StripEndNewlineFromString( sstr3 );
	char *sstr4 = LookupString( READ_STRING() );
	sstr4 = strcpy( szBuf[4], sstr4 );
	StripEndNewlineFromString( sstr4 );
	char *psz = szBuf[5];

	switch ( msg_dest )
	{
	case HUD_PRINTCENTER:
		sprintf( psz, msg_text, sstr1, sstr2, sstr3, sstr4 );
		psz = ConvertCRtoNL( psz );
		CenterPrint( psz, clientId );
		break;

	case HUD_PRINTNOTIFY:
		psz[0] = 1;  // mark this message to go into the notify buffer
		sprintf( psz+1, msg_text, sstr1, sstr2, sstr3, sstr4 );
		psz = ConvertCRtoNL( psz );
		ConsolePrint( psz );
		break;

	case HUD_PRINTTALK:
		sprintf( psz, msg_text, sstr1, sstr2, sstr3, sstr4 );
		gHUD[clientId].m_SayText.SayTextPrint( ConvertCRtoNL( psz ), 128, clientId );
		break;

	case HUD_PRINTCONSOLE:
		sprintf( psz, msg_text, sstr1, sstr2, sstr3, sstr4 );
		psz = ConvertCRtoNL( psz );
		ConsolePrint( psz );
		break;
	}

	return 1;
}
