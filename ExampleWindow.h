#pragma once
#include <utility>
#include <string>
#include <windows.h>
#include "window.h"

class ExampleWindow : public window
{
public:
    using window::window; // Dziedziczenie konstruktorów

    LRESULT window_proc(UINT msg, WPARAM wParam, LPARAM lParam) override;
};