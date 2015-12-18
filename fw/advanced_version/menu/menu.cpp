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

#define FADE_CONST 30

Menu::Menu()
{
    Screen::setupTime = &setup_time;
    current = NULL;
    counter = 0;
    timeout = 0;
    screen_active = false;
    saving_timeout = 0;

    fade.SetMethod(fade_cb);
    fade.SetArg(this);
    fade.SetPeriodMilliseconds(10);

    clocks.SetMethod(clocks_task);
    clocks.SetArg(this);
    clocks.SetPeriodMilliseconds(15);

    fading_stage = 0;
    brightness = 0;
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

    old_state = (state_t)0xff;

    pwm_control();
    render_clocks = current_clocks;
    setMode(CLOCKS, current_clocks);
}

void Menu::clocks_task(arg_t self)
{
    Menu * m = (Menu *) self;
    if (m->machine != CLOCKS && m->fading == IN )
    {
        ma_clear_screen();
        ma_buffer_flush();
        return;
    }
    if (m->render_clocks == DAY)
        ma_time_loop(1);
    else
        ma_time_loop(0);
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

    //dump buttons when fading
    if(m->fade.IsRegistered())
        button = 0;

    uint8_t b = (m->button_old ^ 0b111) & button;
    m->button_old = button;

    if (m->machine == SAVING)
    {
        matrix.enable(true);
        matrix.put("Saving !");

        if (m->saving_timeout++ > 30)
        {
            m->saving_timeout = 0;
            m->setMode(CLOCKS);
        }

    }
    else if (m->machine == MENU)
    {
        //process menu
        matrix.enable(true);
        m->clocks.Unregister();
        m->processMenu(button,b);
    }
    else if (m->machine == CLOCKS)
    //process clock itself
    {
        matrix.enable(false);

        if (!m->clocks.IsRegistered())
        {
            m->clocks.Register();
        }

        //clocks_task(m);
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
                //m->machine = MENU;
                m->setMode(MENU);

                //fill components data from config structure
                //copy configuration to temporary struct
                memcpy (&(m->configuration_temp), &global_configuration, sizeof(configuration_t));
                m->fillComponents(&(m->configuration_temp));
            }
            else if (m->machine == MENU)
            {
                //save temporary configuration to working configuration if changed and save to EEPROM
                //print changes saved
                fillStruct(&(m->configuration_temp));

                //if changed - save
                if (memcmp(&(m->configuration_temp), &global_configuration, sizeof(configuration_t)))
                {
                    memcpy(&global_configuration, &(m->configuration_temp),sizeof(configuration_t));
                    config_save();
                    m->setMode(SAVING);
                }
                else
                {
                    m->setMode(CLOCKS);
                }
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

    m->pwm_control();
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
        setMode(CLOCKS);
        timeout = 0;
    }
    //reset timeout timer if user does something
    if (button_rising_edge)
        timeout = 0;
}

void Menu::setMode(state_t mode, clocks_t clocks)
{
    //if switched clock mode fade out/in
    if (clocks != current_clocks && clocks != INVALID)
    {
        current_clocks = clocks;
        old_state = (state_t)0xff;
    }

    if (old_state != mode)
    {
        old_state = mode;
        fade.Register();
        fading_stage = 0;
    }
}

void Menu::pwm_control()
{
    //start from zero bright - fixed in constructor
    //read values from configuration
    clocks_t c = getClockMode();

    //select day/night pwm brightness and day/night clocks mode

    if (c == DAY)
        brightness_new = global_configuration.maxLuminance * 100;
    else if (c == NIGHT)
        brightness_new = global_configuration.minLuminance * 100;

    if (c != current_clocks && machine == CLOCKS)
    {
        setMode(CLOCKS ,c );
    }

    if (current == &luminance_max && screen_active)
    {
        ma_set_brightness(luminance_max.number * 100);
    }
    else if (current == &luminance_min && screen_active)
    {
        ma_set_brightness(luminance_min.number * 100);
    }


}

Menu::clocks_t Menu::getClockMode()
{
    clocks_t mode = current_clocks;
    if (global_configuration.switch_type == SWITCH_TYPE_TIME)
    {
        rtc_time_t t;
        uint32_t secs_d,secs_n, secs;
        secs_d = global_configuration.toDay.hours * 60 + global_configuration.toDay.minutes ;
        secs_n = global_configuration.toNight.hours * 60 + global_configuration.toNight.minutes ;
        rtc_control_GetTime(&t);
        secs = t.secs / 60;
        if (secs >= secs_d)
        {
            mode = DAY;

            if(secs >= secs_n)
            {
                mode = NIGHT;
            }
        }
    }
    else if (global_configuration.switch_type == SWITCH_TYPE_PHOTO)
    {
        // todo read value from adc
        uint32_t photo = 0;

        if (photo > global_configuration.photoDay)
        {
            mode = DAY;
        }
        else if (photo < global_configuration.photoNight)
        {
            mode = NIGHT;
        }
    }

    return mode;
}

void Menu::fade_cb(arg_t self)
{
    Menu * m = (Menu  *) self;
    m->fade_cb();
}

void Menu::fade_cb()
{
    uint16_t  b = brightness;
    uint16_t  c = brightness_new;
    fading_stage++;


    if (fading_stage < FADE_CONST)
    {
        //fadeout
        ma_set_brightness((FADE_CONST*b - b *  fading_stage) / FADE_CONST);
        fading = OUT;

    }
    else if (fading_stage > FADE_CONST && fading_stage < 2*FADE_CONST)
    {
        //change machine
        machine = old_state;
        render_clocks = current_clocks;
        //fade in
        ma_set_brightness(c * (fading_stage - FADE_CONST) /FADE_CONST);
        fading = IN;

    }
    else if (fading_stage > 2*FADE_CONST)
    {
        //stop
        ma_set_brightness(c);
        brightness = brightness_new;
        fading_stage = 0;
        fade.Unregister();
        fading = FINISHED;
    }
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
