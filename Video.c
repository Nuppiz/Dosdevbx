#include "Common.h"

char far* MDA=(char *)0xB0000000L;        /* this points to MDA/Hercules video memory. */
char far* CGA=(char *)0xB8000000L;        /* this points to CGA video memory. */
char far* EVGA=(char *)0xA0000000L;       /* this points to EGA/VGA video memory. */
char far screen_buf [64000];             // Double screen buffer in system RAM

/* Source: David Brackeen's VGA Programming Guide. Works perfectly well for any mode defined in the IBM video BIOS */
void setVideoMode(char mode)
{
    union REGS regs;

    regs.h.ah = SET_MODE;
    regs.h.al = mode;
    int86(VIDEO_INT, &regs, &regs);
}

/* Source: Descent source code for the first half, second half (MCGA check) was a random snippet I found online */
int checkForVGA()
{
    // uses the VGA/MCGA specific VIDEO_INT BIOS function 1A00 to determine the video adapter
    // this function is absent in older video systems, so it should return an error anyway
    // finally, the regs.h.bl (active display code) check also filters out MCGA (if needed, for example when using unchained VGA modes)

    union REGS regs;

    regs.x.ax = 0x1A00;
	int86(VIDEO_INT, &regs, &regs);
    
    if (regs.h.al == 0x1A && regs.h.bl > 6 && regs.h.bl < 9)
        return TRUE;
    else
        return FALSE;
}

/* Source: Sarix1 */
// loads the VGA palette from a 256-colour bitmap file
void loadVGAPaletteFromBMP(char* filename, VGAPalette_t* pal)
{
    FILE *fp;
    int i;
    
    // Open the file
    fp = fopen(filename, "rb");
    
    // skip forward and read the colour index table
    fseek(fp, 0x0036, SEEK_SET);
    
    // load palette
    for (i = 0; i < 256; i++)
    {
        pal->colors[i].b = fgetc(fp);
        pal->colors[i].g = fgetc(fp);
        pal->colors[i].r = fgetc(fp);

        fgetc(fp); // discarded value (alpha/reserved in bitmaps, not used in DOS VGA)
    }

    fclose(fp);
}

/* Source: same as BMP loading function except with PAL file research on my own */
// loads the VGA palette from a standard Microsoft RIFF PAL file
void loadVGAPaletteFromPAL(char* filename, VGAPalette_t* pal)
{
    FILE *fp;
    int i;
    
    // Open the file
    fp = fopen(filename, "rb");
    
    // skip forward to the colour index table
    fseek(fp, 0x0018, SEEK_SET);
    
    // load palette
    for (i = 0; i < 256; i++)
    {  
        pal->colors[i].r = fgetc(fp);
        pal->colors[i].g = fgetc(fp);
        pal->colors[i].b = fgetc(fp);

        fgetc(fp); // discarded value (alpha/reserved in palettes, not used in DOS VGA)
    }

    fclose(fp);
}

/* Source: Sarix1 */
void setPaletteVGA(VGAPalette_t* pal)
{
    unsigned i;
    outportb(PALETTE_WRITE, 0);

    for (i = 0; i < 256; i++)
    {
        // VGA palette values only go up to 6 bits, (2 ^ 6 == 64 different shades)
        // while bitmap palettes go up to 8 bits, (2 ^ 8 == 256 different shades)
        // right shift (>>) the bits by two places so an 8-bit value becomes a 6-bit
        // this divides the shade value (0-255) by four, giving a value between 0-63
        outportb(PALETTE_DATA, (pal->colors[i].r>>2));
        outportb(PALETTE_DATA, (pal->colors[i].g>>2));
        outportb(PALETTE_DATA, (pal->colors[i].b>>2));
    }
}

void setPaletteEGA(EGAPalette_t* pal)
{

}

void setEGASpecial()
{

}

void setPaletteCGA(char palette)
{

}

/* Source: David Brackeen's VGA Programming Guide. */
void renderVGA()
{     
    // copy off-screen buffer to VGA memory
    memcpy(EVGA, screen_buf, SCREEN_SIZE);

    // clear off-screen buffer so the screen updates properly
    _fmemset(screen_buf, 0, SCREEN_SIZE);
}