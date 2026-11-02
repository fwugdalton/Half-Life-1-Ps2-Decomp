/* Launcher (Gearbox, sem fonte): GUI_ACTION_SYSTEM_IDENTIFY_STATE Initialize|Run|Set <n>.
   Guarda o estado do save do sistema e o copia para a variável SystemSaveInitializeState. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
int func_002290C0(const char *s);			/* atoi */
int func_00228EE0(char *dst, const char *fmt, ...);	/* sprintf */
int func_0022DCC0(int state);				/* avança o estado do save do sistema */
}

class CGUIActionSystemIdentifyState : public CGUIAction
{
public:
	CGUIActionSystemIdentifyState();
	virtual ~CGUIActionSystemIdentifyState();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );

	void SetState( CGUI *gui, int state );

	int m_iState;
};

CGUIActionSystemIdentifyState :: CGUIActionSystemIdentifyState()
{
}

CGUIActionSystemIdentifyState :: ~CGUIActionSystemIdentifyState()
{
}

void CGUIActionSystemIdentifyState :: Init( void *tokenizer )
{
	m_iState = 0;
	m_szName = func_00228D30( "GUI_ACTION_SYSTEM_IDENTIFY_STATE" );
	m_bInitialized = 1;
}

void CGUIActionSystemIdentifyState :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *cmd = func_00108148( tok, 0, 0 );

	if ( func_00228C00( cmd, "Initialize" ) == 1 )
	{
		SetState( gui, 1 );
	}
	else if ( func_00228C00( cmd, "Run" ) == 1 )
	{
		SetState( gui, func_0022DCC0( m_iState ) );
	}
	else if ( func_00228C00( cmd, "Set" ) == 1 )
	{
		char *value = func_00108148( tok, 0, 0 );
		if ( value )
			SetState( gui, func_002290C0( value ) );
	}
}

void CGUIActionSystemIdentifyState :: SetState( CGUI *gui, int state )
{
	char buf[16];

	m_iState = state;
	func_00228EE0( buf, "%d", state );
	gui->SetVariable( "SystemSaveInitializeState", buf );
}
