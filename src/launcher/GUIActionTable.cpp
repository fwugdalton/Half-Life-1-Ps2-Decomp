/* Launcher (Gearbox, sem fonte): lista das ações do GUI (CGUIActionTable, 0x14 bytes, criada
   pelo CGUI) e a classe base CGUIAction. Nome do arquivo tirado da string de alocação. */

#include "launcher/GUI.h"

extern "C" {
void *func_002260D0(void *p, int size, const char *file, int line);	/* realloc */
void Mem_Free(void *ptr);
}

class CGUIActionTable
{
public:
	CGUIActionTable();
	~CGUIActionTable();
	void Init( CGUI *gui );
	void Add( CGUIAction *action );
	CGUIAction *Find( const char *name );
	CGUI *GetGUI( void );
	int Contains( CGUIAction *action );

	int m_bInitialized;
	int m_iMaxActions;
	int m_iNumActions;
	CGUIAction **m_pActions;
	CGUI *m_pGUI;
};

CGUIActionTable :: CGUIActionTable()
{
	m_bInitialized = 0;
}

CGUIActionTable :: ~CGUIActionTable()
{
	int i;

	m_bInitialized = 0;
	for ( i = 0; i < m_iNumActions; i++ )
	{
		if ( m_pActions[i] )
			delete m_pActions[i];
		m_pActions[i] = 0;
	}
	Mem_Free( m_pActions );
	m_pActions = 0;
	m_iNumActions = 0;
}

void CGUIActionTable :: Init( CGUI *gui )
{
	m_pGUI = gui;
	m_bInitialized = 1;
	m_iMaxActions = 0;
	m_pActions = 0;
	m_iNumActions = 0;
}

void CGUIActionTable :: Add( CGUIAction *action )
{
	if ( m_iNumActions == m_iMaxActions )
	{
		m_iMaxActions = m_iNumActions + 10;
		m_pActions = (CGUIAction **)func_002260D0( m_pActions, m_iMaxActions * 4, "Launcher/GUIActionTable.cpp", 65 );
	}
	m_pActions[m_iNumActions] = action;
	m_iNumActions++;
}

CGUIAction *CGUIActionTable :: Find( const char *name )
{
	int i;
	CGUIAction *r = 0;

	for ( i = 0; i < m_iNumActions; i++ )
	{
		if ( m_pActions[i]->IsNamed( name ) )
		{
			r = m_pActions[i];
			break;
		}
	}
	return r;
}

CGUI *CGUIActionTable :: GetGUI( void )
{
	return m_pGUI;
}

int CGUIActionTable :: Contains( CGUIAction *action )
{
	int i;
	int r = 0;

	for ( i = 0; i < m_iNumActions; i++ )
	{
		if ( m_pActions[i]->Equals( action ) )
		{
			r = 1;
			break;
		}
	}
	return r;
}

CGUIAction :: CGUIAction()
{
	m_bInitialized = 0;
	m_szName = 0;
}

CGUIAction :: ~CGUIAction()
{
	Mem_Free( m_szName );
	m_szName = 0;
}

int CGUIAction :: IsNamed( const char *name )
{
	return func_00228C00( name, m_szName ) == 1;
}

int CGUIAction :: Equals( CGUIAction *other )
{
	return IsNamed( other->m_szName );
}
