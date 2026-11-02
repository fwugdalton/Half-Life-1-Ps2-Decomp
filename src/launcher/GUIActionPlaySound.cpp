/* Launcher (Gearbox, sem fonte): GUI_ACTION_PLAY_SOUND <som> <volume>. Toca um som do
   GUISOUND.PAK (Gbx/GuiSound.cpp). O volume vai duas vezes, mas a função do Gbx só usa o nome. */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
float func_002290E0(const char *s);	/* atof */
void func_00231A98(const char *name, float left, float right);
}

class CGUIActionPlaySound : public CGUIAction
{
public:
	CGUIActionPlaySound();
	virtual ~CGUIActionPlaySound();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionPlaySound :: CGUIActionPlaySound()
{
	m_bInitialized = 0;
}

CGUIActionPlaySound :: ~CGUIActionPlaySound()
{
}

void CGUIActionPlaySound :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_PLAY_SOUND" );
}

void CGUIActionPlaySound :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1";
	char name[100];
	void *tok = gui->GetTokenizer();

	func_00108120( tok, params, -1 );
	char *t = func_00108148( tok, 0, 0 );
	func_00228CF0( name, t );
	float vol = func_002290E0( func_00108148( tok, 0, 0 ) );
	func_00231A98( name, vol, vol );
}
