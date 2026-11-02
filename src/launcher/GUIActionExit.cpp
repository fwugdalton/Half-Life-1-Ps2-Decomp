/* Launcher (Gearbox, sem fonte): GUI_ACTION_EXIT. Liga o indicador Set04 do GUI (sai do
   menu). */

#include "launcher/GUI.h"

class CGUIActionExit : public CGUIAction
{
public:
	CGUIActionExit();
	virtual ~CGUIActionExit();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionExit :: CGUIActionExit()
{
}

CGUIActionExit :: ~CGUIActionExit()
{
}

void CGUIActionExit :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_EXIT" );
}

void CGUIActionExit :: Execute( CGUI *gui )
{
	gui->Set04( 1 );
}
