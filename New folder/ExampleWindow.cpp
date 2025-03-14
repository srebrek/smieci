#include "ExampleWindow.h"
#include "Tile.h"
#include <iostream>

ExampleWindow::ExampleWindow(HINSTANCE hInst, const std::wstring& title, int tiles)
    : m_board(tiles),
    m_field_brush(CreateSolidBrush(RGB(255, 0, 0))),
    m_field_brush2(CreateSolidBrush(RGB(0, 0, 255)))
{
    m_hWnd = nullptr;
    m_board = board(tiles);

    RECT size = { 0, 0, m_board.width, m_board.height };
    AdjustWindowRectEx(&size, WS_OVERLAPPEDWINDOW, FALSE, 0);

    int winWidth = size.right - size.left;
    int winHeight = size.bottom - size.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (screenWidth - winWidth) / 2;
    int y = (screenHeight - winHeight) / 2;

    LPCWSTR className = L"ExampleWindow";
    if (!is_class_registered(hInst, className))
        register_class(hInst, className);
    CreateWindowExW(0, className, title.c_str(),
        WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, size.right - size.left, size.bottom - size.top,
        nullptr, nullptr, hInst, reinterpret_cast<LPVOID>(this));

    for (auto& f : m_board.fields())
    {
        Tile* tile = new Tile(hInst, L"", m_hWnd, f.position);
        m_tiles.push_back(tile); // Dodaj Tile do kolekcji
    }

    // Uruchom globalny timer co 2 sekundy (2000 ms)
    SetTimer(m_hWnd, 2, 1, nullptr);
}

LRESULT ExampleWindow::window_proc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            std::cout << "MyWindow1: Escape key pressed, closing window.\n";
            DestroyWindow(m_hWnd);
            return 0;
        }
        break;
    case WM_DESTROY:
        std::cout << "MyWindow1: Window destroyed.\n";
        PostQuitMessage(EXIT_SUCCESS);
        return 0;
    case WM_TIMER:
        if (wParam == 2) // SprawdŸ, czy to nasz globalny timer
        {
            StartRandomTileAnimation(); // Rozpocznij animacjê losowego Tile
        }
        return 0;
    }
    return window::window_proc(msg, wParam, lParam);
}

ExampleWindow::~ExampleWindow()
{
    if (m_field_brush) DeleteObject(m_field_brush);
    if (m_field_brush2) DeleteObject(m_field_brush2);

    // Zatrzymaj globalny timer
    KillTimer(m_hWnd, 2);

    // Usuñ wszystkie Tile
    for (auto tile : m_tiles)
    {
        delete tile;
    }
}

void ExampleWindow::StartRandomTileAnimation()
{
    if (m_tiles.empty()) return; // Jeœli nie ma Tile, zakoñcz

    // Wylosuj indeks Tile
    int randomIndex = rand() % m_tiles.size();

    // Rozpocznij animacjê wylosowanego Tile
    m_tiles[randomIndex]->ChangeColor();
}