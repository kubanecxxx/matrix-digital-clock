#include "hal.h"
#include "ch.h"

#include "menu.h"
#include "matrix_abstraction.h"
#include "scheduler.hpp"

#include "pfont.h"
#include "chsprintf.h"

extern Screen * createScreens();
uint8_t Menu::flash = 2;
uint8_t Menu::cnt = 0;

Menu::Menu()
{
    current = NULL;
    counter = 0;
    timeout = 0;
}

void Menu::Play(void * self)
{
    Menu * m = (Menu*) self;

    //handle buttons
    //if selected - pass buttons into the screen

    uint8_t button;
    button = palReadPad(BUTTON_LEFT_PORT,BUTTON_LEFT_PIN) ^ 1;
    button |= (palReadPad(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN)^1) << 1;
    button |= (palReadPad(BUTTON_ENTER_PORT,BUTTON_ENTER_PIN)^1) << 2;

    uint8_t b = (m->button_old ^ 0b111) & button;
    m->button_old = button;

    //process menu
    if (m->menu_active)
    {
        //screen handles events and printing its data
        if (m->screen_active)
        {
            ma_clear_screen();
            m->current->handle(b);
            ma_buffer_flush();
        }
        //screen is not active - label is handled by Menu
        else if (m->current)
        {
            ma_clear_screen();
            print (m->current->p.label);
            ma_buffer_flush();
        }

        //brighntness flashing variable - used as a selected character highlighting
        if (cnt++ > 4)
        {
            cnt = 0;
            if (m->current->selected)
                flash = flash == 2 ? 7 : 2;
            else
                flash = 2;
        }

        //go through the screens and (de)activate them
        //deactivate
        if (button == (KEY_RIGHT  | KEY_LEFT) &&   (m->screen_active))
        {
            m->screen_active = false;
            m->current->selected = false;

            //save components data to temporary configration table
        }
        //select next screen
        else if (b == KEY_RIGHT&& !m->screen_active)
        {
            m->current = m->current->next;
        }
        //select previous screen;
        else if (b == KEY_LEFT&& !m->screen_active)
        {
            m->current = m->current->prev;
        }
        //enter the screen
        else if (b == KEY_ENTER )
        {
            m->screen_active = true;
        }

        //auto get out after 1/2 of minute if user is inactive
        if (m->timeout++ > 20 * 30)
        {
            m->screen_active = false;
            m->current->selected = false;
            m->menu_active = false;
            m->timeout = 0;
        }
        //reset timeout timer if user does something
        if (b)
            m->timeout = 0;

    }
    //process clock itself
    else
    {
        ma_clear_screen();
        print("Hodiny");

        ma_buffer_flush();
    }

    if ((button == (KEY_RIGHT  | KEY_LEFT)))
    {
        //get out of menu after one second of holding
        //or enable menu after one second
        if (m->counter++ > 20)
        {
            m->counter = 0;
            m->menu_active = !m->menu_active;

            if (!m->menu_active)
            {
                //save temporary configuration to working configuration if changed and save to EEPROM
                //print changes saved
            }
            else
            {
                //fill components data from config structure
                //copy configuration to temporary struct
            }
        }
    }
    else
    {
        //start counting from zero when released
        m->counter = 0;
    }
}

void Menu::Init()
{
    sh.SetMethod(Play);
    sh.SetArg(this);
    sh.SetPeriodMilliseconds(50);
    sh.SetType(PERIODIC);

    sh.Register();

    current = createScreens();

    palSetPadMode(BUTTON_ENTER_PORT,BUTTON_ENTER_PIN, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(BUTTON_LEFT_PORT,BUTTON_LEFT_PIN, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(BUTTON_RIGHT_PORT,BUTTON_RIGHT_PIN, PAL_MODE_INPUT_PULLUP);

}

void Menu::print(const char *str, const uint8_t * bright)
{
    uint16_t x = 50;
    uint16_t y = 1;
    piris::PFont * f = &piris::PFont::terminus12;
    uint8_t b;

    while(*str)
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
}




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
    char buffer[10];
    uint8_t bright[10];
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


    //render current time
    if (p.items_count == 4)
    {
        //fill hours/minutes frome elsewhere - saved and RTC
        //probably arbitrary pointer to user function
        piris::chsprintf(buffer, "%.2d:%.2d",hours,minutes);

        uint8_t inc = index > 1 ? 1 : 0;
        if (was_selected)
        {
            time(buttons);
        }
        bright[index + inc] = Menu::flash;
        Menu::print(buffer,bright);
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
        Menu::print(c->table[(uint8_t)p.data[index]],bright);
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
        else
        {
            idx = index - 3;
        }

        strcpy(buffer,p.data + idx);
        if (was_selected)
        {
            text(buttons);
        }
        bright[index - idx] = Menu::flash;
        Menu::print(buffer,bright);

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
        Menu::print(buffer,bright);
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
        //save values elsewhere (rtc or flash table) - probably done by callback
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
        p.data[index] = data < 126 ? data+1 : 31;
        p.data[index] = (data > 126)  | (data < 31) ? 33 : data;
    }
    else if (buttons == KEY_LEFT)
    {
        p.data[index] = data > 31 ?  data-1  : 126;
        p.data[index] = data < 31 ?  126 : data;
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
