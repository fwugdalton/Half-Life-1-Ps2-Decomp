/* Launcher/GUI.cpp (Gearbox, sem fonte): o GUI dos menus do PS2. Lê um script de um pak
   (com !include e !define), cria as ações GUI_ACTION_* e as telas GUI_SCREEN e desenha a
   tela atual com fade entre telas. Os nomes das classes e métodos são inventados; as classes
   de fora (telas, widgets, variáveis, tokenizador, lista de ações) ficam como funções
   func_XXXXXXXX porque os outros arquivos (PauseMenu, DiscFail, GameSave) já as citam assim. */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
#include "engine/decals.h"

typedef struct {
    int width;
    int height;
} viddef_t;

extern viddef_t vid;
extern int D_0032EF68, D_0032EF6C, D_0032EF74;
extern int D_0032EF78, D_0032EF7C, D_0032EF80, D_0032EF84;

void *__builtin_new(int size);
void *Mem_DbgMalloc(int size, const char *file, int line);
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
void Mem_Free(void *p);
void *Q_memcpy(void *dst, const void *src, int n);
void *memset(void *dst, int c, unsigned int n);
int func_00228DC0(const char *s);					/* strlen */
char *func_00228EC0(const char *s, const char *find);	/* strstr */
char *func_00228E80(const char *s, int c);			/* strchr */
void func_00228CF0(char *dst, const char *src);		/* strcpy */
int func_00228EE0(char *buf, const char *fmt, ...);	/* sprintf */
char *func_00228FA0(int value, char *buf, int radix);	/* itoa */
void func_00228BD0(void);
void Cbuf_AddText(const char *text);
unsigned long func_00229B88(void);
unsigned long func_00229C38(unsigned long start, unsigned long now);
int func_00235280(pakfile_t *pak, const char *name);

void func_0023AD98(void);
void func_0023ADB8(void);
void func_0023C818(float r, float g, float b, float a);
void func_0023C880(int mask);
void glMatrixMode(int mode);
void glPushMatrix(void);
void glPopMatrix(void);
void glLoadIdentity(void);
void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
void glViewport(int x, int y, int width, int height);
void glEnable(int cap);
void glDisable(int cap);
void glBlendFunc(int s, int d);
void glColor4f(float r, float g, float b, float a);
}


extern "C" {
/* telas (0x44 bytes) */
CGUIScreen *func_00105898(void *mem, void *gui);
void func_001058A8(CGUIScreen *s, int flags);
void func_00105988(CGUIScreen *s);
void func_00105C40(CGUIScreen *s);
float func_00105D08(CGUIScreen *s);
float func_00105D10(CGUIScreen *s);
int func_00105D18(CGUIScreen *s);
CGUIWidget *func_00105D20(CGUIScreen *s, const char *name);
CGUIWidget *func_00105DC0(CGUIScreen *s);
void func_00105DF8(CGUIScreen *s, CGUIWidget *w);
/* widgets */
void func_00105830(CGUIWidget *w, int key);
int func_00105860(CGUIWidget *w, const char *name);
const char *func_00105890(CGUIWidget *w);
/* desenho (0x15E8 bytes) */
void *func_001066C0(void *mem);
void func_001066D0(void *p, int flags);
void func_00106758(void *p, pakfile_t *pak);
/* lista de ações */
void *func_00108890(void *mem);
void func_001088A0(void *p, int flags);
void func_00108960(void *p, void *gui);
void func_00108980(void *p, CGUIAction *a);
CGUIAction *func_00108BD8(void *mem);
/* construtores das ações */
CGUIAction *func_00108F40(void *mem);
CGUIAction *func_001090A0(void *mem);
CGUIAction *func_00109240(void *mem);
CGUIAction *func_001093E0(void *mem);
CGUIAction *func_00109498(void *mem);
CGUIAction *func_00109568(void *mem);
CGUIAction *func_00109728(void *mem);
CGUIAction *func_00109910(void *mem);
CGUIAction *func_00109B08(void *mem);
CGUIAction *func_00109C30(void *mem);
CGUIAction *func_0010A010(void *mem);
CGUIAction *func_0010A4D8(void *mem);
CGUIAction *func_0010A220(void *mem);
CGUIAction *func_0010A330(void *mem);
CGUIAction *func_0010AA78(void *mem);
CGUIAction *func_0010AC08(void *mem);
CGUIAction *func_0010AD98(void *mem);
CGUIAction *func_0010B698(void *mem);
CGUIAction *func_0010BFF8(void *mem);
CGUIAction *func_0010C2E8(void *mem);
CGUIAction *func_0010E540(void *mem);
CGUIAction *func_0010E6A8(void *mem);
CGUIAction *func_0010E8F8(void *mem);
CGUIAction *func_0010EA58(void *mem);
CGUIAction *func_0010B8D8(void *mem);
}

int D_00302A58 = 1;
int D_00302A5C = 1;


static char *FindToken( int bStart, char *text, const char *token );

CGUI :: CGUI()
{
	m_bInitialized = 0;
}

CGUI :: ~CGUI()
{
	char buf[100];

	if ( func_00107F00( m_pVariables, "BrightnessValue" ) == 1 )
	{
		CGUIVariable *v = func_00107EE0( m_pVariables, "BrightnessValue" );
		if ( v->IsType( D_003029E8 ) == 1 )
		{
			if ( func_00107000( v ) > 0 )
			{
				func_00228EE0( buf, "gamma %f\n", func_00107000( v ) );
				Cbuf_AddText( buf );
			}
		}
	}
	m_bInitialized = 0;

	unsigned long start = func_00229B88();
	for ( int i = 0; i < m_iNumScreens; i++ )
	{
		if ( m_pScreens[i] )
			func_001058A8( m_pScreens[i], 3 );
		m_pScreens[i] = 0;
	}

	if ( m_pActions )
		func_001088A0( m_pActions, 3 );
	m_pActions = 0;
	if ( m_pTokenizer )
		func_00108018( m_pTokenizer, 3 );
	m_pTokenizer = 0;
	if ( m_pRenderer )
		func_001066D0( m_pRenderer, 3 );
	m_pRenderer = 0;
	if ( m_pVariables )
		func_00107C20( m_pVariables, 3 );
	m_pVariables = 0;

	Mem_Free( m_pScreens );
	m_pScreens = 0;
	func_00229C38( start, func_00229B88() );
	func_00228BD0();
}

void CGUI :: Init( pakfile_t *pak, const char *name, int bInPlace )
{
	char *text;
	unsigned long start;

	if ( m_bInitialized )
		return;

	m_bInitialized = 1;
	m_pActions = 0;
	m_pTokenizer = 0;
	m_pRenderer = 0;
	m_pVariables = 0;
	m_i04 = 0;
	m_i08 = 0;
	m_flAlpha = 0;
	m_iMaxScreens = 0;
	m_iNumScreens = 0;
	m_pScreens = 0;
	m_bFadingOut = 0;
	m_iCurrentScreen = 0;
	text = 0;
	m_Pak = *pak;

	start = func_00229B88();
	LoadFile( name, &text, bInPlace );
	func_00229C38( start, func_00229B88() );

	start = func_00229B88();
	ProcessIncludes( &text );
	func_00229C38( start, func_00229B88() );

	start = func_00229B88();
	ProcessDefines( &text );
	func_00229C38( start, func_00229B88() );

	m_pActions = func_00108890( __builtin_new( 0x14 ) );
	m_pTokenizer = func_00108008( __builtin_new( 0x14 ) );
	m_pRenderer = func_001066C0( __builtin_new( 0x15E8 ) );
	m_pVariables = func_00107C10( __builtin_new( 0x14 ) );

	func_00108960( m_pActions, this );
	func_00108070( m_pTokenizer );
	func_00108120( m_pTokenizer, text, -1 );
	func_00106758( m_pRenderer, &m_Pak );
	func_00107CE0( m_pVariables );

	RegisterActions();

	unsigned long t = func_00229B88();
	ParseScreens();
	func_00229C38( t, func_00229B88() );

	CGUIWidget *w = func_00105DC0( m_pScreens[m_iCurrentScreen] );
	if ( w )
		w->Activate();

	FreeFile( &text, bInPlace );
	m_iFadeInStart = m_i1C;
	text = 0;
}

void CGUI :: BeginDraw( void )
{
	func_0023AD98();
	func_0023C818( 0, 0, 0, 1.0f );
	func_0023C880( 0x4100 );
	glViewport( D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84 );
	glMatrixMode( 0x1701 );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( D_0032EF68, D_0032EF68 + vid.width, D_0032EF6C + D_0032EF74, D_0032EF6C, -99999, 99999 );
	glMatrixMode( 0x1700 );
	glPushMatrix();
	glLoadIdentity();
	SetColor( 1.0f, 1.0f, 1.0f );
}

void CGUI :: EndDraw( void )
{
	glMatrixMode( 0x1701 );
	glPopMatrix();
	glMatrixMode( 0x1700 );
	glPopMatrix();
	glEnable( 0xB71 );
	func_0023ADB8();
}

void CGUI :: SetColor( float r, float g, float b )
{
	glDisable( 0xB71 );
	glDisable( 0xB44 );
	glEnable( 0xBE2 );
	glEnable( 0xBC0 );
	glBlendFunc( 0x302, 0x303 );
	glColor4f( r * m_flAlpha, g * m_flAlpha, b * m_flAlpha, 1.0f );
}

void CGUI :: Update( void )
{
	if ( !m_bInitialized )
		return;

	UpdateFade();
	UpdateFocusVariable();
	func_00105C40( m_pScreens[m_iCurrentScreen] );
}

void CGUI :: HandleKey( int key )
{
	if ( !m_bInitialized )
		return;

	SetKeyNumber( key );
	CGUIWidget *w = func_00105DC0( m_pScreens[m_iCurrentScreen] );
	if ( w )
		func_00105830( w, key );
}

float CGUI :: GetAlpha( void )
{
	return m_flAlpha;
}

void CGUI :: Set04( int v )
{
	m_i04 = v;
}

void CGUI :: Set08( int v )
{
	m_i08 = v;
}

int CGUI :: Get04( void )
{
	return m_i04;
}

int CGUI :: Get08( void )
{
	return m_i08;
}

void CGUI :: SetFocus( const char *name )
{
	if ( func_00105860( func_00105DC0( m_pScreens[m_iCurrentScreen] ), name ) )
		return;

	for ( int i = 0; i < m_iNumScreens; i++ )
	{
		CGUIWidget *w = func_00105D20( m_pScreens[i], name );
		if ( !w )
			continue;

		func_00105DC0( m_pScreens[m_iCurrentScreen] )->Deactivate();

		if ( m_iCurrentScreen != i && !m_bFadingOut )
		{
			if ( func_00105D18( m_pScreens[i] ) == func_00105D18( m_pScreens[m_iCurrentScreen] ) || !D_00302A5C )
			{
				m_iCurrentScreen = i;
			}
			else
			{
				D_00302A5C = 0;
				m_bFadingOut = 1;
				m_iFadeOutStart = func_00229B88();
				m_flFadeOutTime = func_00105D10( m_pScreens[m_iCurrentScreen] );
				m_iNextScreen = i;
			}
		}

		func_00105DF8( m_pScreens[m_iCurrentScreen], w );
		w->Activate();
		return;
	}
}

CGUIWidget *CGUI :: FindWidget( const char *name )
{
	CGUIWidget *w = 0;

	for ( int i = 0; i < m_iNumScreens; i++ )
	{
		w = func_00105D20( m_pScreens[i], name );
		if ( w )
			break;
	}

	return w;
}

void *CGUI :: GetActions( void )
{
	return m_pActions;
}

void *CGUI :: GetTokenizer( void )
{
	return m_pTokenizer;
}

void *CGUI :: GetRenderer( void )
{
	return m_pRenderer;
}

void *CGUI :: GetVariables( void )
{
	return m_pVariables;
}

void CGUI :: SetVariable( const char *name, const char *value )
{
	func_00107EE0( m_pVariables, name )->SetString( value );
}

void CGUI :: ReadFile( const char *name, char *dst )
{
	int i = func_00235280( &m_Pak, name );
	int size = m_Pak.lumps[i].size;

	Q_memcpy( dst, m_Pak.data + m_Pak.lumps[i].filepos, size );
	dst[size] = 0;
}

int CGUI :: GetFileSize( const char *name )
{
	int i = func_00235280( &m_Pak, name );
	return m_Pak.lumps[i].size;
}

void CGUI :: LoadFile( const char *name, char **pText, int bInPlace )
{
	int i = func_00235280( &m_Pak, name );
	int size = m_Pak.lumps[i].size;

	if ( bInPlace == 1 )
	{
		*pText = (char *)m_Pak.data + m_Pak.lumps[i].filepos;
	}
	else
	{
		*pText = (char *)Mem_DbgMalloc( size + 1, "Launcher/GUI.cpp", 658 );
		Q_memcpy( *pText, m_Pak.data + m_Pak.lumps[i].filepos, size );
		(*pText)[size] = 0;
	}
}

void CGUI :: FreeFile( char **pText, int bInPlace )
{
	if ( bInPlace != 1 )
		Mem_Free( *pText );
	*pText = 0;
}

void CGUI :: ProcessIncludes( char **pText )
{
	char tokenizer[0x14];
	char name[100];
	char *p, *end, *buf, *dst;
	int len, size, rest;

	func_00108008( tokenizer );
	func_00108070( tokenizer );

	p = func_00228EC0( *pText, "!include" );
	while ( p )
	{
		end = p;
		func_00108120( tokenizer, p, -1 );
		func_00108148( tokenizer, 0, 0 );
		func_00228CF0( name, func_00108148( tokenizer, 0, 0 ) );
		len = func_00228DC0( *pText );
		size = GetFileSize( name );

		while ( *end != '\n' && *end != '\r' )
			end++;

		buf = (char *)Mem_DbgMalloc( len + size - ( end - p ) + 1, "Launcher/GUI.cpp", 745 );
		Q_memcpy( buf, *pText, p - *pText );
		dst = buf + ( p - *pText );
		ReadFile( name, dst );
		dst += size;
		rest = len - ( end - *pText );
		Q_memcpy( dst, end, rest );
		dst += rest;
		*dst = 0;
		Mem_Free( *pText );
		*pText = buf;
		p = func_00228EC0( buf, "!include" );
	}

	func_00108018( tokenizer, 2 );
}

/* NON_MATCHING: fluxo igual; dois grupos de variáveis trocam s1 e s2 */
void CGUI :: ProcessDefines( char **pText )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUI", ProcessDefines__4CGUIPPc);
#else
	char tokenizer[0x14];
	char name[0x2000];
	char value[0x2000];
	char *p, *c, *v, *q, *buf, *src;
	int depth, i, cont, count, nameLen, valueLen, textLen, n;

	func_00108008( tokenizer );
	func_00108070( tokenizer );

	p = FindToken( 1, *pText, "!define" );
	while ( p )
	{
		memset( name, 0, sizeof( name ) );
		depth = 1;
		c = p;
		memset( value, 0, sizeof( value ) );

		do
		{
			if ( *c == '\n' || *c == '\r' )
			{
				*c = ' ';
				depth--;
			}
			if ( *c == '\\' )
			{
				i = 1;
				cont = 1;
				while ( c[i] != '\n' && c[i] != '\r' )
				{
					if ( !func_00228E80( " \t", c[i] ) )
					{
						cont = 0;
						break;
					}
					i++;
				}
				if ( cont == 1 )
				{
					depth += 2;
					*c = ' ';
				}
			}
			c++;
		} while ( depth > 0 );

		func_00108120( tokenizer, p, -1 );
		c--;
		count = 0;
		func_00108148( tokenizer, 0, 0 );
		func_00228CF0( name, func_00108148( tokenizer, 0, 0 ) );
		nameLen = func_00228DC0( name );
		q = func_00228EC0( p, name ) + nameLen;
		v = func_00228EC0( q, func_00108148( tokenizer, 0, 0 ) );
		valueLen = c - v;
		Q_memcpy( value, v, valueLen );
		value[valueLen] = 0;

		textLen = func_00228DC0( *pText );
		for ( char *f = FindToken( 0, c, name ); f; f = FindToken( 0, f + 1, name ) )
			count++;

		buf = (char *)Mem_DbgMalloc( textLen - ( c - p ) + valueLen * count + 2, "Launcher/GUI.cpp", 900 );
		Q_memcpy( buf, *pText, p - *pText );
		src = c;
		c = buf + ( p - *pText );

		char *r = FindToken( 0, src, name );
		if ( r )
		{
			do
			{
				n = r - src;
				Q_memcpy( c, src, n );
				c += n;
				src += n;
				Q_memcpy( c, value, valueLen );
				c += valueLen;
				src += nameLen;
				r = FindToken( 0, src, name );
			} while ( r );
		}

		Q_memcpy( c, src, textLen - ( src - *pText ) );
		c += textLen - ( src - *pText );
		*c = 0;

		char *next = buf + ( p - *pText );
		Mem_Free( *pText );
		*pText = buf;
		p = FindToken( 0, next, "!define" );
	}

	func_00108018( tokenizer, 2 );
#endif
}

void CGUI :: RegisterActions( void )
{
	AddAction( func_00108F40( __builtin_new( 0xC ) ) );
	AddAction( func_001090A0( __builtin_new( 0xC ) ) );
	AddAction( func_00109240( __builtin_new( 0xC ) ) );
	AddAction( func_001093E0( __builtin_new( 0xC ) ) );
	AddAction( func_00109498( __builtin_new( 0xC ) ) );
	AddAction( func_00109568( __builtin_new( 0xC ) ) );
	AddAction( func_00109728( __builtin_new( 0xC ) ) );
	AddAction( func_00109910( __builtin_new( 0xC ) ) );
	AddAction( func_00109B08( __builtin_new( 0xC ) ) );
	AddAction( func_00109C30( __builtin_new( 0xC ) ) );
	AddAction( func_0010A010( __builtin_new( 0xC ) ) );
	AddAction( func_0010A4D8( __builtin_new( 0x18 ) ) );
	AddAction( func_0010A220( __builtin_new( 0xC ) ) );
	AddAction( func_0010A330( __builtin_new( 0xC ) ) );
	AddAction( func_0010AA78( __builtin_new( 0xC ) ) );
	AddAction( func_0010AC08( __builtin_new( 0xC ) ) );
	AddAction( func_0010AD98( __builtin_new( 0x10 ) ) );
	AddAction( func_0010B698( __builtin_new( 0x10 ) ) );
	AddAction( func_0010BFF8( __builtin_new( 0xC ) ) );
	AddAction( func_0010C2E8( __builtin_new( 0x10 ) ) );
	AddAction( func_0010E540( __builtin_new( 0xC ) ) );
	AddAction( func_0010E6A8( __builtin_new( 0xC ) ) );
	AddAction( func_0010E8F8( __builtin_new( 0x10 ) ) );
	AddAction( func_0010EA58( __builtin_new( 0xC ) ) );
	AddAction( func_0010B8D8( __builtin_new( 0xC ) ) );
}

void CGUI :: ParseScreens( void )
{
	char *token = func_00108148( m_pTokenizer, 0, 0 );

	while ( token && func_00228C00( token, "END" ) != 1 )
	{
		if ( func_00228C00( token, "GUI_SCREEN" ) == 1 )
		{
			CGUIScreen *s = func_00105898( __builtin_new( 0x44 ), this );
			func_00105988( s );
			D_00302A58 = 0;
			AddScreen( s );
		}
		if ( func_00228C00( token, "GUI_ACTION" ) == 1 )
		{
			CGUIAction *a = func_00108BD8( __builtin_new( 0x18 ) );
			a->Init( m_pTokenizer );
			func_00108980( m_pActions, a );
		}
		func_00107CF8( m_pVariables, token, m_pTokenizer );
		token = func_00108148( m_pTokenizer, 0, 0 );
	}
}

void CGUI :: UpdateFade( void )
{
	if ( m_bFadingOut )
	{
		unsigned int now = func_00229B88();
		unsigned int elapsed = func_00229C38( m_iFadeOutStart, now );
		if ( m_flFadeOutTime < (float)elapsed )
		{
			m_flAlpha = 0;
			m_bFadingOut = 0;
			m_iFadeInStart = func_00229B88();
			m_iCurrentScreen = m_iNextScreen;
			CGUIWidget *w = func_00105DC0( m_pScreens[m_iCurrentScreen] );
			if ( w )
				w->Activate();
			return;
		}
		m_flAlpha = 1.0f - (float)elapsed / m_flFadeOutTime;
	}
	else if ( func_00105D08( m_pScreens[m_iCurrentScreen] ) != 0 )
	{
		unsigned int now = func_00229B88();
		unsigned int elapsed = func_00229C38( m_iFadeInStart, now );
		float flElapsed = (float)elapsed;
		if ( func_00105D08( m_pScreens[m_iCurrentScreen] ) < flElapsed )
			m_flAlpha = 1.0f;
		else
			m_flAlpha = (float)elapsed / func_00105D08( m_pScreens[m_iCurrentScreen] );
	}
	else
		m_flAlpha = 1.0f;
}

void CGUI :: AddScreen( CGUIScreen *s )
{
	if ( m_iNumScreens == m_iMaxScreens )
	{
		m_iMaxScreens = m_iNumScreens + 5;
		m_pScreens = (CGUIScreen **)func_002260D0( m_pScreens, m_iMaxScreens * sizeof( CGUIScreen * ), "Launcher/GUI.cpp", 1091 );
	}
	m_pScreens[m_iNumScreens] = s;
	m_iNumScreens++;
}

void CGUI :: AddAction( CGUIAction *a )
{
	a->Init( 0 );
	func_00108980( m_pActions, a );
}

void CGUI :: SetKeyNumber( int key )
{
	char buf[16];

	if ( func_00107F00( m_pVariables, "IN_KEY_NUMBER" ) == 1 )
	{
		CGUIVariable *v = func_00107EE0( m_pVariables, "IN_KEY_NUMBER" );
		func_00228FA0( key, buf, 10 );
		v->SetString( buf );
	}
}

void CGUI :: UpdateFocusVariable( void )
{
	if ( func_00107F00( m_pVariables, "CURRENT_GENERIC_FOCUS" ) == 1 )
	{
		CGUIVariable *v = func_00107EE0( m_pVariables, "CURRENT_GENERIC_FOCUS" );
		const char *name = func_00105890( func_00105DC0( m_pScreens[m_iCurrentScreen] ) );
		v->SetString( name );
	}
}

/* NON_MATCHING: só dois desvios saem beq/bne em vez de beql/bnel */
static char *FindToken( int bStart, char *text, const char *token )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUI", FindToken__FiPcPCc);
#else
	int len = func_00228DC0( token );
	char *found = 0;

	do
	{
		char *p = func_00228EC0( text, token );
		if ( !p )
			break;

		int ok;
		if ( bStart )
			ok = 1;
		else
		{
			ok = 0;
			if ( func_00228E80( " \\\"\t\n\r", p[-1] ) )
				ok = 1;
		}

		if ( ok == 1 )
		{
			char *c = func_00228E80( " \\\"\t\n\r", p[len] );
			if ( p[len] == 0 || c )
				found = p;
		}
		if ( !found )
			text = p + 1;
	} while ( !found );

	return found;
#endif
}
