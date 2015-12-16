#include "ch.h"
#include "screens.h"
#include <string.h>
#include "menu.h"
#include "chsprintf.h"
#include "rtc_control.h"
#include "print.h"

extern print matrix;

Screen * Screen::setupTime;

Screen::Screen(const properties_t &props):
    p(props),
    c(NULL)
{
    index = 0;
    selected = false;
}

void Screen::pairNext(Screen *Next)
{
    next = Next;
    Next->prev = this;
}

//handle button events
void Screen::handle(uint8_t buttons)
{
    char buffer[12];
    uint8_t bright[12];
    memset (bright, 7,10);
    bool was_selected = selected;

    //go through all characters
    if (!selected)
    {
        if (buttons == KEY_RIGHT)
            index < p.items_count - 1 ? index++ : index =0;
        else if (buttons == KEY_LEFT)
            if (p.items_count == 32)
            {
                index > 0 ? index-- : index = 0;
            }
            else
            {
                index > 0 ? index-- : index = p.items_count - 1;
            }

        else if (buttons == KEY_ENTER)
            selected = true;
    }

    if (p.items_count == 4)
    {
        //fill hours/minutes frome elsewhere - saved and RTC
        //probably arbitrary pointer to user function
        //render current time


        piris::chsprintf(buffer, "%.2d:%.2d",hours,minutes);

        uint8_t inc = index > 1 ? 1 : 0;
        if (was_selected)
        {
            time(buttons);
        }
        bright[index + inc] = Menu::flash;
        matrix.put(buffer,bright);
    }
    //combobox
    else if (p.items_count == 1)
    {
        chDbgAssert(c, "No combo box defined");
        if (was_selected)
        {
            combobox(buttons);
            memset(bright, Menu::flash, sizeof(bright));
        }
        matrix.put(c->table[(uint8_t)p.data[index]],bright);
    }
    //text
    else if (p.items_count == 32)
    {
        //shift it according index
        //index in middle
        uint8_t idx ;
        if (index < 3)
        {
            idx = 0;
        }
        else if (index > 22)
        {
            idx = 22;
        }
        else
        {
            idx = index - 3;
        }

        memcpy(buffer, p.data + idx , 9);
        buffer[10] = 0;
        if (was_selected)
        {
            text(buttons);
        }
        bright[index - idx] = Menu::flash;
        matrix.put(buffer,bright);

    }
    //number
    else if (p.items_count == 3)
    {
        piris::chsprintf(buffer, "%.3d", number);
        if (was_selected)
        {
            numberHandle(buttons, 100);
        }
        bright[index] = Menu::flash;
        matrix.put(buffer,bright);
    }


}


void Screen::time(uint8_t buttons)
{
    uint8_t table_time[4] = {2, 9,5,9};
    if (p.data[0] == 2)
        table_time[1] = 3;
    if (p.data[1] > 3)
        table_time[0] = 1;

    if (buttons == KEY_RIGHT)
    {
        p.data[index] = p.data[index] < 0 + table_time[index]  ? p.data[index]+1 : 0;
    }
    else if (buttons == KEY_LEFT)
    {
        p.data[index] = p.data[index] > 0 ?  p.data[index]-1  : table_time[index]  + 0;
    }

    hours = p.data[1]  + (p.data[0] )*10;
    minutes = p.data[3]  + (p.data[2] )*10;

    if (buttons == KEY_ENTER)
    {
        //save values elsewhere (rtc or flash table)
        //save new time
        if (this == setupTime)
        {
            rtc_time_t t;
            t.hours = hours;
            t.minutes = minutes;
            t.seconds = 0;
            rtc_control_SetTime(&t);
        }

        selected = false;
    }
}

void Screen::combobox(uint8_t buttons)
{
    if (buttons == KEY_RIGHT)
    {
        p.data[index] = p.data[index] < c->count - 1  ?  p.data[index] + 1 : 0;
    }
    else if (buttons == KEY_LEFT)
    {
        p.data[index] = p.data[index] > 0 ?  p.data[index]-1  : c->count - 1;
    }
    else if (buttons == KEY_ENTER)
    {
        //save
        selected = false;
    }

}


void Screen::numberHandle(uint8_t buttons, uint16_t maximum)
{
    uint8_t modulo = 0;
    int8_t i = index;
    do
    {
        modulo = maximum % 10;
        maximum = maximum / 10;
        modulo = modulo == 0 ? 9 : modulo;
    } while (i-- > 0);


    if (buttons == KEY_RIGHT)
    {
        p.data[index] = p.data[index] < modulo  ?  p.data[index] + 1 : 0;
    }
    else if (buttons == KEY_LEFT)
    {
        p.data[index] = p.data[index] > 0 ?  p.data[index]-1  : modulo;
    }

    number = p.data[2] + 10 * p.data[1] + 100 * p.data[0];
    if (buttons == KEY_ENTER)
    {
        //save
        selected = false;
    }
}

void Screen::text(uint8_t buttons)
{
    char data = p.data[index];
    if (buttons == KEY_RIGHT)
    {
        data = data < 126 ? data+1 : 31;
        data = (data > 126)  | (data < 31) ? 33 : data;
        p.data[index] = data;
    }
    else if (buttons == KEY_LEFT)
    {
        data = data > 31 ?  data-1  : 126;
        data = data < 31 ?  126 : data;
        p.data[index] = data;
    }
    else if (buttons == KEY_ENTER)
    {
        //save
        //31 is end - and remove data after
        char * d = strchr(p.data, 31);
        if (d)
        {
            uint8_t sze = strlen(d);
            sze = sze < 10 ? sze : 10;
            memset(d,0,sze);

        }
        selected = false;
    }
}
