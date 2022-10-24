#include "../hooks.h"
#include "../../../includes.h"

#include "../../sdk.h"
#include "../../global_variables.h"

#include "../../../base/tools/render.h"

#include "../../../functions/config_vars.h"

__inline void toggle_value(int key, bool& flip, WPARAM wparam, UINT msg)
{
	if (wparam == key)
	{
		if (msg == WM_KEYUP)
			flip = !flip;
	}
}

__inline void toggle_key_value(key_t& key)
{
	key.down = true;
	key.tick = GetTickCount64();
}

__inline void release_key_value(key_t& key)
{
	key.down = false;
}

void parse_pressed_keys(UINT msg, WPARAM wparam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		toggle_key_value(g::keys[VK_LBUTTON]);
		break;
	case WM_LBUTTONUP:
		release_key_value(g::keys[VK_LBUTTON]);
		break;

	case WM_RBUTTONDOWN:
		toggle_key_value(g::keys[VK_RBUTTON]);
		break;
	case WM_RBUTTONUP:
		release_key_value(g::keys[VK_RBUTTON]);
		break;

	case WM_MBUTTONDOWN:
		toggle_key_value(g::keys[VK_MBUTTON]);
		break;
	case WM_MBUTTONUP:
		release_key_value(g::keys[VK_MBUTTON]);
		break;

	case WM_XBUTTONDOWN:
	{
		UINT button = GET_XBUTTON_WPARAM(wparam);
		if (button == XBUTTON1)
			toggle_key_value(g::keys[VK_XBUTTON1]);
		else if (button == XBUTTON2)
			toggle_key_value(g::keys[VK_XBUTTON2]);
		break;
	}
	case WM_XBUTTONUP:
	{
		UINT button = GET_XBUTTON_WPARAM(wparam);
		if (button == XBUTTON1)
			release_key_value(g::keys[VK_XBUTTON1]);
		else if (button == XBUTTON2)
			release_key_value(g::keys[VK_XBUTTON2]);
		break;
	}

	case WM_SYSKEYDOWN:
		toggle_key_value(g::keys[18]);
		break;
	case WM_SYSKEYUP:
		release_key_value(g::keys[18]);
		break;

	case WM_KEYDOWN:
		toggle_key_value(g::keys[wparam]);
		break;
	case WM_KEYUP:
		release_key_value(g::keys[wparam]);
		break;
	default: break;
	}
}

namespace tr
{
	LRESULT __stdcall wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		toggle_value(VK_INSERT, g_cfg.misc.menu, wparam, msg);
		toggle_value(VK_DELETE, g::uninject, wparam, msg);

		parse_pressed_keys(msg, wparam);

		return CallWindowProc(g::backup_window, hwnd, msg, wparam, lparam);
	}
}