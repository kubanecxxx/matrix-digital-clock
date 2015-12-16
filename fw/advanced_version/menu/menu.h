#ifndef MENU_H
#define MENU_H

#include "scheduler.hpp"
#include "configuration.h"
#include "rtc_control.h"

#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_ENTER 4

class Screen;
class TimeScreen;
class NumberScreen;

class Menu
{
public:
    Menu();
    static void Play(void * self);
    void Init();

    static uint8_t flash;
    static uint8_t cnt;

    static void num2str(NumberScreen *scr, uint8_t num);
    static void time2str(TimeScreen *scr, const config_time_t & t);

    typedef enum {CLOCKS,SAVING, MENU} state_t;

    static configuration_t configuration_temp;

private:
    Screen * current;
    Scheduler sh;
    bool screen_active;
    bool screen_active_edge;
    bool screen_active_old;
    uint8_t button_old;
    uint8_t counter;
    uint16_t timeout;
    uint16_t saving_timeout;
    state_t machine;


    void static fillComponents(const configuration_t * c);
    void static fillStruct(configuration_t * c);

    static Screen * createScreens();

    void processMenu(uint8_t button, uint8_t button_rising_edge);
};




#endif // MENU_H
