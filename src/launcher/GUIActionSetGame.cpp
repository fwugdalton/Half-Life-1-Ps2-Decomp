/* Launcher (Gearbox, sem fonte): GUI_ACTION_SET_GAME <jogo>. Escolhe o jogo (Half-Life ou
   Decay) pelo nome, via func_002354D8. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
void func_002354D8(const char *game);
}

class CGUIActionSetGame : public CGUIAction
{
public:
	CGUIActionSetGame();
	virtual ~CGUIActionSetGame();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionSetGame :: CGUIActionSetGame()
{
}

CGUIActionSetGame :: ~CGUIActionSetGame()
{
}

void CGUIActionSetGame :: Init( void *tokenizer )
{
	m_szName = func_00228D30( "GUI_ACTION_SET_GAME" );
	m_bInitialized = 1;
}

void CGUIActionSetGame :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	func_002354D8( func_00108148( tok, 0, 0 ) );
}
