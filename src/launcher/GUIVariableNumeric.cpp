/* Launcher (Gearbox, sem fonte): base das variáveis numéricas do GUI (CGUIVariableNumeric,
   "Numeric"): WRAP, LERP_TIME (em segundos, guardado em ms), LOOP e RUNNING da animação.
   Nome do arquivo inventado. */

#include "launcher/GUI.h"

extern "C" {
float func_002290E0(const char *s);	/* atof */
int func_00228C48(const char *a, const char *b);	/* 1 se iguais (sem caixa) */
unsigned long func_00229B88(void);	/* relógio */
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
};

const char *D_003029E4 = "Numeric";

CGUIVariableNumeric :: CGUIVariableNumeric()
{
	m_bWrap = 0;
	m_bRunning = 0;
	m_bLoop = 0;
	m_iLerpTime = 0;
	m_iStartTime = 0;
}

CGUIVariableNumeric :: ~CGUIVariableNumeric()
{
}

void CGUIVariableNumeric :: Start( void )
{
	m_iStartTime = func_00229B88();
}

void CGUIVariableNumeric :: Restart( void )
{
	m_iStartTime = func_00229B88();
	SetMin();
}

void CGUIVariableNumeric :: Stop( void )
{
	m_bRunning = 0;
}

void CGUIVariableNumeric :: Run( void )
{
	m_bRunning = 1;
}

int CGUIVariableNumeric :: IsType( const char *type )
{
	int r;
	if ( func_00228C48( D_003029E4, type ) == 1 )
		r = 1;
	else
		r = CGUIVariable::IsType( type );
	return r;
}

int CGUIVariableNumeric :: ParseAttribute( const char *token, void *parser )
{
	int r = 0;

	if ( func_00228C00( token, "WRAP" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		if ( func_00228C48( token, "TRUE" ) == 1 )
			m_bWrap = 1;
		else
			m_bWrap = 0;
	}
	if ( func_00228C00( token, "LERP_TIME" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		m_iLerpTime = (unsigned int)( func_002290E0( token ) * 1000.0f );
	}
	if ( func_00228C00( token, "LOOP" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		if ( func_00228C48( token, "TRUE" ) == 1 )
			m_bLoop = 1;
		else
			m_bLoop = 0;
	}
	if ( func_00228C00( token, "RUNNING" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		if ( func_00228C48( token, "TRUE" ) == 1 )
			m_bRunning = 1;
		else
			m_bRunning = 0;
	}
	if ( !r )
		r = CGUIVariable::ParseAttribute( token, parser );
	return r;
}

void CGUIVariableNumeric :: V5( void )
{
	if ( m_iLerpTime )
		Reset();
	else
		Restore();
}
