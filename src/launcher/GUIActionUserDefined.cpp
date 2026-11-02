/* Launcher (Gearbox, sem fonte): ação definida no próprio script do GUI. O Init guarda as
   linhas até "END" (cada uma: primeira palavra + o resto da linha); o Execute roda as que
   começam com GUI_COMMAND. Nome do arquivo tirado da string de alocação. */

#include "launcher/GUI.h"

extern "C" {
char *func_00228CF0(char *dst, const char *src);	/* strcpy */
char *func_00228D80(char *dst, const char *src);	/* strcat */
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
void Mem_Free(void *ptr);
/* comando do GUI (objeto de 0x30 bytes na pilha) */
void func_00106038(void *cmd);
void func_00106048(void *cmd, int flags);
void func_001060D0(void *cmd, void *tokenizer);
void func_001063D8(void *cmd, CGUI *gui);
}

class CGUIActionUserDefined : public CGUIAction
{
public:
	CGUIActionUserDefined();
	virtual ~CGUIActionUserDefined();
	virtual void Init( void *tokenizer );
	virtual void Execute( CGUI *gui );
	void AddLine( const char *line );

	int m_iMaxLines;
	int m_iNumLines;
	char **m_pLines;
};

CGUIActionUserDefined :: CGUIActionUserDefined()
{
}

CGUIActionUserDefined :: ~CGUIActionUserDefined()
{
	int i;

	for ( i = 0; i < m_iNumLines; i++ )
	{
		Mem_Free( m_pLines[i] );
		m_pLines[i] = 0;
	}
	Mem_Free( m_pLines );
	m_pLines = 0;
}

void CGUIActionUserDefined :: Init( void *tokenizer )
{
	char line[200];

	m_iMaxLines = 0;
	m_iNumLines = 0;
	m_pLines = 0;
	m_bInitialized = 1;
	func_00108148( tokenizer, 0, 0 );
	m_szName = func_00228D30( func_00108148( tokenizer, 0, 0 ) );

	char *t = func_00108148( tokenizer, 0, 0 );
	while ( t && func_00228C00( "END", t ) != 1 )
	{
		func_00228CF0( line, t );
		func_00228D80( line, " " );
		func_00228D80( line, func_00108148( tokenizer, (int)"\r\n", 1 ) );
		AddLine( line );
		t = func_00108148( tokenizer, 0, 0 );
	}

}

void CGUIActionUserDefined :: Execute( CGUI *gui )
{
	int i;
	void *tok = gui->GetTokenizer();

	for ( i = 0; i < m_iNumLines; i++ )
	{
		func_00108120( tok, m_pLines[i], -1 );
		if ( func_00228C00( func_00108148( tok, 0, 0 ), "GUI_COMMAND" ) == 1 )
		{
			char cmd[0x30];
			func_00106038( cmd );
			func_001060D0( cmd, tok );
			func_001063D8( cmd, gui );
			func_00106048( cmd, 2 );
		}
	}
}

void CGUIActionUserDefined :: AddLine( const char *line )
{
	if ( m_iNumLines == m_iMaxLines )
	{
		m_iMaxLines = m_iNumLines + 3;
		m_pLines = (char **)func_002260D0( m_pLines, m_iMaxLines * 4, "Launcher/GUIActionUserDefined.cpp", 122 );
	}
	m_pLines[m_iNumLines] = func_00228D30( line );
	m_iNumLines++;
}
