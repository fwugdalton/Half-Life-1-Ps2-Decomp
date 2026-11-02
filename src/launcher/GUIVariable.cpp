/* Launcher (Gearbox, sem fonte): classe base das variáveis do GUI (CGUIVariable, "Variable",
   atributo NAME). Nome do arquivo inventado. */

#include "launcher/GUI.h"

extern "C" {
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
int func_00228C48(const char *a, const char *b);	/* 1 se iguais (sem caixa) */
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
};

const char *D_003029E0 = "Variable";

CGUIVariable :: CGUIVariable()
{
	m_szName[0] = 0;
}

CGUIVariable :: ~CGUIVariable()
{
}

char *CGUIVariable :: GetName( void )
{
	return m_szName;
}

int CGUIVariable :: IsType( const char *type )
{
	return func_00228C48( D_003029E0, type ) == 1;
}

int CGUIVariable :: ParseAttribute( const char *token, void *parser )
{
	int r = 0;

	if ( func_00228C00( token, "NAME" ) == 1 )
	{
		r = 1;
		func_00228CF0( m_szName, ( (CGUIParser *)parser )->NextToken( 0, 0 ) );
	}
	return r;
}
