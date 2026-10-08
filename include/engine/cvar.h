#ifndef ENGINE_CVAR_H
#define ENGINE_CVAR_H

#include "common.h"

#define FCVAR_ARCHIVE 1        /* salvo no config */
#define FCVAR_USERINFO 2       /* vai para o userinfo do cliente */
#define FCVAR_SERVER 4         /* avisa os jogadores quando muda */
#define FCVAR_EXTDLL 8         /* definido pela dll do jogo */
#define FCVAR_CLIENTDLL 16     /* definido pela dll do cliente */
#define FCVAR_SPONLY 64        /* só pode mudar no single player */
#define FCVAR_PRINTABLEONLY 128 /* só caracteres imprimíveis */
#define FCVAR_PLAYER 256       /* bits 16-23 dizem a qual jogador (tela dividida) pertence */

typedef struct cvar_s
{
    char *name;
    char *string;
    int flags;
    float value;
    struct cvar_s *next;
} cvar_t;

extern cvar_t *cvar_vars;

cvar_t *Cvar_FindVar(char *var_name);
float Cvar_VariableValue(char *var_name);
int Cvar_VariableInt(char *var_name);
char *Cvar_VariableString(char *var_name);
char *Cvar_CompleteVariable(char *partial);
void Cvar_Set(char *var_name, char *value);
void Cvar_SetValue(char *var_name, float value);
void Cvar_RegisterVariable(cvar_t *variable);
qboolean Cvar_Command(void);

#endif /* ENGINE_CVAR_H */
