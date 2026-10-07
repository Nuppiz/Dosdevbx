#ifndef MOUSE_H
#define MOUSE_H

#define MOUSE_INT           0x33
#define INIT_MOUSE          0x00
#define SHOW_MOUSE          0x01
#define HIDE_MOUSE          0x02
#define GET_MOUSE_STATUS    0x03

int detectMouse();
void showMouse();
void hideMouse();
void handleMouseInput();

#endif /* MOUSE_H */
