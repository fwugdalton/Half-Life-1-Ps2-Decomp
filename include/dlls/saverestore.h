#ifndef DLLS_SAVERESTORE_H
#define DLLS_SAVERESTORE_H

/* saverestore.h do HLSDK */

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

class CBaseEntity;

class CSaveRestoreBuffer
{
public:
    CSaveRestoreBuffer(void);
    CSaveRestoreBuffer(SAVERESTOREDATA *pdata);
    ~CSaveRestoreBuffer(void);

    int EntityIndex(entvars_t *pevLookup);
    int EntityIndex(edict_t *pentLookup);
    int EntityIndex(EOFFSET eoLookup);
    int EntityIndex(CBaseEntity *pEntity);

    int EntityFlags(int entityIndex, int flags) { return EntityFlagsSet(entityIndex, 0); }
    int EntityFlagsSet(int entityIndex, int flags);

    edict_t *EntityFromIndex(int entityIndex);

    unsigned short TokenHash(const char *pszToken);

protected:
    SAVERESTOREDATA *m_pdata;
    void BufferRewind(int size);
    unsigned int HashString(const char *pszToken);
};

class CSave : public CSaveRestoreBuffer
{
public:
    CSave(SAVERESTOREDATA *pdata) : CSaveRestoreBuffer(pdata) {};

    void WriteShort(const char *pname, const short *value, int count);
    void WriteInt(const char *pname, const int *value, int count);
    void WriteFloat(const char *pname, const float *value, int count);
    void WriteTime(const char *pname, const float *value, int count);
    void WriteData(const char *pname, int size, const char *pdata);
    void WriteString(const char *pname, const char *pstring);
    void WriteString(const char *pname, const int *stringId, int count);
    void WriteVector(const char *pname, const Vector &value);
    void WriteVector(const char *pname, const float *value, int count);
    void WritePositionVector(const char *pname, const Vector &value);
    void WritePositionVector(const char *pname, const float *value, int count);
    void WriteFunction(const char *pname, void **value, int count);
    int WriteEntVars(const char *pname, entvars_t *pev);
    int WriteFields(const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);

private:
    int DataEmpty(const char *pdata, int size);
    void BufferField(const char *pname, int size, const char *pdata);
    void BufferString(char *pdata, int len);
    void BufferData(const char *pdata, int size);
    void BufferHeader(const char *pname, int size);
};

typedef struct
{
    unsigned short size;
    unsigned short token;
    char *pData;
} HEADER;

class CRestore : public CSaveRestoreBuffer
{
public:
    CRestore(SAVERESTOREDATA *pdata) : CSaveRestoreBuffer(pdata) { m_global = 0; m_precache = TRUE; }
    int ReadEntVars(const char *pname, entvars_t *pev);
    int ReadFields(const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
    int ReadField(void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount, int startField, int size, char *pName, void *pData);
    int ReadInt(void);
    short ReadShort(void);
    int ReadNamedInt(const char *pName);
    char *ReadNamedString(const char *pName);
    int Empty(void) { return (m_pdata == NULL) || ((m_pdata->pCurrentData - m_pdata->pBaseData) >= m_pdata->bufferSize); }
    inline void SetGlobalMode(int global) { m_global = global; }
    void PrecacheMode(BOOL mode) { m_precache = mode; }

private:
    char *BufferPointer(void);
    void BufferReadBytes(char *pOutput, int size);
    void BufferSkipBytes(int bytes);
    int BufferSkipZString(void);
    int BufferCheckZString(const char *string);

    void BufferReadHeader(HEADER *pheader);

    int m_global;
    BOOL m_precache;
};

#define MAX_ENTITYARRAY 64

#define _FIELD(type, name, fieldtype, count, flags) { fieldtype, #name, (int)&(((type *)0)->name), count, flags }
#define DEFINE_FIELD(type, name, fieldtype) _FIELD(type, name, fieldtype, 1, 0)
#define DEFINE_ARRAY(type, name, fieldtype, count) _FIELD(type, name, fieldtype, count, 0)
#define FTYPEDESC_GLOBAL 0x0001
#define DEFINE_ENTITY_FIELD(name, fieldtype) _FIELD(entvars_t, name, fieldtype, 1, 0)
#define DEFINE_ENTITY_GLOBAL_FIELD(name, fieldtype) _FIELD(entvars_t, name, fieldtype, 1, FTYPEDESC_GLOBAL)
#define DEFINE_GLOBAL_FIELD(type, name, fieldtype) _FIELD(type, name, fieldtype, 1, FTYPEDESC_GLOBAL)

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
