/* Launcher (Gearbox, sem fonte): texto de um elemento de tela (CGUIElementCaption, 0x48 bytes,
   bloco GUI_CAPTION: TEXT, PARAMS (variáveis do GUI trocadas nos %f/%s do texto), SCALE,
   COLORS (com e sem foco) e WRAP). Desenha com a fonte alfa do motor e troca \X_BUTTON etc.
   pelos ícones dos botões do controle (buttons.psi). Nome do arquivo tirado da string de
   alocação. */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
float func_002290E0(const char *s);	/* atof */
char *func_00228D80(char *dst, const char *src);	/* strcat */
char *func_00228E80(const char *s, int c);			/* strchr */
int func_00228DC0(const char *s);					/* strlen */
int func_00228E40(const char *a, const char *b, int n);	/* strncmp */
int func_00225958(char c);
int sprintf(char *buf, const char *fmt, ...);
char *strcat(char *dst, const char *src);
extern const char *D_003029E8;	/* "Float" */
extern const char *D_003029F0;	/* "String" */
extern int D_00399570, D_00399558;
void glBlendFunc(int sfactor, int dfactor);
void glTexEnvf(int target, int pname, float param);
void glEnable(int cap);
void glDisable(int cap);
void glColor4f(float r, float g, float b, float a);
void glBegin(int mode);
void glEnd(void);
void glTexCoord2f(float s, float t);
void glVertex2f(float x, float y);
void GL_DisableMultitexture(void);
void Draw_BindAlphaFont(void);
void Draw_GetAlphaFontChar(int ch, int *width, int *height, float *s, float *t, float *s2, float *t2);
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
	void ReadFloat4( float *a, float *b, float *c, float *d );
};

class CGUIRenderer
{
public:
	int GetTexture( const char *name );
	int GetWidth( const char *name );
	int GetHeight( const char *name );
	void Bind( int texnum );
};

class CGUIVariableManager
{
public:
	CGUIVariable *Get( const char *name );
};

class CGUIScreen
{
public:
	CGUI *m_pGUI;
};

/* ícone de botão que pode aparecer no texto como \NOME (id 17 = fim da tabela) */
typedef struct
{
	int id;
	char name[0x14];
	int width;
	int height;
	int s;
	int t;
	int s2;
	int t2;
	char texture[0x64];
} GUIIcon_t;

#define ICON_NONE	17

GUIIcon_t D_00301F78[18] =
{
	{ 0, "N", 0, 0, 0, 0, 0, 0, "" },
	{ 1, "X_BUTTON", 22, 22, 105, 85, 127, 107, "buttons.psi" },
	{ 2, "TRI_BUTTON", 22, 22, 70, 108, 91, 129, "buttons.psi" },
	{ 3, "SQUARE_BUTTON", 22, 22, 91, 108, 110, 128, "buttons.psi" },
	{ 4, "CIRCLE_BUTTON", 22, 22, 110, 107, 129, 127, "buttons.psi" },
	{ 5, "START_BUTTON", 25, 21, 1, 90, 26, 111, "buttons.psi" },
	{ 6, "SELECT_BUTTON", 25, 21, 1, 66, 26, 88, "buttons.psi" },
	{ 7, "DPAD_UP", 16, 17, 54, 110, 69, 127, "buttons.psi" },
	{ 8, "DPAD_DOWN", 16, 17, 38, 110, 52, 127, "buttons.psi" },
	{ 9, "DPAD_LEFT", 18, 16, 1, 112, 19, 127, "buttons.psi" },
	{ 10, "DPAD_RIGHT", 18, 16, 20, 112, 37, 127, "buttons.psi" },
	{ 11, "R1_BUTTON", 30, 18, 29, 66, 59, 84, "buttons.psi" },
	{ 12, "R2_BUTTON", 28, 23, 94, 0, 124, 18, "buttons.psi" },
	{ 13, "R3_BUTTON", 21, 21, 84, 84, 105, 107, "buttons.psi" },
	{ 14, "L1_BUTTON", 30, 18, 29, 86, 59, 104, "buttons.psi" },
	{ 15, "L2_BUTTON", 28, 23, 94, 30, 124, 48, "buttons.psi" },
	{ 16, "L3_BUTTON", 21, 21, 62, 85, 83, 108, "buttons.psi" },
	{ 17, "", 0, 0, 0, 0, 0, 0, "" },
};

class CGUIElementCaption : public CGUIWidgetPart
{
public:
	CGUIElementCaption( CGUIScreen *screen );
	CGUIElementCaption( const CGUIElementCaption &other );
	virtual ~CGUIElementCaption();
	virtual void Parse( void );
	void Draw( int focus, int x, int y );
	void DrawString( const char *s, int x, int y, float r, float g, float b, float a );
	int ParseIcon( char **ps, int x );
	int DrawChar( char c, int x, int y );
	int LineHeight( void );
	int NeedWrap( const char *s, int x );
	int Matches( const char *s, int len, const char *name );

	int m_bText;				/* 0x0C */
	char *m_szText;				/* 0x10 TEXT */
	int m_iNumParams;			/* 0x14 */
	char **m_pParams;			/* 0x18 PARAMS */
	float m_flScaleX;			/* 0x1C SCALE */
	float m_flScaleY;
	float m_flWrap;				/* 0x24 WRAP (-1 = não quebra) */
	float m_flFocusColor[4];	/* 0x28 COLORS */
	float m_flColor[4];			/* 0x38 */
};

CGUIElementCaption :: CGUIElementCaption( CGUIScreen *screen ) : CGUIWidgetPart( screen )
{
}

CGUIElementCaption :: CGUIElementCaption( const CGUIElementCaption &other ) : CGUIWidgetPart( other.m_p4 )
{
	if ( ( m_bText = other.m_bText ) )
		m_szText = func_00228D30( other.m_szText );
	else
		m_szText = 0;
	m_flScaleX = other.m_flScaleX;
	m_flScaleY = other.m_flScaleY;
	m_flWrap = other.m_flWrap;
	m_flFocusColor[0] = other.m_flFocusColor[0];
	m_flFocusColor[1] = other.m_flFocusColor[1];
	m_flFocusColor[2] = other.m_flFocusColor[2];
	m_flFocusColor[3] = other.m_flFocusColor[3];
	m_flColor[0] = other.m_flColor[0];
	m_flColor[1] = other.m_flColor[1];
	m_flColor[2] = other.m_flColor[2];
	m_flColor[3] = other.m_flColor[3];
}

CGUIElementCaption :: ~CGUIElementCaption()
{
	int i;

	if ( m_szText )
	{
		Mem_Free( m_szText );
		m_szText = 0;
	}
	for ( i = 0; i < m_iNumParams; i++ )
	{
		Mem_Free( m_pParams[i] );
		m_pParams[i] = 0;
	}
	if ( m_pParams )
	{
		Mem_Free( m_pParams );
		m_pParams = 0;
	}
}

void CGUIElementCaption :: Parse( void )
{
	int f;
	char *t;
	CGUIParser *p;

	p = (CGUIParser *)m_p4->m_pGUI->GetTokenizer();
	m_bText = 0;
	m_szText = 0;
	m_iNumParams = 0;
	m_pParams = 0;
	m_flFocusColor[2] = 0;
	m_flColor[2] = 0;
	m_flFocusColor[1] = 0.88f;
	m_flColor[0] = 0.72f;
	m_flColor[1] = 0.32f;
	m_flScaleX = 1.0f;
	m_flScaleY = 1.0f;
	m_flFocusColor[0] = 1.0f;
	m_flFocusColor[3] = 1.0f;
	m_flColor[3] = 1.0f;
	m_flWrap = -1.0f;
	t = p->NextToken( 0, 0 );
	while ( t && func_00228C00( t, "END" ) != 1 )
	{
		if ( ( f = func_00228C00( t, "TEXT" ) ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			m_szText = func_00228D30( t );
			m_bText = f;
		}
		if ( func_00228C00( t, "PARAMS" ) == 1 )
		{
			for ( t = p->NextToken( 0, 0 ); func_00228C00( t, "END" ) != 1; t = p->NextToken( 0, 0 ) )
			{
				int i;
				char **list = (char **)Mem_DbgMalloc( ( m_iNumParams + 1 ) * sizeof( char * ), "Launcher/GUIElementCaption.cpp", 201 );

				for ( i = 0; i < m_iNumParams; i++ )
					list[i] = m_pParams[i];
				list[i] = func_00228D30( t );
				if ( m_pParams )
					Mem_Free( m_pParams );
				m_pParams = list;
				m_iNumParams++;
			}
		}
		if ( func_00228C00( t, "SCALE" ) == 1 )
		{
			m_flScaleX = func_002290E0( p->NextToken( 0, 0 ) );
			t = p->NextToken( 0, 0 );
			m_flScaleY = func_002290E0( t );
		}
		if ( func_00228C00( t, "COLORS" ) == 1 )
		{
			p->ReadFloat4( &m_flFocusColor[0], &m_flFocusColor[1], &m_flFocusColor[2], &m_flFocusColor[3] );
			p->ReadFloat4( &m_flColor[0], &m_flColor[1], &m_flColor[2], &m_flColor[3] );
		}
		if ( func_00228C00( t, "WRAP" ) == 1 )
			m_flWrap = func_002290E0( p->NextToken( 0, 0 ) );
		t = p->NextToken( 0, 0 );
	}
	m_i0 = 1;
}

void CGUIElementCaption :: Draw( int focus, int x, int y )
{
	float r, g, b, a;

	if ( m_bText == 1 )
	{
		if ( focus == 1 )
		{
			r = m_flFocusColor[0];
			g = m_flFocusColor[1];
			b = m_flFocusColor[2];
			a = m_flFocusColor[3];
		}
		else
		{
			r = m_flColor[0];
			g = m_flColor[1];
			b = m_flColor[2];
			a = m_flColor[3];
		}
		char *text = 0;
		a *= m_p4->m_pGUI->GetAlpha();
		m_p4->m_pGUI->SetColor( r, g, b );

		char buf[200] = "";

		if ( !m_iNumParams )
			text = m_szText;
		else
		{
			char *s = m_szText;
			int i;

			for ( i = 0; i < m_iNumParams; i++ )
			{
				char *fmt = s;
				char *pos = 0;
				int done = 0;
				CGUIVariable *v = ( (CGUIVariableManager *)m_p4->m_pGUI->GetVariables() )->Get( m_pParams[i] );

				while ( !done )
				{
					pos = func_00228E80( s, '%' );
					if ( !pos )
						break;
					if ( pos[1] != '%' )
						done = 1;
					else
						s = pos + 2;
				}
				if ( done == 1 )
				{
					char tmp[200] = "";
					int type = 'f';
					char save;

					if ( v->IsType( D_003029E8 ) != 1 )
						type = 0;
					if ( v->IsType( D_003029F0 ) == 1 )
						type = 's';
					char *end = func_00228E80( pos, type );

					save = end[1];
					s = end + 1;
					end[1] = 0;
					switch ( type )
					{
					case 'f':
						sprintf( tmp, fmt, ( (CGUIVariableFloat *)v )->GetValue() );
						break;
					case 's':
						sprintf( tmp, fmt, ( (CGUIVariableString *)v )->GetValue() );
						break;
					}
					func_00228D80( buf, tmp );
					end[1] = save;
				}
				else
				{
					text = m_szText;
					break;
				}
			}
			if ( !text )
			{
				char tmp[200] = "";

				sprintf( tmp, s );
				strcat( buf, tmp );
				text = buf;
			}
		}
		DrawString( text, x, y, r, g, b, a );
	}
}

/* NON_MATCHING: só registradores (no ícone o original põe o id em v1 e o produto id * 0x94 em a0) */
void CGUIElementCaption :: DrawString( const char *s, int x, int y, float r, float g, float b, float a )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIElementCaption", DrawString__18CGUIElementCaptionPCciiffff);
#else
	int dx = 0;
	int dy = 0;
	char *p;

	glColor4f( r, g, b, a );
	glEnable( 0xBE2 );
	glTexEnvf( 0x2300, 0x2200, 0x2100 );
	glEnable( 0xBC0 );
	glDisable( 0xB71 );
	glBlendFunc( 0x302, 0x303 );
	GL_DisableMultitexture();
	Draw_BindAlphaFont();
	p = (char *)s;
	while ( *p )
	{
		int px = x + dx;
		int id = ParseIcon( &p, px );

		switch ( id )
		{
		case ICON_NONE:
		{
			int py = y + dy;

			glColor4f( 0, 0, 0, a );
			DrawChar( *p, px + 2, py + 2 );
			glColor4f( r, g, b, a );
			dx += DrawChar( *p, px, py );
			p++;
			break;
		}
		case 0:
			dx = 0;
			dy += 5 + LineHeight();
			break;
		default:
		{
			float fx = px;
			float fy = y + dy;
			GUIIcon_t *icon = &D_00301F78[id];
			int tex = ( (CGUIRenderer *)m_p4->m_pGUI->GetRenderer() )->GetTexture( icon->texture );
			float w = ( (CGUIRenderer *)m_p4->m_pGUI->GetRenderer() )->GetWidth( icon->texture );
			float h = ( (CGUIRenderer *)m_p4->m_pGUI->GetRenderer() )->GetHeight( icon->texture );

			glBlendFunc( 0x302, 0x303 );
			( (CGUIRenderer *)m_p4->m_pGUI->GetRenderer() )->Bind( tex );
			glColor4f( 1, 1, 1, 1 );
			glBegin( 6 );
			glTexCoord2f( icon->s / w, icon->t / h );
			glVertex2f( fx, fy );
			glTexCoord2f( icon->s2 / w, icon->t / h );
			glVertex2f( fx + icon->width * m_flScaleX, fy );
			glTexCoord2f( icon->s2 / w, icon->t2 / h );
			glVertex2f( fx + icon->width * m_flScaleX, fy + icon->height * m_flScaleY );
			glTexCoord2f( icon->s / w, icon->t2 / h );
			glVertex2f( fx, fy + icon->height * m_flScaleY );
			glEnd();
			dx += icon->width;
			break;
		}
		}
	}
#endif
}

int CGUIElementCaption :: ParseIcon( char **ps, int x )
{
	int r = ICON_NONE;
	char *s = *ps;

	if ( NeedWrap( s, x ) == 1 )
		r = 0;
	else if ( *s == '\\' )
	{
		char *name = s + 1;
		int len = func_00228DC0( name );
		int i;

		for ( i = 0; D_00301F78[i].id != ICON_NONE; i++ )
		{
			if ( Matches( name, len, D_00301F78[i].name ) == 1 )
			{
				if ( x + D_00301F78[i].width <= m_flWrap || m_flWrap == -1.0f )
				{
					s = name + func_00228DC0( D_00301F78[i].name );
					r = D_00301F78[i].id;
				}
				else
					r = 0;
				break;
			}
		}
	}
	*ps = s;
	return r;
}

int CGUIElementCaption :: DrawChar( char c, int x, int y )
{
	int w, h;
	float s, t, s2, t2;
	float fx, fy;

	Draw_BindAlphaFont();
	Draw_GetAlphaFontChar( c, &w, &h, &s, &t, &s2, &t2 );
	D_00399570++;
	D_00399558 += 4;
	if ( c == 'm' || c == 'n' || c == 'o' || c == 'k' || c == 'x' )
	{
		h = (int)( h * 0.9f );
		t2 = ( t2 - t ) * 0.9f + t;
	}
	if ( c == 'y' || c == 'p' )
	{
		h = (int)( h * 0.95f );
		t2 = ( t2 - t ) * 0.95f + t;
	}
	fx = x;
	glBegin( 6 );
	fy = y;
	glTexCoord2f( s, t );
	glVertex2f( fx, fy );
	glTexCoord2f( s2, t );
	glVertex2f( fx + w * m_flScaleX, fy );
	glTexCoord2f( s2, t2 );
	glVertex2f( fx + w * m_flScaleX, fy + h * m_flScaleY );
	glTexCoord2f( s, t2 );
	glVertex2f( fx, fy + h * m_flScaleY );
	glEnd();
	return (int)( w * m_flScaleX );
}

int CGUIElementCaption :: LineHeight( void )
{
	int w, h;
	float s, t, s2, t2;

	Draw_GetAlphaFontChar( 'a', &w, &h, &s, &t, &s2, &t2 );
	h = (int)( h * m_flScaleY );
	return h;
}

int CGUIElementCaption :: NeedWrap( const char *s, int x )
{
	int r = 0;
	int w, h;
	float fs, ft, fs2, ft2;

	if ( m_flWrap > 0 )
	{
		const char *c = s;

		while ( func_00225958( *c ) == 1 )
		{
			if ( x > m_flWrap && m_flWrap > 0 )
			{
				r = 1;
				break;
			}
			if ( *c == '\\' )
			{
				r = 0;
				break;
			}
			Draw_GetAlphaFontChar( *c, &w, &h, &fs, &ft, &fs2, &ft2 );
			c++;
			x += (int)( w * m_flScaleX );
		}
	}
	return r;
}

int CGUIElementCaption :: Matches( const char *s, int len, const char *name )
{
	int n = func_00228DC0( name );

	if ( len < n )
		return 0;
	return func_00228E40( s, name, n ) == 0;
}
