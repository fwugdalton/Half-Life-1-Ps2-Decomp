/* Launcher (Gearbox, sem fonte): GUI_ACTION_APP_STRING <palavra> <palavra> ... Junta todos os
   parâmetros ($param0, $param1, ... até acabarem) e manda a linha para o console do jogo. */

#include "launcher/GUI.h"

extern "C" {
int func_00228EE0(char *dst, const char *fmt, ...);	/* sprintf */
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
char *func_00228D80(char *dst, const char *src);	/* strcat */
void Cbuf_AddText(char *text);
}

class CGUIActionAppString : public CGUIAction
{
public:
	CGUIActionAppString();
	virtual ~CGUIActionAppString();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionAppString :: CGUIActionAppString()
{
}

CGUIActionAppString :: ~CGUIActionAppString()
{
}

void CGUIActionAppString :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_APP_STRING" );
}

void CGUIActionAppString :: Execute( CGUI *gui )
{
	char cmd[200];
	char text[200];
	char param[16];
	int i = 0;
	int more;
	void *tok = gui->GetTokenizer();

	cmd[0] = 0;
	do
	{
		func_00228EE0( param, "$param%d", i );
		func_00228CF0( text, param );
		func_00108120( tok, text, -1 );
		char *t = func_00108148( tok, 0, 0 );
		if ( func_00228C00( t, param ) == 0 )
		{
			func_00228D80( cmd, t );
			func_00228D80( cmd, " " );
			more = 1;
		}
		else
			more = 0;
		i++;
	} while ( more == 1 );

	Cbuf_AddText( cmd );
	Cbuf_AddText( "\n" );
}
