#pragma once
#include <utility>
#include <string>
#include <windows.h>
#include "window.h"

class Tile : public window
{
private:
    HBRUSH m_field_brush;
    HBRUSH m_field_brush2;
    RECT originalPosition;
    bool isClicked = false;
    bool isAnimating = false;
    bool isGrowing = true; // Flaga wskazuj�ca, czy Tile ro�nie
    float scale = 1.0f; // Pocz�tkowa skala (100%)
    float targetScale = 1.1f; // Docelowa skala (110%)
    const float animationSpeed = 0.01f; // Szybko�� animacji

public:
    Tile(HINSTANCE hInst, const std::wstring& title, HWND parent, const RECT& position);
    ~Tile();
    LRESULT window_proc(UINT msg, WPARAM wParam, LPARAM lParam) override;
	void ChangeColor(); // Zmie� kolor Tile

    void StartAnimation(); // Rozpocznij animacj�
    void UpdateAnimation(); // Zaktualizuj animacj�
};