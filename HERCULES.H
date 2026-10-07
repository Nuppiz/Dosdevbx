#define UP	1
#define DOWN	2
#define LEFT	3
#define RIGHT	4

#define OVERWRITE 2
#define WHITE	1
#define BLACK	0
#define XOR    -1

#define byte_addr(x,y) (((y)/4)*90 + ((y)&3)*0x2000 + ((x)/8))
#define bit_pos(x)     (1 << (7 - ((x)&7)))
#define bit_num(x)     ((x)&7)

struct shape_type {
  int x;
  int y;
  char *shape_array;
};