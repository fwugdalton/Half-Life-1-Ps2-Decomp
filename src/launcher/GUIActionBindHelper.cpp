/* Launcher (Gearbox, sem fonte): GUI_ACTION_BIND_HELPER, a tela de configuração dos
   controles. Guarda uma tabela de ações por aparelho (Dualshock 1, teclado, mouse,
   Dualshock 2), cada uma com o comando, as variáveis do GUI da figura e da tecla, a tecla
   de cada um dos 4 presets e a tecla atual; e uma lista de nomes de teclas (glifos). */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
int func_002290C0(const char *s);			/* atoi */
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
char *func_00228D80(char *dst, const char *src);	/* strcat */
int func_00228DC0(const char *s);			/* strlen */
char func_002258B0(char c);					/* toupper */
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
void Mem_Free(void *ptr);
int Key_FindBinding(char *binding, int max, int skip, int *count, int *keys);
void Key_SetBinding(int keynum, char *binding);
void Key_ClearBinding(int keynum);
const char *func_00106BA0(CGUIVariable *v);	/* nome da variável */
void func_00107398(CGUIVariable *v, float value);	/* muda o valor float */

typedef struct
{
	int down[2];
	int state;
} kbutton_t;

extern kbutton_t in_leftStickSideMove[2];
extern kbutton_t in_leftStickForwardMove[2];
extern kbutton_t in_rightStickForwardMove[2];
extern kbutton_t in_rightStickSideMove[2];
extern kbutton_t in_leftStickYLook[2];
extern kbutton_t in_leftStickXLook[2];
extern kbutton_t in_rightStickYLook[2];
extern kbutton_t in_rightStickXLook[2];
}

/* uma ação configurável */
struct BindEntry
{
	char cmd[0x28];			/* 0x00: comando ligado à tecla */
	char graphic[0x32];		/* 0x28: variável do GUI com a figura da tecla (<nome>Graphic) */
	char number[0x32];		/* 0x5A: variável do GUI com o número da tecla (<nome>Number) */
	int preset[4];			/* 0x8C */
	int value;				/* 0x9C: tecla atual */
};

/* as ações de um aparelho */
struct BindTable
{
	char name[0x34];
	int count;				/* 0x34 */
	BindEntry entries[33];	/* 0x38 */
	int first;				/* 0x14D8: faixa de teclas do aparelho */
	int last;				/* 0x14DC */
};

/* nome (glifo) de uma tecla */
struct KeyInfo
{
	int key;
	char *name;
	int mask;				/* aparelhos em que a tecla existe */
};

extern "C" {
extern BindTable D_0039CFF0;	/* Dualshock 1 */
extern BindTable D_0039E4D0;	/* Keyboard */
extern BindTable D_0039F9B0;	/* Mouse */
extern BindTable D_003A0E90;	/* Dualshock 2 */
extern BindTable *D_003A2370;	/* tabela do aparelho atual */
extern int D_003A2378[4][8];	/* presets dos sticks */

int D_00302A48 = 1;			/* primeira vez */
int D_00302A4C = 0;			/* número de nomes de teclas */
int D_00302A50 = 0;			/* capacidade */
KeyInfo *D_00302A54 = 0;
}

class CGUIActionBindHelper : public CGUIAction
{
public:
	CGUIActionBindHelper();
	virtual ~CGUIActionBindHelper();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );

	void UnbindCommand( char *cmd );
	void SaveSettings( CGUI *gui );
	void ApplyPreset( CGUI *gui, int preset );
	void UpdateBind( CGUI *gui, CGUIVariable *numVar, CGUIVariable *keyVar );
	void FindPreset( CGUIVariable *out );
	void UpdateCustomBinds( void );
	void BindTits( float value );

	int m_iMode;	/* 0x0C: 1 Dualshock 1, 2 Dualshock 2, 4 teclado, 8 mouse */
};

void AddBindEntry( BindTable *table, char *label, char *cmd, int p0, int p1, int p2, int p3, int value );
void InitBindTables( void );
void AddKeyInfo( int key, char *name, int mask );
void SetKeyName( int key, char *name, int mask );
KeyInfo *FindKeyInfo( int key );
void TranslateGlyphs( CGUI *gui );
int TranslateKey( int mask, int key );
int FindStickPreset( int player );
void SetStickButton( kbutton_t *b, int down );
void ApplyStickPreset( int player, int preset );

CGUIActionBindHelper :: CGUIActionBindHelper()
{
}

CGUIActionBindHelper :: ~CGUIActionBindHelper()
{
}

void CGUIActionBindHelper :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_iMode = 0;
	m_szName = func_00228D30( "GUI_ACTION_BIND_HELPER" );
	if ( D_00302A48 == 1 )
	{
		D_00302A48 = 0;
		InitBindTables();
	}
}

void CGUIActionBindHelper :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1 $param2 $param3 $param4 $param5 $param6 $param7 $param8 $param9 $param10";
	char label[0x70];
	char cmdbuf[0x70];
	void *tok = gui->GetTokenizer();
	char *cmd;
	int i;

	func_00108120( tok, params, -1 );
	cmd = func_00108148( tok, 0, 0 );

	if ( func_00228C00( cmd, "KEY_REP" ) == 1 )
	{
		cmd = func_00108148( tok, 0, 0 );
		int key = func_002290C0( cmd );
		SetKeyName( key, func_00108148( tok, 0, 0 ), m_iMode );
	}

	if ( func_00228C00( cmd, "PRESET" ) == 1 )
	{
		cmd = func_00108148( tok, 0, 0 );
		func_00228CF0( label, cmd );
		cmd = func_00108148( tok, 0, 0 );
		func_00228CF0( cmdbuf, cmd );
		int p0 = func_002290C0( func_00108148( tok, 0, 0 ) );
		int p1 = func_002290C0( func_00108148( tok, 0, 0 ) );
		int p2 = func_002290C0( func_00108148( tok, 0, 0 ) );
		int p3 = func_002290C0( func_00108148( tok, 0, 0 ) );
		cmd = func_00108148( tok, 0, 0 );
		int value = func_002290C0( cmd );

		switch ( m_iMode )
		{
		case 1:
			AddBindEntry( &D_0039CFF0, label, cmdbuf, p0, p1, p2, p3, value );
			break;
		case 2:
			AddBindEntry( &D_003A0E90, label, cmdbuf, p0, p1, p2, p3, value );
			break;
		case 4:
			AddBindEntry( &D_0039E4D0, label, cmdbuf, p0, p1, p2, p3, value );
			break;
		case 8:
			AddBindEntry( &D_0039F9B0, label, cmdbuf, p0, p1, p2, p3, value );
			break;
		}
	}

	if ( func_00228C00( cmd, "CONTROLLER1_MODE" ) == 1 )
	{
		m_iMode = 1;
		D_003A2370 = &D_0039CFF0;
	}
	if ( func_00228C00( cmd, "CONTROLLER2_MODE" ) == 1 )
	{
		m_iMode = 2;
		D_003A2370 = &D_003A0E90;
	}
	if ( func_00228C00( cmd, "KEYBOARD_MODE" ) == 1 )
	{
		m_iMode = 4;
		D_003A2370 = &D_0039E4D0;
	}
	if ( func_00228C00( cmd, "MOUSE_MODE" ) == 1 )
	{
		m_iMode = 8;
		D_003A2370 = &D_0039F9B0;
	}

	if ( func_00228C00( cmd, "UPDATE_BIND" ) == 1 )
	{
		CGUIVariable *a = 0;
		cmd = func_00108148( tok, 0, 0 );
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), cmd );
		if ( v->IsType( D_003029E8 ) == 1 )
			a = v;
		cmd = func_00108148( tok, 0, 0 );
		v = func_00107EE0( gui->GetVariables(), cmd );
		CGUIVariable *b = 0;
		if ( v->IsType( D_003029E8 ) == 1 )
			b = v;
		UpdateBind( gui, a, b );
	}

	if ( func_00228C00( cmd, "UNBIND_ACTION" ) == 1 )
	{
		cmd = func_00108148( tok, 0, 0 );
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), cmd );
		for ( i = 0; i < D_003A2370->count; i++ )
		{
			if ( func_00228C00( D_003A2370->entries[i].number, func_00106BA0( v ) ) == 1 )
			{
				UnbindCommand( D_003A2370->entries[i].cmd );
				D_003A2370->entries[i].value = 0;
				break;
			}
		}
		v->SetString( "0" );
	}

	if ( func_00228C00( cmd, "BIND_TITS" ) == 1 )
	{
		cmd = func_00108148( tok, 0, 0 );
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), cmd );
		CGUIVariable *p = 0;
		if ( v->IsType( D_003029E8 ) == 1 )
			p = v;
		if ( m_iMode & 3 )
			BindTits( func_00107000( p ) );
	}

	if ( func_00228C00( cmd, "FIND_PRESET" ) == 1 )
	{
		cmd = func_00108148( tok, 0, 0 );
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), cmd );
		CGUIVariable *p = 0;
		if ( v->IsType( D_003029E8 ) == 1 )
			p = v;
		FindPreset( p );
	}

	if ( func_00228C00( cmd, "IN_KEY_RANGE_VALID" ) == 1 )
	{
		CGUIVariable *out = 0;
		cmd = func_00108148( tok, 0, 0 );
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), cmd );
		if ( v->IsType( D_003029E8 ) == 1 )
			out = v;
		v = func_00107EE0( gui->GetVariables(), "IN_KEY_NUMBER" );
		CGUIVariable *num = 0;
		if ( v->IsType( D_003029E8 ) == 1 )
			num = v;
		func_00107398( out, 0.0f );
		KeyInfo *k = FindKeyInfo( (int)func_00107000( num ) );
		if ( k && ( k->mask & m_iMode ) )
			func_00107398( out, 1.0f );
	}

	if ( func_00228C00( cmd, "TRANSLATE_GLYPHS" ) == 1 )
		TranslateGlyphs( gui );

	int r = func_00228C00( cmd, "APPLY_PRESET" );
	if ( r == 1 )
	{
		cmd = func_00108148( tok, 0, 0 );
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), cmd );
		int preset = 0;
		if ( v->IsType( D_003029E8 ) == r )
			preset = (int)func_00107000( v );
		ApplyPreset( gui, preset );
	}

	if ( func_00228C00( cmd, "UPDATE_CUSTOM_BINDS" ) == 1 )
		UpdateCustomBinds();

	if ( func_00228C00( cmd, "SAVE_SETTINGS" ) == 1 )
		SaveSettings( gui );
}

void CGUIActionBindHelper :: UnbindCommand( char *cmd )
{
	int keys[10];
	int count;
	int i;

	Key_FindBinding( cmd, 10, 0, &count, keys );
	for ( i = 0; i < count; i++ )
	{
		KeyInfo *k = FindKeyInfo( keys[i] );
		if ( k && ( k->mask & m_iMode ) )
			Key_SetBinding( keys[i], "" );
	}
}

/* NON_MATCHING: só a ordem de duas instruções antes do laço (o 56 do índice e os lui das strings) */
void CGUIActionBindHelper :: SaveSettings( CGUI *gui )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIActionBindHelper", SaveSettings__20CGUIActionBindHelperP4CGUI);
#else
	int i;

	for ( i = 0; i < D_003A2370->count; i++ )
	{
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), D_003A2370->entries[i].number );
		int r = v->IsType( D_003029E8 );
		if ( r == 1 )
		{
			if ( func_00228C00( D_003A2370->entries[i].number, "TitsConfigNumber" ) == r )
			{
				if ( m_iMode & 2 )
					ApplyStickPreset( 1, (int)func_00107000( v ) );
				else
					ApplyStickPreset( 0, (int)func_00107000( v ) );
			}
			else
			{
				UnbindCommand( D_003A2370->entries[i].cmd );
				int key = (int)func_00107000( v );
				if ( key > 0 )
					Key_SetBinding( TranslateKey( m_iMode, key ), D_003A2370->entries[i].cmd );
				else
					Key_SetBinding( key, "" );
			}
		}
	}
#endif
}

void CGUIActionBindHelper :: ApplyPreset( CGUI *gui, int preset )
{
	int i;

	for ( i = 0; i < D_003A2370->count; i++ )
	{
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), D_003A2370->entries[i].number );
		if ( v->IsType( D_003029E8 ) == 1 )
			func_00107398( v, (float)D_003A2370->entries[i].preset[preset] );
	}
}

void CGUIActionBindHelper :: UpdateBind( CGUI *gui, CGUIVariable *numVar, CGUIVariable *keyVar )
{
	int key = TranslateKey( m_iMode, (int)func_00107000( keyVar ) );
	int old = (int)func_00107000( numVar );
	int i;

	for ( i = 0; i < D_003A2370->count; i++ )
	{
		if ( !func_00228C00( D_003A2370->entries[i].number, func_00106BA0( numVar ) ) )
		{
			CGUIVariable *v = func_00107EE0( gui->GetVariables(), D_003A2370->entries[i].number );
			if ( v->IsType( D_003029E8 ) == 1 )
			{
				if ( func_002258B0( (char)(int)func_00107000( v ) ) == func_002258B0( (char)key ) )
				{
					D_003A2370->entries[i].value = old;
					func_00107398( v, (float)old );
				}
			}
		}
	}

	for ( i = 0; i < D_003A2370->count; i++ )
	{
		if ( func_00228C00( D_003A2370->entries[i].number, func_00106BA0( numVar ) ) == 1 )
			D_003A2370->entries[i].value = key;
	}

	func_00107398( numVar, (float)key );
}

/* NON_MATCHING: fluxo igual; registradores e variáveis de indução do laço diferentes */
void CGUIActionBindHelper :: FindPreset( CGUIVariable *out )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIActionBindHelper", FindPreset__20CGUIActionBindHelperP12CGUIVariable);
#else
	int preset;
	int ok;
	int i, j;

	for ( preset = 0; preset < 4; preset++ )
	{
		ok = 1;
		for ( i = 0; i < D_003A2370->count; i++ )
		{
			if ( func_00228C00( D_003A2370->entries[i].number, "TitsConfigNumber" ) == 1 )
			{
				if ( FindStickPreset( ( m_iMode & 2 ) ? 1 : 0 ) != D_003A2370->entries[i].preset[preset] )
				{
					ok = 0;
					break;
				}
			}
			else
			{
				int keys[10];
				int count;
				int found = 0;

				Key_FindBinding( D_003A2370->entries[i].cmd, 10, 0, &count, keys );
				for ( j = 0; j < count; j++ )
				{
					KeyInfo *k = FindKeyInfo( keys[j] );
					if ( k && ( k->mask & m_iMode ) )
					{
						int *p = &D_003A2370->entries[i].preset[preset];
						if ( *p == keys[j] || func_002258B0( (char)*p ) == func_002258B0( (char)keys[j] ) )
							found++;
					}
				}

				if ( !found && D_003A2370->entries[i].preset[preset] )
				{
					ok = 0;
					break;
				}
			}
		}

		if ( ok == 1 )
			break;
	}

	func_00107398( out, (float)preset );
#endif
}

/* NON_MATCHING: o teste (m_iMode & 2) não sai com desvio e s1/s2 do laço trocam */
void CGUIActionBindHelper :: UpdateCustomBinds( void )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIActionBindHelper", UpdateCustomBinds__20CGUIActionBindHelper);
#else
	int i, j;

	for ( i = 0; i < D_003A2370->count; i++ )
	{
		if ( func_00228C00( D_003A2370->entries[i].number, "TitsConfigNumber" ) == 1 )
		{
			D_003A2370->entries[i].value = FindStickPreset( ( m_iMode & 2 ) ? 1 : 0 );
		}
		else
		{
			int keys[10];
			int count;

			Key_FindBinding( D_003A2370->entries[i].cmd, 10, 0, &count, keys );
			D_003A2370->entries[i].value = -1;
			for ( j = 0; j < count; j++ )
			{
				KeyInfo *k = FindKeyInfo( keys[j] );
				if ( k && ( k->mask & m_iMode ) )
				{
					D_003A2370->entries[i].value = keys[j];
					break;
				}
			}
		}
	}
#endif
}

void CGUIActionBindHelper :: BindTits( float value )
{
	int i;

	for ( i = 0; i < D_003A2370->count; i++ )
	{
		if ( func_00228C00( D_003A2370->entries[i].number, "TitsConfigNumber" ) == 1 )
		{
			D_003A2370->entries[i].value = (int)value;
			break;
		}
	}
}

void AddBindEntry( BindTable *table, char *label, char *cmd, int p0, int p1, int p2, int p3, int value )
{
	char graphic[0x40];
	char number[0x40];
	int found;
	int i;

	func_00228CF0( graphic, label );
	found = 0;
	func_00228D80( graphic, "Graphic" );
	func_00228CF0( number, label );
	func_00228D80( number, "Number" );

	for ( i = 0; i < table->count; i++ )
	{
		if ( func_00228C00( graphic, table->entries[i].graphic ) == 1
			&& func_00228C00( number, table->entries[i].number ) == 1
			&& func_00228C00( cmd, table->entries[i].cmd ) == 1 )
		{
			found = 1;
			break;
		}
	}

	if ( !found )
	{
		BindEntry *e = &table->entries[table->count];
		func_00228CF0( e->cmd, cmd );
		func_00228CF0( e->graphic, graphic );
		func_00228CF0( e->number, number );
		e->preset[0] = p0;
		e->preset[1] = p1;
		e->preset[2] = p2;
		e->preset[3] = p3;
		e->value = value;
		table->count++;
	}
	else
	{
		BindEntry *e = &table->entries[i];
		e->preset[0] = p0;
		e->preset[1] = p1;
		e->preset[2] = p2;
		e->preset[3] = p3;
		e->value = value;
	}
}

void InitBindTables( void )
{
	D_003A2378[0][0] = 1;
	D_003A2378[0][1] = 1;
	D_003A2378[0][2] = 0;
	D_003A2378[0][3] = 0;
	D_003A2378[0][4] = 0;
	D_003A2378[0][5] = 0;
	D_003A2378[0][6] = 1;
	D_003A2378[0][7] = 1;
	D_003A2378[1][0] = 0;
	D_003A2378[1][1] = 0;
	D_003A2378[1][2] = 1;
	D_003A2378[1][3] = 1;
	D_003A2378[1][4] = 1;
	D_003A2378[1][5] = 1;
	D_003A2378[1][6] = 0;
	D_003A2378[1][7] = 0;
	D_003A2378[2][0] = 1;
	D_003A2378[2][1] = 0;
	D_003A2378[2][2] = 0;
	D_003A2378[2][3] = 1;
	D_003A2378[2][4] = 0;
	D_003A2378[2][5] = 1;
	D_003A2378[2][6] = 1;
	D_003A2378[2][7] = 0;
	D_003A2378[3][0] = 0;
	D_003A2378[3][1] = 1;
	D_003A2378[3][2] = 1;
	D_003A2378[3][3] = 0;
	D_003A2378[3][4] = 1;
	D_003A2378[3][5] = 0;
	D_003A2378[3][6] = 0;
	D_003A2378[3][7] = 1;

	D_0039CFF0.count = 0;
	D_0039E4D0.count = 0;
	D_0039F9B0.count = 0;
	D_003A0E90.count = 0;
	func_00228CF0( D_0039CFF0.name, "Dualshock 1" );
	func_00228CF0( D_0039E4D0.name, "Keyboard" );
	func_00228CF0( D_0039F9B0.name, "Mouse" );
	func_00228CF0( D_003A0E90.name, "Dualshock 2" );
	D_0039CFF0.first = 0xD2;
	D_0039CFF0.last = 0xE1;
	D_0039E4D0.first = 9;
	D_0039E4D0.last = 0x98;
	D_0039F9B0.first = 0xC8;
	D_0039F9B0.last = 0xCA;
	D_003A0E90.first = 0x136;
	D_003A0E90.last = 0x145;
}

void AddKeyInfo( int key, char *name, int mask )
{
	KeyInfo k;

	k.key = key;
	k.name = func_00228D30( name );
	k.mask = mask;
	if ( D_00302A4C == D_00302A50 )
	{
		D_00302A50 = D_00302A4C + 3;
		D_00302A54 = (KeyInfo *)func_002260D0( D_00302A54, D_00302A50 * sizeof( KeyInfo ), "Launcher/GUIActionBindHelper.cpp", 998 );
	}
	D_00302A54[D_00302A4C] = k;
	D_00302A4C++;
}

void SetKeyName( int key, char *name, int mask )
{
	char buf[0x70];
	int len;
	int i;

	len = func_00228DC0( name );
	if ( len >= 2 )
	{
		for ( i = 0; i < len; i++ )
			buf[i] = func_002258B0( *name++ );
		buf[i] = 0;
	}
	else
	{
		buf[0] = name[0];
		buf[1] = 0;
	}

	for ( i = 0; i < D_00302A4C; i++ )
	{
		if ( key == D_00302A54[i].key )
		{
			if ( D_00302A54[i].name )
			{
				Mem_Free( D_00302A54[i].name );
				D_00302A54[i].name = 0;
			}
			D_00302A54[i].name = func_00228D30( buf );
			break;
		}
	}

	if ( i == D_00302A4C )
		AddKeyInfo( key, buf, mask );
}

KeyInfo *FindKeyInfo( int key )
{
	int i;
	KeyInfo *k = 0;

	for ( i = 0; i < D_00302A4C; i++ )
	{
		if ( D_00302A54[i].key == key )
		{
			k = &D_00302A54[i];
			break;
		}
	}
	return k;
}

#ifndef NON_MATCHING
/* o "= " só é usado pelo TranslateGlyphs; com a função em asm ele é definido aqui */
__asm__( ".section .rodata\n.align 3\nD_00338700: .asciz \"= \"\n.text" );
#endif

/* NON_MATCHING: com -fmove-all-movables o 160 do índice é içado e reduzido (o original não) */
void TranslateGlyphs( CGUI *gui )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIActionBindHelper", TranslateGlyphs__FP4CGUI);
#else
	int i;

	for ( i = 0; i < D_003A2370->count; i++ )
	{
		int r = func_00107F00( gui->GetVariables(), D_003A2370->entries[i].number );
		if ( r == 1 )
		{
			CGUIVariable *v = func_00107EE0( gui->GetVariables(), D_003A2370->entries[i].number );
			int key = 0;
			if ( v->IsType( D_003029E8 ) == r )
				key = (int)func_00107000( v );

			KeyInfo *k = FindKeyInfo( key );
			if ( k )
			{
				if ( func_00107F00( gui->GetVariables(), D_003A2370->entries[i].graphic ) == r )
				{
					char buf[0xFF] = "= ";
					CGUIVariable *g = func_00107EE0( gui->GetVariables(), D_003A2370->entries[i].graphic );
					func_00228D80( buf, k->name );
					g->SetString( buf );
				}
			}
			else
			{
				if ( func_00107F00( gui->GetVariables(), D_003A2370->entries[i].graphic ) == r )
				{
					char buf[0xFF] = "= ";
					func_00107EE0( gui->GetVariables(), D_003A2370->entries[i].graphic )->SetString( buf );
				}
			}
		}
	}
#endif
}

int TranslateKey( int mask, int key )
{
	int r = key;
	if ( mask & 1 )
	{
		if ( key >= 0x136 && key <= 0x14F )
			r = key - 100;
	}
	else if ( mask & 2 )
	{
		if ( key >= 0xD2 && key <= 0xEB )
			r = key + 100;
	}
	else if ( mask & 4 )
	{
		if ( key >= 'A' && key <= 'Z' )
			r = key + 32;
	}
	return r;
}

/* NON_MATCHING: os ponteiros dos presets saem de outra variável de indução */
int FindStickPreset( int player )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIActionBindHelper", FindStickPreset__Fi);
#else
	int result = -1;
	int i;

	for ( i = 0; i < 4; i++ )
	{
		if ( ( in_leftStickSideMove[player].state & 1 ) == D_003A2378[i][0]
			&& ( in_leftStickForwardMove[player].state & 1 ) == D_003A2378[i][1]
			&& ( in_rightStickForwardMove[player].state & 1 ) == D_003A2378[i][2]
			&& ( in_rightStickSideMove[player].state & 1 ) == D_003A2378[i][3]
			&& ( in_leftStickYLook[player].state & 1 ) == D_003A2378[i][4]
			&& ( in_leftStickXLook[player].state & 1 ) == D_003A2378[i][5]
			&& ( in_rightStickYLook[player].state & 1 ) == D_003A2378[i][6]
			&& ( in_rightStickXLook[player].state & 1 ) == D_003A2378[i][7] )
		{
			result = i;
			break;
		}
	}
	return result;
#endif
}

void SetStickButton( kbutton_t *b, int down )
{
	if ( down == 1 )
	{
		b->state = 3;
		b->down[0] = -1;
	}
	else
	{
		b->down[1] = 0;
		b->down[0] = 0;
		b->state = ( b->state & ~1 ) | 4;
	}
}

void ApplyStickPreset( int player, int preset )
{
	SetStickButton( &in_leftStickSideMove[player], D_003A2378[preset][0] );
	SetStickButton( &in_leftStickForwardMove[player], D_003A2378[preset][1] );
	SetStickButton( &in_rightStickForwardMove[player], D_003A2378[preset][2] );
	SetStickButton( &in_rightStickSideMove[player], D_003A2378[preset][3] );
	SetStickButton( &in_leftStickYLook[player], D_003A2378[preset][4] );
	SetStickButton( &in_leftStickXLook[player], D_003A2378[preset][5] );
	SetStickButton( &in_rightStickYLook[player], D_003A2378[preset][6] );
	SetStickButton( &in_rightStickXLook[player], D_003A2378[preset][7] );

	if ( player == 0 )
	{
		switch ( preset )
		{
		case 0:
			Key_SetBinding( 0xE2, "+forward 0" );
			Key_SetBinding( 0xE3, "+back 0" );
			Key_SetBinding( 0xE4, "+moveleft 0" );
			Key_SetBinding( 0xE5, "+moveright 0" );
			Key_ClearBinding( 0xE6 );
			Key_ClearBinding( 0xE7 );
			Key_ClearBinding( 0xE8 );
			Key_ClearBinding( 0xE9 );
			break;
		case 1:
			Key_SetBinding( 0xE6, "+forward 0" );
			Key_SetBinding( 0xE7, "+back 0" );
			Key_SetBinding( 0xE8, "+moveleft 0" );
			Key_SetBinding( 0xE9, "+moveright 0" );
			Key_ClearBinding( 0xE2 );
			Key_ClearBinding( 0xE3 );
			Key_ClearBinding( 0xE4 );
			Key_ClearBinding( 0xE5 );
			break;
		case 2:
			Key_SetBinding( 0xE2, "+forward 0" );
			Key_SetBinding( 0xE3, "+back 0" );
			Key_SetBinding( 0xE8, "+moveleft 0" );
			Key_SetBinding( 0xE9, "+moveright 0" );
			Key_ClearBinding( 0xE6 );
			Key_ClearBinding( 0xE7 );
			Key_ClearBinding( 0xE4 );
			Key_ClearBinding( 0xE5 );
			break;
		case 3:
			Key_SetBinding( 0xE6, "+forward 0" );
			Key_SetBinding( 0xE7, "+back 0" );
			Key_SetBinding( 0xE4, "+moveleft 0" );
			Key_SetBinding( 0xE5, "+moveright 0" );
			Key_ClearBinding( 0xE2 );
			Key_ClearBinding( 0xE3 );
			Key_ClearBinding( 0xE8 );
			Key_ClearBinding( 0xE9 );
			break;
		}
	}
	else
	{
		switch ( preset )
		{
		case 0:
			Key_SetBinding( 0x146, "+forward 1" );
			Key_SetBinding( 0x147, "+back 1" );
			Key_SetBinding( 0x148, "+moveleft 1" );
			Key_SetBinding( 0x149, "+moveright 1" );
			Key_ClearBinding( 0x14A );
			Key_ClearBinding( 0x14B );
			Key_ClearBinding( 0x14C );
			Key_ClearBinding( 0x14D );
			break;
		case 1:
			Key_SetBinding( 0x14A, "+forward 1" );
			Key_SetBinding( 0x14B, "+back 1" );
			Key_SetBinding( 0x14C, "+moveleft 1" );
			Key_SetBinding( 0x14D, "+moveright 1" );
			Key_ClearBinding( 0x146 );
			Key_ClearBinding( 0x147 );
			Key_ClearBinding( 0x148 );
			Key_ClearBinding( 0x149 );
			break;
		case 2:
			Key_SetBinding( 0x146, "+forward 1" );
			Key_SetBinding( 0x147, "+back 1" );
			Key_SetBinding( 0x14C, "+moveleft 1" );
			Key_SetBinding( 0x14D, "+moveright 1" );
			Key_ClearBinding( 0x14A );
			Key_ClearBinding( 0x14B );
			Key_ClearBinding( 0x148 );
			Key_ClearBinding( 0x149 );
			break;
		case 3:
			Key_SetBinding( 0x14A, "+forward 1" );
			Key_SetBinding( 0x14B, "+back 1" );
			Key_SetBinding( 0x148, "+moveleft 1" );
			Key_SetBinding( 0x149, "+moveright 1" );
			Key_ClearBinding( 0x146 );
			Key_ClearBinding( 0x147 );
			Key_ClearBinding( 0x14C );
			Key_ClearBinding( 0x14D );
			break;
		}
	}
}
