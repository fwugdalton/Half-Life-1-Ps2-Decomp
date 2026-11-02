/* Launcher (Gearbox, sem fonte): GUI_ACTION_GAME_SAVE <comando> ... Usada pelas telas de
   save do jogo: chama as funções do Gbx/GameSave.cpp e grava o resultado em variáveis do GUI. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
int func_00228C48(const char *a, const char *b);	/* 1 se iguais */
int func_00228EE0(char *dst, const char *fmt, ...);	/* sprintf */
void func_0022E860(void);
void func_0022E8D0(void);
void func_0022E930(const char *name);
void func_0022E9A8(void);
void func_0022EA30(void);
int func_0022EAE0(int slot);
int func_0022EBA0(int slot);
void func_0022ECC0(void);
void func_0022EDE8(int first, int count);
int func_0022F070(int idx);
int func_0022F0E8(int idx);
int func_0022F110(int idx);
int func_0022F360(int idx);
int func_0022F4B0(int idx);
int func_0022F598(int slot);
int func_0022F600(void);
int func_00230CE0(int state);
}

class CGUIActionGameSave : public CGUIAction
{
public:
	CGUIActionGameSave();
	virtual ~CGUIActionGameSave();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
	void SetCheckState( CGUI *gui, int state );

	int m_iCheckState;
};

CGUIActionGameSave :: CGUIActionGameSave()
{
}

CGUIActionGameSave :: ~CGUIActionGameSave()
{
}

void CGUIActionGameSave :: Init( void *tokenizer )
{
	m_szName = func_00228D30( "GUI_ACTION_GAME_SAVE" );
	m_bInitialized = 1;
}

void CGUIActionGameSave :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1 $param2";
	char buf[16];
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *cmd = func_00108148( tok, 0, 0 );

	if ( func_00228C48( cmd, "Initialize" ) == 1 )
		func_0022E860();

	if ( func_00228C48( cmd, "Shutdown" ) == 1 )
		func_0022E8D0();

	if ( func_00228C00( cmd, "ScanSave" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		int state;
		switch ( func_0022EAE0( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) )
		{
		case 1: state = 4; break;
		case 2: state = 5; break;
		case 3: state = 3; break;
		case 5: state = 8; break;
		case 6:
		default: state = 6; break;
		}
		func_00228EE0( buf, "%d", state );
		gui->SetVariable( "ScanSlotState", buf );
	}

	if ( func_00228C00( cmd, "ScanLoad" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		int state;
		switch ( func_0022EBA0( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) )
		{
		case 1: state = 4; break;
		case 2: state = 5; break;
		case 3: state = 3; break;
		case 4: state = 7; break;
		case 6:
		default: state = 6; break;
		}
		func_00228EE0( buf, "%d", state );
		gui->SetVariable( "ScanSlotState", buf );
	}

	if ( func_00228C48( cmd, "SaveToMemFS" ) == 1 )
		func_0022E9A8();

	if ( func_00228C48( cmd, "DeleteMemFSSave" ) == 1 )
		func_0022EA30();

	if ( func_00228C48( cmd, "UpdateCardAvailability" ) == 1 )
		func_0022ECC0();

	if ( func_00228C48( cmd, "UpdateListing" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		int first = (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) );
		name = func_00108148( tok, 0, 0 );
		func_0022EDE8( first, (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) );
	}

	if ( func_00228C48( cmd, "VerifySpace" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022F070( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "IsEnoughSpace", "1" );
		else
			gui->SetVariable( "IsEnoughSpace", "0" );
	}

	if ( func_00228C48( cmd, "CreateSave" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022F110( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "CreateSaveState", "1.0" );
		else
			gui->SetVariable( "CreateSaveState", "2.0" );
	}

	if ( func_00228C48( cmd, "LoadSave" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022F360( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "LoadSaveState", "1.0" );
		else
			gui->SetVariable( "LoadSaveState", "2.0" );
	}

	if ( func_00228C48( cmd, "DeleteSave" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022F4B0( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "DeleteSaveState", "3.0" );
		else
			gui->SetVariable( "DeleteSaveState", "4.0" );
	}

	if ( func_00228C48( cmd, "FormatCard" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022F598( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "FormatCardState", "3.0" );
		else
			gui->SetVariable( "FormatCardState", "4.0" );
	}

	if ( func_00228C48( cmd, "IsOverwrite" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022F0E8( (int)func_00107000( func_00107EE0( gui->GetVariables(), name ) ) ) == 1 )
			gui->SetVariable( "IsOverwrite", "1" );
		else
			gui->SetVariable( "IsOverwrite", "0" );
	}

	if ( func_00228C48( cmd, "InitializeCheck" ) == 1 )
		SetCheckState( gui, 1 );

	if ( func_00228C48( cmd, "RunCheck" ) == 1 )
		SetCheckState( gui, func_00230CE0( m_iCheckState ) );

	if ( func_00228C48( cmd, "SetGameName" ) == 1 )
		func_0022E930( func_00108148( tok, 0, 0 ) );

	if ( func_00228C48( cmd, "VerifyCardPresence" ) == 1 )
	{
		char *name = func_00108148( tok, 0, 0 );
		if ( func_0022F600() == 1 )
			gui->SetVariable( name, "1" );
		else
			gui->SetVariable( name, "0" );
	}
}

void CGUIActionGameSave :: SetCheckState( CGUI *gui, int state )
{
	char buf[16];

	m_iCheckState = state;
	func_00228EE0( buf, "%d", state );
	gui->SetVariable( "GameSaveCheckState", buf );
}
