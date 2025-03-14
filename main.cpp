#include <windows.h>
#include <iostream>
#include "ExampleWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance,
	HINSTANCE /*prevInstance*/,
	LPWSTR /*command_line*/,
	int show_command)
{
    // Tworzenie dwóch okien
    ExampleWindow window1(hInstance, L"Okno 1");

    // Pêtla komunikatów
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
