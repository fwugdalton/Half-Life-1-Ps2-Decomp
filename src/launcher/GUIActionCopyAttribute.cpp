/* Launcher (Gearbox, sem fonte): GUI_ACTION_COPY_ATTRIBUTE <destino> <origem> <atributo>.
   Copia um atributo de um elemento da tela para outro (método virtual do elemento). */

#include "launcher/GUI.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
}

class CGUIActionCopyAttribute : public CGUIAction
{
public:
	CGUIActionCopyAttribute();
	virtual ~CGUIActionCopyAttribute();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
};

CGUIActionCopyAttribute :: CGUIActionCopyAttribute()
{
}

CGUIActionCopyAttribute :: ~CGUIActionCopyAttribute()
{
}

void CGUIActionCopyAttribute :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_COPY_ATTRIBUTE" );
}

void CGUIActionCopyAttribute :: Execute( CGUI *gui )
{
	char params[200] = "$param0 $param1 $param2";
	char attr[100];
	void *tok = gui->GetTokenizer();
	CGUIWidget *dst, *src;

	func_00108120( tok, params, -1 );
	dst = gui->FindWidget( func_00108148( tok, 0, 0 ) );
	src = gui->FindWidget( func_00108148( tok, 0, 0 ) );
	char *name = func_00108148( tok, 0, 0 );
	func_00228CF0( attr, name );
	dst->CopyAttribute( attr, src );
}
