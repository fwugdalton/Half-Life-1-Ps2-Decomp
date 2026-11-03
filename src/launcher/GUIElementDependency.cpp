/* Launcher (Gearbox, sem fonte): parte GUI_DEPENDENCY de um elemento de tela
   (CGUIElementDependency, 0x8C bytes): lista de estados (GUI_ELEMENT_STATE) escolhidos pelo
   valor de uma variável float (NAME). Com STATE_SELECTION_LINEAR TRUE o resultado é
   interpolado entre o estado de baixo e o de cima. Nome do arquivo tirado da string de
   alocação. */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
void Mem_Free(void *ptr);
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
};

class CGUICommand;

class CGUIScreen
{
public:
	CGUI *m_pGUI;
};

/* o que um GUI_DEPENDENCY satisfeito troca no elemento (igual ao do GUIGeneric.cpp) */
typedef struct
{
	int bZPlane;
	int iZPlane;
	int bRect;
	float flRect[4];
	int bTexture;
	int iTexture;
	float flTexWidth;
	float flTexHeight;
	int bTexRect;
	float flTexRect[4];
	int bColor;
	float flColor[3];
	CGUIWidgetPart *pCaption;
	CGUIWidgetPart *pInput;
	CGUICommand *pCommands[4];
} GUIOverride_t;

class CGUIElementState
{
public:
	CGUIElementState();
	~CGUIElementState();
	void Parse( CGUIScreen *screen );
	float GetValue( void );
	GUIOverride_t *GetOverride( void );
	char m_pad[0x70];
};

class CGUIElementDependency : public CGUIWidgetPart
{
public:
	CGUIElementDependency( CGUIScreen *screen );
	virtual ~CGUIElementDependency();
	virtual void Parse( void );
	GUIOverride_t *GetOverride( void );
	void AddState( CGUIElementState *state );
	void FindStates( int *lo, int *hi, float *frac );
	void Blend( GUIOverride_t *out, int lo, int hi, float frac );

	int m_bLinear;					/* 0x0C STATE_SELECTION_LINEAR */
	int m_bHasStates;				/* 0x10 */
	char *m_szVariable;				/* 0x14 NAME */
	int m_iMaxStates;				/* 0x18 */
	int m_iNumStates;				/* 0x1C */
	CGUIElementState **m_pStates;	/* 0x20 */
	GUIOverride_t m_Override;		/* 0x24 */
};

CGUIElementDependency :: CGUIElementDependency( CGUIScreen *screen ) : CGUIWidgetPart( screen )
{
	m_i0 = 0;
}

CGUIElementDependency :: ~CGUIElementDependency()
{
	int i;

	for ( i = 0; i < m_iNumStates; i++ )
	{
		if ( m_pStates[i] )
			delete m_pStates[i];
		m_pStates[i] = 0;
	}
	Mem_Free( m_pStates );
	m_pStates = 0;
	if ( m_szVariable )
	{
		Mem_Free( m_szVariable );
		m_szVariable = 0;
	}
}

void CGUIElementDependency :: Parse( void )
{
/* NON_MATCHING: o alvo tira do laço o %hi de STATE_SELECTION_LINEAR, TRUE e
   GUI_ELEMENT_STATE e o 1 do GUI_ELEMENT_STATE, mas não o de NAME */
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIElementDependency", Parse__21CGUIElementDependency);
#else
	CGUIParser *p;
	char *t;
	int f;
	int g;

	if ( m_i0 )
		return;

	m_bLinear = 0;
	m_bHasStates = 0;
	m_iMaxStates = 0;
	m_iNumStates = 0;
	m_pStates = 0;
	m_Override.pCaption = 0;
	m_Override.pInput = 0;
	m_Override.bColor = 0;
	m_Override.pCommands[0] = 0;
	m_Override.pCommands[1] = 0;
	m_Override.pCommands[2] = 0;
	m_szVariable = 0;

	p = (CGUIParser *)m_p4->m_pGUI->GetTokenizer();
	t = p->NextToken( 0, 0 );
	while ( t && func_00228C00( t, "END" ) != 1 )
	{
		if ( func_00228C00( t, "NAME" ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			m_szVariable = func_00228D30( t );
		}
		if ( ( f = func_00228C00( t, "STATE_SELECTION_LINEAR" ) ) == 1 )
		{
			t = p->NextToken( 0, 0 );
			if ( func_00228C00( t, "TRUE" ) == f )
				m_bLinear = f;
			else
				m_bLinear = 0;
		}
		if ( ( g = func_00228C00( t, "GUI_ELEMENT_STATE" ) ) == 1 )
		{
			m_bHasStates = g;
			CGUIElementState *s = new CGUIElementState;
			s->Parse( m_p4 );
			AddState( s );
		}
		t = p->NextToken( 0, 0 );
	}
#endif
}

GUIOverride_t *CGUIElementDependency :: GetOverride( void )
{
	int lo, hi;
	float frac;
	GUIOverride_t *r;

	if ( m_bHasStates )
	{
		FindStates( &lo, &hi, &frac );
		Blend( &m_Override, lo, hi, frac );
		r = &m_Override;
	}
	else
		r = 0;
	return r;
}

void CGUIElementDependency :: AddState( CGUIElementState *state )
{
	if ( m_iNumStates == m_iMaxStates )
	{
		m_iMaxStates++;
		m_pStates = (CGUIElementState **)func_002260D0( m_pStates, m_iMaxStates * sizeof( CGUIElementState * ), "Launcher/GUIElementDependency.cpp", 157 );
	}
	m_pStates[m_iNumStates] = state;
	m_iNumStates++;
}

/* estado mais perto abaixo (ou igual) e acima do valor da variável */
void CGUIElementDependency :: FindStates( int *lo, int *hi, float *frac )
{
	CGUIVariable *v;
	float value;
	float best, d;
	int i, found, idx;
	int found2, idx2;

	v = func_00107EE0( m_p4->m_pGUI->GetVariables(), m_szVariable );
	if ( v->IsType( D_003029E8 ) == 1 )
		value = ( (CGUIVariableFloat *)v )->GetValue();
	else
		value = 0.0f;

	found = 0;
	if ( m_iNumStates == 1 )
	{
		*lo = 0;
		*hi = 0;
	}
	else
	{
		best = 0.0f;
		idx = 0;
		for ( i = 0; i < m_iNumStates; i++ )
		{
			float diff = m_pStates[i]->GetValue() - value;
			if ( ( !found || diff > best ) && diff <= 0.0f )
			{
				best = diff;
				found = 1;
				idx = i;
			}
		}
		*lo = idx;

		i = 0;
		found2 = 0;
		best = 0.0f;
		idx2 = 0;
		for ( ; i < m_iNumStates; i++ )
		{
			float diff = m_pStates[i]->GetValue() - value;
			if ( ( !found2 || diff < best ) && diff > 0.0f )
			{
				best = diff;
				found2 = 1;
				idx2 = i;
			}
		}
		*hi = idx2;
	}

	if ( m_bLinear )
	{
		d = __builtin_fabsf( m_pStates[*lo]->GetValue() - value );
		*frac = d / ( d + __builtin_fabsf( m_pStates[*hi]->GetValue() - value ) );
	}
	else
		*frac = 0;
}

/* comando do estado de baixo perto dele, do de cima perto dele */
static CGUICommand *BlendCommand( CGUICommand *a, CGUICommand *b, float frac )
{
/* NON_MATCHING: no alvo o último `return r` fica com o delay slot vazio */
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIElementDependency", BlendCommand__FP11CGUICommandT0f);
#else
	CGUICommand *r = 0;

	if ( ( a != 0 ) ^ ( b != 0 ) )
	{
		if ( a )
		{
			if ( frac < 0.01f )
				r = a;
		}
		else
		{
			if ( frac > 0.99f )
				r = b;
		}
	}
	else if ( a )
	{
		if ( frac < 0.01f )
			r = a;
	}
	else
		r = 0;
	return r;
#endif
}

#define XOR( x, y )	( ( ( x ) != 0 ) ^ ( ( y ) != 0 ) )

void CGUIElementDependency :: Blend( GUIOverride_t *out, int lo, int hi, float frac )
{
	GUIOverride_t *a = m_pStates[lo]->GetOverride();
	GUIOverride_t *b = m_pStates[hi]->GetOverride();

	if ( XOR( a->bZPlane, b->bZPlane ) )
	{
		out->bZPlane = 1;
		if ( a->bZPlane )
			out->iZPlane = a->iZPlane;
		else
			out->iZPlane = b->iZPlane;
	}
	else if ( a->bZPlane && b->bZPlane )
	{
		out->bZPlane = 1;
		out->iZPlane = (int)( ( b->iZPlane - a->iZPlane ) * frac + a->iZPlane );
	}
	else
		out->bZPlane = 0;

	if ( XOR( a->bRect, b->bRect ) )
	{
		out->bRect = 1;
		if ( a->bRect )
		{
			out->flRect[0] = a->flRect[0];
			out->flRect[1] = a->flRect[1];
			out->flRect[2] = a->flRect[2];
			out->flRect[3] = a->flRect[3];
		}
		else
		{
			out->flRect[0] = b->flRect[0];
			out->flRect[1] = b->flRect[1];
			out->flRect[2] = b->flRect[2];
			out->flRect[3] = b->flRect[3];
		}
	}
	else if ( a->bRect && b->bRect )
	{
		out->bRect = 1;
		out->flRect[0] = ( b->flRect[0] - a->flRect[0] ) * frac + a->flRect[0];
		out->flRect[1] = ( b->flRect[1] - a->flRect[1] ) * frac + a->flRect[1];
		out->flRect[2] = ( b->flRect[2] - a->flRect[2] ) * frac + a->flRect[2];
		out->flRect[3] = ( b->flRect[3] - a->flRect[3] ) * frac + a->flRect[3];
	}
	else
		out->bRect = 0;

	if ( XOR( a->bTexture, b->bTexture ) )
	{
		out->bTexture = 1;
		if ( a->bTexture )
		{
			out->iTexture = a->iTexture;
			out->flTexHeight = a->flTexHeight;
			out->flTexWidth = a->flTexWidth;
		}
		else
		{
			out->iTexture = b->iTexture;
			out->flTexHeight = b->flTexHeight;
			out->flTexWidth = b->flTexWidth;
		}
	}
	else if ( a->bTexture )
	{
		out->bTexture = 1;
		out->iTexture = a->iTexture;
		out->flTexHeight = a->flTexHeight;
		out->flTexWidth = a->flTexWidth;
	}
	else
		out->bTexture = 0;

	if ( XOR( a->bTexRect, b->bTexRect ) )
	{
		out->bTexRect = 1;
		if ( a->bTexRect )
		{
			out->flTexRect[0] = a->flTexRect[0];
			out->flTexRect[1] = a->flTexRect[1];
			out->flTexRect[2] = a->flTexRect[2];
			out->flTexRect[3] = a->flTexRect[3];
		}
		else
		{
			out->flTexRect[0] = b->flTexRect[0];
			out->flTexRect[1] = b->flTexRect[1];
			out->flTexRect[2] = b->flTexRect[2];
			out->flTexRect[3] = b->flTexRect[3];
		}
	}
	else if ( a->bTexRect && b->bTexRect )
	{
		out->bTexRect = 1;
		out->flTexRect[0] = ( b->flTexRect[0] - a->flTexRect[0] ) * frac + a->flTexRect[0];
		out->flTexRect[1] = ( b->flTexRect[1] - a->flTexRect[1] ) * frac + a->flTexRect[1];
		out->flTexRect[2] = ( b->flTexRect[2] - a->flTexRect[2] ) * frac + a->flTexRect[2];
		out->flTexRect[3] = ( b->flTexRect[3] - a->flTexRect[3] ) * frac + a->flTexRect[3];
	}
	else
		out->bTexRect = 0;

	if ( XOR( a->bColor, b->bColor ) )
	{
		out->bColor = 1;
		if ( a->bColor )
		{
			out->flColor[0] = a->flColor[0];
			out->flColor[1] = a->flColor[1];
			out->flColor[2] = a->flColor[2];
		}
		else
		{
			out->flColor[0] = b->flColor[0];
			out->flColor[1] = b->flColor[1];
			out->flColor[2] = b->flColor[2];
		}
	}
	else if ( a->bColor && b->bColor )
	{
		out->bColor = 1;
		out->flColor[0] = ( b->flColor[0] - a->flColor[0] ) * frac + a->flColor[0];
		out->flColor[1] = ( b->flColor[1] - a->flColor[1] ) * frac + a->flColor[1];
		out->flColor[2] = ( b->flColor[2] - a->flColor[2] ) * frac + a->flColor[2];
	}
	else
		out->bColor = 0;

	if ( XOR( a->pCaption, b->pCaption ) )
	{
		if ( a->pCaption )
			out->pCaption = a->pCaption;
		else
			out->pCaption = b->pCaption;
	}
	else if ( a->pCaption )
		out->pCaption = a->pCaption;
	else
		out->pCaption = 0;

	if ( XOR( a->pInput, b->pInput ) )
	{
		if ( a->pInput )
			out->pInput = a->pInput;
		else
			out->pInput = b->pInput;
	}
	else if ( a->pInput )
		out->pInput = a->pInput;
	else
		out->pInput = 0;

	out->pCommands[0] = BlendCommand( a->pCommands[0], b->pCommands[0], frac );
	out->pCommands[1] = BlendCommand( a->pCommands[1], b->pCommands[1], frac );
	out->pCommands[2] = BlendCommand( a->pCommands[2], b->pCommands[2], frac );
	out->pCommands[3] = BlendCommand( a->pCommands[3], b->pCommands[3], frac );
}
