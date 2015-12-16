#ifndef __SCREEN_H__
#define __SCREEN_H__


#define BUFFER_SIZE 10

class Screen
{
public:
    typedef enum {TIME, TEXT, COMBO, NUMBER} type_t;

    Screen(const char * lab);

    void pairNext(Screen * next);

    friend class Menu;
private:
    Screen * next;
    Screen * prev;
    void handle(uint8_t buttons);

    const char * label;


protected:
    uint8_t index;
    bool selected;
    type_t type;
    uint8_t items_count;


    static Screen * setupTime;

    virtual void subHandle(uint8_t buttons, bool was_selected) = 0;
};

//*********************** Text screen ********************
class TextScreen : public Screen
{
public:
    TextScreen(const char * lab, char * data, uint8_t count);

protected:
    void subHandle(uint8_t buttons, bool was_selected);
private:
    void text(uint8_t buttons);
    char * data;
};

//*********************** Number screen ********************
class NumberScreen : public Screen
{
public:
    NumberScreen(const char * lab);
    uint16_t number;

protected:
    void subHandle(uint8_t buttons, bool was_selected);
private:
    void numberHandle(uint8_t buttons, uint16_t minimum , uint16_t  maximum);

};

//*********************** Time screen ********************
class TimeScreen : public Screen
{
public:
    TimeScreen(const char * lab );
    uint8_t hours;
    uint8_t minutes;

protected:
    void subHandle(uint8_t buttons, bool was_selected);



};

//*********************** Combo screen ********************
class ComboScreen : public Screen
{
public:
    ComboScreen(const char * lab );
    void setCombo(const char ** table, uint8_t count) {this->table = table; this->count = count;}
    uint8_t comboIndex;

protected:
    void subHandle(uint8_t buttons, bool was_selected);
private:
    void combobox(uint8_t buttons);
    const char ** table;
    uint8_t count;

};

//*********************** Helper macro ********************

#define DECL_SCREEN(type, name, label) \
    static type##Screen name(label)

#define DECL_SCREEN_CUSTOM(type,name, label, count, dta) \
    static type##Screen name(label, dta, count)

#endif
