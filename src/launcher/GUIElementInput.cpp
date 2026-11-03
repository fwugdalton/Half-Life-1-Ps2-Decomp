/* Launcher (Gearbox, sem fonte): teclas de um elemento de tela (CGUIElementInput, 0x18
   bytes, bloco GUI_INPUT: nomes de botões (UP, P1_X, ANY...) seguidos de GUI_COMMAND). A
   tabela de nomes → teclas (50 entradas) é montada na primeira vez. Nome do arquivo tirado
   da string de alocação. */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void Q_memcpy(void *dest, const void *src, int count);
void func_00225860(void *dest, int c, int count);	/* Q_memset */
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
char *func_00228EC0(const char *s, const char *find);	/* strstr */
}

class CGUIParser
{
public:
	char *NextToken( const char *delims, int toDelim );
};

class CGUICommand
{
public:
	CGUICommand();
	CGUICommand( const CGUICommand &other );
	~CGUICommand();
	void Parse( CGUIParser *parser );
	void Execute( CGUI *gui );
	char m_pad[0x24];
};

class CGUIScreen
{
public:
	CGUI *m_pGUI;
};

/* nome de botão do script e as teclas que ele aceita */
typedef struct
{
	char name[0x14];
	int count;
	int *keys;
} GUIKeyName_t;

/* uma linha <botões> GUI_COMMAND ... (count -1 = ANY) */
typedef struct
{
	int count;
	int keys[10];
	CGUICommand *command;
} GUIKeyBinding_t;

int D_00301F70 = 1;	/* a tabela ainda não foi montada */
int D_00301F74 = 0;
extern int D_0039A260;				/* nº de nomes na tabela */
extern GUIKeyName_t D_0039A268[50];

class CGUIElementInput : public CGUIWidgetPart
{
public:
	CGUIElementInput( CGUIScreen *screen );
	CGUIElementInput( const CGUIElementInput &other );
	virtual ~CGUIElementInput();
	virtual void Parse( void );
	void AddBinding( int count, int *keys );
	int Matches( GUIKeyBinding_t *b, int key );
	static void AddKey( GUIKeyName_t *k, const char *name, int key );
	static void InitKeyTable( void );

	int m_iNumBindings;				/* 0x0C */
	int m_iMaxBindings;				/* 0x10 */
	GUIKeyBinding_t **m_pBindings;	/* 0x14 */
};

CGUIElementInput :: CGUIElementInput( CGUIScreen *screen ) : CGUIWidgetPart( screen )
{
	m_i0 = 0;
}

CGUIElementInput :: ~CGUIElementInput()
{
	int i;

	for ( i = 0; i < m_iMaxBindings; i++ )
	{
		delete m_pBindings[i]->command;
		m_pBindings[i]->command = 0;
		Mem_Free( m_pBindings[i] );
		m_pBindings[i] = 0;
	}
	if ( m_pBindings )
	{
		Mem_Free( m_pBindings );
		m_pBindings = 0;
	}
	m_i0 = 0;
}

/* o original não aloca a lista m_pBindings aqui */
/* o construtor de cópia e o Parse vêm do asm, então a string do nome do arquivo
   (primeira da .rodata) sai aqui antes deles */
#ifndef NON_MATCHING
__asm__( ".section .rodata\n\t.align 3\nD_00336E70:\n\t.asciz \"Launcher/GUIElementInput.cpp\"\n\t.previous" );
extern const char D_00336E70[];
#define INPUT_FILE D_00336E70
#else
#define INPUT_FILE "Launcher/GUIElementInput.cpp"
#endif

CGUIElementInput :: CGUIElementInput( const CGUIElementInput &other ) : CGUIWidgetPart( other.m_p4 )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIElementInput", __16CGUIElementInputRC16CGUIElementInput);
#else
	int i;

	m_i0 = 1;
	m_iNumBindings = other.m_iNumBindings;
	m_iMaxBindings = other.m_iMaxBindings;
	for ( i = 0; i < m_iMaxBindings; i++ )
	{
		m_pBindings[i] = (GUIKeyBinding_t *)Mem_DbgMalloc( sizeof( GUIKeyBinding_t ), INPUT_FILE, 91 );
		m_pBindings[i]->count = 0;
		m_pBindings[i]->command = 0;
	}
	for ( i = 0; i < m_iNumBindings; i++ )
	{
		m_pBindings[i]->count = other.m_pBindings[i]->count;
		Q_memcpy( m_pBindings[i]->keys, other.m_pBindings[i]->keys, m_pBindings[i]->count * sizeof( int ) );
		m_pBindings[i]->command = new CGUICommand( *other.m_pBindings[i]->command );
	}
#endif
}

void CGUIElementInput :: Parse( void )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIElementInput", Parse__16CGUIElementInput);
#else
	int keys[10];
	int count;
	int i;
	char *t;
	CGUIParser *p;

	if ( D_00301F70 == 1 )
	{
		D_00301F70 = 0;
		Mem_PushTag();
		Mem_SetTag( 1 );
		InitKeyTable();
		Mem_PopTag();
	}
	if ( m_i0 )
		return;
	m_iNumBindings = 0;
	count = 0;
	m_iMaxBindings = 0;
	m_pBindings = 0;
	p = (CGUIParser *)m_p4->m_pGUI->GetTokenizer();
	t = p->NextToken( 0, 0 );
	while ( t && func_00228C00( t, "END" ) != 1 )
	{
		if ( func_00228C00( t, "ANY" ) == 1 )
		{
			count = -1;
			func_00225860( keys, 0, sizeof( keys ) );
		}
		for ( i = 0; i < D_0039A260; i++ )
		{
			if ( func_00228C00( t, D_0039A268[i].name ) == 1 )
			{
				Q_memcpy( keys, D_0039A268[i].keys, D_0039A268[i].count * sizeof( int ) );
				count = D_0039A268[i].count;
				break;
			}
		}
		if ( i == D_0039A260 && !func_00228EC0( t, "ANY" ) && func_00228C00( t, "GUI_COMMAND" ) != 1 )
			;	/* aviso de botão desconhecido tirado da versão final */
		if ( func_00228C00( t, "GUI_COMMAND" ) == 1 )
		{
			AddBinding( count, keys );
			count = 0;
		}
		t = p->NextToken( 0, 0 );
	}
	m_i0 = 1;
#endif
}

/* declarado no CGUIWidgetPart (o widget só guarda a parte de teclas como base) */
void CGUIWidgetPart :: HandleKey( int key )
{
	CGUIElementInput *in = (CGUIElementInput *)this;
	int i;

	for ( i = 0; i < in->m_iNumBindings; i++ )
	{
		if ( in->Matches( in->m_pBindings[i], key ) == 1 )
		{
			in->m_pBindings[i]->command->Execute( m_p4->m_pGUI );
			break;
		}
	}
}

void CGUIElementInput :: AddBinding( int count, int *keys )
{
	CGUIParser *p = (CGUIParser *)m_p4->m_pGUI->GetTokenizer();
	int i;

	if ( m_iNumBindings == m_iMaxBindings )
	{
		m_iMaxBindings = m_iNumBindings + 1;
		m_pBindings = (GUIKeyBinding_t **)func_002260D0( m_pBindings, m_iMaxBindings * sizeof( GUIKeyBinding_t * ), INPUT_FILE, 230 );
		for ( i = m_iNumBindings; i < m_iMaxBindings; i++ )
		{
			m_pBindings[i] = (GUIKeyBinding_t *)Mem_DbgMalloc( sizeof( GUIKeyBinding_t ), INPUT_FILE, 238 );
			m_pBindings[i]->count = 0;
			m_pBindings[i]->command = 0;
		}
	}
	m_pBindings[m_iNumBindings]->command = new CGUICommand;
	m_pBindings[m_iNumBindings]->command->Parse( p );
	m_pBindings[m_iNumBindings]->count = count;
	if ( count > 0 )
		Q_memcpy( m_pBindings[m_iNumBindings]->keys, keys, count * sizeof( int ) );
	m_iNumBindings++;
}

int CGUIElementInput :: Matches( GUIKeyBinding_t *b, int key )
{
	int r = 0;
	int i;

	if ( b->count == -1 )
		r = 1;
	else
	{
		for ( i = 0; i < b->count; i++ )
		{
			if ( b->keys[i] == key )
			{
				r = 1;
				break;
			}
		}
	}
	return r;
}

void CGUIElementInput :: AddKey( GUIKeyName_t *k, const char *name, int key )
{
	int *keys = (int *)Mem_DbgMalloc( ( k->count + 1 ) * sizeof( int ), INPUT_FILE, 297 );

	if ( k->keys )
	{
		Q_memcpy( keys, k->keys, k->count * sizeof( int ) );
		Mem_Free( k->keys );
	}
	k->keys = keys;
	keys[k->count] = key;
	k->count++;
	if ( !k->name[0] )
	{
		func_00228CF0( k->name, name );
		D_0039A260++;
	}
}

void CGUIElementInput :: InitKeyTable( void )
{
	int i;

	for ( i = 0; i < 50; i++ )
	{
		D_0039A268[i].count = 0;
		D_0039A268[i].keys = 0;
		D_0039A268[i].name[0] = 0;
	}
	D_0039A260 = 0;
	AddKey( &D_0039A268[0], "UP", 210 );
	AddKey( &D_0039A268[0], "UP", 226 );
	AddKey( &D_0039A268[0], "UP", 310 );
	AddKey( &D_0039A268[0], "UP", 326 );
	AddKey( &D_0039A268[0], "UP", 128 );
	AddKey( &D_0039A268[0], "UP", 208 );
	AddKey( &D_0039A268[1], "DOWN", 211 );
	AddKey( &D_0039A268[1], "DOWN", 227 );
	AddKey( &D_0039A268[1], "DOWN", 311 );
	AddKey( &D_0039A268[1], "DOWN", 327 );
	AddKey( &D_0039A268[1], "DOWN", 129 );
	AddKey( &D_0039A268[1], "DOWN", 207 );
	AddKey( &D_0039A268[2], "LEFT", 212 );
	AddKey( &D_0039A268[2], "LEFT", 228 );
	AddKey( &D_0039A268[2], "LEFT", 312 );
	AddKey( &D_0039A268[2], "LEFT", 328 );
	AddKey( &D_0039A268[2], "LEFT", 130 );
	AddKey( &D_0039A268[3], "RIGHT", 213 );
	AddKey( &D_0039A268[3], "RIGHT", 229 );
	AddKey( &D_0039A268[3], "RIGHT", 313 );
	AddKey( &D_0039A268[3], "RIGHT", 329 );
	AddKey( &D_0039A268[3], "RIGHT", 131 );
	AddKey( &D_0039A268[4], "TRIANGLE", 214 );
	AddKey( &D_0039A268[4], "TRIANGLE", 314 );
	AddKey( &D_0039A268[4], "TRIANGLE", 127 );
	AddKey( &D_0039A268[4], "TRIANGLE", 201 );
	AddKey( &D_0039A268[4], "TRIANGLE", 27 );
	AddKey( &D_0039A268[5], "CIRCLE", 217 );
	AddKey( &D_0039A268[5], "CIRCLE", 317 );
	AddKey( &D_0039A268[6], "X", 215 );
	AddKey( &D_0039A268[6], "X", 315 );
	AddKey( &D_0039A268[6], "X", 13 );
	AddKey( &D_0039A268[6], "X", 200 );
	AddKey( &D_0039A268[7], "SQUARE", 216 );
	AddKey( &D_0039A268[7], "SQUARE", 316 );
	AddKey( &D_0039A268[7], "SQUARE", 148 );
	AddKey( &D_0039A268[8], "RIGHTSHOULDER1", 221 );
	AddKey( &D_0039A268[8], "RIGHTSHOULDER1", 321 );
	AddKey( &D_0039A268[9], "RIGHTSHOULDER2", 222 );
	AddKey( &D_0039A268[9], "RIGHTSHOULDER2", 322 );
	AddKey( &D_0039A268[10], "R3", 223 );
	AddKey( &D_0039A268[10], "R3", 323 );
	AddKey( &D_0039A268[11], "LEFTSHOULDER1", 218 );
	AddKey( &D_0039A268[11], "LEFTSHOULDER1", 318 );
	AddKey( &D_0039A268[12], "LEFTSHOULDER2", 219 );
	AddKey( &D_0039A268[12], "LEFTSHOULDER2", 319 );
	AddKey( &D_0039A268[13], "L3", 220 );
	AddKey( &D_0039A268[13], "L3", 320 );
	AddKey( &D_0039A268[14], "SELECT", 224 );
	AddKey( &D_0039A268[14], "SELECT", 324 );
	AddKey( &D_0039A268[14], "SELECT", 9 );
	AddKey( &D_0039A268[15], "START", 225 );
	AddKey( &D_0039A268[15], "START", 325 );
	AddKey( &D_0039A268[15], "START", 13 );
	AddKey( &D_0039A268[16], "P1_UP", 210 );
	AddKey( &D_0039A268[16], "P1_UP", 226 );
	AddKey( &D_0039A268[16], "P1_UP", 128 );
	AddKey( &D_0039A268[17], "P1_DOWN", 211 );
	AddKey( &D_0039A268[17], "P1_DOWN", 227 );
	AddKey( &D_0039A268[17], "P1_DOWN", 129 );
	AddKey( &D_0039A268[18], "P1_LEFT", 212 );
	AddKey( &D_0039A268[18], "P1_LEFT", 228 );
	AddKey( &D_0039A268[18], "P1_LEFT", 130 );
	AddKey( &D_0039A268[19], "P1_RIGHT", 213 );
	AddKey( &D_0039A268[19], "P1_RIGHT", 229 );
	AddKey( &D_0039A268[19], "P1_RIGHT", 131 );
	AddKey( &D_0039A268[20], "P1_TRIANGLE", 214 );
	AddKey( &D_0039A268[20], "P1_TRIANGLE", 127 );
	AddKey( &D_0039A268[20], "P1_TRIANGLE", 27 );
	AddKey( &D_0039A268[21], "P1_CIRCLE", 217 );
	AddKey( &D_0039A268[22], "P1_X", 215 );
	AddKey( &D_0039A268[22], "P1_X", 13 );
	AddKey( &D_0039A268[23], "P1_SQUARE", 216 );
	AddKey( &D_0039A268[24], "P1_RIGHTSHOULDER1", 221 );
	AddKey( &D_0039A268[25], "P1_RIGHTSHOULDER2", 222 );
	AddKey( &D_0039A268[26], "P1_R3", 223 );
	AddKey( &D_0039A268[27], "P1_LEFTSHOULDER1", 218 );
	AddKey( &D_0039A268[28], "P1_LEFTSHOULDER2", 219 );
	AddKey( &D_0039A268[29], "P1_L3", 220 );
	AddKey( &D_0039A268[30], "P1_SELECT", 224 );
	AddKey( &D_0039A268[30], "P1_SELECT", 9 );
	AddKey( &D_0039A268[31], "P1_START", 225 );
	AddKey( &D_0039A268[31], "P1_START", 13 );
	AddKey( &D_0039A268[32], "P2_UP", 310 );
	AddKey( &D_0039A268[32], "P2_UP", 326 );
	AddKey( &D_0039A268[33], "P2_DOWN", 311 );
	AddKey( &D_0039A268[33], "P2_DOWN", 327 );
	AddKey( &D_0039A268[34], "P2_LEFT", 312 );
	AddKey( &D_0039A268[34], "P2_LEFT", 328 );
	AddKey( &D_0039A268[35], "P2_RIGHT", 313 );
	AddKey( &D_0039A268[35], "P2_RIGHT", 329 );
	AddKey( &D_0039A268[36], "P2_TRIANGLE", 314 );
	AddKey( &D_0039A268[37], "P2_CIRCLE", 317 );
	AddKey( &D_0039A268[38], "P2_X", 315 );
	AddKey( &D_0039A268[39], "P2_SQUARE", 316 );
	AddKey( &D_0039A268[40], "P2_RIGHTSHOULDER1", 321 );
	AddKey( &D_0039A268[41], "P2_RIGHTSHOULDER2", 322 );
	AddKey( &D_0039A268[42], "P2_R3", 323 );
	AddKey( &D_0039A268[43], "P2_LEFTSHOULDER1", 318 );
	AddKey( &D_0039A268[44], "P2_LEFTSHOULDER2", 319 );
	AddKey( &D_0039A268[45], "P2_L3", 320 );
	AddKey( &D_0039A268[46], "P2_SELECT", 324 );
	AddKey( &D_0039A268[47], "P2_START", 325 );
	AddKey( &D_0039A268[48], "DEL", 148 );
	AddKey( &D_0039A268[49], "MOUSE1", 200 );
}
