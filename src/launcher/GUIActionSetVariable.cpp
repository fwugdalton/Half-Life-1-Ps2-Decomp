/* Launcher (Gearbox, sem fonte): GUI_ACTION_SET_VARIABLE <variável> <valor>. Grava o valor
   (texto) na variável do GUI; "NULL" vira a string vazia. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
int func_00228C48(const char *a, const char *b);	/* 1 se iguais */
}

class CGUIActionSetVariable : public CGUIAction
{
public:
	CGUIActionSetVariable();
	virtual ~CGUIActionSetVariable();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionSetVariable :: CGUIActionSetVariable()
{
}

CGUIActionSetVariable :: ~CGUIActionSetVariable()
{
}

void CGUIActionSetVariable :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_SET_VARIABLE" );
}

void CGUIActionSetVariable :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	char name[100];
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *t = func_00108148( tok, 0, 0 );
	func_00228CF0( name, t );
	CGUIVariable *v = func_00107EE0( gui->GetVariables(), name );
	char *value = func_00108148( tok, 0, 0 );
	if ( func_00228C48( value, "NULL" ) == 1 )
		v->SetString( "" );
	else
		v->SetString( value );
}
