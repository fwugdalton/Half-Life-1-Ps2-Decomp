/* Launcher (Gearbox, sem fonte): GUI_ACTION_ADD_VARIABLE <variável> <valor>. Soma o valor a
   uma variável do GUI (métodos virtuais diferentes para os dois tipos que aceitam soma). */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
extern const char *D_003029E4;
extern const char *D_003029F0;
}

class CGUIActionAddVariable : public CGUIAction
{
public:
	CGUIActionAddVariable();
	virtual ~CGUIActionAddVariable();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionAddVariable :: CGUIActionAddVariable()
{
}

CGUIActionAddVariable :: ~CGUIActionAddVariable()
{
}

void CGUIActionAddVariable :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_ADD_VARIABLE" );
}

void CGUIActionAddVariable :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	char name[100];
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *t = func_00108148( tok, 0, 0 );
	func_00228CF0( name, t );
	char *value = func_00108148( tok, 0, 0 );

	CGUIVariable *v = func_00107EE0( gui->GetVariables(), name );
	if ( v->IsType( D_003029E4 ) == 1 )
		( (CGUIVariableAnimated *)v )->AddAnim( value );
	if ( v->IsType( D_003029F0 ) == 1 )
		v->Add( value );
}
