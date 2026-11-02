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
	char m_pad[0x64];
	virtual ~CGUIVariable();
	virtual int IsType( const char *type );
	virtual int ParseAttribute( const char *token, void *parser );	/* linha do bloco GUI_VARIABLE_* */
	virtual void SetString( const char *value );
	virtual void Copy( CGUIVariable *from );	/* GUI_ACTION_SET_VARIABLE_VARIABLE */
	virtual void V5( void );
	virtual void Add( const char *value );		/* GUI_ACTION_ADD_VARIABLE (tipo D_003029F0) */
	virtual void AddAnim( const char *value );	/* GUI_ACTION_ADD_VARIABLE (tipo D_003029E4) */
};

/* elemento de tela (vptr em 0x34) */
class CGUIWidget
{
public:
	char m_pad[0x34];
	virtual void V0( void );
	virtual void V1( void );
	virtual void V2( void );
	virtual void Activate( void );
	virtual void Deactivate( void );
	virtual void CopyAttribute( const char *attr, CGUIWidget *from );	/* GUI_ACTION_COPY_ATTRIBUTE */
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
