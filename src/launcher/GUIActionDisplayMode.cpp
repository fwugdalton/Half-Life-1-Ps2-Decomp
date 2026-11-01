/* Launcher (Gearbox, sem fonte): GUI_ACTION_DISPLAY_MODE NTSC|PAL. */

#include "launcher/GUI.h"

extern "C" {
void func_0023CE98(int pal);
}

class CGUIActionDisplayMode : public CGUIAction
{
public:
	CGUIActionDisplayMode();
	virtual ~CGUIActionDisplayMode();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );

	int m_iUnused;
};

CGUIActionDisplayMode :: CGUIActionDisplayMode()
{
}

CGUIActionDisplayMode :: ~CGUIActionDisplayMode()
{
}

void CGUIActionDisplayMode :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_DISPLAY_MODE" );
}

void CGUIActionDisplayMode :: Execute( CGUI *gui )
{
	char params[200] = "$param0";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *mode = func_00108148( tok, 0, 0 );
	if ( func_00228C00( mode, "NTSC" ) == 1 )
		func_0023CE98( 0 );
	if ( func_00228C00( mode, "PAL" ) == 1 )
		func_0023CE98( 1 );
}
