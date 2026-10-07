#include "Common.h"

extern char far screen_buf [];
extern char alphabet [];

// text drawing functions for a custom font

void drawSymbol(int x, int y, int symbol_index, char color)
{
    char index_x = 0;
    char index_y = 0;
    symbol_index = symbol_index * CHARACTER_SIZE; // pixel start index of the symbol in the bitmap file

    for (index_y = 0; index_y < CHARACTER_HEIGHT; index_y++)
    {
        for (index_x = 0; index_x < CHARACTER_WIDTH; index_x++)
        {
            if (alphabet[symbol_index] != TRANSPARENT_COLOR)
            {
                SET_PIXEL(x, y, alphabet[symbol_index] + color);
                symbol_index++;
                x++;
            }
            else
            {
                symbol_index++;
                x++;
            }
        }
        index_x = 0;
        x = x - CHARACTER_WIDTH;
        y++;
    }
    index_y = 0;
    symbol_index = 0;
}

void drawText(int x, int y, char* string, char color)
{
    int i = 0;
    int start_x = x;
    char c;
    
    while ((c = string[i++]) != 0)
    {
        if (c == '\n')
        {
            x = start_x;
            y += 10; // change row
            continue;
        }
        // scancode (c) is based on the original IBM PC code page 437
        // this specific font starts at the "up arrow" glyph, therefore it skips the first 24 special characters (c - 24)
        drawSymbol(x, y, c - 24, color);
        x += 10;
    }
}