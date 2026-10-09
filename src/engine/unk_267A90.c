#include "common.h"

/* carga de modelos de estúdio (.mdl): no PS2 as texturas ficam no próprio arquivo
 * em cache e são reenviadas para a GS quando o cache é restaurado */

#define mod_studio 3

#define GL_UNSIGNED_BYTE 0x1401
#define GL_COLOR_INDEX 0x1900
#define GL_RGBA 0x1908
#define GL_TEXTURE_2D 0xDE1
#define GL_COLOR_INDEX8_EXT 0x8801 /* valor do PS2 */

typedef struct
{
	char name[64];
	int needload;           /* 0x40 */
	int type;               /* 0x44 */
	byte unk_48[0x38];
	int flags;              /* 0x80 */
	byte unk_84[0x110];
	cache_user_t cache;     /* 0x194 */
} model_t;

typedef struct
{
	byte unk_00[0x88];
	int flags;              /* 0x88 */
	byte unk_8C[0x28];
	int numtextures;        /* 0xB4 */
	int textureindex;       /* 0xB8 */
} studiohdr_t;

typedef struct
{
	char name[60];
	int texture;            /* 0x3C: no PS2 o fim do nome guarda o id da textura */
	int flags;              /* 0x40 */
	int width;              /* 0x44 */
	int height;             /* 0x48 */
	int index;              /* 0x4C */
} mstudiotexture_t;

/* textura dentro do .mdl do PS2 */
typedef struct
{
	byte unk_00[0x13];
	char nummips;              /* 0x13 */
	int type;               /* 0x14: 2 = 8 bits com paleta, 5 = RGBA */
	unsigned short width;   /* 0x18 */
	unsigned short height;  /* 0x1A */
	byte unk_1C[4];
	byte palette[0x400];    /* 0x20 */
	byte pixels[1];         /* 0x420 */
} studiotex_t;

typedef void (*cachecallback_t)(cache_user_t *c, unsigned int mode);

int GL_LoadDecalTexture(char *name, int type, int width, int height, byte *pixels, byte *palette, byte nummips);
void GL_Bind(int texnum);
void func_002AF9A0(int texnum);
void glTexImage2D(int target, int level, int internalformat, int width, int height, int border, int format, int type, void *pixels);
void *COM_LoadFileCached(char *path, cache_user_t *cu, cachecallback_t callback);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);

void Mod_StudioCacheCallback(cache_user_t *c, unsigned int mode);

extern void (*qglColorTableEXT)(int target, int internalformat, int width, int format, int type, void *table);

void Mod_LoadStudioModel(model_t *mod, void *buffer)
{
	studiohdr_t *phdr;
	mstudiotexture_t *ptexture;
	studiotex_t *tex;
	byte *pal;
	byte *data;
	int i;
	char name[256];

	phdr = (studiohdr_t *)buffer;
	mod->type = mod_studio;
	mod->flags = phdr->flags;
	mod->cache.data = phdr;
	ptexture = (mstudiotexture_t *)((byte *)phdr + phdr->textureindex);

	if (phdr->textureindex)
	{
		for (i = 0; i < phdr->numtextures; i++)
		{
			tex = (studiotex_t *)((byte *)phdr + ptexture[i].index);
			pal = tex->palette;
			data = tex->pixels;
			strcpy(name, mod->name);
			strcat(name, ptexture[i].name);
			ptexture[i].texture = GL_LoadDecalTexture(name, 3, tex->width, tex->height, data, pal, tex->nummips);
		}
	}
}

#ifdef NON_MATCHING
void Mod_StudioCacheCallback(cache_user_t *c, unsigned int mode)
{
	if (!c)
		return;

	switch (mode)
	{
	case 0:
		break;

	case 1:
	{
		studiohdr_t *phdr;
		mstudiotexture_t *ptexture;
		studiotex_t *tex;
		byte *pixels;
		byte *pal;
		byte *data;
		int i, j, nummips, w, h;

		phdr = (studiohdr_t *)c->data;
		if (!phdr->textureindex)
			break;
		ptexture = (mstudiotexture_t *)((byte *)phdr + phdr->textureindex);
		for (i = 0; i < phdr->numtextures; i++)
		{
			pixels = NULL;
			tex = (studiotex_t *)((byte *)c->data + ptexture[i].index);
			GL_Bind(ptexture[i].texture);
			pal = tex->palette;
			data = tex->pixels;
			if (tex->type == 2)
			{
				qglColorTableEXT(GL_TEXTURE_2D, GL_RGBA, 256, GL_COLOR_INDEX8_EXT, GL_UNSIGNED_BYTE, pal);
				glTexImage2D(GL_TEXTURE_2D, 0, 1, tex->width, tex->height, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, data);
				pixels = (byte *)ptexture + (tex->width * tex->height + 0x400);
			}
			else if (tex->type == 5)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, 1, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				pixels = (byte *)ptexture + tex->width * tex->height * 4;
			}
			nummips = tex->nummips + 1;
			for (j = 1; j < nummips; j++)
			{
				w = tex->width >> j;
				h = tex->height >> j;
				glTexImage2D(GL_TEXTURE_2D, j, 1, w, h, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, pixels);
				pixels += w * h;
			}
		}
		break;
	}

	case 2:
	{
		studiohdr_t *phdr;
		mstudiotexture_t *ptexture;
		int i;

		phdr = (studiohdr_t *)c->data;
		if (!phdr->textureindex)
			break;
		ptexture = (mstudiotexture_t *)((byte *)phdr + phdr->textureindex);
		for (i = 0; i < phdr->numtextures; i++)
		{
			func_002AF9A0(ptexture[i].texture);
			ptexture[i].texture = 0;
		}
		break;
	}
	}
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/unk_267A90", Mod_StudioCacheCallback);
#endif

void Mod_LoadStudioModelCached(model_t *mod)
{
	COM_LoadFileCached(mod->name, &mod->cache, Mod_StudioCacheCallback);
	Mod_LoadStudioModel(mod, mod->cache.data);
}
