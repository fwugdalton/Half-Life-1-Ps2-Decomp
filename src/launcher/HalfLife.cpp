/* Launcher (Gearbox, sem fonte): main do jogo no PS2. Inicia os sistemas, roda o front-end
   (GUI do GUIDATA\FRONTEND.PAK) e o laço do motor; o motor recebe a tabela de funções do
   launcher (D_00301E18) e o launcher guarda as funções do motor que usa (D_00399000...).
   Nome do arquivo tirado da string de alocação. */

#include <stdarg.h>
#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
void func_002DD440(void);
int func_00226410(void);
void func_00225A58(int size);
void Mem_SetTag(int tag);
void Mem_PushTag(void);
void Mem_PopTag(void);
void func_00229B78(void);
void func_00239D50(int a);
void func_00229720(void);
void func_00226418(const char *name);
void func_00231908(void);
void func_00238328(void);
void func_00235688(void);
void func_0022AE48(void);
void func_00231FD0(void);
void func_00227498(int a);
void func_002253C8(void);
void func_002319F8(void);
void func_00236858(void);
void func_0023AB28(void);
void func_00235E48(void);
void func_00239870(void);
void func_00217510(void);
void func_002D0520(int a);
void Mod_InitKnown(void);
void func_00235770(void);
void func_00239C80(void);
void func_0022D5B0(char *a);
void func_00235490(void);
void func_00235500(void);
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, unsigned int n);
void func_002374D8(char *path);
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
void func_00225458(void);
void func_00225438(int a);
void func_002262E0(int a);
void func_002393D8(const char *name, int a);
int Hunk_LowMark(void);
void Hunk_FreeToLowMark(int mark);
void Pak_Load(pakfile_t *pak, char *path);
void func_002255B0(void);
void func_00225590(CGUI *gui);
void func_002254A8(void);
void func_00239488(void);
void func_002255E8(void);
void func_002262D8(void);
void func_00225490(void);
void func_00236A88(void);
void func_0021C220(void);
byte *func_00235A00(void);
void func_00235198(pakfile_t *pak);
void func_00235A18(void);
void func_00225860(void *dst, int c, int count);	/* Q_memset */
void *func_00225D48(int size, int align, const char *file, int line);
void func_00239550(void);
void func_002368B8(void);
int func_00236E28(void);
void func_002EF6A0(int a);
void func_002CECE0(void);
void func_0021C340(void);
void func_002394F0(void);
unsigned long func_00229B88(void);

/* funções do motor guardadas pelo launcher */
void func_00299CF8(void);
void Host_Shutdown(void);
void Key_Event(void);
void Key_ClearStates(void);
void Sys_FloatTime(void);
void func_002C5AB8(void);
void func_002C54F0(void);
void func_002C5388(void);
void func_002C5380(void);
void func_002C53B8(void);
void func_002CE130(void);
void func_002CE108(void);
void func_002CD450(void);
void Con_Printf(void);
void Con_SafePrintf(void);
void func_0029A060(void);
void func_002C5FC0(void);
void func_002C5270(void);
void func_002C5FA0(void);
void func_002C5230(void);
void func_002C5FB0(void);
void Cvar_Set(void);
void func_002C5F60(void);
void Host_Frame(void);
void Cvar_SetValue(void);
void Cvar_VariableValue(void);
void Cvar_VariableInt(void);
void Cvar_VariableString(void);
void Cvar_CompleteVariable(void);
void func_002C5F40(void);
void Cbuf_AddText(void);
void Cbuf_InsertText(void);
void Cmd_AddCommand(void);
void Cmd_Argc(void);
void Cmd_Argv(void);
void Cmd_Args(void);
void func_002C4F60(void);
void func_002C63F8(void);
void func_002C6458(void);
void func_002C64D8(void);
void func_002C6478(void);
void func_002C64E8(void);
void func_002C6468(void);
void func_00277B58(void);

extern int key_dest;
extern int giActive;
extern int D_00328B1C;
extern int D_0032EF68, D_0032EF6C, D_0032EF74;
extern struct { int width; int height; } vid;
extern char D_56A324[];		/* fim do programa */
extern char D_81000[];		/* tamanho da pilha */
extern char D_00399A60[];	/* linha de comando */

/* .bss: funções do motor (preenchidas no func_00101078) */
extern void (*D_00399000)( char *text );
extern void *D_00399008;
extern void *D_0039900C;
extern void (*D_00399010)( int a );
extern void (*D_00399014)( const char *name, float value );
extern void (*D_00399018)( void );
extern void *D_0039901C;
extern void (*D_00399020)( const char *name );
extern void (*D_00399028)( void );
extern void (*D_0039902C)( void );
extern void (*D_00399030)( void );
extern void (*D_00399034)( void *info );
extern void *D_00399038;
extern void *D_0039903C;
extern void *D_00399040;
extern void *D_00399044;
extern void *D_00399048;
extern void *D_0039904C;
extern void (*D_00399050)( ... );
extern void *D_00399054;
extern void *D_00399058;
extern void *D_00399060;
extern void *D_00399064;
extern void *D_00399068;
extern void (*D_0039906C)( void );
extern void *D_00399070;
extern void (*D_00399074)( void );
extern void *D_00399078;
extern void *D_0039907C;
extern void *D_00399080;
extern void (*D_00399084)( int key, int down );
extern void *D_00399088;
extern void *D_0039908C;
extern void (*D_00399090)( int a );
extern void *D_00399094;
extern int (*D_00399098)( float frametime, int a, int *state );
extern void *D_0039909C;
extern void *D_003990A0;
extern void *D_003990A4;
extern void *D_003990A8;
extern void (*D_003990B0)( char *cmdline, void *heap, int heapsize, void *api, const char *name, int a );
}

/* vídeo passado ao motor (D_00399034) e copiado pelo func_00100E20 */
typedef struct
{
	int pad[6];
	int i18;
	int width;		/* 0x1C */
	int height;		/* 0x20 */
	float f24;
	int i28;
	int i2C;
	int i30;
	int i34;
	int i38;
	int i3C;
	int i40;
	int i44;
	int pad2[2];
} launchvid_t;

/* funções do launcher passadas ao motor (D_003990B0) */
typedef struct
{
	int i00;
	int version;	/* 0x8C9 */
	void *funcs[0x21];
} launcherapi_t;

extern "C" {

CGUI *D_00301CC0 = 0;
int D_00301CC4 = 0;
char D_00301CC8[0x38] = { 0 };
char D_00301D00[0x38] = { 0 };
char D_00301D38[0x68] = { 0 };
void *D_00301DA0 = 0;
int D_00301DA4 = 0x2000000;
int D_00301DA8 = 0;
int D_00301DAC = 0;
int D_00301DB0 = 0;
int D_00301DB4 = 0;
int D_00301DB8 = 0;
int D_00301DBC = 0;
int D_00301DC0 = 0;
int D_00301DC4 = 0;
launchvid_t D_00301DC8 = { { 0 } };
launcherapi_t D_00301E18 = { 0 };
int D_00301EA4 = 0;
int D_00301EA8 = 0;
int D_00301EAC = 0;
char D_00301EB0[0x28] = "Sean Mode";
void (*D_00301ED8)( int a ) = 0;
int D_00301EDC = 0;
int D_00301EE0 = 0;
void *D_00301EE4 = 0;
void *D_00301EE8 = 0;
int D_00301EEC = 0;
unsigned char D_00301EF0[0x80] =
{
	0, 27, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, 127, 9,
	113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 13, 133, 97, 115,
	100, 102, 103, 104, 106, 107, 108, 59, 39, 96, 134, 92, 122, 120, 99, 118,
	98, 110, 109, 44, 46, 47, 134, 42, 132, 32, 0, 135, 136, 137, 138, 139,
	140, 141, 142, 143, 144, 255, 0, 151, 128, 150, 45, 130, 53, 131, 43, 152,
	129, 149, 147, 148, 0, 0, 0, 145, 146, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void func_00100138( void )
{
}

int func_001002D0( void );
void func_001002F8( int size );
void func_00100300( int argc, char **argv );
void func_00100318( char *pak, char *def, char *focus );
void func_00100408( void );
void func_001007C0( char *cmdline );
void func_00100990( void );

/* main (o crt0 chama este endereço; com o nome main o gcc põe uma chamada ao __main) */
int func_00100140( int argc, char **argv )
{
	int size;
	int old;

	func_002DD440();
	size = func_001002D0();
	size -= func_00226410();
	size &= ~15;
	func_00225A58( size );
	Mem_SetTag( 2 );
	func_001002F8( size );
	func_00229B78();
	Mem_PushTag();
	Mem_SetTag( 1 );
	func_00239D50( 0 );
	func_00229720();
	func_00226418( "Out Of Memory Report.txt" );
	func_00231908();
	func_00238328();
	func_00235688();
	func_0022AE48();
	func_00231FD0();
	func_00227498( 0x10000 );
	func_002253C8();
	func_002319F8();
	func_00236858();
	func_0023AB28();
	func_00235E48();
	func_00239870();
	func_00217510();
	func_002D0520( 1 );
	Mod_InitKnown();
	func_00235770();
	func_00100300( argc, argv );
	func_001007C0( D_00399A60 );
	func_00239C80();
	func_0022D5B0( argv[0] );
	func_00235490();
	Mem_PopTag();
	func_00100318( "GUIDATA\\FRONTEND.PAK", "guidef.txt", 0 );
	for ( ;; )
	{
		old = key_dest;
		key_dest = 4;
		func_00100408();
		func_00235500();
		key_dest = old;
		func_00100990();
	}
}

int func_001002D0( void )
{
	return 0x1FFEFF0 - (int)( D_56A324 + (int)D_81000 );
}

void func_001002F8( int size )
{
}

void func_00100300( int argc, char **argv )
{
	strcpy( D_00399A60, "" );
}

void func_00100358( char *pak );
void func_00100398( char *def );
void func_001003C0( char *focus );

void func_00100318( char *pak, char *def, char *focus )
{
	func_00100358( pak );
	func_00100398( def );
	func_001003C0( focus );
}

void func_00100358( char *pak )
{
	sprintf( D_00301CC8, "%s;1", pak );
	func_002374D8( D_00301CC8 );
}

void func_00100398( char *def )
{
	func_00228CF0( D_00301D00, def );
}

void func_001003C0( char *focus )
{
	if ( focus )
		func_00228CF0( D_00301D38, focus );
	else
		func_00228CF0( D_00301D38, "\0" );	/* string separada do "" no .rodata */
}

void func_00100448( void );
void func_00100608( void );

void func_00100408( void )
{
	if ( giActive == 2 )
		func_00100608();
	else
		func_00100448();
}

int func_001007B0( void );

/* front-end */
void func_00100448( void )
{
	pakfile_t pak;
	int mark;

	func_00225458();
	func_00225438( 1 );
	func_002262E0( 0 );
	D_00301CC0 = 0;
	do
	{
		func_002393D8( "FELoop", 1 );
		mark = Hunk_LowMark();
		pak.inMemory = 2;
		Pak_Load( &pak, D_00301CC8 );
		if ( D_00301CC0 )
		{
			delete D_00301CC0;
			D_00301CC0 = 0;
		}
		D_00301CC0 = new CGUI;
		D_00301CC0->Init( &pak, D_00301D00, 1 );
		if ( func_001007B0() == 1 )
			D_00301CC0->SetFocus( D_00301D38 );
		func_002255B0();
		func_00225590( D_00301CC0 );
		while ( !D_00301CC0->Get04() )
		{
			func_002254A8();
			D_00301CC0->BeginDraw();
			D_00301CC0->Update();
			D_00301CC0->EndDraw();
		}
		func_00239488();
		func_002255E8();
		Hunk_FreeToLowMark( mark );
	} while ( D_00301CC0->Get08() );
	if ( D_00301CC0 )
		delete D_00301CC0;
	D_00301CC0 = 0;
	func_002262E0( 1 );
	func_002262D8();
	func_00225490();
	func_00236A88();
}

/* front-end com o jogo em andamento (menu de pausa) */
void func_00100608( void )
{
	pakfile_t pak;

	func_00225458();
	func_00225438( 1 );
	func_002262E0( 0 );
	D_00301CC0 = 0;
	do
	{
		func_0021C220();
		pak.data = func_00235A00();
		pak.inMemory = 3;
		Pak_Load( &pak, D_00301CC8 );
		if ( D_00301CC0 )
		{
			delete D_00301CC0;
			D_00301CC0 = 0;
		}
		D_00301CC0 = new CGUI;
		D_00301CC0->Init( &pak, D_00301D00, 1 );
		if ( func_001007B0() == 1 )
			D_00301CC0->SetFocus( D_00301D38 );
		func_002255B0();
		func_00225590( D_00301CC0 );
		while ( !D_00301CC0->Get04() )
		{
			func_002254A8();
			D_00301CC0->BeginDraw();
			D_00301CC0->Update();
			D_00301CC0->EndDraw();
		}
		func_002255E8();
		func_00235198( &pak );
		func_00235A18();
	} while ( D_00301CC0->Get08() );
	if ( D_00301CC0 )
		delete D_00301CC0;
	D_00301CC0 = 0;
	func_002262E0( 1 );
	func_002262D8();
	func_00225490();
	func_00236A88();
}

int func_001007B0( void )
{
	return D_00301D38[0] != 0;
}

void func_00101078( void );
void func_001013E0( void );
void func_00100B48( void );

void func_001007C0( char *cmdline )
{
/* NON_MATCHING: ordem dos loads/stores do retângulo da tela */
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/HalfLife", func_001007C0);
#else
	int r[4];

	func_00101078();
	func_00225860( &D_00301DC8, 0, sizeof( D_00301DC8 ) );
	D_00301DC8.i18 = 0;
	D_00301DC8.width = 320;
	D_00301DC8.height = 240;
	D_00301DC8.f24 = 1.0f;
	D_00301DC8.i28 = 1;
	D_00301DC8.i2C = 1;
	D_00301DC8.i34 = 0;
	D_00301DC8.i38 = 320;
	D_00301DC8.i3C = 240;
	D_00301DC8.i40 = 320;
	D_00301DC8.i44 = 200;
	D_00399034( &D_00301DC8 );
	D_00399090( 1 );
	r[0] = D_0032EF68;
	r[1] = D_0032EF6C;
	r[2] = D_0032EF68 + vid.width;
	r[3] = D_0032EF6C + D_0032EF74;
	D_00399050( r );
	D_00399018();
	D_00399028();
	func_001013E0();
	D_00301ED8( 0 );
	D_00301DA4 = 0x11F5600;
	D_00301DA0 = func_00225D48( 0x11F5600, 0x40, "Launcher/HalfLife.cpp", 91 );
	func_00100B48();
	D_003990B0( cmdline, D_00301DA0, D_00301DA4, &D_00301E18, "Player", 0 );
	D_00399010( 1 );
	D_00399020( "Player" );
	D_00399018();
#endif
}

float func_00100AF0( void );

/* laço do jogo */
void func_00100990( void )
{
/* NON_MATCHING: no alvo o ponteiro do D_0039902C fica em v0 (r copiado para s0 antes) */
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/HalfLife", func_00100990);
#else
	int state[4];
	float last, now;
	int r;

	func_00225458();
	func_00225438( 4 );
	D_00399090( 1 );
	last = func_00100AF0();
	func_00239550();
	do
	{
		func_002254A8();
		func_002368B8();
		D_00301DB0 = 1;
		now = func_00100AF0();
		r = D_00399098( now - last, 1, state );
		last = now;
		D_00301DB0 = 0;
		D_0039902C();
		D_00301DAC++;
		if ( func_00236E28() == 1 )
			func_002EF6A0( D_00328B1C );
	} while ( r != 2 && r != 3 );
	D_00399014( "pause_sound", 0.0f );
	func_002CECE0();
	func_0021C340();
	if ( r == 2 )
		func_002394F0();
	else
		func_00239488();
	func_00225490();
#endif
}

float func_00100AF0( void )
{
	return func_00229B88() * 0.001f;
}

void func_00100DE0( void );
void func_00100DE8( void );
void func_00100DF0( void );
void func_00100DF8( void );
int func_00100E00( void );
void func_00100E08( void );
void func_00100E10( void );
void func_00100E18( void );
void func_00100E20( launchvid_t *info );
void func_00100EE8( void );
char *func_00100F18( void );
int func_00100F28( void );
int func_00100F30( int a, int b, int c, int d );
void func_00100F38( char *buf, int *len, int *a );
int func_00100F68( void );
int func_00100F70( void );
int func_00100F78( void );
int func_00100F80( void );
void func_00100F90( const char *fmt, ... );
void func_00100FE0( void );
void func_00100FE8( void );
int func_00100FF0( void );
int func_00100FF8( void );
int func_00101000( void );
void func_00101008( void );
void func_00101030( void );
void func_00101038( void );
void func_00101040( void );
int func_00101048( int a, int b );

void func_00100B48( void )
{
/* NON_MATCHING: ordem dos stores (o escalonador reordena; a ordem do alvo está no .s) */
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/HalfLife", func_00100B48);
#else
	func_00225860( &D_00301E18, 0, sizeof( D_00301E18 ) );
	D_00301E18.i00 = 1;
	D_00301E18.funcs[( 0x54 - 8 ) / 4] = (void *)func_00100F90;
	D_00301E18.funcs[( 0x58 - 8 ) / 4] = (void *)func_00100F38;
	D_00301E18.funcs[( 0x88 - 8 ) / 4] = (void *)func_00100F70;
	D_00301E18.funcs[( 0x84 - 8 ) / 4] = (void *)func_00100FE8;
	D_00301E18.funcs[( 0x80 - 8 ) / 4] = (void *)func_00100F28;
	D_00301E18.funcs[( 0x1C - 8 ) / 4] = (void *)func_00100E00;
	D_00301E18.funcs[( 0x20 - 8 ) / 4] = (void *)func_00100EE8;
	D_00301E18.funcs[( 0x24 - 8 ) / 4] = (void *)func_00100F18;
	D_00301E18.funcs[( 0x28 - 8 ) / 4] = (void *)func_00100E20;
	D_00301E18.funcs[( 0x2C - 8 ) / 4] = (void *)func_00100E10;
	D_00301E18.funcs[( 0x30 - 8 ) / 4] = (void *)func_00100E18;
	D_00301E18.funcs[( 0x34 - 8 ) / 4] = (void *)func_00101008;
	D_00301E18.funcs[( 0x3C - 8 ) / 4] = (void *)func_00101030;
	D_00301E18.funcs[( 0x40 - 8 ) / 4] = (void *)func_00101038;
	D_00301E18.funcs[( 0x44 - 8 ) / 4] = (void *)func_00101040;
	D_00301E18.funcs[( 0x4C - 8 ) / 4] = (void *)func_00100138;
	D_00301E18.version = 0x8C9;
	D_00301E18.funcs[( 0x48 - 8 ) / 4] = (void *)func_00100FE0;
	D_00301E18.funcs[( 0x70 - 8 ) / 4] = (void *)func_00100F78;
	D_00301E18.funcs[( 0x68 - 8 ) / 4] = (void *)func_00100F68;
	D_00301E18.funcs[( 0x6C - 8 ) / 4] = (void *)func_00100F80;
	D_00301E18.funcs[( 0x64 - 8 ) / 4] = (void *)func_00100F30;
	D_00301E18.funcs[( 0x78 - 8 ) / 4] = (void *)func_00101000;
	D_00301E18.funcs[( 0x7C - 8 ) / 4] = (void *)func_00100FF0;
	D_00301E18.funcs[( 0x74 - 8 ) / 4] = (void *)func_00100FF8;
	D_00301E18.funcs[( 0x08 - 8 ) / 4] = (void *)func_00100DF0;
	D_00301E18.funcs[( 0x0C - 8 ) / 4] = (void *)func_00100DF8;
	D_00301E18.funcs[( 0x10 - 8 ) / 4] = (void *)func_00100DE8;
	D_00301E18.funcs[( 0x14 - 8 ) / 4] = (void *)func_00100DE0;
	D_00301E18.funcs[( 0x18 - 8 ) / 4] = (void *)func_00100E08;
	D_00301E18.funcs[( 0x50 - 8 ) / 4] = (void *)func_00101048;
#endif
}

void func_00100D58( void )
{
	int r[4];
	int cx, cy;

	r[0] = D_00301DB4;
	r[1] = D_00301DB8;
	r[2] = D_00301DB4 + D_00301DBC;
	r[3] = D_00301DB8 + D_00301DC0;
	cx = D_00301DB4 + D_00301DBC / 2;
	cy = D_00301DB8 + D_00301DC0 / 2;
	if ( D_00399050 )
		D_00399050( r, cx, cy );
}


void func_00100DE0( void )
{
}

void func_00100DE8( void )
{
}

void func_00100DF0( void )
{
}

void func_00100DF8( void )
{
}

int func_00100E00( void )
{
	return 0;
}

void func_00100E08( void )
{
}

void func_00100E10( void )
{
}

void func_00100E18( void )
{
}

void func_00100E20( launchvid_t *info )
{
	if ( info )
		*info = D_00301DC8;
}

void func_00100EE8( void )
{
	D_0039906C();
}

void func_00100F10( void )
{
}

char *func_00100F18( void )
{
	return D_00301EB0;
}

int func_00100F28( void )
{
	return 1;
}

int func_00100F30( int a, int b, int c, int d )
{
	return d;
}

void func_00100F38( char *buf, int *len, int *a )
{
	*a = 0;
	*len = 13;
	strncpy( buf, "2712128302580", 13 );
}

int func_00100F68( void )
{
	return 2;
}

int func_00100F70( void )
{
	return 0x8C9;
}

int func_00100F78( void )
{
	return 1;
}

int func_00100F80( void )
{
	return 1;
}

void func_00100F88( void )
{
}

void func_00100F90( const char *fmt, ... )
{
	char buf[1024];
	va_list args;

	va_start( args, fmt );
	vsprintf( buf, fmt, args );
}

void func_00100FE0( void )
{
}

void func_00100FE8( void )
{
}

int func_00100FF0( void )
{
	return 2;
}

int func_00100FF8( void )
{
	return 2;
}

int func_00101000( void )
{
	return 2;
}

void func_00101008( void )
{
	func_001013E0();
}

void func_00101028( void )
{
}

void func_00101030( void )
{
}

void func_00101038( void )
{
}

void func_00101040( void )
{
}

int func_00101048( int a, int b )
{
	int r = 0x200000;
	int v = a * b * 2;

	if ( v > 0xFA00 )
		r += ( v - 0xFA00 ) * 3;
	return r;
}

#define SET( g, f )	g = (__typeof__( g ))f

void func_00101078( void )
{
/* NON_MATCHING: ordem dos stores, como no func_00100B48 */
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/HalfLife", func_00101078);
#else
	SET( D_0039907C, func_00299CF8 );
	SET( D_003990A8, Host_Shutdown );
	SET( D_00399084, Key_Event );
	SET( D_00399030, Key_ClearStates );
	SET( D_00301EE4, Sys_FloatTime );
	SET( D_00399074, func_002C5AB8 );
	SET( D_0039900C, func_002C54F0 );
	SET( D_00399018, func_002C5388 );
	SET( D_00399040, func_002C5380 );
	SET( D_0039906C, func_002C53B8 );
	SET( D_0039901C, func_002CE130 );
	SET( D_00399008, func_002CE108 );
	SET( D_00399088, func_002CD450 );
	SET( D_00399038, Con_Printf );
	SET( D_003990A0, Con_SafePrintf );
	SET( D_00399070, func_0029A060 );
	SET( D_003990B0, func_002C5FC0 );
	SET( D_00399034, func_002C5270 );
	SET( D_00399090, func_002C5FA0 );
	SET( D_00399050, func_002C5230 );
	SET( D_00301ED8, func_002C5FB0 );
	SET( D_00399068, Cvar_Set );
	SET( D_003990A4, func_002C5F60 );
	SET( D_00399098, Host_Frame );
	SET( D_00399014, Cvar_SetValue );
	SET( D_0039904C, Cvar_VariableValue );
	SET( D_0039903C, Cvar_VariableInt );
	SET( D_00399058, Cvar_VariableString );
	SET( D_00399080, Cvar_CompleteVariable );
	SET( D_0039902C, func_002C5F40 );
	SET( D_00399000, Cbuf_AddText );
	SET( D_00399060, Cbuf_InsertText );
	D_00301DB0 = 0;
	SET( D_00399078, Cmd_AddCommand );
	SET( D_0039909C, Cmd_Argc );
	SET( D_00399054, Cmd_Argv );
	SET( D_00399064, Cmd_Args );
	SET( D_00399028, func_002C4F60 );
	SET( D_00399020, func_002C63F8 );
	SET( D_00399010, func_002C6458 );
	SET( D_00399094, func_002C64D8 );
	SET( D_0039908C, func_002C6478 );
	SET( D_00399044, func_002C64E8 );
	SET( D_00399048, func_002C6468 );
	SET( D_00301EE8, func_00277B58 );
#endif
}

int func_001013B0( int key )
{
	key = ( key >> 16 ) & 0xFF;
	if ( key >= 0x80 )
		return 0;
	return D_00301EF0[key];
}

void func_001013E0( void )
{
	int i;

	for ( i = 0; i < 256; i++ )
		D_00399084( i, 0 );
	D_00399030();
	D_00399074();
}

}
