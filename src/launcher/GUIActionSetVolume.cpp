/* Launcher (Gearbox, sem fonte): GUI_ACTION_SET_VOLUME BGMVolume|EffectVolume (aplica o
   valor das cvars de volume do som). */

#include "launcher/GUI.h"

extern "C" {
#include "engine/cvar.h"

int func_00228C48(const char *a, const char *b);	/* 1 se iguais */
void func_00231D68(float volume);
void func_00231CB8(float volume);
extern cvar_t D_00334D40;
extern cvar_t D_00334D58;
}

class CGUIActionSetVolume : public CGUIAction
{
public:
	CGUIActionSetVolume();
	virtual ~CGUIActionSetVolume();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionSetVolume :: CGUIActionSetVolume()
{
}

CGUIActionSetVolume :: ~CGUIActionSetVolume()
{
}

void CGUIActionSetVolume :: Init( void *tokenizer )
{
	m_szName = func_00228D30( "GUI_ACTION_SET_VOLUME" );
	m_bInitialized = 1;
}

void CGUIActionSetVolume :: Execute( CGUI *gui )
{
	char params[200] = "$param0";
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *which = func_00108148( tok, 0, 0 );
	if ( func_00228C48( "BGMVolume", which ) == 1 )
		func_00231D68( D_00334D40.value );
	if ( func_00228C48( "EffectVolume", which ) == 1 )
		func_00231CB8( D_00334D58.value );
}
