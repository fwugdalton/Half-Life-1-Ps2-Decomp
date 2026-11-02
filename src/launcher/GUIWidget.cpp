/* Launcher (Gearbox, sem fonte): classe base dos elementos de tela do GUI (CGUIWidget, vptr
   em 0x34). Nome do arquivo inventado. */

#include "launcher/GUI.h"

extern "C" {
void Mem_Free(void *ptr);
}

CGUIWidget :: CGUIWidget( CGUI *gui )
{
	int i;

	for ( i = 3; i >= 0; i-- )
		m_pActions[i] = 0;
	m_pGUI = gui;
	m_i1C = 0;
	m_bOwnKeys = 0;
	m_pKeys = 0;
	m_bOwn18 = 0;
	m_p18 = 0;
	m_i4 = 0;
	m_szName = 0;
}

CGUIWidget :: ~CGUIWidget()
{
	int i;

	for ( i = 0; i < 4; i++ )
	{
		if ( m_pActions[i] )
		{
			delete m_pActions[i];
			m_pActions[i] = 0;
		}
	}
	if ( m_pKeys && m_bOwnKeys == 1 )
	{
		delete m_pKeys;
		m_pKeys = 0;
	}
	if ( m_p18 && m_bOwn18 == 1 )
	{
		delete m_p18;
		m_p18 = 0;
	}
	if ( m_szName )
	{
		Mem_Free( m_szName );
		m_szName = 0;
	}
}

void CGUIWidget :: HandleKey( int key )
{
	if ( m_pKeys )
		m_pKeys->HandleKey( key );
}

int CGUIWidget :: Get8( void )
{
	return m_i8;
}

int CGUIWidget :: IsNamed( const char *name )
{
	return func_00228C00( name, m_szName ) == 1;
}

char *CGUIWidget :: GetName( void )
{
	return m_szName;
}
