#include "common.h"
#include "engine/cmd.h"

int strlen(const char *);
void *memcpy(void *, const void *, unsigned int);
char *strncpy(char *, const char *, unsigned int);
int strncasecmp(const char *, const char *, unsigned int);
double atof(const char *);
typedef struct client_textmessage_s
{
    int effect;
    byte r1, g1, b1, a1;
    byte r2, g2, b2, a2;
    float x;
    float y;
    float fadein;
    float fadeout;
    float holdtime;
    float fxtime;
    const char *pName;
    const char *pMessage;
} client_textmessage_t; /* 0x2C */

#define MAX_MESSAGES 400
#define NAME_HEAP_SIZE 8192
#define MSGFILE_NAME 0
#define MSGFILE_TEXT 1

/* os nomes "__NETMESSAGE__N" ficam no começo do .rodata (em ordem inversa): são os
   inicializadores de gNetworkTextMessage[].pName, cujo .data ainda está em asm */
extern const char D_003901F8[], D_00390208[], D_00390218[], D_00390228[];
#define NETWORK_MESSAGE1 D_00390228
#define NETWORK_MESSAGE2 D_00390218
#define NETWORK_MESSAGE3 D_00390208
#define NETWORK_MESSAGE4 D_003901F8

extern client_textmessage_t gMessageParms;
extern client_textmessage_t *gMessageTable;
extern int gMessageTableCount;
extern client_textmessage_t gNetworkTextMessage[];

char *strcpy(char *, const char *);
int strcmp(const char *, const char *);
int strcasecmp(const char *, const char *);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *p);
byte *COM_LoadFileForMe(char *filename, int *pLength);



char *func_0028B350(byte *pMemFile, int fileSize, int *pFilePos, char *pBuffer, int bufferSize)
{
    int filePos = *pFilePos;
    int i, last, stop;
    int size;

    if (!pMemFile || !pBuffer)
        return NULL;

    if (filePos >= fileSize)
        return NULL;

    i = filePos;
    last = fileSize;

    if (last - filePos > bufferSize - 1)
        last = filePos + (bufferSize - 1);

    stop = 0;

    while (i < last && !stop)
    {
        if (pMemFile[i] == '\n')
            stop = 1;
        i++;
    }

    if (i != filePos)
    {
        size = i - filePos;
        memcpy(pBuffer, pMemFile + filePos, size);

        if (size < bufferSize)
            pBuffer[size] = 0;

        *pFilePos = i;
        return pBuffer;
    }

    return NULL;
}

int func_0028B438(char *pText)
{
    int length;

    if (pText)
    {
        length = strlen(pText);

        if (length >= 2 && pText[0] == '/' && pText[1] == '/')
            return 1;

        if (length > 0)
            return 0;
    }

    return 1;
}

int func_0028B498(char *pText)
{
    if (pText)
    {
        if (pText[0] == '{')
            return 1;
    }
    return 0;
}

int func_0028B4B8(char *pText)
{
    if (pText)
    {
        if (pText[0] == '}')
            return 1;
    }
    return 0;
}

int func_0028B4D8(char space)
{
    if (space == ' ' || space == '\t' || space == '\r' || space == '\n')
        return 1;
    return 0;
}

const char *func_0028B518(const char *pText)
{
    int pos;

    if (pText)
    {
        pos = 0;
        while (pText[pos] && func_0028B4D8(pText[pos]))
            pos++;
        return pText + pos;
    }
    return NULL;
}

const char *func_0028B580(const char *pText)
{
    int pos;

    if (pText)
    {
        pos = 0;
        while (pText[pos] && !func_0028B4D8(pText[pos]))
            pos++;
        return pText + pos;
    }
    return NULL;
}

int func_0028B5F0(const char *pText, float *pFloat, int count)
{
    const char *pTemp = pText;
    int index = 0;

    while (pTemp && count > 0)
    {
        pTemp = func_0028B580(pTemp);
        pTemp = func_0028B518(pTemp);
        if (pTemp)
        {
            pFloat[index] = (float)atof(pTemp);
            count--;
            index++;
        }
    }

    if (count == 0)
        return 1;

    return 0;
}

void func_0028B670(const char *source, char *dest)
{
    int start, end, length;

    start = 0;
    end = strlen(source);

    while (source[start] && func_0028B4D8(source[start]))
        start++;

    end--;
    while (end > 0 && func_0028B4D8(source[end]))
        end--;

    end++;

    length = end - start;
    if (length > 0)
        strncpy(dest, source + start, length);
    else
        length = 0;

    dest[length] = 0;
}

int func_0028B748(const char *pText, const char *pTokenName)
{
    if (!pText || !pTokenName)
        return 0;

    if (!strncasecmp(pText + 1, pTokenName, strlen(pTokenName)))
        return 1;

    return 0;
}

INCLUDE_RODATA("asm/nonmatchings/engine/tmessage", D_003901F8);

INCLUDE_RODATA("asm/nonmatchings/engine/tmessage", D_00390208);

INCLUDE_RODATA("asm/nonmatchings/engine/tmessage", D_00390218);

INCLUDE_RODATA("asm/nonmatchings/engine/tmessage", D_00390228);

int func_0028B7A0(const char *pText)
{
    float tempFloat[8];

    if (pText && pText[0] == '$')
    {
        if (func_0028B748(pText, "position"))
        {
            if (func_0028B5F0(pText, tempFloat, 2))
            {
                gMessageParms.x = tempFloat[0];
                gMessageParms.y = tempFloat[1];
            }
        }
        else if (func_0028B748(pText, "effect"))
        {
            if (func_0028B5F0(pText, tempFloat, 1))
                gMessageParms.effect = (int)tempFloat[0];
        }
        else if (func_0028B748(pText, "fxtime"))
        {
            if (func_0028B5F0(pText, tempFloat, 1))
                gMessageParms.fxtime = tempFloat[0];
        }
        else if (func_0028B748(pText, "color2"))
        {
            if (func_0028B5F0(pText, tempFloat, 3))
            {
                gMessageParms.r2 = (int)tempFloat[0];
                gMessageParms.g2 = (int)tempFloat[1];
                gMessageParms.b2 = (int)tempFloat[2];
            }
        }
        else if (func_0028B748(pText, "color"))
        {
            if (func_0028B5F0(pText, tempFloat, 3))
            {
                gMessageParms.r1 = (int)tempFloat[0];
                gMessageParms.g1 = (int)tempFloat[1];
                gMessageParms.b1 = (int)tempFloat[2];
            }
        }
        else if (func_0028B748(pText, "fadein"))
        {
            if (func_0028B5F0(pText, tempFloat, 1))
                gMessageParms.fadein = tempFloat[0];
        }
        else if (func_0028B748(pText, "fadeout"))
        {
            if (func_0028B5F0(pText, tempFloat, 3))
                gMessageParms.fadeout = tempFloat[0];
        }
        else if (func_0028B748(pText, "holdtime"))
        {
            if (func_0028B5F0(pText, tempFloat, 3))
                gMessageParms.holdtime = tempFloat[0];
        }
        else
        {
            Con_DPrintf("Unknown token: %s\n", pText);
        }

        return 1;
    }

    return 0;
}

void func_0028BA00(byte *pMemFile, int fileSize)
{
    char buf[512], trim[512];
    char *pCurrentText = 0, *pNameHeap;
    char currentName[512], nameHeap[NAME_HEAP_SIZE];
    int lastNamePos;
    int mode = 0;
    int lineNumber, filePos, lastLinePos;
    int messageCount;
    client_textmessage_t textMessages[MAX_MESSAGES];
    int i, nameHeapSize, textHeapSize, messageSize, nameOffset;
    int length;

    lastNamePos = 0;
    lineNumber = 0;
    filePos = 0;
    lastLinePos = 0;
    messageCount = 0;

    while (func_0028B350(pMemFile, fileSize, &filePos, buf, 512) != NULL)
    {
        func_0028B670(buf, trim);
        switch (mode)
        {
        case MSGFILE_NAME:
            if (func_0028B438(trim))
                break;

            if (func_0028B7A0(trim))
                break;

            if (func_0028B498(trim))
            {
                mode = MSGFILE_TEXT;
                pCurrentText = (char *)(pMemFile + filePos);
                break;
            }
            if (func_0028B4B8(trim))
            {
                Con_DPrintf("Unexpected '}' found, line %d\n", lineNumber);
                return;
            }
            strcpy(currentName, trim);
            break;

        case MSGFILE_TEXT:
            if (func_0028B4B8(trim))
            {
                length = strlen(currentName);

                if (lastNamePos + length > NAME_HEAP_SIZE)
                {
                    Con_DPrintf("Error parsing file!\n");
                    return;
                }
                strcpy(nameHeap + lastNamePos, currentName);

                pMemFile[lastLinePos - 1] = 0;

                textMessages[messageCount] = gMessageParms;
                textMessages[messageCount].pName = nameHeap + lastNamePos;
                lastNamePos += strlen(currentName) + 1;
                textMessages[messageCount].pMessage = pCurrentText;
                messageCount++;

                mode = MSGFILE_NAME;
                break;
            }
            if (func_0028B498(trim))
            {
                Con_DPrintf("Unexpected '{' found, line %d\n", lineNumber);
                return;
            }
            break;
        }

        lineNumber++;
        lastLinePos = filePos;
    }

    Con_DPrintf("Parsed %d text messages\n", messageCount);
    nameHeapSize = lastNamePos;
    textHeapSize = 0;
    for (i = 0; i < messageCount; i++)
        textHeapSize += strlen(textMessages[i].pMessage) + 1;

    messageSize = messageCount * sizeof(client_textmessage_t);

    gMessageTable = Mem_DbgMalloc(textHeapSize + nameHeapSize + messageSize, "engine/tmessage.c", 433);

    memcpy(gMessageTable, textMessages, messageSize);

    pNameHeap = (char *)gMessageTable + messageSize;
    memcpy(pNameHeap, nameHeap, nameHeapSize);
    nameOffset = pNameHeap - gMessageTable[0].pName;

    pCurrentText = pNameHeap + nameHeapSize;

    for (i = 0; i < messageCount; i++)
    {
        gMessageTable[i].pName += nameOffset;
        strcpy(pCurrentText, gMessageTable[i].pMessage);
        gMessageTable[i].pMessage = pCurrentText;
        pCurrentText += strlen(pCurrentText) + 1;
    }

    gMessageTableCount = messageCount;
}

void func_0028BD90(void)
{
    int fileSize;
    byte *pMemFile;

    if (gMessageTable)
    {
        Mem_Free(gMessageTable);
        gMessageTable = NULL;
    }

    pMemFile = COM_LoadFileForMe("titles.txt", &fileSize);

    if (pMemFile)
        func_0028BA00(pMemFile, fileSize);
}

client_textmessage_t *func_0028BDE8(const char *pName)
{
    int i;

    if (!strcasecmp(pName, NETWORK_MESSAGE1))
        return &gNetworkTextMessage[0];
    if (!strcasecmp(pName, NETWORK_MESSAGE2))
        return &gNetworkTextMessage[1];
    if (!strcasecmp(pName, NETWORK_MESSAGE3))
        return &gNetworkTextMessage[2];
    if (!strcasecmp(pName, NETWORK_MESSAGE4))
        return &gNetworkTextMessage[3];

    for (i = 0; i < gMessageTableCount; i++)
    {
        if (!strcmp(pName, gMessageTable[i].pName))
            return &gMessageTable[i];
    }

    return NULL;
}
