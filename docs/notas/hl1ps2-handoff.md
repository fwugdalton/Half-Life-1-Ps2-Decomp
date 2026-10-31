---
name: hl1ps2-handoff
description: projeto pausado em 2026-10-28; instruções de retomada em LEIA_PRIMEIRO_INSTRUCOES_PARA_IA.md no repo
metadata:
  type: project
---
Em 2026-10-28 o Dalton pausou o decomp do HL1 PS2 e pediu instruções de retomada legíveis por qualquer LLM. Elas estão em `LEIA_PRIMEIRO_INSTRUCOES_PARA_IA.md` na raiz do repo (commit 5f59d99), com CLAUDE.md, AGENTS.md e o topo do README apontando para ele. As notas desta memória foram copiadas para `docs/notas/` e os scripts da sessão (build completo, commit seguro, cmp por arquivo, datacmp, wrapnm, vtry) para `tools/handoff/`.

**Why:** a memória e o scratchpad só existem para o Claude; outra LLM precisa achar tudo no repo.
**How to apply:** ao retomar, ler esse arquivo primeiro; próximo arquivo é o do item_generic (hornetgun até handgrenade, h_*, gman, glock, globals, ggrenade e genericmonster feitos em 2026-10-31) (ver [[hl1ps2-dlls]]). Ao mudar notas aqui, atualizar também docs/notas no repo.
