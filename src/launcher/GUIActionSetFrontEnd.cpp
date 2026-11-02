/* Launcher (Gearbox, sem fonte): GUI_ACTION_SET_FRONT_END <pak> <arquivo> <tela>. Passa os
   nomes para o menu principal (Launcher, 0x100358…0x1003C0); sem o terceiro parâmetro, a tela fica NULL. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
int func_00228C48(const char *a, const char *b);	/* 1 se iguais */
void func_00100358(const char *s);
void func_00100398(const char *s);
void func_001003C0(const char *s);
}

class CGUIActionSetFrontEnd : public CGUIAction
{
public:
	CGUIActionSetFrontEnd();
	virtual ~CGUIActionSetFrontEnd();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionSetFrontEnd :: CGUIActionSetFrontEnd()
{
}

CGUIActionSetFrontEnd :: ~CGUIActionSetFrontEnd()
{
}

void CGUIActionSetFrontEnd :: Init( void *tokenizer )
{
	m_szName = func_00228D30( "GUI_ACTION_SET_FRONT_END" );
	m_bInitialized = 1;
}

void CGUIActionSetFrontEnd :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1 $param2";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	func_00100358( func_00108148( tok, 0, 0 ) );
	func_00100398( func_00108148( tok, 0, 0 ) );

	char *screen = func_00108148( tok, 0, 0 );
	if ( !func_00228C48( screen, "$param2" ) )
		func_001003C0( screen );
	else
		func_001003C0( 0 );
}
