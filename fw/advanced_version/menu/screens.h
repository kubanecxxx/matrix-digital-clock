#ifndef __SCREEN_H__
#define __SCREEN_H__


#define BUFFER_SIZE 10

class Screen
{
public:
    typedef struct
    {
        const char * label;
        uint8_t items_count;
        char * data;

    } properties_t;

    typedef struct
    {
        uint8_t count;
        const char ** table;
    } combo_t;

    typedef enum {TIME, TEXT, COMBO, NUMBER} type_t;

    Screen(const properties_t & props);
    void pairNext(Screen * next);

    friend class Menu;
private:
    Screen * next;
    Screen * prev;
    void handle(uint8_t buttons);

protected:
    bool selected;
    type_t type;
    uint8_t index;

    const properties_t & p;
    static Screen * setupTime;

    virtual void subHandle(uint8_t buttons, bool was_selected) = 0;
};

//*********************** Text screen ********************
class TextScreen : public Screen
{
public:
    TextScreen(const properties_t & p );

protected:
    void subHandle(uint8_t buttons, bool was_selected);
private:
    void text(uint8_t buttons);
};

//*********************** Number screen ********************
class NumberScreen : public Screen
{
public:
    NumberScreen(const properties_t & p );
    uint16_t number;

protected:
    void subHandle(uint8_t buttons, bool was_selected);
private:
    void numberHandle(uint8_t, const uint16_t  maximum);

};

//*********************** Time screen ********************
class TimeScreen : public Screen
{
public:
    TimeScreen(const properties_t & p );
    uint8_t hours;
    uint8_t minutes;

protected:
    void subHandle(uint8_t buttons, bool was_selected);

private:
    void time(uint8_t buttons);

};

//*********************** Combo screen ********************
class ComboScreen : public Screen
{
public:
    ComboScreen(const properties_t & p );
    void setCombo(const combo_t * combo) {c = combo;}

protected:
    void subHandle(uint8_t buttons, bool was_selected);
private:
    void combobox(uint8_t buttons);
    const combo_t * c;
};

//*********************** Helper macro ********************

#define DECL_SCREEN(type, name, label, count) \
    static char name##_buffer[count]; \
    static const Screen::properties_t name##_props = {label, count ,name##_buffer}; \
    static type##Screen name(name##_props)

#define DECL_SCREEN_CUSTOM(type,name, label, count, dta) \
    static const Screen::properties_t name##_props = {label, count ,dta}; \
    static type##Screen name(name##_props)

#endif
