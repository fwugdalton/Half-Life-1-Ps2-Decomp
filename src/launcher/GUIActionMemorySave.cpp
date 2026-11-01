/* Launcher (Gearbox, sem fonte): GUI_ACTION_MEMORY_SAVE CreateMemorySave|DoesMemorySaveExist
   <nome>: grava o jogo num slot ou põe MemorySaveExists em 1.0/0.0. */

#include "launcher/GUI.h"

extern "C" {
int func_00228C48(const char *a, const char *b);	/* 1 se iguais */
int func_00228EE0(char *buf, const char *fmt, ...);	/* sprintf */
int sprintf(char *buf, const char *fmt, ...);
char *func_0022E958(void);
void func_00299420(char *pszComment);
int SaveGameSlot(char *pSaveName, char *pSaveComment);
int func_0022B6D0(const char *name);
}

class CGUIActionMemorySave : public CGUIAction
{
public:
	CGUIActionMemorySave();
	virtual ~CGUIActionMemorySave();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionMemorySave :: CGUIActionMemorySave()
{
}

CGUIActionMemorySave :: ~CGUIActionMemorySave()
{
}

void CGUIActionMemorySave :: Init( void *tokenizer )
{
	m_szName = func_00228D30( "GUI_ACTION_MEMORY_SAVE" );
	m_bInitialized = 1;
}

void CGUIActionMemorySave :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	char path[256];
	char comment[80];
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *cmd = func_00108148( tok, 0, 0 );
	if ( func_00228C48( "CreateMemorySave", cmd ) == 1 )
	{
		cmd = func_00108148( tok, 0, 0 );
		sprintf( path, "%s%s", func_0022E958(), cmd );
		func_00299420( comment );
		SaveGameSlot( cmd, comment );
	}
	if ( func_00228C48( "DoesMemorySaveExist", cmd ) == 1 )
	{
		cmd = func_00108148( tok, 0, 0 );
		func_00228EE0( path, "%s%s", func_0022E958(), cmd );
		if ( func_0022B6D0( path ) == 1 )
			gui->SetVariable( "MemorySaveExists", "1.0" );
		else
			gui->SetVariable( "MemorySaveExists", "0.0" );
	}
}
