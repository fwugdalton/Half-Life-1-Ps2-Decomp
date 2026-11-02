/* Launcher (Gearbox, sem fonte): uma tela do GUI (CGUIScreen, 0x44 bytes): NAME,
   FADE_IN_TIME/FADE_OUT_TIME (segundos, guardados em ms), TEXTURE e TEXTURE_RECT do fundo e os
   elementos GUI_GENERIC, desenhados em 100 camadas. Nome do arquivo tirado da string de
   alocação. */

#include "launcher/GUI.h"

extern "C" {
float func_002290E0(const char *s);	/* atof */
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
void Mem_Free(void *ptr);
void glBegin(int mode);
void glEnd(void);
void glTexCoord2f(float s, float t);
void glVertex2f(float x, float y);
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
	void Bind( int texnum );
};

class CGUIScreen;

class CGUIGeneric : public CGUIWidget
{
public:
	CGUIGeneric( CGUIScreen *screen );
	virtual ~CGUIGeneric();
	virtual int Parse( void );
	virtual void V2( void );
	virtual void Activate( void );
	virtual void Deactivate( void );
	virtual void CopyAttribute( const char *attr, CGUIWidget *from );
	char m_pad2[0xA0 - 0x38];
};

class CGUIScreen
{
public:
	CGUIScreen( CGUI *gui );
	~CGUIScreen();
	int Parse( void );
	void Draw( void );
	char *GetName( void );
	float GetFadeInTime( void );
	float GetFadeOutTime( void );
	int GetTexture( void );
	CGUIWidget *FindWidget( const char *name );
	CGUIWidget *GetFocus( void );
	inline CGUIWidget *GetWidget( int i )
	{
		if ( i < m_iNumWidgets && i >= 0 )
			return m_pWidgets[i];
		return 0;
	}
	void SetFocus( CGUIWidget *w );
	int IsFocus( CGUIWidget *w );
	void Add( CGUIWidget *w );
	void DrawBackground( void );

	CGUI *m_pGUI;
	int m_bParsed;
	char *m_szName;
	int m_bTexture;
	int m_iTexture;
	float m_flRect[4];
	float m_flWidth;
	float m_flHeight;
	float m_flFadeIn;
	float m_flFadeOut;
	int m_iNumWidgets;
	int m_iMaxWidgets;
	int m_iFocus;
	CGUIWidget **m_pWidgets;
};

CGUIScreen :: CGUIScreen( CGUI *gui )
{
	m_pGUI = gui;
	m_bParsed = 0;
}

CGUIScreen :: ~CGUIScreen()
{
	int i;

	m_bParsed = 0;
	for ( i = 0; i < m_iNumWidgets; i++ )
	{
		if ( m_pWidgets[i] )
		{
			delete m_pWidgets[i];
			m_pWidgets[i] = 0;
		}
	}
	if ( m_pWidgets )
	{
		Mem_Free( m_pWidgets );
		m_pWidgets = 0;
	}
	if ( m_szName )
	{
		Mem_Free( m_szName );
		m_szName = 0;
	}
}

int CGUIScreen :: Parse( void )
{
	int r;
	char *t;
	CGUIParser *p;

	if ( !m_bParsed )
	{
		r = 1;
		m_iFocus = 0;
		m_szName = 0;
		m_pWidgets = 0;
		m_iMaxWidgets = 0;
		m_iNumWidgets = 0;
		m_bTexture = 0;
		m_flFadeIn = 0;
		m_flFadeOut = 0;
		p = (CGUIParser *)m_pGUI->GetTokenizer();
		t = p->NextToken( 0, 0 );
		while ( t && func_00228C00( t, "END" ) != 1 )
		{
			int isTex;
			if ( func_00228C00( t, "NAME" ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_szName = func_00228D30( t );
			}
			if ( func_00228C00( t, "FADE_IN_TIME" ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_flFadeIn = func_002290E0( t ) * 1000.0f;
			}
			if ( func_00228C00( t, "FADE_OUT_TIME" ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_flFadeOut = func_002290E0( t ) * 1000.0f;
			}
			if ( ( isTex = func_00228C00( t, "TEXTURE" ) ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				CGUIRenderer *rd = (CGUIRenderer *)m_pGUI->GetRenderer();
				m_iTexture = rd->GetTexture( t );
				m_flWidth = rd->GetWidth( t );
				m_flHeight = rd->GetWidth( t );
				m_bTexture = isTex;
			}
			if ( func_00228C00( t, "TEXTURE_RECT" ) == 1 )
				p->ReadFloat4( &m_flRect[0], &m_flRect[1], &m_flRect[2], &m_flRect[3] );
			if ( func_00228C00( t, "GUI_GENERIC" ) == 1 )
			{
				CGUIWidget *w = new CGUIGeneric( this );
				w->Parse();
				Add( w );
			}
			t = p->NextToken( 0, 0 );
		}
		m_bParsed = 1;
	}
	else
		r = 0;
	return r;
}

void CGUIScreen :: Draw( void )
{
	int layer, i;

	DrawBackground();
	for ( layer = 0; layer < 100; layer++ )
	{
		for ( i = 0; i < m_iNumWidgets; i++ )
		{
			if ( m_pWidgets[i]->Get8() == layer )
				m_pWidgets[i]->V2();
		}
	}
}

char *CGUIScreen :: GetName( void )
{
	return m_szName;
}

float CGUIScreen :: GetFadeInTime( void )
{
	return m_flFadeIn;
}

float CGUIScreen :: GetFadeOutTime( void )
{
	return m_flFadeOut;
}

int CGUIScreen :: GetTexture( void )
{
	return m_iTexture;
}

CGUIWidget *CGUIScreen :: FindWidget( const char *name )
{
	int i;
	CGUIWidget *w = 0;

	for ( i = 0; i < m_iNumWidgets; i++ )
	{
		if ( m_pWidgets[i]->IsNamed( name ) )
		{
			w = m_pWidgets[i];
			break;
		}
	}
	if ( i == m_iNumWidgets )
		return 0;
	return w;
}

CGUIWidget *CGUIScreen :: GetFocus( void )
{
	return GetWidget( m_iFocus );
}

void CGUIScreen :: SetFocus( CGUIWidget *w )
{
	int i;

	for ( i = 0; i < m_iNumWidgets; i++ )
	{
		if ( m_pWidgets[i] == w )
		{
			m_iFocus = i;
			break;
		}
	}
}

int CGUIScreen :: IsFocus( CGUIWidget *w )
{
	return w == m_pWidgets[m_iFocus];
}

void CGUIScreen :: Add( CGUIWidget *w )
{
	if ( m_iNumWidgets == m_iMaxWidgets )
	{
		m_iMaxWidgets = m_iNumWidgets + 5;
		m_pWidgets = (CGUIWidget **)func_002260D0( m_pWidgets, m_iMaxWidgets * 4, "Launcher/GUIScreen.cpp", 304 );
	}
	m_pWidgets[m_iNumWidgets] = w;
	m_iNumWidgets++;
}

void CGUIScreen :: DrawBackground( void )
{
	if ( m_bTexture == 1 )
	{
		m_pGUI->SetColor( 1.0f, 1.0f, 1.0f );
		( (CGUIRenderer *)m_pGUI->GetRenderer() )->Bind( m_iTexture );
		glBegin( 9 );
		glTexCoord2f( m_flRect[0] / m_flWidth, m_flRect[1] / m_flHeight );
		glVertex2f( 0.0f, -1.0f );
		glTexCoord2f( m_flRect[2] / m_flWidth, m_flRect[1] / m_flHeight );
		glVertex2f( 512.0f, -1.0f );
		glTexCoord2f( m_flRect[2] / m_flWidth, m_flRect[3] / m_flHeight );
		glVertex2f( 512.0f, 448.0f );
		glTexCoord2f( m_flRect[0] / m_flWidth, m_flRect[3] / m_flHeight );
		glVertex2f( 0.0f, 448.0f );
		glEnd();
	}
}
