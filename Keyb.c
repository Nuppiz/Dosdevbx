#include <stdint.h>
#include <dos.h>
#include "Common.h"

/*  AT-compatible rudiementary keyboard handler
    Source: Sarix1, based on research of old DOS game source codes. */

#define KEYHANDLER_INT      9
#define KEY_HIT_FLAG        1
#define KEY_PRESSED_FLAG    2
#define KEY_RELEASED_FLAG   0x80 // 128, MSB
#define KEY_SPECIAL_CODE    0xE0 // 224

Keyboard_t Keyboard = {0};
static void interrupt (far *old_Keyhandler_ISR)(void);

void pushKeyEvent(KeyEvent_t event)
{
    if ((char)(Keyboard.queue_tail+1) != Keyboard.queue_head)
        Keyboard.queue[Keyboard.queue_tail++] = event;
}

static void handleScanCode(uint8_t scan)
{
    KeyEvent_t event;
    static char status = 0;

    if (scan == KEY_SPECIAL_CODE)
    {
        status = 0x80;
        return;
    }

    event.keycode = (scan & ~KEY_RELEASED_FLAG) | status;

    if (scan & KEY_RELEASED_FLAG)
    {
        // Clear key down flag, set key released flag
        Keyboard.keystates[event.keycode] &= ~KEY_PRESSED_FLAG;
        Keyboard.keystates[event.keycode] |= KEY_RELEASED_FLAG;
        event.type = KEY_RELEASED_FLAG;
        pushKeyEvent(event);
    }
    else if (!(Keyboard.keystates[event.keycode] == KEY_PRESSED_FLAG))
    {
        // Key newly pressed (not a repeat); set key down and key struck flags
        Keyboard.keystates[event.keycode] |= KEY_PRESSED_FLAG|KEY_HIT_FLAG;
        event.type = KEY_HIT_FLAG;
        pushKeyEvent(event);
    }
    status = 0;
}

void interrupt far Keyhandler()
{
    while (inportb(0x64) & 1)
        handleScanCode(inportb(0x60));
    outportb(0x20, 0x20);
}

void initKeyboard()
{
    char far *bios_key_state;
    asm cli
    // save address of current keyhandler interrupt function
    old_Keyhandler_ISR = getvect(KEYHANDLER_INT);
    // caps lock & num lock off
    bios_key_state = MK_FP(0x040, 0x017);
    *bios_key_state &= (~(32|64));
    old_Keyhandler_ISR(); 
    // replace old keyhandler with new keyhandler function
    setvect(9, Keyhandler);
    asm sti
}

void deInitKeyboard()
{
    // restore old keyhandler
    asm cli
    setvect(KEYHANDLER_INT, old_Keyhandler_ISR);
    asm sti
}

void clearKeys()
{
    int i;
    for (i = 0; i < KB_ARRAY_LENGTH; i++)
        Keyboard.keystates[i] &= KEY_PRESSED_FLAG;
}