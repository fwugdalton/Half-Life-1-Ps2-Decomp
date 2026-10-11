#include "common.h"
#include "engine/cmd.h"
#include "engine/model.h"

void *Mem_DbgCalloc(int num, int size, const char *file, int line);

extern byte mod_novis[MODEL_MAX_PVS];
extern byte cm_decompressed[MODEL_MAX_PVS];
extern int gPVSRowBytes;
extern model_t *sv_worldmodel;

byte *gPAS = NULL;
byte *gPVS = NULL;
int D_00331C14 = 0; /* sem referências; pode ser de outro arquivo */

void Mod_Init(void)
{
    memset(mod_novis, 0xFF, sizeof(mod_novis));
}

void CM_DecompressPVS(byte *in, byte *decompressed, int byteCount);
byte *CM_LeafPVS(int leafnum);

byte *Mod_DecompressVis(byte *in, model_t *model)
{
    int row;

    row = (model->numleafs + 7) >> 3;
    if (!in)
        return mod_novis;

    CM_DecompressPVS(in, cm_decompressed, row);
    return cm_decompressed;
}

byte *Mod_LeafPVS(mleaf_t *leaf, model_t *model)
{
    if (leaf == model->leafs)
        return mod_novis;

    if (gPVS)
        return CM_LeafPVS(leaf - model->leafs);

    return Mod_DecompressVis(leaf->compressed_vis, model);
}

/* a versão em C troca os registradores de in/out (in em v1, out em a0) */
#ifdef NON_MATCHING
void CM_DecompressPVS(byte *in, byte *decompressed, int byteCount)
{
    int c;
    byte *out;

    if (!in)
    {
        memcpy(decompressed, mod_novis, byteCount);
        return;
    }

    out = decompressed;
    do
    {
        if (*in)
        {
            *out++ = *in++;
            continue;
        }

        c = in[1];
        in += 2;
        while (c)
        {
            *out++ = 0;
            c--;
        }
    } while (out < decompressed + byteCount);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cmodel", CM_DecompressPVS);
#endif

byte *CM_LeafPVS(int leafnum)
{
    if (!gPVS)
        return mod_novis;
    return &gPVS[leafnum * gPVSRowBytes];
}

byte *CM_LeafPAS(int leafnum)
{
    if (!gPAS)
        return mod_novis;
    return &gPAS[leafnum * gPVSRowBytes];
}

void CM_FreePAS(void)
{
    if (gPAS)
        Mem_Free(gPAS);
    if (gPVS)
        Mem_Free(gPVS);
    gPAS = NULL;
    gPVS = NULL;
}

/* só difere na alocação de registradores temporários (t4-t7) */
void CM_CalcPAS(model_t *pModel)
{
    int rows, rowwords;
    int actualRowBytes;
    int i, j, k, l;
    int index;
    int bitbyte;
    unsigned int *dest, *src;
    byte *scan;
    int count, vcount, acount;

    Con_DPrintf("Building PAS...\n");
    CM_FreePAS();

    rows = (pModel->numleafs + 7) >> 3;
    count = pModel->numleafs + 1;
    actualRowBytes = (rows + 3) & ~3;
    gPVSRowBytes = actualRowBytes;
    rowwords = actualRowBytes >> 2;

    gPVS = Mem_DbgCalloc(gPVSRowBytes, count, "engine/cmodel.c", 160);

    scan = gPVS;
    vcount = 0;
    for (i = 0; i < count; i++, scan += gPVSRowBytes)
    {
        CM_DecompressPVS(pModel->leafs[i].compressed_vis, scan, rows);
        if (i == 0)
            continue;

        for (j = 0; j < count; j++)
        {
            if (scan[j >> 3] & (1 << (j & 7)))
                vcount++;
        }
    }

    gPAS = Mem_DbgCalloc(gPVSRowBytes, count, "engine/cmodel.c", 179);

    acount = 0;
    scan = gPVS;
    dest = (unsigned int *)gPAS;
    for (i = 0; i < count; i++, dest += rowwords, scan += gPVSRowBytes)
    {
        memcpy(dest, scan, gPVSRowBytes);

        for (j = 0; j < gPVSRowBytes; j++)
        {
            bitbyte = scan[j];
            if (!bitbyte)
                continue;

            for (k = 0; k < 8; k++)
            {
                if (!(bitbyte & (1 << k)))
                    continue;

                index = (j << 3) + k + 1;
                if (index >= count)
                    continue;
                if (index <= 0)
                    continue;

                src = (unsigned int *)&gPVS[index * gPVSRowBytes];
                for (l = 0; l < rowwords; l++)
                    dest[l] |= src[l];
            }
        }

        if (i == 0)
            continue;

        for (j = 0; j < count; j++)
        {
            if (((byte *)dest)[j >> 3] & (1 << (j & 7)))
                acount++;
        }
    }

    Con_DPrintf("Average leaves visible / audible / total: %i / %i / %i\n", vcount / count, acount / count, count);
}

qboolean CM_HeadnodeVisible(mnode_t *node, byte *visbits)
{
    int leafnum;

    if (!node || node->contents == CONTENTS_SOLID)
        return 0;

    if (node->contents < 0)
    {
        leafnum = ((mleaf_t *)node - sv_worldmodel->leafs) - 1;
        return (visbits[leafnum >> 3] >> (leafnum & 7)) & 1;
    }

    if (CM_HeadnodeVisible(node->children[0], visbits))
        return 1;

    return CM_HeadnodeVisible(node->children[1], visbits);
}
