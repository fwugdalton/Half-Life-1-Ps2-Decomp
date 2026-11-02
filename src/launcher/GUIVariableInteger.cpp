/* Launcher (Gearbox, sem fonte): variável inteira do GUI (CGUIVariableInteger, 0x90 bytes;
   INITIAL_VALUE, MAX_VALUE, MIN_VALUE). O valor fica preso entre o mínimo e o máximo, anda
   do mínimo ao máximo em LERP_TIME quando RUNNING e é espelhado na cvar de mesmo nome.
   Nome do arquivo inventado. */

#include "launcher/GUI.h"

extern "C" {
#include "engine/cvar.h"
int func_002290C0(const char *s);	/* atoi */
int func_00228C48(const char *a, const char *b);	/* 1 se iguais (sem caixa) */
unsigned long func_00229B88(void);	/* relógio */
unsigned long func_00229C38(unsigned long start, unsigned long now);	/* tempo passado */
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
};

const char *D_003029EC = "Integer";

CGUIVariableInteger :: CGUIVariableInteger()
{
	m_iMaxValue = 0;
	m_iMinValue = 0;
	m_iInitialValue = 0;
}

CGUIVariableInteger :: ~CGUIVariableInteger()
{
}

int CGUIVariableInteger :: GetValue( void )
{
	if ( m_bRunning == 1 )
	{
		unsigned int now = func_00229B88();
		unsigned int elapsed = func_00229C38( m_iStartTime, now );
		if ( m_iLerpTime < elapsed )
		{
			if ( m_bLoop == 1 )
				m_iStartTime = now;
			else
			{
				m_bRunning = 0;
				elapsed = m_iLerpTime;
			}
		}
		float t = (float)elapsed / (float)m_iLerpTime;
		SetValue( (int)( (float)( m_iMaxValue - m_iMinValue ) * t + (float)m_iMinValue ) );
	}
	ReadCvar();
	return m_iValue;
}

int CGUIVariableInteger :: IsType( const char *type )
{
	int r;
	if ( func_00228C48( D_003029EC, type ) == 1 )
		r = 1;
	else
		r = CGUIVariableNumeric::IsType( type );
	return r;
}

int CGUIVariableInteger :: ParseAttribute( const char *token, void *parser )
{
	int r = 0;

	if ( func_00228C00( token, "INITIAL_VALUE" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		m_iInitialValue = func_002290C0( token );
	}
	if ( func_00228C00( token, "MAX_VALUE" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		m_iMaxValue = func_002290C0( token );
	}
	if ( func_00228C00( token, "MIN_VALUE" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		m_iMinValue = func_002290C0( token );
	}
	if ( !r )
		r = CGUIVariableNumeric::ParseAttribute( token, parser );
	return r;
}

void CGUIVariableInteger :: SetString( const char *value )
{
	SetValue( func_002290C0( value ) );
}

void CGUIVariableInteger :: Copy( CGUIVariable *from )
{
	if ( from->IsType( D_003029EC ) == 1 )
	{
		int v = ( (CGUIVariableInteger *)from )->GetValue();
		SetValue( v );
	}
}

void CGUIVariableInteger :: SetMin( void )
{
	SetValue( m_iMinValue );
}

void CGUIVariableInteger :: AddAnim( const char *value )
{
	int v = GetValue();
	SetValue( v + func_002290C0( value ) );
}

void CGUIVariableInteger :: SetValue( int value )
{
	m_iValue = value;
	if ( value < m_iMinValue )
		m_iValue = m_iMinValue;
	else if ( m_iMaxValue < value )
		m_iValue = m_iMaxValue;
	UpdateCvar();
}

void CGUIVariableInteger :: UpdateCvar( void )
{
	if ( Cvar_FindVar( m_szName ) )
		Cvar_SetValue( m_szName, (float)m_iValue );
}

void CGUIVariableInteger :: ReadCvar( void )
{
	cvar_t *c = Cvar_FindVar( m_szName );
	if ( c )
		m_iValue = (int)c->value;
}

void CGUIVariableInteger :: Reset( void )
{
	m_iValue = m_iInitialValue = m_iMinValue;
}

void CGUIVariableInteger :: Restore( void )
{
	m_iValue = m_iInitialValue;
}
