---
name: hl1ps2-github-repo
description: repo GitHub do decomp HL1 PS2 (fwugdalton/Half-Life-1-Ps2-Decomp), push direto na master
metadata:
  type: project
---
Em 2026-10-09 o Dalton criou https://github.com/fwugdalton/Half-Life-1-Ps2-Decomp (público). O repo local /home/claude/hl1ps2 tem `origin` apontando para ele e a master foi enviada (36 commits).

**Why:** antes não havia repo e a entrega era só o git bundle em /mnt/project-files/hl1ps2/hl1ps2.bundle.
**How to apply:** depois de cada commit verificado, `git push origin master` (o repo está no projeto; numa sessão nova use add_repo com access push). Continue gerando o bundle também. Nunca envie o ELF/ROM/asm (o .gitignore já os exclui). Ver [[hl1ps2-project-setup]] e [[hl1ps2-next-steps]].
