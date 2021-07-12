#include "checkup_graphics/platform.h"

namespace checkup
{
	namespace graphics
	{
#if defined(LINUXPC)


#elif defined(WINDOWSPC)
		static bool window_state = false;
		LRESULT CALLBACK wndProc(HWND hwnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
		{
			switch (msg)
			{
				case WM_CLOSE: {
					window_state = false;
					return 0;
				}

				case WM_SIZE: {
					//long winWidth = (long)LOWORD(lParam);
					//long winHeight = (long)HIWORD(lParam);
					return 0;
				}
			}

			return (DefWindowProc(hwnd, msg, wParam, lParam));
		}

		TNativeWindowType* create_native_window(const char* name, uint32_t width, uint32_t height)
		{
			HINSTANCE hInstance = GetModuleHandle(NULL);
			WNDCLASSEX wcex;

			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_OWNDC;
			wcex.lpfnWndProc = &DefWindowProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hInstance;
			wcex.hIcon = NULL;
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground = 0;
			wcex.lpszMenuName = NULL;
			wcex.lpszClassName = "eglsamplewnd";
			wcex.hIconSm = NULL;
			wcex.lpfnWndProc = wndProc;

			RegisterClassEx(&wcex);
			RECT rect = { 0, 0, (LONG)width, (LONG)height };
			int style = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
			AdjustWindowRect(&rect, style, FALSE);

			HWND hwnd = CreateWindow("eglsamplewnd", name, style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, GetModuleHandle(NULL), NULL);
			ShowWindow(hwnd, SW_SHOW);

			window_state = true;

			return (TNativeWindowType*)hwnd;
		}

		void destroy_native_window(TNativeWindowType* window)
		{
			HWND hwnd = (HWND)window;
			DestroyWindow(hwnd);
		}

		TNativeDisplayType* get_native_display(TNativeWindowType* window)
		{
			HWND hwnd = (HWND)window;
			HDC hdc = GetDC(hwnd);
			return (TNativeDisplayType*)hdc;
		}

		void show_window(TNativeWindowType* window)
		{
			HWND hwnd = (HWND)window;
			ShowWindow(hwnd, SW_SHOW);
		}

		void hide_window(TNativeWindowType* window)
		{
			HWND hwnd = (HWND)window;
			ShowWindow(hwnd, SW_HIDE);
		}

		bool get_window_state(TNativeWindowType*)
		{
			return window_state;
		}
#endif
	}
}