#include "hal.h"
#include "ch.h"
#include "menu.h"
#include "matrix_abstraction.h"
#include "scheduler.hpp"
#include "chsprintf.h"
#include "screens.h"
#include "print.h"
#include <string.h>

print matrix;

uint8_t Menu::flash = 2;
uint8_t Menu::cnt = 0;
configuration_t  Menu::configuration_temp;

DECL_SCREEN(Time,setup_time, "Time");
DECL_SCREEN(Combo,time_source,"Source");
DECL_SCREEN_CUSTOM(Text,wifi_ssid,"SSID",32,Menu::configuration_temp.ssid);
DECL_SCREEN_CUSTOM(Text,wifi_pass,"Password",32,Menu::configuration_temp.password);
DECL_SCREEN(Number,luminance_max,"Max luminance");
DECL_SCREEN(Number,luminance_min,"Min luminance");
DECL_SCREEN(Number,photo_day, "Day opto");
DECL_SCREEN(Number,photo_night, "Night opto");
DECL_SCREEN(Combo, photoTime , "Switch type" );
DECL_SCREEN(Time, toDay, "Day time" );
DECL_SCREEN(Time, toNight, "Night time"  );


Menu::Menu()
{
    Screen::setupTime = &setup_time;
    current = NULL;
    counter = 0;
    timeout = 0;
    screen_active = false;
    saving_timeout = 0;
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

    ma_init();
    config_retrieve();
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

    if (m->machine == SAVING)
    {
        matrix.enable(true);
        matrix.put("Saving !");

        if (m->saving_timeout++ > 30)
        {
            m->saving_timeout = 0;
            m->machine = CLOCKS;
        }

    }
    else if (m->machine == MENU)
    {
        //process menu
        matrix.enable(true);
        m->processMenu(button,b);
    }
    else if (m->machine == CLOCKS)
    //process clock itself
    {
        //matrix.put("Hodiny");
        //matrix.enable(false);
        //ma_time_loop(1);
        char buffer[20];
        rtc_time_t t;
        rtc_control_GetTime(&t);
        piris::chsprintf(buffer,"%2d:%.2d:%.2d", t.hours,t.minutes,t.seconds );
        matrix.put(buffer, NULL);
    }

    //escape from menu or enter to menu
    if ((button == (KEY_RIGHT  | KEY_LEFT)) && m->machine != SAVING)
    {
        //get out of menu after one second of holding
        //or enable menu after one second
        if (m->counter++ > 20)
        {
            m->counter = 0;
            if (m->machine == CLOCKS)
            {
                m->machine = MENU;
            }
            else if (m->machine == MENU)
            {
                m->machine = CLOCKS;
            }

            if (m->machine == CLOCKS)
            {
                //save temporary configuration to working configuration if changed and save to EEPROM
                //print changes saved
                fillStruct(&(m->configuration_temp));

                //if changed - save
                if (memcmp(&(m->configuration_temp), &global_configuration, sizeof(configuration_t)))
                {
                    memcpy(&global_configuration, &(m->configuration_temp),sizeof(configuration_t));
                    config_save();
                    m->machine = SAVING;
                    //save
                }
                else
                {
                    //do nothing
                }

            }
            else if(m->machine == MENU)
            {
                //fill components data from config structure
                //copy configuration to temporary struct
                memcpy (&(m->configuration_temp), &global_configuration, sizeof(configuration_t));
                m->fillComponents(&(m->configuration_temp));
            }
        }
    }
    else
    {
        //start counting from zero when released
        m->counter = 0;
    }

    m->screen_active_edge = false;
    if (m->screen_active != m->screen_active_old)
    {
        m->screen_active_edge = m->screen_active && !m->screen_active_old;
        m->screen_active_old = m->screen_active;
    }
}

void Menu::processMenu(uint8_t button, uint8_t button_rising_edge)
{
    //screen handles events and printing its data
    if (screen_active)
    {
        if (current == &setup_time  && screen_active_edge)
        {
            rtc_time_t t;
            rtc_control_GetTime(&t);

            setup_time.hours = t.hours;
            setup_time.minutes = t.minutes;
        }

        current->handle(button_rising_edge);
    }
    //screen is not active - label is handled by Menu
    else if (current)
    {
        matrix.put(current->label);
    }

    //brighntness flashing variable - used as a selected character highlighting
    if (cnt++ > 4)
    {
        cnt = 0;
        if (current->selected)
            flash = flash == 2 ? 7 : 2;
        else
            flash = 2;
    }

    //go through the screens and (de)activate them
    //deactivate
    if (button == (KEY_RIGHT  | KEY_LEFT) &&   (screen_active))
    {
        screen_active = false;
        current->selected = false;

        //save components data to temporary configration table
    }
    //select next screen
    else if (button_rising_edge == KEY_RIGHT && !screen_active)
    {
        current = current->next;
    }
    //select previous screen;
    else if (button_rising_edge == KEY_LEFT&& !screen_active)
    {
        current = current->prev;
    }
    //enter the screen
    else if (button_rising_edge == KEY_ENTER )
    {
        screen_active = true;
    }

    //auto get out after 1/2 of minute if user is inactive
    if (timeout++ > 20 * 30)
    {
        screen_active = false;
        current->selected = false;
        machine = CLOCKS;
        timeout = 0;
    }
    //reset timeout timer if user does something
    if (button_rising_edge)
        timeout = 0;
}

void Menu::fillComponents(const configuration_t *c)
{
    time_source.comboIndex = c->source;
    photoTime.comboIndex = c->switch_type;
    toDay.hours = c->toDay.hours;
    toDay.minutes = c->toDay.minutes;
    toNight.hours = c->toNight.hours;
    toNight.minutes = c->toNight.minutes;
    luminance_max.number = c->maxLuminance;
    luminance_min.number = c->minLuminance;
    photo_day.number = c->photoDay;
    photo_night.number = c->photoNight;

    //password and ssid are directly passed as pointer
    //current time is transfered elsewhere
}

void Menu::fillStruct(configuration_t *c)
{
    c->source = time_source.comboIndex;
    c->switch_type = photoTime.comboIndex;
    c->maxLuminance = luminance_max.number;
    c->minLuminance = luminance_min.number;
    c->photoDay = photo_day.number;
    c->photoNight = photo_night.number;
    c->toDay.hours = toDay.hours;
    c->toDay.minutes = toDay.minutes;
    c->toNight.hours = toNight.hours;
    c->toNight.minutes = toNight.minutes;

    //password and ssid are directly passed as pointer
    //current time is transfered elsewhere
}

static const char * combo1[] = {"Manual", "DCF", "Wifi"};
static const char * combo2[] = {"Time" , "Opto"};


Screen * Menu::createScreens()
{
    setup_time.pairNext(&time_source);
    time_source.pairNext(&wifi_ssid);
    wifi_ssid.pairNext(&wifi_pass);
    wifi_pass.pairNext(&luminance_min);
    luminance_min.pairNext(&luminance_max);
    luminance_max.pairNext(&photo_day);
    photo_day.pairNext(&photo_night);
    photo_night.pairNext(&photoTime);
    photoTime.pairNext(&toDay);
    toDay.pairNext(&toNight);
    toNight.pairNext(&setup_time);


    time_source.setCombo(combo1,3);
    photoTime.setCombo(combo2,2);

    return &setup_time;
}
