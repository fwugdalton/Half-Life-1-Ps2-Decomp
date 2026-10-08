#ifndef ENGINE_SERVER_H
#define ENGINE_SERVER_H

#include "common.h"

/* parcial: só os campos já identificados */
typedef struct server_s
{
    qboolean active;
} server_t;

extern server_t sv;

#endif /* ENGINE_SERVER_H */
