/* Launcher (Gearbox, sem fonte): GUI_ACTION_PRINTF <texto>. A impressão foi tirada da versão
   final: só lê o parâmetro. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
}

class CGUIActionPrintf : public CGUIAction
{
public:
	CGUIActionPrintf();
	virtual ~CGUIActionPrintf();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionPrintf :: CGUIActionPrintf()
{
}

CGUIActionPrintf :: ~CGUIActionPrintf()
{
}

void CGUIActionPrintf :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_PRINTF" );
}

void CGUIActionPrintf :: Execute( CGUI *gui )
{
	char params[200] = "$param0";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	func_00108148( tok, 0, 0 );
}
