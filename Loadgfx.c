#include "Common.h"
#include "Defines.h"

char alphabet [FONT_FILE_SIZE];

void loadFont()
{
    FILE* file_ptr;
    file_ptr = fopen("FONT.7UP", "rb");
    fread(alphabet, 1, FONT_FILE_SIZE, file_ptr);
    fclose(file_ptr);
}