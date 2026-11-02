/* Launcher (Gearbox, sem fonte): base das partes de um elemento de tela (CGUIWidgetPart:
   GUI_CAPTION, GUI_DEPENDENCY e GUI_INPUT, Launcher/GUIElement*.cpp). Só o construtor, o
   destrutor e a vtable. Nome do arquivo inventado. */

#include "launcher/GUI.h"

CGUIWidgetPart :: CGUIWidgetPart( CGUIScreen *screen )
{
	m_p4 = screen;
	m_i0 = 0;
}

CGUIWidgetPart :: ~CGUIWidgetPart()
{
}
