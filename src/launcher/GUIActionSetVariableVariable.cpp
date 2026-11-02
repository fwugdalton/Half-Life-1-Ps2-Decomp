/* Launcher (Gearbox, sem fonte): GUI_ACTION_SET_VARIABLE_VARIABLE <destino> <origem>. Copia
   uma variável do GUI para outra; se a origem não for variável do GUI, usa a cvar de mesmo
   nome (quando não vazia). */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
char *Cvar_VariableString(const char *name);
}

class CGUIActionSetVariableVariable : public CGUIAction
{
public:
	CGUIActionSetVariableVariable();
	virtual ~CGUIActionSetVariableVariable();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionSetVariableVariable :: CGUIActionSetVariableVariable()
{
}

CGUIActionSetVariableVariable :: ~CGUIActionSetVariableVariable()
{
}

void CGUIActionSetVariableVariable :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_SET_VARIABLE_VARIABLE" );
}

void CGUIActionSetVariableVariable :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *name = func_00108148( tok, 0, 0 );
	CGUIVariable *dst = func_00107EE0( gui->GetVariables(), name );
	name = func_00108148( tok, 0, 0 );
	if ( func_00107F00( gui->GetVariables(), name ) == 1 )
	{
		CGUIVariable *src = func_00107EE0( gui->GetVariables(), name );
		dst->Copy( src );
	}
	else
	{
		char *s = Cvar_VariableString( name );
		if ( *s )
			dst->SetString( s );
	}
}
