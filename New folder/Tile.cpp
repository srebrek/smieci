#include "Tile.h"
#include <cmath>


Tile::Tile(HINSTANCE hInst, const std::wstring& title, HWND parent, const RECT& position)
    : m_field_brush(CreateSolidBrush(RGB(255, 0, 0))), // Czerwony pêdzel
    m_field_brush2(CreateSolidBrush(RGB(0, 0, 255))), // Niebieski pêdzel
    originalPosition(position)
{
    LPCWSTR className = L"TileClass";
    if (!is_class_registered(hInst, className))
        register_class(hInst, className);

    CreateWindowExW(0, className, title.c_str(),
        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY,
        position.left, position.top,
        position.right - position.left,
        position.bottom - position.top,
        parent, nullptr, hInst, reinterpret_cast<LPVOID>(this));

    HRGN hRgn = CreateRoundRectRgn(0, 0, position.right - position.left, position.bottom - position.top, 20, 20);
    if (hRgn) {
        SetWindowRgn(m_hWnd, hRgn, TRUE);
        DeleteObject(hRgn); // Zwolnij region
    }
}

Tile::~Tile()
{
    if (m_field_brush) DeleteObject(m_field_brush);
    if (m_field_brush2) DeleteObject(m_field_brush2);
}

LRESULT Tile::window_proc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hWnd, &ps);

        // Oblicz aktualny rozmiar na podstawie skali
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        int scaledWidth = static_cast<int>(width * scale);
        int scaledHeight = static_cast<int>(height * scale);
        int offsetX = (width - scaledWidth) / 2;
        int offsetY = (height - scaledHeight) / 2;

        // Narysuj zaokr¹glony prostok¹t z aktualn¹ skal¹
        auto oldBrush = isClicked ? SelectObject(hdc, m_field_brush2) : SelectObject(hdc, m_field_brush);
        auto oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
        RoundRect(hdc, offsetX, offsetY, offsetX + scaledWidth, offsetY + scaledHeight, 11, 11);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);

        EndPaint(m_hWnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN:
        if (!isAnimating) // Rozpocznij animacjê tylko, jeœli nie jest ju¿ aktywna
        {
            isClicked = !isClicked;
            StartAnimation();
        }
        return 0;
    case WM_TIMER:
        if (wParam == 1) // Timer do animacji
        {
            UpdateAnimation();
        }
        return 0;
    }
    return window::window_proc(msg, wParam, lParam);
}

void Tile::StartAnimation()
{
    isAnimating = true;
    isGrowing = true; // Rozpocznij od fazy powiêkszania
    scale = 1.0f; // Pocz¹tkowa skala (100%)
    SetTimer(m_hWnd, 1, 16, nullptr); // Uruchom timer co 16 ms (~60 FPS)
}

void Tile::UpdateAnimation()
{
    if (isGrowing)
    {
        scale += animationSpeed; // Zwiêksz skalê
        if (scale >= targetScale) // Jeœli osi¹gniêto docelow¹ skalê (110%)
        {
            scale = targetScale;
            isGrowing = false; // Prze³¹cz na fazê pomniejszania
        }
    }
    else
    {
        scale -= animationSpeed; // Zmniejsz skalê
        if (scale <= 1.0f) // Jeœli wrócono do 100%
        {
            scale = 1.0f;
            isAnimating = false; // Zakoñcz animacjê
            KillTimer(m_hWnd, 1); // Zatrzymaj timer
        }
    }

    int newWidth = static_cast<int>((originalPosition.right - originalPosition.left) * scale);
    int newHeight = static_cast<int>((originalPosition.bottom - originalPosition.top) * scale);
    int newX = originalPosition.left - (newWidth - (originalPosition.right - originalPosition.left)) / 2;
    int newY = originalPosition.top - (newHeight - (originalPosition.bottom - originalPosition.top)) / 2;

    // Zmieñ rozmiar i pozycjê okna
    SetWindowPos(
        m_hWnd,
        nullptr,
        newX,
        newY,
        newWidth,
        newHeight,
        SWP_NOZORDER
    );

    HRGN hRgn = CreateRoundRectRgn(0, 0, newWidth, newHeight, 20, 20);
    SetWindowRgn(m_hWnd, hRgn, TRUE);

    InvalidateRect(m_hWnd, nullptr, TRUE); // Wymuœ przerysowanie
}

void Tile::ChangeColor()
{
    isClicked = !isClicked;
    StartAnimation();
}