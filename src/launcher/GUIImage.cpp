/* Launcher (Gearbox, sem fonte): cabeçalho das imagens .psi do pak do GUI, usado pelo
   CGUIRenderer (tipo 2: 8 bits com paleta de 256 cores em 0x20 e pixels em 0x420; tipo 5:
   RGBA em 0x20). Nome do arquivo inventado. */

#include "launcher/GUI.h"

struct GUIImage
{
	int width;
	int height;
	void *palette;
	void *pixels;
	int paletted;
};

typedef struct
{
	char pad[0x14];
	int type;					/* 0x14 */
	unsigned short width;		/* 0x18 */
	unsigned short height;		/* 0x1A */
	char pad2[4];
	byte palette[0x400];		/* 0x20 */
	byte pixels[1];				/* 0x420 */
} GUIImageHeader;

extern "C" {

void func_00101450( GUIImageHeader *data, GUIImage *img )
{
	img->pixels = data;
	switch ( data->type )
	{
	case 2:
		img->paletted = 1;
		img->palette = data->palette;
		img->pixels = data->pixels;
		break;
	case 5:
		img->paletted = 0;
		img->pixels = data->palette;
		img->palette = 0;
		break;
	}
	img->width = data->width;
	img->height = data->height;
}

void func_001014B8( GUIImage *img )
{
}

}
