---
name: hl1ps2-dlls-osprey
description: osprey.cpp e pathcorner.cpp (faixas, mudanças do PS2, NON_MATCHING) e regras do fixasm sobre mtc1/mfc1
metadata:
  type: project
---
pathcorner.cpp (texto 0x1BC3D0–0x1BD368, .data 0x317718, .rodata 0x368A80): HLSDK puro, tudo casa (commit feb6eae). O plats.o passou a ter .rodata alinhado em 16 no Makefile.

osprey.cpp (texto 0x1B97E8–0x1BC3D0, .data 0x3175D8, .rodata 0x368350–0x368A80; commit 03470db):
- PS2: takedamage = DAMAGE_AIM no Spawn; MakeGrunt usa UTIL_TraceLineNoEnts; BEAM_FSOLID é 0x20; Killed dispara m_iszTriggerTarget com AITRIGGER_DEATH (igual turret.cpp); pitch do Flight é int; TraceAttack usa 3 + (int)(flDamage / 5.0f).
- Função de registro func_001BC1B0 ("212".."218") depois do TraceAttack.
- effects.h ganhou CBeam::SetFlags. As constantes PMF 0x368380 (CSprite::AnimateUntilDead, CBaseEntity::SUB_Remove) vêm das inline do effects.h.
- NON_MATCHING: MakeGrunt (o gcc guarda sp+64 em $s8), Flight e TraceAttack (escalonamento; no ROM não há nop depois do mfc1 ali, mas outros arquivos têm: NÃO criar regra de fixasm para mfc1, quebra ~30 arquivos).
- fixasm: mtc1 antes de rótulo cuja instrução está num bloco noreorder (jal) também vira .word.
- Ao mudar o fixasm, apague os .o (o Makefile não depende dele) e confira com o full2.sh. Para testar o efeito de uma regra: rodar fixasm antigo e novo em todos os build/src/**/*.s0, montar e comparar o .text.
- Cuidado: git stash -- arquivo guarda o arquivo modificado; use git show HEAD:arquivo para pegar a versão antiga.
