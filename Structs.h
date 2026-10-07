#ifndef STRUCTS_H
#define STRUCTS_H

#define KB_ARRAY_LENGTH     256
#define KB_QUEUE_LENGTH     256

typedef struct
{
    int x, y;
} Vec2_int;

typedef struct
{
    char r, g, b;
} VGAColor_t;

typedef struct
{
    VGAColor_t colors[256];
} VGAPalette_t;

typedef struct
{
    char colors[16];
} EGAPalette_t;

typedef struct
{
    char keycode;
    char type;
    uint32_t time;
} KeyEvent_t;

typedef struct
{
    KeyEvent_t queue[KB_QUEUE_LENGTH];
    char keystates[KB_ARRAY_LENGTH];
    char queue_head;
    char queue_tail;
} Keyboard_t;

#endif /* STRUCTS_H */
