/* Launcher da Gearbox (sem fonte): CGUI (Launcher/GUI.cpp) e as classes que ele usa.
   Nomes inventados; as classes ainda em asm ficam como funções func_XXXXXXXX. */

#ifndef LAUNCHER_GUI_H
#define LAUNCHER_GUI_H

#include "common.h"

extern "C" {
#include "engine/decals.h"

char *func_00228D30(const char *s);				/* strdup */
int func_00228C00(const char *a, const char *b);	/* 1 se iguais */
extern const char *D_003029E8;	/* nome do tipo float das variáveis do GUI */
}

/* variável do GUI (vptr em 0x64) */
class CGUIVariable
{
public:
	CGUIVariable();
	char m_szName[0x64];	/* NAME (é o nome da cvar) */
	virtual ~CGUIVariable();
	virtual int IsType( const char *type );
	virtual int ParseAttribute( const char *token, void *parser );	/* linha do bloco GUI_VARIABLE_* */
	virtual void SetString( const char *value ) = 0;
	virtual void Copy( CGUIVariable *from ) = 0;	/* GUI_ACTION_SET_VARIABLE_VARIABLE */
	virtual void V5( void ) = 0;	/* chamado ao entrar na lista */
	char *GetName( void );
};

/* variável de texto ("String", tipo D_003029F0; Launcher/GUIVariableString.cpp) */
class CGUIVariableString : public CGUIVariable
{
public:
	CGUIVariableString();
	virtual ~CGUIVariableString();
	char *GetValue( void );
	virtual int IsType( const char *type );
	virtual int ParseAttribute( const char *token, void *parser );
	virtual void SetString( const char *value );
	virtual void Copy( CGUIVariable *from );
	virtual void V5( void );
	virtual void Add( const char *value );		/* GUI_ACTION_ADD_VARIABLE */

	char m_szValue[0x100];
};

/* variável numérica ("Numeric", tipo D_003029E4): base das Integer/Float, com animação
   (WRAP, LOOP, LERP_TIME, RUNNING); slots 7..9 novos */
class CGUIVariableNumeric : public CGUIVariable
{
public:
	CGUIVariableNumeric();
	virtual ~CGUIVariableNumeric();
	virtual int IsType( const char *type );
	virtual int ParseAttribute( const char *token, void *parser );
	virtual void V5( void );
	virtual void SetMin( void ) = 0;
	virtual void AddAnim( const char *value ) = 0;	/* GUI_ACTION_ADD_VARIABLE */
	virtual void Reset( void ) = 0;
	virtual void Restore( void ) = 0;
	void Start( void );
	void Restart( void );	/* GUI_ACTION_VCR_VARIABLE */
	void Stop( void );
	void Run( void );

	int m_bWrap;
	int m_bRunning;
	int m_bLoop;
	unsigned int m_iLerpTime;
	unsigned long m_iStartTime;
};

/* "Float" (D_003029E8, 0x98 bytes) e "Integer" (D_003029EC, 0x90 bytes) */
class CGUIVariableFloat : public CGUIVariableNumeric
{
public:
	CGUIVariableFloat();
	virtual ~CGUIVariableFloat();
	float GetValue( void );
	virtual int IsType( const char *type );
	virtual int ParseAttribute( const char *token, void *parser );
	virtual void SetString( const char *value );
	virtual void Copy( CGUIVariable *from );
	virtual void SetMin( void );
	virtual void AddAnim( const char *value );
	void SetValue( float value );
	void UpdateCvar( void );
	void ReadCvar( void );
	virtual void Reset( void );
	virtual void Restore( void );

	float m_fInitialValue;
	float m_fValue;
	float m_fMaxValue;
	float m_fMinValue;
	int m_bCvar;
	int m_iPad;
};

class CGUIVariableInteger : public CGUIVariableNumeric
{
public:
	CGUIVariableInteger();
	virtual ~CGUIVariableInteger();
	int GetValue( void );
	virtual int IsType( const char *type );
	virtual int ParseAttribute( const char *token, void *parser );
	virtual void SetString( const char *value );
	virtual void Copy( CGUIVariable *from );
	virtual void SetMin( void );
	virtual void AddAnim( const char *value );
	void SetValue( int value );
	void UpdateCvar( void );
	void ReadCvar( void );
	virtual void Reset( void );
	virtual void Restore( void );

	int m_iInitialValue;
	int m_iValue;
	int m_iMaxValue;
	int m_iMinValue;
};

/* elemento de tela (vptr em 0x34) */
class CGUI;

/* objeto apagado pelo widget (vptr em 8, como o CGUIAction) */
class CGUIWidgetPart
{
public:
	char m_pad[8];
	virtual ~CGUIWidgetPart();
	void HandleKey( int key );	/* 0x102AB8 */
};

class CGUIWidget
{
public:
	CGUIWidget( CGUI *gui );
	virtual ~CGUIWidget();
	virtual int Parse( void ) = 0;	/* lê o bloco do script */
	virtual void V2( void ) = 0;
	virtual void Activate( void ) = 0;
	virtual void Deactivate( void ) = 0;
	virtual void CopyAttribute( const char *attr, CGUIWidget *from ) = 0;	/* GUI_ACTION_COPY_ATTRIBUTE */
	void HandleKey( int key );
	int Get8( void );
	int IsNamed( const char *name );
	char *GetName( void );

	char *m_szName;
	int m_i4;
	int m_i8;
	int m_bOwnKeys;
	CGUIWidgetPart *m_pKeys;
	int m_bOwn18;
	CGUIWidgetPart *m_p18;
	int m_i1C;
	CGUIWidgetPart *m_pActions[4];
	CGUI *m_pGUI;
};

class CGUI;

/* GUI_ACTION_* (Launcher/GUIAction*.cpp): nome (strdup) em 0, 4 = 1 depois do Init, vptr em 8 */
class CGUIAction
{
public:
	CGUIAction();
	virtual ~CGUIAction();
	virtual void Init( void *tokenizer ) = 0;
	virtual void Execute( CGUI *gui ) = 0;
	int IsNamed( const char *name );
	int Equals( CGUIAction *other );

	char *m_szName;
	int m_bInitialized;
};

struct CGUIScreen;

extern "C" {
/* variáveis */
float func_00107000(CGUIVariable *v);
void *func_00107C10(void *mem);
void func_00107C20(void *p, int flags);
void func_00107CE0(void *p);
void func_00107CF8(void *p, const char *token, void *tokenizer);
CGUIVariable *func_00107EE0(void *p, const char *name);
int func_00107F00(void *p, const char *name);
/* tokenizador (0x14 bytes) */
void *func_00108008(void *mem);
void func_00108018(void *p, int flags);
void func_00108070(void *p);
void func_00108120(void *p, char *text, int len);
char *func_00108148(void *p, int a, int b);
}

class CGUI
{
public:
	CGUI();
	~CGUI();
	void Init( pakfile_t *pak, const char *name, int bInPlace );
	void BeginDraw( void );
	void EndDraw( void );
	void SetColor( float r, float g, float b );
	void Update( void );
	void HandleKey( int key );
	float GetAlpha( void );
	void Set04( int v );
	void Set08( int v );
	int Get04( void );
	int Get08( void );
	void SetFocus( const char *name );
	CGUIWidget *FindWidget( const char *name );
	void *GetActions( void );
	void *GetTokenizer( void );
	void *GetRenderer( void );
	void *GetVariables( void );
	void SetVariable( const char *name, const char *value );
	void ReadFile( const char *name, char *dst );
	int GetFileSize( const char *name );
	void LoadFile( const char *name, char **pText, int bInPlace );
	void FreeFile( char **pText, int bInPlace );
	void ProcessIncludes( char **pText );
	void ProcessDefines( char **pText );
	void RegisterActions( void );
	void ParseScreens( void );
	void UpdateFade( void );
	void AddScreen( CGUIScreen *s );
	void AddAction( CGUIAction *a );
	void SetKeyNumber( int key );
	void UpdateFocusVariable( void );

	int m_bInitialized;			/* 0x00 */
	int m_i04;
	int m_i08;
	int m_iCurrentScreen;		/* 0x0C */
	int m_iMaxScreens;			/* 0x10 */
	int m_iNumScreens;			/* 0x14 */
	CGUIScreen **m_pScreens;	/* 0x18 */
	unsigned int m_i1C;
	unsigned int m_iFadeInStart;	/* 0x20 */
	float m_flFadeOutTime;		/* 0x24 */
	unsigned int m_iFadeOutStart;	/* 0x28 */
	int m_bFadingOut;			/* 0x2C */
	int m_iNextScreen;			/* 0x30 */
	float m_flAlpha;			/* 0x34 */
	void *m_pActions;			/* 0x38 */
	void *m_pTokenizer;			/* 0x3C */
	void *m_pRenderer;			/* 0x40 */
	void *m_pVariables;			/* 0x44 */
	pakfile_t m_Pak;			/* 0x48 */
};

#endif
