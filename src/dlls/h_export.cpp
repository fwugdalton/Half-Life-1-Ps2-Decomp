/* dlls/h_export.cpp: entrada da dll do jogo; recebe do motor a tabela de
   funções e as variáveis globais. No PS2 g_engfuncs e gpGlobals ficam na
   .bss do executável, então aqui só fica a função. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "include_asm.h"

extern "C" void GiveFnptrsToDll(	enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;
}
