/* Launcher (Gearbox, sem fonte): GUI_ACTION_ADD_ON_STATE Initialize|Run|Set. Máquina de estados
   da troca para o disco extra: o estado vai para a cvar AddOnSwapState; no fim lê do disco um
   arquivo "SDJ <x> <caminho>" e troca o caminho dos arquivos do jogo. Nome do arquivo tirado da
   string de alocação ("Launcher/GUIActionAddOnDisc.cpp"). */

#include "launcher/GUI.h"

extern "C" {
#include "engine/cvar.h"
void *memset(void *dst, int c, unsigned int n);
void *func_00225D48(int size, int align, const char *file, int line);	/* malloc */
void func_00225DB8(void *p);	/* free */
void CD_Read(int lba, int sectors, void *buf);
void func_001003C0(const char *s);
void func_00236F90(void);
void func_00236FD8(const char *path);
int func_00237258(int *size, int *lba, int *sectors);
void func_00237440(void);
void func_00238098(void);
int func_002380A8(void);
int func_002380B8(void);
void func_002CF690(void);
void Draw_DecalShutdown(void);
void func_00217538(void);
}

cvar_t D_00302A18 = { "AddOnSwapState", "0" };
cvar_t D_00302A30 = { "AddOnErrorMessage", "0" };
int D_00302A44 = 1;

class CGUIActionAddOnDisc : public CGUIAction
{
public:
	CGUIActionAddOnDisc();
	virtual ~CGUIActionAddOnDisc();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
	void Initialize( void );
	void Run( void );
	void SetState( unsigned int state );
	int Load( void );
	int Parse( void );

	unsigned int m_iState;
	char *m_pBuffer;
	int m_iSize;
};

CGUIActionAddOnDisc :: CGUIActionAddOnDisc()
{
	m_pBuffer = 0;
	m_iSize = 0;
}

CGUIActionAddOnDisc :: ~CGUIActionAddOnDisc()
{
	if ( m_pBuffer )
		func_00225DB8( m_pBuffer );
}

void CGUIActionAddOnDisc :: Init( void *tokenizer )
{
	if ( D_00302A44 == 1 )
	{
		Cvar_RegisterVariable( &D_00302A18 );
		Cvar_RegisterVariable( &D_00302A30 );
		D_00302A44 = 0;
	}
	m_iState = 0;
	m_szName = func_00228D30( "GUI_ACTION_ADD_ON_STATE" );
	m_bInitialized = 1;
}

void CGUIActionAddOnDisc :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *cmd = func_00108148( tok, 0, 0 );

	if ( func_00228C00( cmd, "Initialize" ) == 1 )
		Initialize();
	else if ( func_00228C00( cmd, "Run" ) == 1 )
		Run();
	else if ( func_00228C00( cmd, "Set" ) == 1 )
		func_00108148( tok, 0, 0 );
}

void CGUIActionAddOnDisc :: Initialize( void )
{
	SetState( 1 );
}

void CGUIActionAddOnDisc :: Run( void )
{
	unsigned int next = 0;

	switch ( m_iState )
	{
	case 0:
		next = 0;
		break;
	case 1:
		func_001003C0( 0 );
		next = 2;
		func_00238098();
		break;
	case 2:
	case 3:
	case 4:
		switch ( func_002380B8() )
		{
		case 0: next = 2; break;
		case 1: next = 3; break;
		case 2:
			next = 4;
			Cvar_SetValue( "AddOnErrorMessage", 0 );
			break;
		case 3: next = 5; break;
		case 4: next = 7; break;
		case 5: next = 10; break;
		case 6: next = 8; break;
		}
		break;
	case 5:
	{
		int ok = Load();
		next = 7;
		func_002380A8();
		if ( ok == 1 )
			next = 6;
		break;
	}
	case 6:
		next = 6;
		break;
	case 7:
		Cvar_SetValue( "AddOnErrorMessage", 1.0f );
		Initialize();
		next = 1;
		break;
	case 10:
		next = 2;
		Cvar_SetValue( "AddOnErrorMessage", 1.0f );
		break;
	case 8:
	case 9:
		next = m_iState;
		break;
	}
	SetState( next );
}

void CGUIActionAddOnDisc :: SetState( unsigned int state )
{
	m_iState = state;
	Cvar_SetValue( "AddOnSwapState", (float)state );
}

int CGUIActionAddOnDisc :: Load( void )
{
	int r = 0;
	int lba, sectors;

	func_00237440();
	func_00236F90();
	if ( func_00237258( &m_iSize, &lba, &sectors ) == 1 )
	{
		m_pBuffer = (char *)func_00225D48( sectors << 11, 0x40, "Launcher/GUIActionAddOnDisc.cpp", 282 );
		CD_Read( lba, sectors, m_pBuffer );
		r = Parse();
	}
	return r;
}

int CGUIActionAddOnDisc :: Parse( void )
{
	char tok[0x14];
	int r;
	char *path;

	func_00108008( tok );
	r = 0;
	m_pBuffer[m_iSize] = 0;
	func_00108070( tok );
	func_00108120( tok, m_pBuffer, -1 );
	if ( func_00228C00( func_00108148( tok, 0, 0 ), "SDJ" ) == 1 )
	{
		if ( func_00108148( tok, 0, 0 ) && ( path = func_00108148( tok, 0, 0 ) ) )
		{
			r = 1;
			func_00236FD8( path );
			func_002CF690();
			Draw_DecalShutdown();
			func_00217538();
		}
	}
	func_00108018( tok, 2 );
	return r;
}
