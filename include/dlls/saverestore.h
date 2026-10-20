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

class CSave
{
public:
    int WriteFields(const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
};

class CRestore
{
public:
    int ReadFields(const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
};

#define _FIELD(type, name, fieldtype, count, flags) { fieldtype, #name, (int)&(((type *)0)->name), count, flags }
#define DEFINE_FIELD(type, name, fieldtype) _FIELD(type, name, fieldtype, 1, 0)
#define DEFINE_ARRAY(type, name, fieldtype, count) _FIELD(type, name, fieldtype, count, 0)

#define IMPLEMENT_SAVERESTORE(derivedClass, baseClass) \
    int derivedClass::Save(CSave &save) \
    { \
        if (!baseClass::Save(save)) \
            return 0; \
        return save.WriteFields(#derivedClass, this, m_SaveData, ARRAYSIZE(m_SaveData)); \
    } \
    int derivedClass::Restore(CRestore &restore) \
    { \
        if (!baseClass::Restore(restore)) \
            return 0; \
        return restore.ReadFields(#derivedClass, this, m_SaveData, ARRAYSIZE(m_SaveData)); \
    }

#endif
