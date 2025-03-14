#pragma once
#include <utility>
#include <string>
#include <windows.h>
#include "window.h"
#include "Board.h"
#include "Tile.h"
#include <set>

class ExampleWindow : public window
{
private:
    board m_board;
    HBRUSH m_field_brush;
    HBRUSH m_field_brush2;
    std::set<HWND> m_clickedControls;
    std::vector<Tile*> m_tiles;
public:
    ExampleWindow(HINSTANCE, const std::wstring&, int tiles);
    ~ExampleWindow();

    LRESULT window_proc(UINT msg, WPARAM wParam, LPARAM lParam) override;
private:
    // Oznacz kontrolkę jako klikniętą
    void MarkControlAsClicked(HWND hControl) {
        m_clickedControls.insert(hControl);
    }

    // Sprawdź, czy kontrolka została kliknięta
    bool IsControlClicked(HWND hControl) const {
        return m_clickedControls.find(hControl) != m_clickedControls.end();
    }

    void StartRandomTileAnimation(); // Funkcja do rozpoczynania animacji losowego Tile
};