#ifndef MENU_H
#define MENU_H

#include "scheduler.hpp"
#include "configuration.h"

#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_ENTER 4

class Screen;

class Menu
{
public:
    Menu();
    static void Play(void * self);
    void Init();

    static void print(const char * str, const uint8_t * bright = NULL);
    static uint8_t flash;
    static uint8_t cnt;

private:
    Screen * current;
    Scheduler sh;
    bool screen_active;
    uint8_t button_old;
    bool menu_active;
    uint8_t counter;
    uint16_t timeout;

    void fillComponents(const configuration_t* c);
    void fillStruct(configuration_t * c);

};

class Screen
{
public:
    typedef struct
    {
        const char * label;
        uint8_t items_count;
        //1 - combobox
        //3 - percents
        //4 - time
        //32 - text
        char * data;

    } properties_t;

    typedef struct
    {
        uint8_t count;
        const char ** table;
    } combo_t;

    Screen(const properties_t & props);
    void setCombo(const combo_t * combo) {c = combo;}
    void pairNext(Screen * next);

private:
    Screen * next;
    Screen * prev;
    uint8_t index;
    bool selected;
    const properties_t & p;
    const combo_t * c;
    uint16_t number;
    uint8_t hours;
    uint8_t minutes;

    void handle(uint8_t buttons);

    void time(uint8_t buttons);
    void combobox(uint8_t buttons);
    void text(uint8_t buttons);
    void numberHandle(uint8_t, const uint16_t  maximum);
    void handleSelected(uint8_t buttons, const uint8_t * table);

    friend class Menu;
};

#define DECL_SCREEN(name, label, count, dta) \
    static const Screen::properties_t name##_props = {label, count ,dta}; \
    Screen name(name##_props)

#endif // MENU_H
