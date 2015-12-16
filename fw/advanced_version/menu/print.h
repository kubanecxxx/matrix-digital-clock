#ifndef PRINT_H
#define PRINT_H

#include "scheduler.hpp"
#include "pfont.h"

class print
{
public:
    print(void);
    void put(const char * str, const uint8_t * bright = NULL, const piris::PFont * font = NULL);

    void enable(bool en);

private:
    Scheduler psh;
    void print_task(arg_t a);
    char print_buffer[32];
    uint8_t bright_buffer[32];
    bool zero;
    uint8_t offset_pixel;
    int8_t dir;
    int8_t dir_temp;
    uint8_t timeout;
    const piris::PFont * font;

    void task();
    static void sh_cb(arg_t self);
};

#endif // PRINT_H
