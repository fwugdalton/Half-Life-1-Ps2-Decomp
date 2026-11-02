/* Launcher (Gearbox, sem fonte): elemento genérico de tela (CGUIGeneric, 0xA0 bytes, bloco
   GUI_GENERIC de uma tela): retângulo, textura com BLEND_FUNC/TEX_ENV, cor, comandos de foco
   (WHILE_FOCUS, GET_FOCUS, LOSE_FOCUS, ON_RENDER) e as partes GUI_CAPTION, GUI_DEPENDENCY e
   GUI_INPUT. Nome do arquivo inventado (as partes são Launcher/GUIElement*.cpp). */

#include "launcher/GUI.h"

extern "C" {
int func_002290C0(const char *s);	/* atoi */
float func_002290E0(const char *s);	/* atof */
void glBlendFunc(int sfactor, int dfactor);
void glTexEnvf(int target, int pname, float param);
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
	int GetHeight( const char *name );
	void Bind( int texnum );
};

class CGUICommand
{
public:
	CGUICommand();
	~CGUICommand();
	void Parse( CGUIParser *parser );
	void Execute( CGUI *gui );
	char m_pad[0x24];
};

class CGUIScreen
{
public:
	int IsFocus( CGUIWidget *w );
	CGUI *m_pGUI;
};

/* o que um GUI_DEPENDENCY satisfeito troca no elemento (Launcher/GUIElementDependency.cpp) */
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
	void Draw( int focus, int x, int y );
};

class CGUIElementDependency : public CGUIWidgetPart
{
public:
	CGUIElementDependency( CGUIScreen *screen );
	virtual void Parse( void );
	char m_pad2[0x8C - 0xC];
	GUIOverride_t *GetOverride( void );
};

class CGUIElementInput : public CGUIWidgetPart
{
public:
	CGUIElementInput( CGUIScreen *screen );
	virtual void Parse( void );
	char m_pad2[0x18 - 0xC];
};

#define SCREEN ( (CGUIScreen *)m_pGUI )

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
	int GetBlendFunc( const char *s );
	int GetTexEnv( const char *s );
	void ApplyDependencies( void );
	static void SetCommand( CGUICommand *cmd, CGUICommand **pp, int *own );
	void ApplyOverride( GUIOverride_t *ov );

	float m_flRect[4];			/* 0x38 SCREEN_RECT */
	int m_bTexture;				/* 0x48 */
	int m_iTexture;				/* 0x4C */
	int m_iBlendSrc;			/* 0x50 */
	int m_iBlendDst;			/* 0x54 */
	int m_iTexEnv;				/* 0x58 */
	float m_flTexRect[4];		/* 0x5C */
	float m_flTexWidth;			/* 0x6C */
	float m_flTexHeight;		/* 0x70 */
	float m_flColor[3];			/* 0x74 COLOR_MODULATE */
	int m_bOwnCommand[4];		/* 0x80 */
	CGUICommand *m_pCommand[4];	/* 0x90 WHILE_FOCUS, GET_FOCUS, LOSE_FOCUS, ON_RENDER */
};

CGUIGeneric :: CGUIGeneric( CGUIScreen *screen ) : CGUIWidget( (CGUI *)screen )
{
}

CGUIGeneric :: ~CGUIGeneric()
{
	if ( m_pCommand[0] && m_bOwnCommand[0] == 1 )
	{
		m_bOwnCommand[0] = 0;
		delete m_pCommand[0];
		m_pCommand[0] = 0;
	}
	if ( m_pCommand[1] && m_bOwnCommand[1] == 1 )
	{
		m_bOwnCommand[1] = 0;
		delete m_pCommand[1];
		m_pCommand[1] = 0;
	}
	if ( m_pCommand[2] && m_bOwnCommand[2] == 1 )
	{
		m_bOwnCommand[2] = 0;
		delete m_pCommand[2];
		m_pCommand[2] = 0;
	}
	if ( m_pCommand[3] && m_bOwnCommand[3] == 1 )
	{
		m_bOwnCommand[3] = 0;
		delete m_pCommand[3];
		m_pCommand[3] = 0;
	}
}

int CGUIGeneric :: Parse( void )
{
	int r;
	int f;
	char *t;
	CGUIParser *p;

	if ( !m_i4 )
	{
		p = (CGUIParser *)SCREEN->m_pGUI->GetTokenizer();
		m_bTexture = 0;
		m_p18 = 0;
		m_flTexRect[0] = 0;
		m_flTexRect[1] = 0;
		m_flTexRect[2] = 0;
		m_flTexRect[3] = 0;
		m_flTexWidth = 0;
		m_flTexHeight = 0;
		m_bOwnCommand[0] = 0;
		m_bOwnCommand[1] = 0;
		m_bOwnCommand[2] = 0;
		m_bOwnCommand[3] = 0;
		m_pCommand[0] = 0;
		m_pCommand[1] = 0;
		m_pCommand[2] = 0;
		m_pCommand[3] = 0;
		m_iBlendSrc = 1;
		m_iBlendDst = 1;
		m_flColor[0] = 1.0f;
		m_iTexEnv = 0x1E01;
		m_flColor[1] = 1.0f;
		m_flColor[2] = 1.0f;
		t = p->NextToken( 0, 0 );
		while ( t && func_00228C00( t, "END" ) != 1 )
		{
			if ( func_00228C00( t, "NAME" ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_szName = func_00228D30( t );
			}
			if ( func_00228C00( t, "Z_PLANE" ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_i8 = func_002290C0( t );
			}
			if ( ( f = func_00228C00( t, "GUI_CAPTION" ) ) == 1 )
			{
				m_p18 = new CGUIElementCaption( SCREEN );
				m_p18->Parse();
				m_bOwn18 = f;
			}
			if ( func_00228C00( t, "COLOR_MODULATE" ) == 1 )
			{
				m_flColor[0] = func_002290E0( p->NextToken( 0, 0 ) );
				m_flColor[1] = func_002290E0( p->NextToken( 0, 0 ) );
				t = p->NextToken( 0, 0 );
				m_flColor[2] = func_002290E0( t );
			}
			if ( ( f = func_00228C00( t, "WHILE_FOCUS" ) ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_bOwnCommand[0] = f;
				m_pCommand[0] = new CGUICommand;
				m_pCommand[0]->Parse( (CGUIParser *)SCREEN->m_pGUI->GetTokenizer() );
			}
			if ( ( f = func_00228C00( t, "GET_FOCUS" ) ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_bOwnCommand[1] = f;
				m_pCommand[1] = new CGUICommand;
				m_pCommand[1]->Parse( (CGUIParser *)SCREEN->m_pGUI->GetTokenizer() );
			}
			if ( ( f = func_00228C00( t, "LOSE_FOCUS" ) ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_bOwnCommand[2] = f;
				m_pCommand[2] = new CGUICommand;
				m_pCommand[2]->Parse( (CGUIParser *)SCREEN->m_pGUI->GetTokenizer() );
			}
			if ( ( f = func_00228C00( t, "ON_RENDER" ) ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_bOwnCommand[3] = f;
				m_pCommand[3] = new CGUICommand;
				m_pCommand[3]->Parse( (CGUIParser *)SCREEN->m_pGUI->GetTokenizer() );
			}
			if ( func_00228C00( t, "SCREEN_RECT" ) == 1 )
				p->ReadFloat4( &m_flRect[0], &m_flRect[1], &m_flRect[2], &m_flRect[3] );
			if ( func_00228C00( t, "TEXTURE" ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				CGUIRenderer *rd = (CGUIRenderer *)SCREEN->m_pGUI->GetRenderer();
				m_iTexture = rd->GetTexture( t );
				m_flTexWidth = rd->GetWidth( t );
				m_flTexHeight = rd->GetHeight( t );
				m_bTexture = 1;
			}
			if ( func_00228C00( t, "BLEND_FUNC" ) == 1 )
			{
				m_iBlendSrc = GetBlendFunc( p->NextToken( 0, 0 ) );
				t = p->NextToken( 0, 0 );
				m_iBlendDst = GetBlendFunc( t );
			}
			if ( func_00228C00( t, "TEX_ENV" ) == 1 )
			{
				t = p->NextToken( 0, 0 );
				m_iTexEnv = GetTexEnv( t );
			}
			if ( func_00228C00( t, "TEXTURE_RECT" ) == 1 )
				p->ReadFloat4( &m_flTexRect[0], &m_flTexRect[1], &m_flTexRect[2], &m_flTexRect[3] );
			if ( func_00228C00( t, "GUI_DEPENDENCY" ) == 1 )
			{
				m_pActions[m_i1C] = new CGUIElementDependency( SCREEN );
				m_pActions[m_i1C]->Parse();
				m_i1C++;
			}
			if ( ( f = func_00228C00( t, "GUI_INPUT" ) ) == 1 )
			{
				m_pKeys = new CGUIElementInput( SCREEN );
				m_pKeys->Parse();
				m_bOwnKeys = f;
			}
			t = p->NextToken( 0, 0 );
		}
		m_i4 = 1;
		r = 1;
	}
	else
		r = 0;
	return r;
}

void CGUIGeneric :: V2( void )
{
	ApplyDependencies();
	if ( SCREEN->IsFocus( this ) == 1 && m_pCommand[0] )
		m_pCommand[0]->Execute( SCREEN->m_pGUI );
	if ( m_pCommand[3] )
		m_pCommand[3]->Execute( SCREEN->m_pGUI );
	ApplyDependencies();
	SCREEN->m_pGUI->SetColor( m_flColor[0], m_flColor[1], m_flColor[2] );
	if ( m_bTexture == 1 )
	{
		glBlendFunc( m_iBlendSrc, m_iBlendDst );
		glTexEnvf( 0x2300, 0x2200, m_iTexEnv );
		( (CGUIRenderer *)SCREEN->m_pGUI->GetRenderer() )->Bind( m_iTexture );
		glBegin( 6 );
		glTexCoord2f( m_flTexRect[0] / m_flTexWidth, m_flTexRect[1] / m_flTexHeight );
		glVertex2f( m_flRect[0], m_flRect[1] );
		glTexCoord2f( m_flTexRect[2] / m_flTexWidth, m_flTexRect[1] / m_flTexHeight );
		glVertex2f( m_flRect[2], m_flRect[1] );
		glTexCoord2f( m_flTexRect[2] / m_flTexWidth, m_flTexRect[3] / m_flTexHeight );
		glVertex2f( m_flRect[2], m_flRect[3] );
		glTexCoord2f( m_flTexRect[0] / m_flTexWidth, m_flTexRect[3] / m_flTexHeight );
		glVertex2f( m_flRect[0], m_flRect[3] );
		glEnd();
	}
	if ( m_p18 )
		( (CGUIElementCaption *)m_p18 )->Draw( SCREEN->IsFocus( this ), (int)m_flRect[0], (int)m_flRect[1] );
}

void CGUIGeneric :: Activate( void )
{
	ApplyDependencies();
	if ( m_pCommand[1] )
		m_pCommand[1]->Execute( SCREEN->m_pGUI );
}

void CGUIGeneric :: Deactivate( void )
{
	ApplyDependencies();
	if ( m_pCommand[2] )
		m_pCommand[2]->Execute( SCREEN->m_pGUI );
}

void CGUIGeneric :: CopyAttribute( const char *attr, CGUIWidget *from )
{
	if ( func_00228C00( attr, "SCREEN_RECT" ) == 1 )
	{
		m_flRect[0] = ( (CGUIGeneric *)from )->m_flRect[0];
		m_flRect[1] = ( (CGUIGeneric *)from )->m_flRect[1];
		m_flRect[2] = ( (CGUIGeneric *)from )->m_flRect[2];
		m_flRect[3] = ( (CGUIGeneric *)from )->m_flRect[3];
	}
}

int CGUIGeneric :: GetBlendFunc( const char *s )
{
	int r = -1;

	if ( func_00228C00( "GL_ONE", s ) == 1 )
		r = 1;
	if ( func_00228C00( "GL_SRC_ALPHA", s ) == 1 )
		r = 0x302;
	if ( func_00228C00( "GL_ONE_MINUS_SRC_ALPHA", s ) == 1 )
		r = 0x303;
	if ( func_00228C00( "GL_DST_COLOR", s ) == 1 )
		r = 0x306;
	if ( func_00228C00( "GL_SRC_COLOR", s ) == 1 )
		r = 0x300;
	if ( func_00228C00( "GL_ZERO", s ) == 1 )
		r = 0;
	return r;
}

int CGUIGeneric :: GetTexEnv( const char *s )
{
	int r = -1;

	if ( func_00228C00( "GL_MODULATE", s ) == 1 )
		r = 0x2100;
	if ( func_00228C00( "GL_REPLACE", s ) == 1 )
		r = 0x1E01;
	if ( func_00228C00( "GL_ALPHA", s ) == 1 )
		r = 0x303;
	return r;
}

void CGUIGeneric :: ApplyDependencies( void )
{
	int i;

	for ( i = 0; i < 4; i++ )
	{
		if ( m_pActions[i] )
			ApplyOverride( ( (CGUIElementDependency *)m_pActions[i] )->GetOverride() );
		else
			break;
	}
}

void CGUIGeneric :: SetCommand( CGUICommand *cmd, CGUICommand **pp, int *own )
{
	if ( cmd )
	{
		if ( *pp && *own == 1 )
		{
			*own = 0;
			delete *pp;
			*pp = 0;
		}
		*pp = cmd;
	}
}

void CGUIGeneric :: ApplyOverride( GUIOverride_t *ov )
{
	if ( ov->bZPlane )
		m_i8 = ov->iZPlane;
	if ( ov->bRect )
	{
		m_flRect[0] = ov->flRect[0];
		m_flRect[1] = ov->flRect[1];
		m_flRect[2] = ov->flRect[2];
		m_flRect[3] = ov->flRect[3];
	}
	if ( ov->bTexture )
	{
		m_bTexture = 1;
		m_iTexture = ov->iTexture;
		m_flTexHeight = ov->flTexHeight;
		m_flTexWidth = ov->flTexWidth;
	}
	if ( ov->bTexRect )
	{
		m_flTexRect[0] = ov->flTexRect[0];
		m_flTexRect[1] = ov->flTexRect[1];
		m_flTexRect[2] = ov->flTexRect[2];
		m_flTexRect[3] = ov->flTexRect[3];
	}
	if ( ov->bColor )
	{
		m_flColor[0] = ov->flColor[0];
		m_flColor[1] = ov->flColor[1];
		m_flColor[2] = ov->flColor[2];
	}
	if ( ov->pCaption )
	{
		if ( m_p18 && m_bOwn18 == 1 )
		{
			m_bOwn18 = 0;
			delete m_p18;
			m_p18 = 0;
		}
		m_p18 = ov->pCaption;
	}
	if ( ov->pInput )
	{
		if ( m_pKeys && m_bOwnKeys == 1 )
		{
			m_bOwnKeys = 0;
			delete m_pKeys;
			m_pKeys = 0;
		}
		m_pKeys = ov->pInput;
	}
	SetCommand( ov->pCommands[0], &m_pCommand[0], &m_bOwnCommand[0] );
	SetCommand( ov->pCommands[1], &m_pCommand[1], &m_bOwnCommand[1] );
	SetCommand( ov->pCommands[2], &m_pCommand[2], &m_bOwnCommand[2] );
	SetCommand( ov->pCommands[3], &m_pCommand[3], &m_bOwnCommand[3] );
}
