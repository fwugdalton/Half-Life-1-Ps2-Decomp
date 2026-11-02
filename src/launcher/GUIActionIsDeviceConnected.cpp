/* Launcher (Gearbox, sem fonte): GUI_ACTION_IS_DEVICE_CONNECTED <variável> <aparelho>.
   Grava na variável 1/0 se o aparelho (DUALSHOCK1, KEYBOARD, MOUSE, DUALSHOCK2, ANY) está
   conectado, ou com ALL_DEVICE_HASH os bits 1/2/4/8 de todos eles. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
char *func_00228FA0(int value, char *buf, int radix);	/* itoa */
extern int D_00334FB8;	/* Dualshock 1 conectado */
extern int D_00334FBC;	/* Dualshock 2 */
extern int D_00334FC0;	/* teclado */
extern int D_00334FC4;	/* mouse */
}

class CGUIActionIsDeviceConnected : public CGUIAction
{
public:
	CGUIActionIsDeviceConnected();
	virtual ~CGUIActionIsDeviceConnected();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionIsDeviceConnected :: CGUIActionIsDeviceConnected()
{
}

CGUIActionIsDeviceConnected :: ~CGUIActionIsDeviceConnected()
{
}

void CGUIActionIsDeviceConnected :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_IS_DEVICE_CONNECTED" );
}

void CGUIActionIsDeviceConnected :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	char buf[16];
	int result = 0;
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *name = func_00108148( tok, 0, 0 );
	CGUIVariable *v = func_00107EE0( gui->GetVariables(), name );
	name = func_00108148( tok, 0, 0 );

	if ( func_00228C00( name, "DUALSHOCK1" ) == 1 )
	{
		if ( D_00334FB8 == 1 )
			result = 1;
		else
			result = 0;
	}
	if ( func_00228C00( name, "KEYBOARD" ) == 1 )
	{
		if ( D_00334FC0 == 1 )
			result = 1;
	}
	if ( func_00228C00( name, "MOUSE" ) == 1 )
	{
		if ( D_00334FC4 == 1 )
			result = 1;
	}
	if ( func_00228C00( name, "DUALSHOCK2" ) == 1 )
	{
		if ( D_00334FBC == 1 )
			result = 1;
	}
	if ( func_00228C00( name, "ANY" ) == 1 )
	{
		if ( D_00334FB8 == 1 || D_00334FC0 == 1 || D_00334FC4 == 1 || D_00334FBC == 1 )
			result = 1;
	}
	if ( func_00228C00( name, "ALL_DEVICE_HASH" ) == 1 )
	{
		if ( D_00334FB8 == 1 )
			result |= 1;
		if ( D_00334FC0 == 1 )
			result |= 2;
		if ( D_00334FC4 == 1 )
			result |= 4;
		if ( D_00334FBC == 1 )
			result |= 8;
	}

	func_00228FA0( result, buf, 10 );
	v->SetString( buf );
}
