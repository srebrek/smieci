#pragma once
#include <vector>
#include <windows.h>

struct field {
    RECT position;
};

class board {
public:
    static constexpr LONG margin = 5;
    static constexpr LONG field_size = 90;

    LONG columns;
    LONG rows;
    LONG field_count;
    LONG width;
    LONG height;

    board(int size = 3);
    const std::vector<field>& fields() const { return m_fields; }

private:
    std::vector<field> m_fields;
};