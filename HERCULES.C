/*****************************************************************************/
/*	Hercules Compatible graphics routines written for Microsoft Quick C  */
/*	version 1.0.  Compile with large model (option /AL).		     */
/*									     */
/*	Written by:    Ben Bederson   (c) 1988				     */
/*		       bbb7889@acf5.NYU.EDU				     */
/*									     */
/*		       222 East 19th St.  #3H				     */
/*		       New York, NY  10003				     */
/*		       (212) 260-2667					     */
/*									     */
/*	These routines were written by myself for my own purposes and I make */
/*	no guarantees, warentees, expressed, or implied, etc.  However,      */
/*	I hope they are useful.  Everybody is free and encouraged to copy    */
/*	and use these routines under the condition that this statement is    */
/*	kept at the top without modification.  I would appreciate any	     */
/*	comments or criticisms or improvement/ideas about these routines.    */
/*									     */
/*	Good luck.							     */
/*****************************************************************************/

/* Changes made by Nuppiz in October 2026:
-General readability improvements (spaces, indentations)
-Modified code to ANSI C standards (added "void" function data types, moved function argument data types to function title)
-Changed outp() calls to outportb() calls for Borland compatibility and added header to convert them back to Watcom calls
-Renamed all functions to be Hercules-specific (HGC prefix) to avoid conflicts
-Added function prototypes where necessary to avoid compiler warnings
-Added some comments here and there
-Renamed code and header files to "Hercules" (original was just "Graphics")*/

#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include <dos.h>
#include <mem.h>
#include "hercules.h"
#include "Borwat.h"

/*****************************************************************************/
/*									     */
/*	     Routines in this file:					     */
/*									     */
/*	     graphics() - Enter monochrome 720x348 graphics mode (clear scn) */
/*	     text()	- Enter text mode				     */
/*	     cls()	- Clear the text screen 			     */
/*	     grcls()	- Clear the graphics screen			     */
/*	     fill_screen()	- Fill the graphics screen to white	     */
/*	     dot(x,y,color)	- Put a dot of color WHITE,BLACK, or XOR     */
/*				  at the location x,y on the graphics screen */
/*	     line(x1,y1,x2,y2,color)					     */
/*				- Draw a line from x1,y1 to x2,y2 in color   */
/*	     draw(shape,x,y,color)					     */
/*				- Draw the shape as specified by a	     */
/*				  shape_type structure in the specified      */
/*				  color at the specified location x,y.	     */
/*				  Not too fast.  Try draw_block for some     */
/*				  really fast drawing.			     */
/*	     draw_block(shape,x,y,color)				     */
/*				- Draw the shape as specified by a	     */
/*				  shape_type structure in the specified      */
/*				  color at the specified location x,y.	     */
/*				  The location must be a byte boundary	     */
/*				  as this routine does not shift pixels.     */
/*				  But, it is very fast!!!  It will not	     */
/*				  give an error if x,y is not a byte	     */
/*				  boundary, but will shift the block to      */
/*				  draw it on a byte boundary.  (This only    */
/*				  applies to the x-axis.		     */
/*	     shift_up(shape,x,y,shift_num)				     */
/*	     shift_down(shape,x,y,shift_num)				     */
/*	     shift_left(shape,x,y,shift_num)				     */
/*	     shift_right(shape,x,y,shift_num)				     */
/*				- Shifts the specified shape at the	     */
/*				  specified location by shift_num pixels     */
/*				  in the direction corresponding to the      */
/*				  routine name.  Really, just a rectangle    */
/*				  is shifted specified by the size in the    */
/*				  shape.  This is relatively slow because    */
/*				  bits have to be shifted from one byte to   */
/*				  the next.				     */
/*									     */
/*			Note: Currently, only shift_right is implemented.    */
/*			      Either use the block shifts, or it should be   */
/*			      very easy to write shift_up,down, and left as  */
/*			      they are just mirror images of shift_right.    */
/*	     shift_up_block(shape,x,y)					     */
/*	     shift_down_block(shape,x,y)				     */
/*	     shift_left_block(shape,x,y)				     */
/*	     shift_right_block(shape,x,y)				     */
/*				- Shifts the specified shape at the	     */
/*				  specified location by 8 pixels in the      */
/*				  direction corresponding to the routine     */
/*				  name.  Really, just a rectangle is shifted */
/*				  specified by the size in the shape.  This  */
/*				  is very fast as entire bytes are moved     */
/*				  without worrying about shifting bits.      */
/*				Note: The shape must start on a byte	     */
/*				  boundary, or extra pixels will be moved    */
/*				  with it.				     */
/*									     */
/*****************************************************************************/

/*****************************************************************************/
/*									     */
/*	     User Info: 						     */
/*	     ---------- 						     */
/*									     */
/*	     struct shape_type						     */
/*									     */
/*	     This structure represents a general shape.  x and y contain     */
/*	     the size in bytes of a shape.  shape_array is a pointer to      */
/*	     an array that must be allocated by the user.  The array is      */
/*	     a two dimensional array of single-byte characters where each    */
/*	     bit describes the shape: 1 ON, and 0 OFF.			     */
/*									     */
/*	     The colors are:						     */
/*	       WHITE	- Draw with pixels on.				     */
/*	       BLACK	- Draw with pixels off. 			     */
/*	       XOR	- Draw with pixels opposite state of what they were  */
/*	       OVERWRITE - For drawing shapes, 1's put a pixel on, and       */
/*			   0's put a pixel off.  For other colors, 0's       */
/*			   don't do anything.                                */
/*									     */
/*****************************************************************************/

char far* screen = (char far *)0xb0000000; // Same as MDA memory
int gr6845[] = {0x38, 0x2d, 0x30, 0x08, 0x5a, 0x00, 0x57, 0x57, 0x02,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int te6845[] = {0x61, 0x50, 0x52, 0x0f, 0x19, 0x06, 0x19, 0x19, 0x02,
		0x0d, 0x0b, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*****************************************************************************/
/*									     */
/*	I enter graphics mode by directly accessing the 6845 graphics	     */
/*	controller chip.  I found the register addresses and data by	     */
/*	debugging a sample graphics program that came with my Everex	     */
/*	Hercules compatible graphics card.  I have tried them on both	     */
/*	Leading Edge and Five Star computers with Hercules-compatible	     */
/*	graphics cards with 100% success, so I expect that it should	     */
/*	work.								     */
/*									     */
/*****************************************************************************/

// clear/fill functions (have to be here because they're called early on)

void HGCClearGraphics() // formerly called just grcls()
{
  memset(&screen[0], 0, 0x1eef);
  memset(&screen[0x2000], 0, 0x1eef);
  memset(&screen[0x4000], 0, 0x1eef);
  memset(&screen[0x6000], 0, 0x1eef);
}

void HGCFillScreen() // formerly called just fill_screen()
{
  memset(&screen[0], 0xff, 0x1eef);
  memset(&screen[0x2000], 0xff, 0x1eef);
  memset(&screen[0x4000], 0xff, 0x1eef);
  memset(&screen[0x6000], 0xff, 0x1eef);
}

void HGCClearText()// formerly called just cls()
{
  memset(screen, ' ', 2 * 80 * 25);
}

void HGCGraphicsMode() // formerly called just graphics()
{
  int i;

  outportb(0x3b8, 0);
  outportb(0x3bf, 3);
  for (i = 0; i < 18; i++)
  {
    outportb(0x3b4, i);
    outportb(0x3b5, gr6845[i]);
  }
  outportb(0x3b8, 6);
  HGCClearGraphics();
  outportb(0x3b8, 14);
}

/*****************************************************************************/
/*									     */
/*	Read the comment before about the graphics() routine to find	     */
/*	about this routine.						     */
/*									     */
/*****************************************************************************/

void HGCTextMode() // formerly called just text()
{
  int i;
  union REGS regs;

  regs.x.ax = 2;
  int86(0x10, &regs, &regs);
  outportb(0x3bf, 0);
  outportb(0x3b8, 0x28);
  for (i=0; i<18; i++) {
    outportb(0x3b4, i);
    outportb(0x3b5, te6845[i]);
  }
  HGCClearText();
}

/*****************************************************************************/
/*									     */
/*	The graphics memory starts at B000:0000, and very unfortunately      */
/*	is not mapped as you might expect.  Each row is simple.  A byte      */
/*	controls 8 pixels, one bit per pixel, high-order bit controlling     */
/*	the left-most pixel.  This continues sequentially accross the	     */
/*	row.  To find the address of the next row, however, is not so	     */
/*	easy.  By plotting many points, I finally came upon the algorithm    */
/*	in the macro byte_addr(x,y) in the graphics.h file.  Essentially,    */
/*	you add 0x2000 to get the next row unless you are greater than	     */
/*	0x8000 in which case you subtract 0x8000 and add 0x5a.	If	     */
/*	anybody understands why the screen was memory mapped in this	     */
/*	crazy way, please tell me.  I am very curious.			     */
/*									     */
/*****************************************************************************/

// formerly called just dot()
void HGCDrawDot(x, y, color)	      /* color: 1-dot on;  0-dot off */		      /*	x -1 is xor	     */
{
  int index;
  int bit_pos;

  index = byte_addr(x, y);
  bit_pos = bit_pos(x);
  if (color == WHITE) {
    screen[index] |= bit_pos;
  }
  else if (color == BLACK) {
    screen[index] &= ~bit_pos;
  }
  else if (color == XOR) {
    screen [index] ^= bit_pos;
  }
}

/*****************************************************************************/
/*									     */
/*	     This routine is taken from BYTE March 1988 page 252	     */
/*									     */
/*****************************************************************************/

// formerly called just line()
void HGCDrawLine(int xstart, int ystart, int xend, int yend, int color)
{
  int x, y, d, a, b, i;
  int dx_diag, dy_diag, dx_nondiag, dy_nondiag;
  int diag_inc, nondiag_inc;
  int swap;
  int index, bit_pos;

  x = xstart;
  y = ystart;
  a = xend - xstart;
  b = yend - ystart;
  if (a < 0) {
    a = -1 * a;
    dx_diag = -1;
  }
  else {
    dx_diag = 1;
  }
  if (b < 0) {
    b = -1 * b;
    dy_diag = -1;
  }
  else {
    dy_diag = 1;
  }
  if (a < b) {
    swap = a;
    a = b;
    b = swap;
    dx_nondiag = 0;
    dy_nondiag = dy_diag;
  }
  else {
    dx_nondiag = dx_diag;
    dy_nondiag = 0;
  }
  d = b + b - a;
  nondiag_inc = b + b;
  diag_inc = b + b - a - a;
  for (i = 0; i <= a; i++) {
    index = byte_addr(x, y);		/* This is an exact copy of the      */
    bit_pos = bit_pos(x);		    /* dot routine to subroutine calls   */
    if (color == WHITE) {		    /* which actually speeds up the      */
      screen[index] |= bit_pos; /* line routine 30%!!!		     */
    }
    else if (color == BLACK) {
      screen[index] &= ~bit_pos;
    }
    else if (color == XOR) {
      screen [index] ^= bit_pos;
    }
    if (d < 0) {
      x = x + dx_nondiag;
      y = y + dy_nondiag;
      d = d + nondiag_inc;
    }
    else {
      x = x + dx_diag;
      y = y + dy_diag;
      d = d + diag_inc;
    }
  }
}

/*****************************************************************************/
/*									     */
/*	     This is the line routine I wrote which is about 20% slower      */
/*	     than the BYTE routine, so I'll keep it for prosperity, but      */
/*	     won't use it.                                                   */
/*									     */
/*****************************************************************************/

// formerly called just line1()
void HGCDrawLineAlt(int x1, int y1, int x2, int y2, int color)
{
  int i;
  int dx, dy;
  int dxs, dys;

  dx = x2 - x1;
  dy = y2 - y1;
  dxs = ((dx >= 0) ? (1) : (-1));
  dys = ((dy >= 0) ? (1) : (-1));
  dx = dxs * dx;
  dy = dys * dy;
  if (dx == 0) {
    for (i= y1; ((dys==1) ? (i <= y2) : (i >= y2)); i += dys) {
      HGCDrawDot(x1, i, color);
    }
  }
  else if (dy == 0) {
    for (i = x1; ((dxs == 1) ? (i <= x2) : (i >= x2)); i += dxs) {
      HGCDrawDot(i, y1, color);
    }
  }
  else {
    if (dx >= dy) {
      for (i= x1; ((dxs==1) ? (i <= x2) : (i >= x2)); i += dxs) {
	      HGCDrawDot(i,((dys == 1) ? (y1 + (dxs * (i - x1) * dy) / dx) : (y1 - (dxs * (i-x1) *dy) /dx)), color);
      }
    }
    else {
      for (i = y1; ((dys == 1) ? (i <= y2) : (i >= y2)); i += dys) {
	      HGCDrawDot(((dxs == 1) ? (x1 + (dys * (i-y1) * dx) /dy) : (x1 - (dys * (i- y1) *dx) /dy)), i, color);
      }
    }
  }
}

/*****************************************************************************/
/*									     */
/* Module Name: draw							     */
/*									     */
/* Function: Draw draws the specified shape at the specified coordinates     */
/*	     in the specified color (white, black, or xor).  It does not     */
/*	     have to be on a byte boundary.				     */
/*									     */
/*****************************************************************************/

// formerly just called draw()
void HGCDrawShape(struct shape_type shape, int x, int y, int color)
{
  int a;
  int i;
  int j;
  int temp;
  int scr_index;
  int temp_index;
  int shift_num;

  scr_index = byte_addr(x,y);
  shift_num = bit_num(x);
  for (i = 0; i < shape.x; i++) {
    temp_index = scr_index;
    for (j = 0; j < shape.y; j++) {
        a = shape.shape_array[shape.x - i - 1 + j * shape.x] & 0xff;
        temp = a >> shift_num;
        if (color == OVERWRITE) {
	        screen[temp_index + shape.x - i] = (a^(temp << shift_num)) << (8 - shift_num);
	        screen[temp_index + shape.x - i - 1] = temp;
        }
        else if (color == WHITE) {
	        screen[temp_index+shape.x - i] |= (a^(temp << shift_num)) << (8 - shift_num);
	        screen[temp_index+shape.x - i - 1] |= temp;
      }
      else if (color == BLACK) {
	        screen[temp_index+shape.x - i] &= ~((a^(temp << shift_num)) << (8 - shift_num));
	        screen[temp_index+shape.x - i - 1] &= ~temp;
      }
      else if (color == XOR) {
	      screen[temp_index + shape. x - i] ^= (a^(temp << shift_num)) << (8 - shift_num);
	      screen[temp_index + shape.x- i - 1] ^= temp;
      }
      temp_index += 0x2000;
      if (temp_index > 0x8000) {
	      temp_index = temp_index - 0x8000 + 0x5a;
      }
    }
  }
}

/*****************************************************************************/
/*									     */
/* Module Name: draw_block						     */
/*									     */
/* Function: Draw draws the specified shape at the specified coordinates     */
/*	     in the specified color (white, black, or xor).  You must	     */
/*	     draw on byte boundaries only.  This is because this routine     */
/*	     does not shift bits at all.  It simply copies from the	     */
/*	     shape structure directly to the screen.  Because of this,	     */
/*	     it is very FAST.						     */
/*									     */
/*****************************************************************************/

// formerly called just draw_block()
void HGCDrawBlock(struct shape_type shape, int x, int y, int color)
{
  int i;
  int j;
  int scr_index;

  scr_index = byte_addr(x, y);
  for (j = 0; j < shape.y; j++) {
    switch (color) {
      case OVERWRITE:
	memmove(&screen[scr_index], &shape.shape_array[j * shape.x], shape.x);
	break;
      case WHITE:
	for (i = 0; i < shape.x; i++) {
	  screen[scr_index + i] |= shape.shape_array[j * shape.x + i];
	}
	break;
      case BLACK:
	for (i = 0; i < shape.x; i++) {
	  screen[scr_index + i] &= ~(shape.shape_array[j * shape.x + i]);
	}
	break;
      case XOR:
	for (i = 0; i < shape.x; i++) {
	  screen[scr_index + i] ^= shape.shape_array[j * shape.x + i];
	}
	break;
    }
    scr_index += 0x2000;
    if (scr_index > 0x8000) {
      scr_index = scr_index - 0x8000 + 0x5a;
    }
  }
}

/* These shift-by-pixel functions are all incomplete except for shift_right, with a little effort these should be doable based on that */

// formerly called just shift_up()
void HGCShiftUp(struct shape_type shape, int x, int y, int shift_num)
{

}

// formerly called just shift_down()
void HGCShiftDown(struct shape_type shape, int x, int y, int shift_num)
{
  
}

// formerly called just shift_left()
void HGCShiftLeft(struct shape_type shape, int x, int y, int shift_num)
{
  
}
/*****************************************************************************/
/*									     */
/* Module Name: shift_right						     */
/*									     */
/* Function: Shifts the rectangle whose upper-left corner is at x,y and      */
/*	     whose size is determined by the size of shape shift_num	     */
/*	     pixels to the right, with a maximum shift_num of 8.  It is      */
/*	     not too fast because it computes the addresses of all pixels    */
/*	     in every call, and it has to shift the bits from byte into      */
/*	     the next.	To shift right by 8 pixels (one byte), use the	     */
/*	     routine shift_right_block which does not have to shift pixels,  */
/*	     but can just move a byte of memory.			     */
/*									     */
/*****************************************************************************/

// formerly called just shift_right()
void HGCShiftRight(struct shape_type shape, int x, int y, int shift_num)
{
  int a;
  int b;
  int i;
  int j;
  int temp;
  int scr_index;
  int temp_index;

  scr_index = byte_addr(x, y);
  for (i = 0; i<shape.x + 2; i++) {
    temp_index = scr_index;
    for (j = 0; j < shape.y; j++) {
      a = screen[temp_index + shape.x - i] & 0xff;
      b = screen[temp_index + shape.x - i + 1] & 0xff;
      temp = a >> shift_num;
      screen[temp_index + shape.x - i + 1] = b >> shift_num;
      screen[temp_index + shape.x-i + 1] |= (a^(temp << shift_num)) << (8 - shift_num);
      temp_index += 0x2000;
      if (temp_index > 0x8000) {
	      temp_index = temp_index - 0x8000 + 0x5a;
      }
    }
  }
}

/*****************************************************************************/
/*									     */
/* Module Name: shift_xxx_block 					     */
/*									     */
/* Function: The shift block routines shift a specified shape by 8 bits      */
/*	     in any direction.	The shape must be "left-justified" on a      */
/*	     byte boundary.  That means, drawing the shape at x-coord	     */
/*	     16, 24, 32, etc. is ok, but not 25 (unless you want to shift    */
/*	     extra pixels).						     */
/*									     */
/*	     These are the FASTEST way to move shape around.		     */
/*									     */
/*****************************************************************************/

// formerly called just shift_up_block()
void HGCShiftBlockUp(struct shape_type shape, int x, int y)
{
  int i;
  int old_index;
  int scr_index;

  old_index = byte_addr(x, y);
  scr_index = old_index - 2 * 0x5a;
  for (i = 0; i < shape.y; i++) {
    memmove(screen + scr_index, screen + old_index, shape.x);
    old_index += 0x2000;
    if (old_index > 0x8000) {
      old_index = old_index - 0x8000 + 0x5a;
    }
    scr_index += 0x2000;
    if (scr_index > 0x8000) {
      scr_index = scr_index - 0x8000 + 0x5a;
    }
  }
  scr_index = byte_addr(x, y + shape.y - 1);
  for (i = 0; i < 8; i++) {
    memset(screen + scr_index, 0, shape.x);
    scr_index -= 0x2000;
    if (scr_index < 0) {
      scr_index = scr_index + 0x8000 - 0x5a;
    }
  }
}

// formerly called just shift_down_block()
void HGCShiftBlockDown(struct shape_type shape, int x, int y)
{
  int i;
  int old_index;
  int scr_index;

  old_index = byte_addr(x, y + shape.y - 1);
  scr_index = old_index + 2 * 0x5a;
  for (i = 0; i < shape.y; i++) {
    memmove(screen + scr_index, screen + old_index, shape.x);
    old_index -= 0x2000;
    if (old_index < 0) {
      old_index = old_index + 0x8000 - 0x5a;
    }
    scr_index -= 0x2000;
    if (scr_index < 0) {
      scr_index = scr_index + 0x8000 - 0x5a;
    }
  }
  scr_index = byte_addr(x, y);
  for (i = 0; i < 8; i++) {
    memset(screen+scr_index,0,shape.x);
    scr_index += 0x2000;
    if (scr_index > 0x8000) {
      scr_index = scr_index - 0x8000 + 0x5a;
    }
  }
}

// formerly called just shift_left_block()
void HGCShiftBlockLeft(struct shape_type shape, int x, int y)
{
  int i;
  int scr_index;

  scr_index = byte_addr(x, y);
  for (i = 0; i < shape.y; i++) {
    memmove(screen + scr_index - 1, screen + scr_index, shape.x);
    screen[scr_index + shape.x - 1] = 0;
    scr_index += 0x2000;
    if (scr_index > 0x8000) {
      scr_index = scr_index - 0x8000 + 0x5a;
    }
  }
}

// formerly called just shift_right_block()
void HGCShiftBlockRight(struct shape_type shape, int x, int y)
{
  int i;
  int scr_index;

  scr_index = byte_addr(x, y);
  for (i = 0; i < shape.y; i++) {
    memmove(screen + scr_index + 1, screen + scr_index, shape.x);
    screen[scr_index] = 0;
    scr_index += 0x2000;
    if (scr_index > 0x8000) {
      scr_index = scr_index - 0x8000 + 0x5a;
    }
  }
}
