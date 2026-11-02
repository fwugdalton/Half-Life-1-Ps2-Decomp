/* Launcher (Gearbox, sem fonte): tokenizador dos scripts do GUI (CGUIParser, 0x14 bytes).
   Lê texto puro ou comprimido com zlib (prefixo "COMPRESSED"), com comentários (#), aspas e
   uma pilha de #defines (10 níveis de 11 nomes) trocados nos tokens. Nome do arquivo tirado
   da string de alocação. */

#include "launcher/GUI.h"
#include "inline_include_asm.h"

extern "C" {
#include "zlib/zlib.h"
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
float func_002290E0(const char *s);	/* atof */
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
char *func_00228D10(char *dst, const char *src, int n);	/* strncpy */
char *func_00228D80(char *dst, const char *src);	/* strcat */
int func_00228DC0(const char *s);	/* strlen */
int func_00228E20(const char *a, const char *b, int n);	/* strncmp */
char *func_00228E80(const char *s, int c);	/* strchr */
char *func_00228EC0(const char *s, const char *find);	/* strstr */
extern char D_0039A7E0[];	/* token devolvido (.bss) */
}

struct GUIDefine
{
	char name[100];
	char value[100];
};

struct GUIDefineLevel
{
	int count;
	GUIDefine defs[11];
};

struct GUIDefineStack
{
	int depth;
	GUIDefineLevel levels[10];
};

/* z_stream seguido da posição de leitura e do buffer de saída */
struct GUIStream
{
	z_stream z;
	int pos;
	char buf[0x800];
};

char D_003029F8[] = "COMPRESSED";

class CGUIParser
{
public:
	CGUIParser();
	~CGUIParser();
	void Init( void );
	void SetText( char *text, int len );
	char *NextToken( const char *delims, int toDelim );
	void ReadFloat4( float *a, float *b, float *c, float *d );
	void PushDefines( void );
	void PopDefines( void );
	void AddDefine( const char *name, const char *value );
	void ApplyDefines( char *text );
	void Replace( char *text, const char *name, const char *value );
	void Close( void );
	char Peek( void );
	void Open( int len, char *text );
	void Advance( void );

	int m_bInitialized;
	GUIDefineStack *m_pDefines;
	char *m_pText;
	int m_iPos;
	GUIStream *m_pStream;
};

CGUIParser :: CGUIParser()
{
	m_bInitialized = 0;
}

CGUIParser :: ~CGUIParser()
{
	m_bInitialized = 0;
	Mem_Free( m_pDefines );
	m_pDefines = 0;
	Close();
}

void CGUIParser :: Init( void )
{
	int i, j;

	m_pDefines = (GUIDefineStack *)Mem_DbgMalloc( sizeof( GUIDefineStack ), "Launcher/GUIParser.cpp", 65 );
	m_bInitialized = 1;
	m_iPos = 0;
	m_pText = 0;
	m_pStream = 0;
	m_pDefines->depth = 0;
	for ( i = 0; i < 10; i++ )
	{
		m_pDefines->levels[i].count = 0;
		for ( j = 0; j < 11; j++ )
		{
			m_pDefines->levels[i].defs[j].name[0] = 0;
			m_pDefines->levels[i].defs[j].value[0] = 0;
		}
	}
}

void CGUIParser :: SetText( char *text, int len )
{
	Open( len, text );
}

/* NON_MATCHING: só registradores (this/len trocados; o original guarda o endereço do token num registro salvo no fim) */
char *CGUIParser :: NextToken( const char *delims, int toDelim )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIParser", NextToken__10CGUIParserPCci);
#else
	char defaults[5] = " \t\n\r";
	const char *d = defaults;
	int len = 0;
	char c;

	if ( delims )
		d = delims;

	if ( toDelim == 1 )
	{
		int got = 0;
		while ( ( c = Peek() ) != 0 )
		{
			if ( !func_00228E80( d, c ) )
			{
				got = 1;
				D_0039A7E0[len++] = c;
			}
			else if ( got == 1 )
				break;
			Advance();
		}
	}
	else
	{
		int comment = 0;
		int quote = 0;
		int got = 0;
		while ( ( c = Peek() ) != 0 )
		{
			if ( c == '#' )
			{
				if ( got == 1 )
					break;
				comment = 1;
			}
			else if ( c == '"' )
			{
				if ( quote == 1 )
					quote = 0;
				else
					quote = 1;
			}
			char *r = func_00228E80( d, c );
			if ( comment == 1 )
			{
				if ( r && ( *r == '\n' || *r == '\r' ) )
					comment = 0;
			}
			else if ( ( !r || quote == 1 ) && c != '"' )
			{
				got = 1;
				D_0039A7E0[len++] = c;
			}
			else if ( !quote && c != '"' && got == 1 )
				break;
			Advance();
		}
	}

	char *p = D_0039A7E0;
	p[len] = 0;
	if ( Peek() == '"' )
		Advance();
	char *ret = p;
	if ( !D_0039A7E0[0] )
		ret = 0;
	ApplyDefines( p );
	return ret;

#endif
}

void CGUIParser :: ReadFloat4( float *a, float *b, float *c, float *d )
{
	*a = func_002290E0( NextToken( 0, 0 ) );
	*b = func_002290E0( NextToken( 0, 0 ) );
	*c = func_002290E0( NextToken( 0, 0 ) );
	*d = func_002290E0( NextToken( 0, 0 ) );
}

void CGUIParser :: PushDefines( void )
{
	m_pDefines->depth++;
	m_pDefines->levels[m_pDefines->depth].count = 0;
}

void CGUIParser :: PopDefines( void )
{
	m_pDefines->depth--;
}

void CGUIParser :: AddDefine( const char *name, const char *value )
{
	GUIDefineLevel *l = &m_pDefines->levels[m_pDefines->depth];

	func_00228CF0( l->defs[l->count].name, name );
	func_00228CF0( l->defs[l->count].value, value );
	l->count++;
}

void CGUIParser :: ApplyDefines( char *text )
{
	int i;
	GUIDefineLevel *l = &m_pDefines->levels[m_pDefines->depth];

	for ( i = 0; i < l->count; i++ )
		Replace( text, l->defs[i].name, l->defs[i].value );
}

void CGUIParser :: Replace( char *text, const char *name, const char *value )
{
	char buf[200];
	int more;

	do
	{
		char *p = func_00228EC0( text, name );
		if ( p )
		{
			int n = p - text;
			func_00228D10( buf, text, n );
			buf[n] = 0;
			func_00228D80( buf, value );
			func_00228D80( buf, p + func_00228DC0( name ) );
			func_00228CF0( text, buf );
			more = 1;
		}
		else
			more = 0;
	} while ( more == 1 );
}

void CGUIParser :: Close( void )
{
	if ( m_pStream )
	{
		inflateEnd( &m_pStream->z );
		Mem_Free( m_pStream );
		m_pStream = 0;
	}
	else if ( m_pText )
	{
		m_pText = 0;
		m_iPos = 0;
	}
}

char CGUIParser :: Peek( void )
{
	return m_pStream ? m_pStream->buf[m_pStream->pos] : m_pText[m_iPos];

}

/* NON_MATCHING: o original grava m_pStream com o v0 do malloc e zera m_iPos antes de m_pText */
void CGUIParser :: Open( int len, char *text )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIParser", Open__10CGUIParseriPc);
#else
	GUIStream *s;

	if ( !func_00228E20( text, D_003029F8, func_00228DC0( D_003029F8 ) ) )
	{
		if ( !m_pStream )
		{
			s = (GUIStream *)Mem_DbgMalloc( sizeof( GUIStream ), "Launcher/GUIParser.cpp", 493 );
			m_pStream = s;
		}
		else
		{
			s = m_pStream;
			inflateEnd( &s->z );
		}


		m_iPos = 0;
		m_pText = 0;
		len -= func_00228DC0( D_003029F8 );
		text += func_00228DC0( D_003029F8 );
		s->z.avail_in = len;
		s->z.next_in = (Bytef *)text;
		s->z.zalloc = 0;
		s->z.zfree = 0;
		s->z.opaque = 0;
		s->z.next_out = (Bytef *)s->buf;
		s->z.avail_out = 0x800;
		inflateInit_( &s->z, "1.1.3", sizeof( z_stream ) );
		inflate( &s->z, Z_SYNC_FLUSH );
		s->pos = 0;
	}
	else
	{
		m_pText = text;
		m_pStream = 0;
		m_iPos = 0;
	}
#endif
}

/* NON_MATCHING: o ++pos fica em a0 no original (a1 no nosso) */
void CGUIParser :: Advance( void )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/launcher/GUIParser", Advance__10CGUIParser);
#else
	GUIStream *s = m_pStream;

	if ( s )
	{
		if ( ++s->pos == 0x800 )
		{
			s->z.avail_out = 0x800;
			s->pos = 0;
			s->z.next_out = (Bytef *)s->buf;
			if ( inflate( &s->z, Z_SYNC_FLUSH ) == Z_STREAM_END )
				inflateEnd( &s->z );
		}
	}
	else
		m_iPos++;
#endif
}
