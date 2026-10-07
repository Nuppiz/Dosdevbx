#ifndef VIDEO_H
#define VIDEO_H

#include "Structs.h"

void setVideoMode(char mode);
void loadVGAPaletteFromBMP(char* filename, VGAPalette_t* pal);
void loadVGAPaletteFromPAL(char* filename, VGAPalette_t* pal);
void setPaletteVGA(VGAPalette_t* pal);
void setPaletteEGA(EGAPalette_t* pal);
void setEGASpecial();
void setPaletteCGA(char palette);
void renderVGA();

#endif /* VIDEO_H */
