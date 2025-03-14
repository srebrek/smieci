#pragma once
#include <utility>
#include <string>
#include <windows.h>

class window
{
protected:
	static bool is_class_registered(HINSTANCE, LPCWSTR);
	static void register_class(HINSTANCE, LPCWSTR);
	HWND m_hWnd;
public:
	static LRESULT window_proc(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT window_proc(UINT, WPARAM, LPARAM);
	window() : m_hWnd{ nullptr } {}
	window(const window&) = delete;
	window(window&& other) : m_hWnd{ nullptr } { *this = std::move(other); }
	window(HINSTANCE, const std::wstring&);
	window& operator=(const window&) = delete;
	window& operator=(window&& other) { std::swap(m_hWnd, other.m_hWnd); return *this; }
	operator HWND() const { return m_hWnd; }
	virtual ~window();
};
