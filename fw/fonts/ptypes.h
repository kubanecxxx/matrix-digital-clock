#ifndef PTYPES_H
#define PTYPES_H

#include <stdint.h>

//#define NULL ((void*) 0)

namespace piris
{

//keyevent
typedef uint16_t key_t;

//default keys, other can be easily added
#define kUP   1
#define kDOWN 2
#define kENTER 4


typedef enum keyEvt_t
{
   kNONE, RELEASED, PRESSED
} keyEvt_t;

typedef uint16_t pixel_t;
typedef uint16_t color_t;

typedef enum globalColors_t
{
    INVALID= 0,RED = 0xF100, GREEN = 0x07E0, BLACK = 1, WHITE = 0xFFFF, BLUE = 0x001F
} globalColors_t;

//touch
typedef enum touchEvt_t
{
    NONE,PRESS, RELEASE, MOVE
} touchEvt_t;

}

#endif // PTYPES_H
