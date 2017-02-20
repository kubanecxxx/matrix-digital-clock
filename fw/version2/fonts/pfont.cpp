#include "stdint.h"
#include "pfont.h"

namespace piris
{

PFont::PFont(properties_t &prop):
    m(prop)
{
    m.ro = 0;
}

PFont::PFont(const uint8_t *table, pixel_t heigth, pixel_t width):
    m(*new properties_t)
{
    m.table = table;
    m.height = heigth;
    m.width = width;
    m.ro = 0;
}

PFont::PFont(const properties_t &prop):
    m((properties_t &) prop)
{
}

const uint8_t * PFont::character(char letter) const
{
    passert(m.table, "table not defined");

    //default character when out of boundary
    if (letter < ' ' || letter > '~')
        return character(' ');


    return &m.table[(letter - ' ')*height()*bytesPerLine()];
}


uint32_t PFont::line(const uint8_t *character, uint8_t line) const
{
    passert(m.width < 32, "font is too wide");
    passert(m.bytesPerLine < 4, "font is too wide");
    passert(line < height(), "out of boundary");
    uint32_t temp = 0;
    const uint8_t * t = character + line * bytesPerLine();

    if (m.bytesPerLine == 1)
        temp = t[0] << 24;
    else if (m.bytesPerLine == 2)
        temp = (t[0] << 24) | (t[1] << 16);
    else if (m.bytesPerLine == 3)
        temp = t[2] << 8 | t[1] << 16 | t[0] << 24;
    else if (m.bytesPerLine == 4)
        temp = t[3] | t[2] << 8 | t[1] << 16 | t[0] << 24;


    //memcpy(&temp,t,bytesPerLine());

    temp >>= 32 - m.width;
    return temp;
}

uint32_t PFont::line(char letter, uint8_t lin) const
{
    return line(character(letter),lin);
}

}

// some basic fonts - user can add his own fonts in his source files

#include "fonts/terminus12.c"
#include "fonts/terminus16.c"
#include "fonts/sans16.c"

namespace piris
{
const DECL_FONT(PFont::font_sans_16_p,sans16_Bitmaps,10,16,2);
const DECL_FONT(PFont::font_terminus_16_p,terminus16_Bitmaps,10,16,2);
const DECL_FONT(PFont::font_terminus_12_p,terminus12_Bitmaps,6,12,1);
PFont PFont::sans16(font_sans_16_p);
PFont PFont::terminus16(font_terminus_16_p);
PFont PFont::terminus12(font_terminus_12_p);
}
