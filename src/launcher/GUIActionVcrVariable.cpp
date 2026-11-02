/* Launcher (Gearbox, sem fonte): GUI_ACTION_VCR_VARIABLE <variável> PAUSE|PLAY|REWIND.
   Controla uma variável animada do GUI como um videocassete. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
extern const char *D_003029E4;	/* nome do tipo da variável animada */
void func_00106CE8(CGUIVariable *v);	/* rebobina */
void func_00106D28(CGUIVariable *v);	/* pausa */
void func_00106D30(CGUIVariable *v);	/* toca */
}

class CGUIActionVcrVariable : public CGUIAction
{
public:
	CGUIActionVcrVariable();
	virtual ~CGUIActionVcrVariable();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionVcrVariable :: CGUIActionVcrVariable()
{
}

CGUIActionVcrVariable :: ~CGUIActionVcrVariable()
{
}

void CGUIActionVcrVariable :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_VCR_VARIABLE" );
}

void CGUIActionVcrVariable :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	char name[100];
	char cmd[100];
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *t = func_00108148( tok, 0, 0 );
	func_00228CF0( name, t );
	t = func_00108148( tok, 0, 0 );
	func_00228CF0( cmd, t );

	CGUIVariable *v = func_00107EE0( gui->GetVariables(), name );
	if ( v->IsType( D_003029E4 ) == 1 )
	{
		if ( func_00228C00( cmd, "PAUSE" ) == 1 )
			func_00106D28( v );
		else if ( func_00228C00( cmd, "PLAY" ) == 1 )
			func_00106D30( v );
		else if ( func_00228C00( cmd, "REWIND" ) == 1 )
			func_00106CE8( v );
	}
}
