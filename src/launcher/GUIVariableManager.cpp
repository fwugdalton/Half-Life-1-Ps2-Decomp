/* Launcher (Gearbox, sem fonte): lista das variáveis do GUI (CGUIVariableManager, 0x14 bytes,
   criada pelo CGUI). Lê os blocos GUI_VARIABLE_FLOAT/GUI_VARIABLE_STRING ... END dos scripts.
   Nome do arquivo tirado da string de alocação. */

#include "launcher/GUI.h"

extern "C" {
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
void Mem_Free(void *ptr);
unsigned long func_00229B88(void);	/* relógio */
const char *func_00106BA0(CGUIVariable *v);	/* nome da variável */
void func_00106CC0(CGUIVariable *v);
extern const char *D_003029E4;	/* nome do tipo da variável animada */
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
};

class CGUIVariableFloat : public CGUIVariable
{
public:
	CGUIVariableFloat();
	char m_pad2[0x98 - 0x68];
};

class CGUIVariableString : public CGUIVariable
{
public:
	CGUIVariableString();
	char m_pad2[0x168 - 0x68];
};

class CGUIVariableManager
{
public:
	CGUIVariableManager();
	~CGUIVariableManager();
	void Init( void );
	void Parse( const char *token, CGUIParser *parser );
	void Add( CGUIVariable *v );
	CGUIVariable *Get( const char *name );
	int Exists( const char *name );
	CGUIVariable *Find( const char *name );
	void Grow( void );

	int m_iTime;
	int m_bInitialized;
	int m_iMaxVariables;
	int m_iNumVariables;
	CGUIVariable **m_pVariables;
};

CGUIVariableManager :: CGUIVariableManager()
{
	m_bInitialized = 0;
}

CGUIVariableManager :: ~CGUIVariableManager()
{
	int i;

	m_bInitialized = 0;
	if ( m_pVariables )
	{
		for ( i = 0; i < m_iNumVariables; i++ )
		{
			if ( m_pVariables[i] )
				delete m_pVariables[i];
		}
		Mem_Free( m_pVariables );
	}
	m_iTime = func_00229B88();
	m_pVariables = 0;
}

void CGUIVariableManager :: Init( void )
{
	m_bInitialized = 1;
	m_iNumVariables = 0;
	m_pVariables = 0;
	m_iMaxVariables = 0;
}

void CGUIVariableManager :: Parse( const char *token, CGUIParser *parser )
{
	CGUIVariable *v = 0;
	char *t;

	if ( func_00228C00( "GUI_VARIABLE_FLOAT", token ) == 1 )
		v = new CGUIVariableFloat;
	if ( func_00228C00( "GUI_VARIABLE_STRING", token ) == 1 )
		v = new CGUIVariableString;
	if ( !v )
		return;

	t = parser->NextToken( 0, 0 );
	while ( t && func_00228C00( t, "END" ) != 1 )
	{
		v->ParseAttribute( t, parser );
		t = parser->NextToken( 0, 0 );
	}
	Add( v );
}

void CGUIVariableManager :: Add( CGUIVariable *v )
{
	if ( Find( func_00106BA0( v ) ) )
		return;
	if ( m_iNumVariables == m_iMaxVariables )
		Grow();
	if ( v->IsType( D_003029E4 ) == 1 )
		func_00106CC0( v );
	v->V5();
	m_pVariables[m_iNumVariables] = v;
	m_iNumVariables++;
}

CGUIVariable *CGUIVariableManager :: Get( const char *name )
{
	return Find( name );
}

int CGUIVariableManager :: Exists( const char *name )
{
	return Find( name ) != 0;
}

CGUIVariable *CGUIVariableManager :: Find( const char *name )
{
	int i;
	CGUIVariable *r = 0;

	for ( i = 0; i < m_iNumVariables; i++ )
	{
		if ( func_00228C00( func_00106BA0( m_pVariables[i] ), name ) == 1 )
		{
			r = m_pVariables[i];
			break;
		}
	}
	return r;
}

void CGUIVariableManager :: Grow( void )
{
	m_iMaxVariables += 10;
	m_pVariables = (CGUIVariable **)func_002260D0( m_pVariables, m_iMaxVariables * 4, "Launcher/GUIVariableManager.cpp", 192 );
}
