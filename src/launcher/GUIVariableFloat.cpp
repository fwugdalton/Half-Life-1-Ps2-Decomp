/* Launcher (Gearbox, sem fonte): variável float do GUI (CGUIVariableFloat, 0x98 bytes;
   INITIAL_VALUE, MAX_VALUE, MIN_VALUE). Como a inteira, mas com WRAP (passa do máximo para o
   mínimo e vice-versa) e a cvar só é usada enquanto existir. Nome do arquivo inventado. */

#include "launcher/GUI.h"

extern "C" {
#include "engine/cvar.h"
float func_002290E0(const char *s);	/* atof */
int func_00228C48(const char *a, const char *b);	/* 1 se iguais (sem caixa) */
unsigned long func_00229B88(void);	/* relógio */
unsigned long func_00229C38(unsigned long start, unsigned long now);	/* tempo passado */
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
};

const char *D_003029E8 = "Float";

class CGUIVariableFloat : public CGUIVariableNumeric
{
public:
	CGUIVariableFloat();
	virtual ~CGUIVariableFloat();
	float GetValue( void );
	virtual int IsType( const char *type );
	virtual int ParseAttribute( const char *token, void *parser );
	virtual void SetString( const char *value );
	virtual void Copy( CGUIVariable *from );
	virtual void Add( const char *value );
	virtual void AddAnim( const char *value );
	void SetValue( float value );
	void UpdateCvar( void );
	void ReadCvar( void );
	virtual void Reset( void );
	virtual void Restore( void );

	float m_fInitialValue;
	float m_fValue;
	float m_fMaxValue;
	float m_fMinValue;
	int m_bCvar;
	int m_iPad;
};

CGUIVariableFloat :: CGUIVariableFloat()
{
	m_bCvar = 1;
	m_fMaxValue = 0;
	m_fMinValue = 0;
	m_fInitialValue = 0;
}

CGUIVariableFloat :: ~CGUIVariableFloat()
{
}

float CGUIVariableFloat :: GetValue( void )
{
	if ( m_bRunning == 1 )
	{
		unsigned long now = func_00229B88();
		unsigned long elapsed = func_00229C38( m_iStartTime, now );
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
		SetValue( ( m_fMaxValue - m_fMinValue ) * t + m_fMinValue );
	}
	ReadCvar();
	return m_fValue;
}

int CGUIVariableFloat :: IsType( const char *type )
{
	int r;
	if ( func_00228C48( D_003029E8, type ) == 1 )
		r = 1;
	else
		r = CGUIVariableNumeric::IsType( type );
	return r;
}

int CGUIVariableFloat :: ParseAttribute( const char *token, void *parser )
{
	int r = 0;

	if ( func_00228C00( token, "INITIAL_VALUE" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		m_fInitialValue = func_002290E0( token );
	}
	if ( func_00228C00( token, "MAX_VALUE" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		m_fMaxValue = func_002290E0( token );
	}
	if ( func_00228C00( token, "MIN_VALUE" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		m_fMinValue = func_002290E0( token );
	}
	if ( !r )
		r = CGUIVariableNumeric::ParseAttribute( token, parser );
	return r;
}

void CGUIVariableFloat :: SetString( const char *value )
{
	SetValue( func_002290E0( value ) );
}

void CGUIVariableFloat :: Copy( CGUIVariable *from )
{
	if ( from->IsType( D_003029E8 ) == 1 )
	{
		float v = ( (CGUIVariableFloat *)from )->GetValue();
		SetValue( v );
	}
}

void CGUIVariableFloat :: Add( const char *value )
{
	SetValue( m_fMinValue );
}

void CGUIVariableFloat :: AddAnim( const char *value )
{
	float v = GetValue();
	SetValue( v + func_002290E0( value ) );
}

void CGUIVariableFloat :: SetValue( float value )
{
	m_fValue = value;
	if ( value < m_fMinValue )
	{
		if ( m_bWrap == 1 )
			m_fValue = m_fMaxValue;
		else
			m_fValue = m_fMinValue;
	}
	else if ( m_fMaxValue < value )
	{
		if ( m_bWrap == 1 )
			m_fValue = m_fMinValue;
		else
			m_fValue = m_fMaxValue;
	}
	UpdateCvar();
}

void CGUIVariableFloat :: UpdateCvar( void )
{
	if ( m_bCvar == 1 )
	{
		if ( Cvar_FindVar( m_szName ) )
			Cvar_SetValue( m_szName, m_fValue );
		else
			m_bCvar = 0;
	}
}

void CGUIVariableFloat :: ReadCvar( void )
{
	if ( m_bCvar == 1 )
	{
		cvar_t *c = Cvar_FindVar( m_szName );
		if ( c )
			m_fValue = c->value;
		else
			m_bCvar = 0;
	}
}

void CGUIVariableFloat :: Reset( void )
{
	m_fValue = m_fInitialValue = m_fMinValue;
}

void CGUIVariableFloat :: Restore( void )
{
	m_fValue = m_fInitialValue;
}
