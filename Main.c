#include "Common.h"
#include "Draw.h"
#include "Keylist.h"
#include "Keyb.h"
#include "Loadgfx.h"
#include "Text.h"
#include "Timer.h"
#include "Video.h"

extern Keyboard_t Keyboard;
extern uint32_t program_time;

char running = 1;

char graphics_mode_menu;
char CGA_palette; // palette 0 or 1 in mode 4
char EGA_special; // special hybrid mode D/10 (mode D resolution with mode 10 colours)

int menu()
{
    printf("Select graphics mode. Selections 5 and 6 require a multi-sync monitor.\n\n"
            "1) CGA Mode 4 (320x200, 4 colours, two fixed palettes):\n"
            "2) CGA Mode 5 (320x200, 4 colours, fixed palette):\n"
            "3) Hercules Monochrome (720x348, b/w):\n"
            "4) EGA Mode D (320x200, 16 colours, fixed/custom palette):\n"
            "5) EGA Mode 10 (640x350, 16 colours, custom palette):\n"
            "6) VGA Mode 13 (320x200, 256, colours, custom palette):\n");
    scanf("%d", &graphics_mode_menu);

    switch (graphics_mode_menu)
    {
        case 1:
            return CGA_MODE_4;

        case 2:
            return CGA_MODE_5;
        
        case 3:
            return HERCULES_GRAPHICS;

        case 4:
            return EGA_MODE_D;

        case 5:
            return EGA_MODE_10;
        
        case 6:
            return VGA_MODE_13;

        default:
            return CGA_MODE_4;
    }
}

int quit()
{
    setVideoMode(TEXT_MODE_80);
    deInitKeyboard();
    deInitTimer();
    return 0;
}

void main()
{
    int video_mode;
    int update_interval = 1000 / 33; // 30 FPS target
    long last_update = 0;
    loadFont();
    video_mode = menu();

    if (video_mode == CGA_MODE_4)
    {
        printf("Select CGA palette (0 or 1):\n");
        scanf("%d", &CGA_palette);
    }
    else if (video_mode == EGA_MODE_D)
    {
        printf("Do you want to use the special hybrid mode with a custom palette?\n"
                "This requires a multi-sync monitor. 0 for no, 1 for yes.\n");
        scanf("%d", &EGA_special);
    }
    setVideoMode(video_mode);

    initKeyboard();
    initTimer();

    if (video_mode == CGA_MODE_4)
        setPaletteCGA(CGA_palette);
    
    else if (EGA_special == TRUE)
        setEGASpecial();
    
    while (running == 1)
    {
        if (last_update + update_interval < program_time)
        {

        }
    }
    quit();
}