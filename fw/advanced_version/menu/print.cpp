#include "print.h"
#include "pfont.h"
#include "matrix_abstraction.h"
#include <string.h>

print::print()
{
    psh.SetMethod(sh_cb);
    psh.SetArg(this);
    psh.SetPeriod(MS2ST(50));

    psh.Register();

    offset_pixel = 0;
    zero = true;
    memset (print_buffer, 0,32);
    memset (bright_buffer , 7, 32);
}

void print::put(const char *str, const uint8_t *bright, const piris::PFont * font)
{
    if (strcmp(str,print_buffer))
    {
        strcpy(print_buffer, str);
        offset_pixel = 0;
        timeout = 0;
        dir_temp = 0;
        dir = 0;
        this->font = font;
    }

    zero = true;

    if (bright)
    {
        memcpy(bright_buffer, bright, 10);
        zero = false;
    }
}


void print::sh_cb(arg_t self)
{
    print * m = (print *) self;
    m->task();
}

void print::task()
{

    const piris::PFont * f;
    f = font ? font : &piris::PFont::terminus12;
    uint8_t w = f->width();
    uint16_t x = 63 - 2 - w;
    uint16_t y = 1;
    uint8_t b;

    char * str = print_buffer;
    uint8_t * bright = NULL;
    if (!zero)
        bright = bright_buffer;

    uint8_t len = strlen(str);
    if (len > 10)
    {
        int8_t d = dir_temp;
        //after some time
        if (offset_pixel == 0 )
        {
            dir_temp = 1;
            dir = 0;
        }
        else if (offset_pixel == w * (len - 10))
        {
            dir_temp = -1;
            dir = 0;
        }

        if (d != dir_temp)
            timeout = 0;

        if (timeout++ > 10)
        {
            dir = dir_temp;
        }


        offset_pixel += dir;


        x += offset_pixel ;
        //str += offset_pixel / f->width();
    }

    ma_clear_screen();
    while(*str )
    {
        if (bright)
        {
            b = *bright++;
        }
        else
        {
            b = 7;
        }

        ma_putchar(*str++,x,y,f,b);
        x -= f->width();
    }
    ma_buffer_flush();
}

void print::enable(bool en)
{
    en ? psh.Register() : psh.Unregister();
}
