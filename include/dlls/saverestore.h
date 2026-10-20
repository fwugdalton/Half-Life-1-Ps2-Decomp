#ifndef DLLS_SAVERESTORE_H
#define DLLS_SAVERESTORE_H

/* só as declarações usadas pelos cabeçalhos (saverestore.h do HLSDK) */

class CSave;
class CRestore;

typedef enum
{
    FIELD_FLOAT = 0,
    FIELD_STRING,
    FIELD_ENTITY,
    FIELD_CLASSPTR,
    FIELD_EHANDLE,
    FIELD_EVARS,
    FIELD_EDICT,
    FIELD_VECTOR,
    FIELD_POSITION_VECTOR,
    FIELD_POINTER,
    FIELD_INTEGER,
    FIELD_FUNCTION,
    FIELD_BOOLEAN,
    FIELD_SHORT,
    FIELD_CHARACTER,
    FIELD_TIME,
    FIELD_MODELNAME,
    FIELD_SOUNDNAME,

    FIELD_TYPECOUNT
} FIELDTYPE;

typedef struct
{
    FIELDTYPE fieldType;
    char *fieldName;
    int fieldOffset;
    short fieldSize;
    short flags;
} TYPEDESCRIPTION;

#endif
