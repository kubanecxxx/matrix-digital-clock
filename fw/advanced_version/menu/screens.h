#ifndef __SCREEN_H__
#define __SCREEN_H__

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

    static Screen * setupTime;

    friend class Menu;
};

#define DECL_SCREEN(name, label, count) \
    static char name##_buffer[count]; \
    static const Screen::properties_t name##_props = {label, count ,name##_buffer}; \
    static Screen name(name##_props)

#define DECL_SCREEN_CUSTOM(name, label, count, dta) \
    static const Screen::properties_t name##_props = {label, count ,dta}; \
    static Screen name(name##_props)

#endif
