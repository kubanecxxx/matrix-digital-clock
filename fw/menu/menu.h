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

    typedef enum {CLOCKS,SAVING, MENU} state_t;
    typedef enum {DAY, NIGHT, INVALID} clocks_t;
    typedef enum {OUT, IN, FINISHED} fading_t;

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
    state_t old_state;
    clocks_t current_clocks;
    clocks_t render_clocks;

    void setMode(state_t mode, clocks_t clocks = INVALID);

    static void fade_cb(arg_t self);
    void fade_cb();
    Scheduler fade;
    uint16_t fading_stage;
    fading_t fading;


    Scheduler clocks;
    static void clocks_task(arg_t self);
    uint8_t centr;



    void pwm_control();
    clocks_t getClockMode();
    uint16_t brightness;
    uint16_t brightness_new;


    void static fillComponents(const configuration_t * c);
    void static fillStruct(configuration_t * c);

    static Screen * createScreens();

    void processMenu(uint8_t button, uint8_t button_rising_edge);
};




#endif // MENU_H
