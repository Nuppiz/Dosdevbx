#include "Common.h"
#include "Mouse.h"

union REGS in, out;

/* Basic DOS mouse functions using INT 33h 
   Source: https://www.equestionanswers.com/c/c-int33-mouse-service.php */

// Vec2_int mouse_location // struct to save mouse x-y

// try detecting if a mouse is installed, returns FFFFh (65535) if yes, 0 if no
int detectMouse()
{
    in.x.ax = INIT_MOUSE;
    int86(MOUSE_INT, &in, &out);
    return out.x.ax; 
}

// show cursor
void showMouse()
{
    in.x.ax = SHOW_MOUSE;
    int86(MOUSE_INT, &in, &out); 
}

// hide cursor
void hideMouse()
{
    in.x.ax = HIDE_MOUSE;
    int86(MOUSE_INT, &in, &out); 
}

// poll mouse status for updates
static void getMouse(int* x, int* y, int* left, int* right)
{
    in.x.ax = GET_MOUSE_STATUS;
    int86(MOUSE_INT, &in, &out);
    *x = out.x.cx;
    *y = out.x.dx; 
    *left = out.x.bx & 0x1;
    *right = out.x.bx & 0x2;
}

// do things with our mouse data
void handleMouseInput()
{
    int mouse_x, mouse_y, mouse_lb, mouse_rb;

    getMouse(&mouse_x, &mouse_y, &mouse_lb, &mouse_rb);

    if (mouse_lb)
    {
      // do thing if left button is clicked

    }
    else if (mouse_rb)
    {
      // do another thing if left button is clicked
    }
    // update mouse location
    // mouse_location.x = mouse_x;
    // mouse_location.y = mouse_y;
}
