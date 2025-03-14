#include <windows.h>
#include <iostream>
#include "ExampleWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance,
    HINSTANCE /*prevInstance*/,
    LPWSTR /*command_line*/,
    int show_command)
{
    int size = 4;
    ExampleWindow window1(hInstance, L"Memory", size);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
