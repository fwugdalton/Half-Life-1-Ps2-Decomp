---
name: hl1ps2-dlls
description: dlls do jogo (HLSDK C++) em src/dlls: headers, flags, vtables, como integrar um .cpp (zombie.cpp feito)
metadata:
  type: project
  modified: 2026-10-20T01:20:59.937Z
---
Código do jogo (dlls HLSDK, gcc 2.95 C++) fica no bloco asm cod/000000 (rom 0x0–0x117280), arquivos em ordem alfabética, feitos de trás pra frente a partir do zombie.cpp; lista dos arquivos feitos em [[hl1ps2-dlls-done]] (até [[hl1ps2-dlls-done-9]]). Próximo: ver [[hl1ps2-dlls-done-9]] (texto termina em 0x154480). Fonte do HLSDK que falta em /tmp/claude-0/hlsdk: baixar de raw.githubusercontent.com/ValveSoftware/halflife/master/dlls/. Lições por arquivo: [[hl1ps2-dlls-osprey]], [[hl1ps2-dlls-nodes]], [[hl1ps2-dlls-nihilanth]], [[hl1ps2-dlls-multiplay]], [[hl1ps2-dlls-hgrunt]], [[hl1ps2-dlls-healthkit]]. player.cpp (0x1C3350–0x1D3508, ~180 funções, .rodata 0x36AF40–0x36DCC8) ficou para depois por ser enorme; seu asm está em asm/cod/0C3350.s.

Headers em include/dlls (extdll, vector, util, cbase, basemonster, monsters, skill, gamerules...).
- `long` é 64 bits no ee-gcc: no enginefuncs use `int` (RandomLong etc.), senão aparece dsll32/dsra32.
- edict_t: pvPrivateData 0xB4, v 0xB8. CGameRules: vptr em 0x10, IsMultiplayer é o 10º virtual (offset 0x50); GetGameDescription inline devolve "Half-Life" (string aparece no .rodata de todo arquivo que inclui gamerules.h, junto com "classname"/"targetname"/"target" de util.h).
- Constantes float: escrever 0.3f (0.3 gera conversão double).
- Makefile: src/dlls/%.o usa -ffast-math (sqrt.s direto) e `objcopy -R .bss` (o objeto estático do LINK_ENTITY_TO_CLASS, 1 byte, já está no .bss em asm).
- vtable: entradas de 8 bytes {short delta; short idx; pfn}, slot = (off-8)/8+1. CBaseEntity tem virtual extra Ps2Unk58, CBaseMonster tem Ps2Unk127.

Lições do xen.cpp:
- virtuais inline (corpo na classe) só saem junto com a vtable da classe (arquivo da 1ª virtual não inline); por isso CBaseEntity::Spawn etc. são símbolos externos. CPointEntity::ObjectCaps é não inline no PS2.
- constantes de ponteiro para membro de funções inline sem uso (SetThink em CSprite::AnimateAndDie, CBeam::LiveForTime) vão para o .rodata do arquivo: incluir effects.h coloca 0x10 bytes antes das strings.
- função inline chamada antes de ser definida no corpo da classe (CXenSpore::Attack) não é inlinada e sai como weak no fim do .text.
- .data das dlls alinhado em 8 (objcopy no Makefile).

Lições do wheelchair.cpp:
- fim de cada arquivo de jogo: função de registro `extern "C" void func_X(void) { func_00239810("345", static_cast<USEPTR>(&CTalkMonster::FollowerUse)); }` (2397D0 Think, 2397F0 Touch, 239810 Use).
- HasConditions é chamada fora de linha (0x1A8BE8), mas no GetSchedule casou com `inline BOOL CBaseMonster::HasConditions(...)` definida no .cpp logo antes da função (gera movz em alguns ifs).
- FRefreshRoute no PS2 recebe um int extra (a1=0): `BOOL FRefreshRoute(int iUnk = 0)`.
- if/else de SetUse invertido: `if (!(flags)) SetUse(Follower); else SetUse(NULL);`.
- valores de .data (máscaras de som) podem diferir do HLSDK: comparar a ROM byte a byte (cmp -l) se só a ROM falhar.
- um engine .o referenciava D_00382E30 (no meio do rodata do arquivo): definido em linker_script_extra.ld.
- headers novos: talkmonster.h, soundent.h, defaultai.h, effects.h; enums completos em schedule.h.

Integração de um .cpp:
1. yaml: `[rom, cpp, dlls/x]` no texto, `.data` e `.rodata` do arquivo (rodata começa no "classname" do arquivo; vtable vem depois das strings e o nome da entidade no fim).
2. `python3 tools/resolveundef.py build/src/dlls/x.o .text=VRAM .rodata=VRAM .data=VRAM` gera os endereços dos símbolos indefinidos (mangled) a partir das relocações; somar ao symbol_addrs.txt também os símbolos definidos (nm T/D/V). Construtores inline (ex. __12CBaseMonster=0x1242A8) são weak: nomear a primeira cópia.
3. difffunc/cmpfunc aceitam `nome=0xADDR`.

Ver [[hl1ps2-gbx-cpp]] para mangling do gcc 2.95.
