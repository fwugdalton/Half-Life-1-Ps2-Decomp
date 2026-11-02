/* Launcher (Gearbox, sem fonte): GUI_ACTION_SYSTEM_SAVE_MANUAL_USE <comando> ... Usada pelas
   telas de opções e de save do sistema: marca/compara os controles gravados num .cfg e
   chama as funções do cartão de memória, gravando o estado em variáveis do GUI. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
int func_00228C48(const char *a, const char *b);	/* 1 se iguais */
int func_00228EE0(char *dst, const char *fmt, ...);	/* sprintf */
void *func_0022B118(const char *name, const char *mode);	/* abre arquivo */
void func_0022B238(void *f);			/* fecha */
void func_0022B248(const char *name);	/* apaga */
int func_0022BCB8(const char *a, const char *b);	/* 1 se os arquivos são iguais */
void func_00263328(void *f);
void Host_WriteControls(void *f);
int func_0022E2E8(int slot);	/* procura save de configurações */
int func_0022E440(int slot);	/* carrega */
int func_0022E628(int slot);	/* formata */
int func_0022E470(int slot);	/* procura espaço para gravar */
int func_0022E5E8(int slot);	/* cria save */
int func_0022E608(int slot);	/* atualiza save */
int func_0022E678(int slot);	/* cartão presente */
}

class CGUIActionSystemSaveManualUse : public CGUIAction
{
public:
	CGUIActionSystemSaveManualUse();
	virtual ~CGUIActionSystemSaveManualUse();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionSystemSaveManualUse :: CGUIActionSystemSaveManualUse()
{
}

CGUIActionSystemSaveManualUse :: ~CGUIActionSystemSaveManualUse()
{
}

void CGUIActionSystemSaveManualUse :: Init( void *tokenizer )
{
	m_szName = func_00228D30( "GUI_ACTION_SYSTEM_SAVE_MANUAL_USE" );
	m_bInitialized = 1;
}

void CGUIActionSystemSaveManualUse :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1 $param2";
	char buf[16];
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *cmd = func_00108148( tok, 0, 0 );

	if ( func_00228C00( cmd, "MarkStartingSettings" ) == 1 )
	{
		void *f = func_0022B118( "MarkedSettings.cfg", "w" );
		func_00263328( f );
		Host_WriteControls( f );
		func_0022B238( f );
	}

	if ( func_00228C00( cmd, "HaveSettingsChanged" ) == 1 )
	{
		void *f = func_0022B118( "EndingSettings.cfg", "w" );
		func_00263328( f );
		Host_WriteControls( f );
		func_0022B238( f );
		if ( func_0022BCB8( "MarkedSettings.cfg", "EndingSettings.cfg" ) == 1 )
			gui->SetVariable( "Options_HaveSettingsChanged", "0" );
		else
			gui->SetVariable( "Options_HaveSettingsChanged", "1" );
		func_0022B248( "MarkedSettings.cfg" );
		func_0022B248( "EndingSettings.cfg" );
	}

	if ( func_00228C00( cmd, "Scan" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		int state;
		switch ( func_0022E2E8( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) )
		{
		case 1: state = 4; break;
		case 2: state = 5; break;
		case 3: state = 3; break;
		case 4: state = 7; break;
		case 0:
		default: state = 6; break;
		}
		func_00228EE0( buf, "%d", state );
		gui->SetVariable( "LoadSettings_ScanSlotState", buf );
	}

	if ( func_00228C48( cmd, "Load" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022E440( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "LoadSettings_LoadState", "1.0" );
		else
			gui->SetVariable( "LoadSettings_LoadState", "2.0" );
	}

	if ( func_00228C48( cmd, "Format" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022E628( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "SaveSettings_FormatState", "10.0" );
		else
			gui->SetVariable( "SaveSettings_FormatState", "11.0" );
	}

	if ( func_00228C00( cmd, "SaveScan" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		int state;
		switch ( func_0022E470( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) )
		{
		case 1: state = 10; break;
		case 2: state = 11; break;
		case 3: state = 12; break;
		case 4: state = 13; break;
		case 5: state = 14; break;
		case 0:
		default: state = 3; break;
		}
		func_00228EE0( buf, "%d", state );
		gui->SetVariable( "SaveSettings_ScanSlotState", buf );
	}

	if ( func_00228C48( cmd, "CreateSave" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022E5E8( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "SaveSettings_CreateSaveState", "10.0" );
		else
			gui->SetVariable( "SaveSettings_CreateSaveState", "11.0" );
	}

	if ( func_00228C48( cmd, "UpdateSave" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022E608( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "SaveSettings_UpdateSaveState", "10.0" );
		else
			gui->SetVariable( "SaveSettings_UpdateSaveState", "11.0" );
	}

	if ( func_00228C48( cmd, "VerifyCardPresence" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		int slot = (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) );
		name = func_00108148( tok, 0, 0 );
		if ( func_0022E678( slot ) == 1 )
			gui->SetVariable( name, "1" );
		else
			gui->SetVariable( name, "0" );
	}
}
