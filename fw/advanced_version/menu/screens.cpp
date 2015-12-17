#include "ch.h"
#include "screens.h"
#include <string.h>
#include "menu.h"
#include "chsprintf.h"
#include "rtc_control.h"
#include "print.h"

extern print matrix;

Screen * Screen::setupTime;

/********************************************************
 * base class
 *******************************************************/
Screen::Screen(const char *lab):
    label(lab),
    index(0),
    selected(false)
{

}

void Screen::pairNext(Screen *Next)
{
    next = Next;
    Next->prev = this;
}

//handle button events
void Screen::handle(uint8_t buttons)
{
    bool was_selected = selected;

    //go through all characters
    if (!selected)
    {
        if (buttons == KEY_RIGHT)
            index < items_count - 1 ? index++ : index =0;
        else if (buttons == KEY_LEFT)
            if (type == TEXT)
            {
                index > 0 ? index-- : index = 0;
            }
            else
            {
                index > 0 ? index-- : index = items_count - 1;
            }

        else if (buttons == KEY_ENTER)
            selected = true;
    }

    subHandle(buttons,was_selected);

}

uint32_t Screen::strToNum(const char *str)
{
    uint8_t len = strlen(str);
    uint32_t number = 0;

    for (int i = 0 ; i < len; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            number = str[i]-'0'  + 10 * number ;
        }
        else
        {
            number = -1;
            break;
        }
    }

    return number;
}

uint32_t Screen::power(uint8_t num, uint8_t exp)
{
    if (!exp)
        return 1;

    exp--;

    uint32_t t = num;
    while(exp-- )
    {
        t *= num;
    }

    return t;
}

/********************************************************
 * text screen
 *******************************************************/
TextScreen::TextScreen(const char * p, char * dat, uint8_t count):
    Screen(p),
    data(dat)
{
    type = TEXT;
    items_count = count;
}

void TextScreen::subHandle(uint8_t buttons, bool was_selected)
{
    char buffer[BUFFER_SIZE];
    uint8_t bright[BUFFER_SIZE];
    memset(bright,7,BUFFER_SIZE);
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

    memcpy(buffer, data + idx , 9);
    buffer[10] = 0;
    if (was_selected)
    {
        text(buttons);
    }
    bright[index - idx] = Menu::flash;
    matrix.put(buffer,bright);
}

void TextScreen::text(uint8_t buttons)
{
    char & data (this->data[index]);
    if (buttons == KEY_RIGHT)
    {
        data = data < 126 ? data+1 : 31;
        data = (data > 126)  | (data < 31) ? 33 : data;
    }
    else if (buttons == KEY_LEFT)
    {
        data = data > 31 ?  data-1  : 126;
        data = data < 31 ?  126 : data;
    }
    else if (buttons == KEY_ENTER)
    {
        //save
        //31 is end - and remove data after
        char * d = strchr(this->data, 31);
        if (d)
        {
            uint8_t sze = strlen(d);
            sze = sze < 10 ? sze : 10;
            memset(d,0,sze);

        }
        selected = false;
    }
}

/********************************************************
 * number screen
 *******************************************************/
NumberScreen::NumberScreen(const char * p):
    Screen(p)
{
    type = NUMBER;
    items_count = 4;
}

void NumberScreen::subHandle(uint8_t buttons, bool was_selected)
{
    uint8_t bright[BUFFER_SIZE];
    memset(bright,7,BUFFER_SIZE);
    char buffer[10];

    if (was_selected)
    {
        numberHandle(buttons,0, 100);
    }
    piris::chsprintf(buffer, "%.3d", number);
    bright[index] = Menu::flash;
    matrix.put(buffer,bright);
}

void NumberScreen::numberHandle(uint8_t buttons, uint16_t minimum, uint16_t maximum)
{
    if (buttons == KEY_RIGHT)
    {
        number += power(10,2-index);
    }
    else if (buttons == KEY_LEFT)
    {
        number -= power(10,2-index);
    }

    number = number <= maximum ? number : maximum;
    number = number >= minimum ? number : minimum;

    if (buttons == KEY_ENTER)
    {
        //save
        selected = false;
    }
}

/********************************************************
 * time screen
 *******************************************************/
TimeScreen::TimeScreen(const char * p):
    Screen(p)
{
    type = TIME;
    items_count = 4;
}

void TimeScreen::subHandle(uint8_t buttons, bool was_selected)
{
    char buffer[BUFFER_SIZE];
    uint8_t bright[BUFFER_SIZE];
    memset(bright,7,BUFFER_SIZE);

    //fill hours/minutes frome elsewhere - saved and RTC
    uint8_t inc = index > 1 ? 1 : 0;
    if (was_selected)
    {
        piris::chsprintf(buffer, "%.2d%.2d",hours,minutes);
        //char & d(buffer[index]);
        char d = buffer[index] - '0';
        uint8_t table_time[4] = {2, 9,5,9};
        if (buffer[0] - '0' == 2)
            table_time[1] = 3;
        if (buffer[1] - '0' > 3)
            table_time[0] = 1;

        if (buttons == KEY_RIGHT)
        {
            d = d < table_time[index]  ? d+1 : 0;
        }
        else if (buttons == KEY_LEFT)
        {
            d = d > 0 ?  d-1  : table_time[index] ;
        }

        buffer[index] = d + '0';

        minutes = strToNum(buffer + 2);
        buffer[2] = 0;
        hours = strToNum(buffer);

        if (buttons == KEY_ENTER)
        {
            //just save modified time
            //rest is saved elsewhere
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

    piris::chsprintf(buffer, "%.2d:%.2d",hours,minutes);
    bright[index + inc] = Menu::flash;
    matrix.put(buffer,bright);
}

/********************************************************
 * combo screen
 *******************************************************/
ComboScreen::ComboScreen(const char * p):
    Screen(p),
    comboIndex(0),
    table(NULL),
    count(0)
{
    type = COMBO;
    items_count = 1;
}

void ComboScreen::subHandle(uint8_t buttons, bool was_selected)
{
    uint8_t bright[BUFFER_SIZE];
    memset(bright,7,BUFFER_SIZE);

    chDbgAssert(table, "No combo box defined");
    if (was_selected)
    {
        combobox(buttons);
        memset(bright, Menu::flash, sizeof(bright));
    }
    matrix.put(table[comboIndex],bright);
}

void ComboScreen::combobox(uint8_t buttons)
{
    if (buttons == KEY_RIGHT)
    {
        comboIndex = comboIndex < count - 1  ?  comboIndex + 1 : 0;
    }
    else if (buttons == KEY_LEFT)
    {
        comboIndex = comboIndex > 0 ?  comboIndex -1 : count - 1;
    }
    else if (buttons == KEY_ENTER)
    {
        //save
        selected = false;
    }

}
