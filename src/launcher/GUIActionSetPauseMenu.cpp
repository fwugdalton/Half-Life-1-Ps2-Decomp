/* Launcher (Gearbox, sem fonte): GUI_ACTION_SET_PAUSE_MENU <pak> <arquivo> <tela>. Passa os
   nomes para o menu de pausa (Gbx/PauseMenu.cpp); sem o terceiro parâmetro, a tela fica NULL. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
int func_00228C48(const char *a, const char *b);	/* 1 se iguais */
void func_002365C0(const char *s);
void func_002365E8(const char *s);
void func_00236610(const char *s);
}

class CGUIActionSetPauseMenu : public CGUIAction
{
public:
	CGUIActionSetPauseMenu();
	virtual ~CGUIActionSetPauseMenu();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionSetPauseMenu :: CGUIActionSetPauseMenu()
{
}

CGUIActionSetPauseMenu :: ~CGUIActionSetPauseMenu()
{
}

void CGUIActionSetPauseMenu :: Init( void *tokenizer )
{
	m_szName = func_00228D30( "GUI_ACTION_SET_PAUSE_MENU" );
	m_bInitialized = 1;
}

void CGUIActionSetPauseMenu :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1 $param2";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	func_002365C0( func_00108148( tok, 0, 0 ) );
	func_002365E8( func_00108148( tok, 0, 0 ) );

	char *screen = func_00108148( tok, 0, 0 );
	if ( !func_00228C48( screen, "$param2" ) )
		func_00236610( screen );
	else
		func_00236610( 0 );
}
