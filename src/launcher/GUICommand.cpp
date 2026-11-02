/* Launcher (Gearbox, sem fonte): uma linha GUI_COMMAND das ações definidas no script
   (CGUICommand, 0x24 bytes). Formato: [EQUALS|NOTEQUALS|GREATERTHAN|LESSTHAN a b] ação
   parâmetros... END. Os parâmetros viram os #defines $param0, $param1... enquanto a ação roda.
   Nome do arquivo tirado da string de alocação. */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
char *func_00228BB0(const char *s);	/* strdup */
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
int func_00228EE0(char *buf, const char *fmt, ...);	/* sprintf */
float func_002290E0(const char *s);	/* atof */
void *Mem_DbgMalloc(int size, const char *file, int line);
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
void Mem_Free(void *ptr);
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
	void PushDefines( void );
	void PopDefines( void );
	void AddDefine( const char *name, const char *value );
};

class CGUIActionTable
{
public:
	CGUIAction *Find( const char *name );
};

class CGUIVariableManager
{
public:
	CGUIVariable *Get( const char *name );
};

class CGUICommand
{
public:
	CGUICommand();
	~CGUICommand();
	void Parse( CGUIParser *parser );
	CGUICommand( const CGUICommand &other );
	void Execute( CGUI *gui );
	char *GetParam( int i );
	float Evaluate( CGUI *gui, const char *s );

	int m_bInitialized;
	char *m_szAction;
	char *m_szArg1;
	char *m_szArg2;
	int m_iCondition;	/* 1 EQUALS, 2 NOTEQUALS, 3 GREATERTHAN, 4 LESSTHAN */
	int m_iNumParams;
	int m_iMaxParams;
	char **m_pParams;
	int m_i20;
};

CGUICommand :: CGUICommand()
{
	m_bInitialized = 0;
}

CGUICommand :: ~CGUICommand()
{
	int i;

	for ( i = 0; i < m_iNumParams; i++ )
		m_pParams[i] = 0;
	Mem_Free( m_pParams );
	m_pParams = 0;
	m_szAction = 0;
	m_szArg1 = 0;
	m_szArg2 = 0;
}

void CGUICommand :: Parse( CGUIParser *parser )
{
	char *t;
	int isCond;

	m_bInitialized = 1;
	m_szAction = 0;
	m_szArg1 = 0;
	m_szArg2 = 0;
	m_iCondition = 0;
	m_iNumParams = 0;
	m_iMaxParams = 0;
	m_pParams = 0;

	t = parser->NextToken( 0, 0 );
	while ( t && ( isCond = 0, func_00228C00( t, "END" ) != 1 ) )
	{
		if ( func_00228C00( t, "EQUALS" ) == 1 )
		{
			m_iCondition = 1;
			isCond = 1;
		}
		if ( func_00228C00( t, "NOTEQUALS" ) == 1 )
		{
			m_iCondition = 2;
			isCond = 1;
		}
		if ( func_00228C00( t, "GREATERTHAN" ) == 1 )
		{
			m_iCondition = 3;
			isCond = 1;
		}
		if ( func_00228C00( t, "LESSTHAN" ) == 1 )
		{
			m_iCondition = 4;
			isCond = 1;
		}
		if ( isCond == 1 )
		{
			m_szArg1 = func_00228BB0( parser->NextToken( 0, 0 ) );
			t = parser->NextToken( 0, 0 );
			m_szArg2 = func_00228BB0( t );
		}
		if ( !isCond )
		{
			if ( !m_szAction )
				m_szAction = func_00228BB0( t );
			else
			{
				if ( m_iNumParams == m_iMaxParams )
				{
					m_iMaxParams = m_iNumParams + 4;
					m_pParams = (char **)func_002260D0( m_pParams, m_iMaxParams * 4, "Launcher/GUICommand.cpp", 137 );
				}
				m_pParams[m_iNumParams] = func_00228BB0( t );
				m_iNumParams++;
			}
		}
		t = parser->NextToken( 0, 0 );
	}
}

CGUICommand :: CGUICommand( const CGUICommand &other )
{
	int i;

	m_bInitialized = other.m_bInitialized;
	if ( other.m_szAction )
		m_szAction = func_00228BB0( other.m_szAction );
	else
		m_szAction = 0;
	if ( other.m_szArg1 )
		m_szArg1 = func_00228BB0( other.m_szArg1 );
	else
		m_szArg1 = 0;
	if ( other.m_szArg2 )
		m_szArg2 = func_00228BB0( other.m_szArg2 );
	else
		m_szArg2 = 0;
	m_pParams = (char **)Mem_DbgMalloc( other.m_iNumParams * 4, "Launcher/GUICommand.cpp", 191 );
	m_iMaxParams = m_iNumParams = other.m_iNumParams;
	for ( i = 0; i < m_iNumParams; i++ )
		m_pParams[i] = func_00228BB0( other.m_pParams[i] );
	m_i20 = other.m_i20;
}

void CGUICommand :: Execute( CGUI *gui )
{
	int ok = 1;
	int i;

	if ( m_iCondition )
	{
		float a = Evaluate( gui, m_szArg1 );
		float b = Evaluate( gui, m_szArg2 );
		switch ( m_iCondition )
		{
		case 1:
			if ( !( a == b ) )
				ok = 0;
			break;
		case 2:
			if ( a == b )
				ok = 0;
			break;
		case 3:
			if ( !( b < a ) )
				ok = 0;
			break;
		case 4:
			if ( !( a < b ) )
				ok = 0;
			break;
		}
	}

	if ( ok == 1 )
	{
		CGUIParser *p = (CGUIParser *)gui->GetTokenizer();
		p->PushDefines();
		for ( i = 0; i < m_iNumParams; i++ )
		{
			char name[100];
			char value[100];
			func_00228EE0( name, "$param%d", i );
			func_00228CF0( value, m_pParams[i] );
			p->AddDefine( name, value );
		}
		if ( m_szAction )
			( (CGUIActionTable *)gui->GetActions() )->Find( m_szAction )->Execute( gui );
		p->PopDefines();
	}
}

char *CGUICommand :: GetParam( int i )
{
	return m_pParams[i];
}

/* NON_MATCHING: o original não iça o 9 do '\t' para fora do laço e o reusa no teste do dígito
   (sltu 9 < c com xori); o nosso faz sltiu c < 10, e os registradores mudam */
float CGUICommand :: Evaluate( CGUI *gui, const char *s )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUICommand", Evaluate__11CGUICommandP4CGUIPCc);
#else
	int isNum = 0;
	int minus = 0;
	const char *c = s;

	while ( *c )
	{
		if ( *c == '\n' || *c == '\r' || *c == '\t' || *c == ' ' )
		{
			c++;
			continue;
		}
		if ( *c != '-' )
		{
			isNum = (unsigned char)( *c - '0' ) <= 9;
			break;
		}
		if ( minus == 1 )
			break;
		minus = 1;
		c++;
	}

	if ( isNum )
		return func_002290E0( s );

	CGUIVariable *v = ( (CGUIVariableManager *)gui->GetVariables() )->Get( s );
	if ( v->IsType( D_003029E8 ) == 1 )
		return ( (CGUIVariableFloat *)v )->GetValue();
	return 0.0f;
#endif
}
