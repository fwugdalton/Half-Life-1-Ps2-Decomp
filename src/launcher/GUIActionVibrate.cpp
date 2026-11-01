/* Launcher (Gearbox, sem fonte): GUI_ACTION_VIBRATE. Parâmetros: variável float com a porta
   do controle, dois bytes e a duração em float; chama a vibração do pad. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
int func_002290C0(const char *s);		/* atoi */
float func_002290E0(const char *s);	/* atof */
void func_0021C288(int port, int a, int b, float time);
}

class CGUIActionVibrate : public CGUIAction
{
public:
	CGUIActionVibrate();
	virtual ~CGUIActionVibrate();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionVibrate :: CGUIActionVibrate()
{
}

CGUIActionVibrate :: ~CGUIActionVibrate()
{
}

void CGUIActionVibrate :: Init( void *tokenizer )
{
	m_szName = func_00228D30( "GUI_ACTION_VIBRATE" );
	m_bInitialized = 1;
}

void CGUIActionVibrate :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1 $param2 $param3";
	void *tok = gui->GetTokenizer();
	unsigned char a, b;

	func_00108120( tok, params, -1 );
	char *name = func_00108148( tok, 0, 0 );
	CGUIVariable *v = func_00107EE0( gui->GetVariables(), name );
	unsigned int port;
	if ( v->IsType( D_003029E8 ) == 1 )
		port = (unsigned int)func_00107000( v );
	else
		port = 0;
	a = func_002290C0( func_00108148( tok, 0, 0 ) );
	b = func_002290C0( func_00108148( tok, 0, 0 ) );
	func_0021C288( port, a, b, func_002290E0( func_00108148( tok, 0, 0 ) ) );
}
