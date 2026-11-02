/* Launcher (Gearbox, sem fonte): GUI_ACTION_SET_FOCUS <elemento>. Põe o foco no elemento; se o
   parâmetro for uma variável do GUI, usa o texto dela (ou nenhum foco, se não for de texto). */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
extern const char *D_003029F0;
const char *func_00107A40(CGUIVariable *v);	/* texto da variável */
}

class CGUIActionSetFocus : public CGUIAction
{
public:
	CGUIActionSetFocus();
	virtual ~CGUIActionSetFocus();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionSetFocus :: CGUIActionSetFocus()
{
	m_bInitialized = 0;
}

CGUIActionSetFocus :: ~CGUIActionSetFocus()
{
}

void CGUIActionSetFocus :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_SET_FOCUS" );
}

void CGUIActionSetFocus :: Execute( CGUI *gui )
{
	char params[200] = "$param0";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *name = func_00108148( tok, 0, 0 );
	const char *focus;
	if ( func_00107F00( gui->GetVariables(), name ) == 1 )
	{
		CGUIVariable *v = func_00107EE0( gui->GetVariables(), name );
		if ( v->IsType( D_003029F0 ) == 1 )
			focus = func_00107A40( v );
		else
			focus = 0;
	}
	else
		focus = name;
	gui->SetFocus( focus );
}
