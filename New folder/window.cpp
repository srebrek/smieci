#include "window.h"


bool window::is_class_registered(HINSTANCE hInst, LPCWSTR cName)
{
	WNDCLASSEXW wcx;
	return GetClassInfoExW(hInst, cName, &wcx);
}
void window::register_class(HINSTANCE hInst, LPCWSTR cName)
{
	WNDCLASSEXW wcx{};
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_VREDRAW | CS_HREDRAW;
	wcx.lpfnWndProc = window_proc;
	wcx.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH));
	wcx.lpszClassName = cName;
	RegisterClassExW(&wcx);
}

window::window(HINSTANCE hInst, const std::wstring& title)
	: m_hWnd{ nullptr }
{
	LPCWSTR className = L"My Window Class";
	if (!is_class_registered(hInst, className))
		register_class(hInst, className);
	CreateWindowExW(0, className, title.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInst, reinterpret_cast<LPVOID>(this));
	//m_hWnd will be set on WM_NCCREATE
}
window::~window()
{
	if (m_hWnd)
		DestroyWindow(m_hWnd);
}

LRESULT window::window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	window* w = nullptr;
	if (msg == WM_NCCREATE) {
		auto pcs = reinterpret_cast<LPCREATESTRUCTW>(lParam);
		w = reinterpret_cast<window*>(pcs->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
		w->m_hWnd = hWnd;
	}
	else w = reinterpret_cast<window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	if (w) {
		auto r = w->window_proc(msg, wParam, lParam);
		if (msg == WM_NCDESTROY) {
			w->m_hWnd = nullptr;
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
		}
		return r;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT window::window_proc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	//handle window logic
	//Example:
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(m_hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}
