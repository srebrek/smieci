#include "ExampleWindow.h"
#include <iostream>

LRESULT ExampleWindow::window_proc(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            std::cout << "MyWindow1: Escape key pressed, closing window.\n";
            DestroyWindow(m_hWnd);
            return 0;
        }
        break;
    case WM_DESTROY:
        std::cout << "MyWindow1: Window destroyed.\n";
        PostQuitMessage(EXIT_SUCCESS);
        return 0;
    }
    return window::window_proc(msg, wParam, lParam);
}