/* Launcher (Gearbox, sem fonte): variável de texto do GUI (CGUIVariableString, 0x168 bytes,
   bloco GUI_VARIABLE_STRING com o atributo VALUE). Nome do arquivo inventado. */

#include "launcher/GUI.h"

extern "C" {
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
char *func_00228D80(char *dst, const char *src);	/* strcat */
int func_00228C48(const char *a, const char *b);	/* 1 se iguais (sem caixa) */
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
};

const char *D_003029F0 = "String";

CGUIVariableString :: CGUIVariableString()
{
	m_szValue[0] = 0;
}

CGUIVariableString :: ~CGUIVariableString()
{
}

char *CGUIVariableString :: GetValue( void )
{
	return m_szValue;
}

int CGUIVariableString :: IsType( const char *type )
{
	int r;
	if ( func_00228C48( D_003029F0, type ) == 1 )
		r = 1;
	else
		r = CGUIVariable::IsType( type );
	return r;
}

int CGUIVariableString :: ParseAttribute( const char *token, void *parser )
{
	int r = 0;

	if ( func_00228C00( token, "VALUE" ) == 1 )
	{
		r = 1;
		token = ( (CGUIParser *)parser )->NextToken( 0, 0 );
		SetString( token );
	}
	if ( !r )
		r = CGUIVariable::ParseAttribute( token, parser );
	return r;
}

void CGUIVariableString :: SetString( const char *value )
{
	func_00228CF0( m_szValue, value );
}

void CGUIVariableString :: Copy( CGUIVariable *from )
{
	if ( from->IsType( D_003029F0 ) == 1 )
	{
		char *v = ( (CGUIVariableString *)from )->GetValue();
		SetString( v );
	}
}

void CGUIVariableString :: Add( const char *value )
{
	func_00228D80( m_szValue, value );
}

void CGUIVariableString :: V5( void )
{
}
