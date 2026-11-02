/* Launcher (Gearbox, sem fonte): GUI_ACTION_MODIFY_VIEWPORT <variável>. Encolhe a área de
   desenho do jogo (centralizada) entre a tela cheia e 1/10 dela, conforme o valor (0..1) da
   variável. Os tamanhos são guardados na primeira chamada (flags em .data, valores em .bss). */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
void *memset(void *dst, int c, unsigned int n);
extern int vid;						/* vid.width */
extern int D_0032EF74;				/* vid.height */
extern int D_0032EF68, D_0032EF6C;	/* canto da área de desenho */
extern int D_0032EF80, D_0032EF84;	/* tamanho da tela */
extern int D_0039CFE0, D_0039CFE4;	/* tamanho máximo */
extern int D_0039CFE8, D_0039CFEC;	/* tamanho mínimo */
}

int D_00302A04 = 0;
int D_00302A08 = 0;
int D_00302A0C = 0;
int D_00302A10 = 0;

class CGUIActionModifyViewport : public CGUIAction
{
public:
	CGUIActionModifyViewport();
	virtual ~CGUIActionModifyViewport();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
	void SetViewport( int width, int height );
};

CGUIActionModifyViewport :: CGUIActionModifyViewport()
{
}

CGUIActionModifyViewport :: ~CGUIActionModifyViewport()
{
}

void CGUIActionModifyViewport :: Init( void *tokenizer )
{
	m_bInitialized = 1;
	m_szName = func_00228D30( "GUI_ACTION_MODIFY_VIEWPORT" );
}

void CGUIActionModifyViewport :: Execute( CGUI *gui )
{
	char params[200] = "$param0";
	float f;

	if ( !D_00302A04 )
	{
		D_00302A04 = 1;
		D_0039CFE0 = D_0032EF80;
	}
	if ( !D_00302A08 )
	{
		D_00302A08 = 1;
		D_0039CFE4 = D_0032EF84;
	}
	if ( !D_00302A0C )
	{
		D_0039CFE8 = D_0032EF80 / 10;
		D_00302A0C = 1;
	}
	if ( !D_00302A10 )
	{
		D_0039CFEC = D_0032EF84 / 10;
		D_00302A10 = 1;
	}

	void *tok = gui->GetTokenizer();
	func_00108120( tok, params, -1 );
	char *name = func_00108148( tok, 0, 0 );
	CGUIVariable *v = func_00107EE0( gui->GetVariables(), name );
	if ( v->IsType( D_003029E8 ) == 1 )
		f = func_00107000( v );
	else
		f = 1.0f;

	int w, h;
	if ( f > 0.99f )
	{
		w = D_0039CFE0;
		h = D_0039CFE4;
	}
	else
	{
		w = (int)( (float)( D_0039CFE0 - D_0039CFE8 ) * f + (float)D_0039CFE8 );
		h = (int)( (float)( D_0039CFE4 - D_0039CFEC ) * f + (float)D_0039CFEC );
	}
	SetViewport( w, h );

	if ( D_0032EF68 < 0 || D_0032EF6C < 0 )
		SetViewport( D_0039CFE0, D_0039CFE4 );
	else if ( D_0032EF80 / 2 < D_0032EF68 || D_0032EF84 / 2 < D_0032EF6C )
		SetViewport( D_0039CFE8, D_0039CFEC );
}

/* NON_MATCHING: só os registradores dos lui dos quatro globais (pares trocados) */
void CGUIActionModifyViewport :: SetViewport( int width, int height )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIActionModifyViewport", SetViewport__24CGUIActionModifyViewportii);
#else
	int hw = width / 2;
	int hh = height / 2;
	vid = width;
	D_0032EF74 = height;
	D_0032EF68 = D_0032EF80 / 2 - hw;
	D_0032EF6C = D_0032EF84 / 2 - hh;
#endif
}
