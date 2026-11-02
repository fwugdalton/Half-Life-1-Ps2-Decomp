/* Launcher (Gearbox, sem fonte): GUI_ACTION_REFRESH. Liga os dois indicadores do GUI
   (Set08 e Set04) para a tela ser redesenhada. */

#include "launcher/GUI.h"

class CGUIActionRefresh : public CGUIAction
{
public:
	CGUIActionRefresh();
	virtual ~CGUIActionRefresh();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionRefresh :: CGUIActionRefresh()
{
}

CGUIActionRefresh :: ~CGUIActionRefresh()
{
}

void CGUIActionRefresh :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_REFRESH" );
}

void CGUIActionRefresh :: Execute( CGUI *gui )
{
	gui->Set08( 1 );
	gui->Set04( 1 );
}
