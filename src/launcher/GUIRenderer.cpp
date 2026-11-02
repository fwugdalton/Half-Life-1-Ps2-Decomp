/* Launcher (Gearbox, sem fonte): cache das texturas do GUI (CGUIRenderer, 0x15E8 bytes, 50
   texturas lidas do pak do GUI e mandadas ao GL). Nome do arquivo inventado. */

#include "launcher/GUI.h"

extern "C" {
char *strcpy(char *dst, const char *src);
int func_00235280(pakfile_t *pak, const char *name);	/* índice do arquivo no pak */
void func_00101450(void *data, void *img);	/* lê o cabeçalho da imagem */
void func_001014B8(void *img);
void GL_Bind(int texnum);
void glTexImage2D(int target, int level, int internalformat, int width, int height, int border, int format, int type, void *pixels);
void func_0023C450(int target, int internalformat, int width, int format, int type, void *table);	/* glColorTableEXT */
void func_0023C600(int target, int pname, float param);	/* glTexParameterf */
void func_0023C7C8(int n, int *textures);	/* glDeleteTextures */
void func_0023C7E8(int n, int *textures);	/* glGenTextures */
}

#define GL_TEXTURE_2D 0x0DE1
#define GL_UNSIGNED_BYTE 0x1401
#define GL_COLOR_INDEX 0x1900
#define GL_RGBA 0x1908
#define GL_COLOR_FORMAT_PS2 0x8801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_LINEAR 9729.0f

struct GUIImage
{
	int width;
	int height;
	void *palette;
	void *pixels;
	int paletted;
};

struct GUITexture
{
	char name[0x64];
	int texnum;
	int width;
	int height;
};

class CGUIRenderer
{
public:
	CGUIRenderer();
	~CGUIRenderer();
	void Init( pakfile_t *pak );
	int GetTexture( const char *name );
	int GetWidth( const char *name );
	int GetHeight( const char *name );
	void Bind( int texnum );
	void Load( const char *name, int *texnum, int *width, int *height );

	int m_bInitialized;
	pakfile_t *m_pPak;
	GUITexture m_Textures[50];
};

CGUIRenderer :: CGUIRenderer()
{
	m_bInitialized = 0;
}

CGUIRenderer :: ~CGUIRenderer()
{
	int i;

	m_bInitialized = 0;
	for ( i = 0; i < 50; i++ )
	{
		if ( m_Textures[i].name[0] )
		{
			int t = m_Textures[i].texnum;
			func_0023C7C8( 1, &t );
		}
	}
}

void CGUIRenderer :: Init( pakfile_t *pak )
{
	int i;

	for ( i = 0; i < 50; i++ )
	{
		m_Textures[i].texnum = 0;
		m_Textures[i].name[0] = 0;
	}
	m_pPak = pak;
	m_bInitialized = 1;
}

int CGUIRenderer :: GetTexture( const char *name )
{
	int i;
	int freeSlot = -1;
	int tex = -1;

	for ( i = 0; i < 50; i++ )
	{
		if ( func_00228C00( m_Textures[i].name, name ) == 1 )
		{
			tex = m_Textures[i].texnum;
			break;
		}
		if ( freeSlot == -1 && !m_Textures[i].name[0] )
			freeSlot = i;
	}

	if ( i == 50 && freeSlot != -1 )
	{
		int t, w, h;
		Load( name, &t, &w, &h );
		tex = t;
		m_Textures[freeSlot].width = w;
		m_Textures[freeSlot].height = h;
		m_Textures[freeSlot].texnum = tex;
		strcpy( m_Textures[freeSlot].name, name );
	}
	return tex;
}

int CGUIRenderer :: GetWidth( const char *name )
{
	int i;
	int r = -1;

	for ( i = 0; i < 50; i++ )
	{
		if ( func_00228C00( m_Textures[i].name, name ) == 1 )
		{
			r = m_Textures[i].width;
			break;
		}
	}
	return r;
}

int CGUIRenderer :: GetHeight( const char *name )
{
	int i;
	int r = -1;

	for ( i = 0; i < 50; i++ )
	{
		if ( func_00228C00( m_Textures[i].name, name ) == 1 )
		{
			r = m_Textures[i].height;
			break;
		}
	}
	return r;
}

void CGUIRenderer :: Bind( int texnum )
{
	GL_Bind( texnum );
}

void CGUIRenderer :: Load( const char *name, int *texnum, int *width, int *height )
{
	GUIImage img;
	int idx = func_00235280( m_pPak, name );

	byte *data = m_pPak->data + m_pPak->lumps[idx].filepos;
	img.pixels = 0;
	func_00101450( data, &img );
	*width = img.width;
	*height = img.height;
	func_0023C7E8( 1, texnum );
	Bind( *texnum );
	if ( img.paletted )
	{
		func_0023C450( GL_TEXTURE_2D, GL_RGBA, 256, GL_COLOR_FORMAT_PS2, GL_UNSIGNED_BYTE, img.palette );
		glTexImage2D( GL_TEXTURE_2D, 0, 1, img.width, img.height, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, img.pixels );
	}
	else
		glTexImage2D( GL_TEXTURE_2D, 0, 1, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.pixels );
	func_0023C600( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	func_0023C600( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	func_001014B8( &img );
}
