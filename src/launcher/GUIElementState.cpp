/* Launcher (Gearbox, sem fonte): um estado de um GUI_DEPENDENCY (CGUIElementState, 0x70
   bytes, bloco GUI_ELEMENT_STATE): o valor da variável (DEPENDS_VALUE) e o que o elemento
   troca quando ele vale (Z_PLANE, SCREEN_RECT, TEXTURE, TEXTURE_RECT, COLOR_MODULATE, as partes
   GUI_CAPTION/GUI_INPUT e os comandos de foco). Sem vtable; nome do arquivo inventado. */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
int func_002290C0(const char *s);	/* atoi */
float func_002290E0(const char *s);	/* atof */
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
};

class CGUICommand
{
public:
	CGUICommand();
	~CGUICommand();
	void Parse( CGUIParser *parser );
	char m_pad[0x24];
};

class CGUIScreen
{
public:
	CGUI *m_pGUI;
};

/* o que um GUI_DEPENDENCY satisfeito troca no elemento (igual ao do GUIGeneric.cpp) */
typedef struct
{
	int bZPlane;
	int iZPlane;
	int bRect;
	float flRect[4];
	int bTexture;
	int iTexture;
	float flTexWidth;
	float flTexHeight;
	int bTexRect;
	float flTexRect[4];
	int bColor;
	float flColor[3];
	CGUIWidgetPart *pCaption;
	CGUIWidgetPart *pInput;
	CGUICommand *pCommands[4];
} GUIOverride_t;

class CGUIElementCaption : public CGUIWidgetPart
{
public:
	CGUIElementCaption( CGUIScreen *screen );
	virtual void Parse( void );
	char m_pad2[0x48 - 0xC];
};

class CGUIElementInput : public CGUIWidgetPart
{
public:
	CGUIElementInput( CGUIScreen *screen );
	virtual void Parse( void );
	char m_pad2[0x18 - 0xC];
};

class CGUIElementState
{
public:
	CGUIElementState();
	~CGUIElementState();
	void Parse( CGUIScreen *screen );
	float GetValue( void );
	GUIOverride_t *GetOverride( void );

	int m_bParsed;				/* 0x00 */
	float m_flValue;			/* 0x04 DEPENDS_VALUE */
	GUIOverride_t m_Override;	/* 0x08 */
};

CGUIElementState :: CGUIElementState()
{
	m_bParsed = 0;
}

CGUIElementState :: ~CGUIElementState()
{
	m_bParsed = 0;
	if ( m_Override.pCaption )
	{
		delete m_Override.pCaption;
		m_Override.pCaption = 0;
	}
	if ( m_Override.pInput )
	{
		delete m_Override.pInput;
		m_Override.pInput = 0;
	}
	if ( m_Override.pCommands[0] )
	{
		delete m_Override.pCommands[0];
		m_Override.pCommands[0] = 0;
	}
	if ( m_Override.pCommands[1] )
	{
		delete m_Override.pCommands[1];
		m_Override.pCommands[1] = 0;
	}
	if ( m_Override.pCommands[2] )
	{
		delete m_Override.pCommands[2];
		m_Override.pCommands[2] = 0;
	}
}

void CGUIElementState :: Parse( CGUIScreen *screen )
{
/* NON_MATCHING: o alvo guarda o %hi do "END" em s6 fora do laço (só esse); o resto é igual */
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIElementState", Parse__16CGUIElementStateP10CGUIScreen);
#else
	CGUIParser *p;
	char *t;
	int f;
	int g;

	if ( m_bParsed )
		return;

	p = (CGUIParser *)screen->m_pGUI->GetTokenizer();
	m_Override.bZPlane = 0;
	m_Override.bRect = 0;
	m_Override.bTexture = 0;
	m_Override.bTexRect = 0;
	m_Override.pCaption = 0;
	m_Override.pInput = 0;
	m_Override.bColor = 0;
	m_Override.pCommands[0] = 0;
	m_Override.pCommands[1] = 0;
	m_Override.pCommands[2] = 0;
	m_Override.pCommands[3] = 0;

	t = p->NextToken( 0, 0 );
	while ( t && func_00228C00( t, "END" ) != 1 )
	{
		if ( func_00228C00( t, "DEPENDS_VALUE" ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			m_flValue = func_002290E0( t );
		}
		if ( ( f = func_00228C00( t, "Z_PLANE" ) ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			m_Override.bZPlane = f;
			m_Override.iZPlane = func_002290C0( t );
		}
		if ( ( g = func_00228C00( t, "SCREEN_RECT" ) ) == 1 )
		{
			m_Override.bRect = g;
			p->ReadFloat4( &m_Override.flRect[0], &m_Override.flRect[1], &m_Override.flRect[2], &m_Override.flRect[3] );
		}
		if ( ( g = func_00228C00( t, "TEXTURE" ) ) == 1 )
		{
			m_Override.bTexture = g;
			t = p->NextToken( 0, 0 );
			m_Override.iTexture = ( (CGUIRenderer *)screen->m_pGUI->GetRenderer() )->GetTexture( t );
			m_Override.flTexWidth = ( (CGUIRenderer *)screen->m_pGUI->GetRenderer() )->GetWidth( t );
			m_Override.flTexHeight = ( (CGUIRenderer *)screen->m_pGUI->GetRenderer() )->GetHeight( t );
		}
		if ( ( g = func_00228C00( t, "TEXTURE_RECT" ) ) == 1 )
		{
			m_Override.bTexRect = g;
			p->ReadFloat4( &m_Override.flTexRect[0], &m_Override.flTexRect[1], &m_Override.flTexRect[2], &m_Override.flTexRect[3] );
		}
		if ( ( g = func_00228C00( t, "COLOR_MODULATE" ) ) == 1 )
		{
			m_Override.bColor = g;
			m_Override.flColor[0] = func_002290E0( p->NextToken( 0, 0 ) );
			m_Override.flColor[1] = func_002290E0( p->NextToken( 0, 0 ) );
			t = p->NextToken( 0, 0 );
			m_Override.flColor[2] = func_002290E0( t );
		}
		if ( func_00228C00( t, "GUI_CAPTION" ) == 1 )
		{
			m_Override.pCaption = new CGUIElementCaption( screen );
			m_Override.pCaption->Parse();
		}
		if ( func_00228C00( t, "GUI_INPUT" ) == 1 )
		{
			m_Override.pInput = new CGUIElementInput( screen );
			m_Override.pInput->Parse();
		}
		if ( func_00228C00( t, "WHILE_FOCUS" ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			m_Override.pCommands[0] = new CGUICommand;
			m_Override.pCommands[0]->Parse( p );
		}
		if ( func_00228C00( t, "GET_FOCUS" ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			m_Override.pCommands[1] = new CGUICommand;
			m_Override.pCommands[1]->Parse( p );
		}
		if ( func_00228C00( t, "LOSE_FOCUS" ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			m_Override.pCommands[2] = new CGUICommand;
			m_Override.pCommands[2]->Parse( p );
		}
		if ( func_00228C00( t, "ON_RENDER" ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			m_Override.pCommands[3] = new CGUICommand;
			m_Override.pCommands[3]->Parse( p );
		}
		t = p->NextToken( 0, 0 );
	}
	m_bParsed = 1;
#endif
}

float CGUIElementState :: GetValue( void )
{
	return m_flValue;
}

GUIOverride_t *CGUIElementState :: GetOverride( void )
{
	return &m_Override;
}
