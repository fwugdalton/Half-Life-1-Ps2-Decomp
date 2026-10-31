---
name: hl1ps2-dlls-done-3
description: continuação da lista de .cpp das dlls feitos (hornetgun em diante)
metadata:
  type: project
---
Continuação de [[hl1ps2-dlls-done-2]] (formato: arquivo (texto, commit, funções casadas)).

- hornetgun (0x18EDA0, 2566f06, 17/17): sem .data; .rodata 0x35D3C0 (o "hornet" em 0x35D3B8 é do hornet.cpp). houndeye.o precisou de .rodata=16 no Makefile. Versão antiga do SDK (gpGlobals->time, recarga fixa 0.5f, sem eventos). PS2: rumble func_002071C8(m_pPlayer, 1, RANDOM_LONG(50,200), 50) no primário e (50,180), 25 no secundário; AddToPlayer enche com g_pGameRules->Ps2Virt07(); secundário termina com punchangle.x = RANDOM_FLOAT(0,2). Registro "350" &CHornet::StartDart em func_0018FDD8; StartDart__7CHornet = 0x18DC28. Pegadinha: o cmpfile.sh deu tudo igual com velocidade 300 em vez de 1200 (ignora o imediato do lui de constante float); só o cmp da ROM pegou. Sempre confirmar com o build completo.
