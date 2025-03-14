#include "Board.h"

board::board(int size)
    : columns(size), rows(size), field_count(size* size),
    width(size* (field_size + margin) + margin),
    height(size* (field_size + margin) + margin),
    m_fields(size* size)
{
    for (LONG row = 0; row < rows; ++row) {
        for (LONG column = 0; column < columns; ++column) {
            auto& f = m_fields[row * columns + column];
            f.position.top = row * (field_size + margin) + margin;
            f.position.left = column * (field_size + margin) + margin;
            f.position.bottom = f.position.top + field_size;
            f.position.right = f.position.left + field_size;
        }
    }
}