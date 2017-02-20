#ifndef PFONT_H
#define PFONT_H

#include "ptypes.h"
#include "passert.h"

namespace piris
{

#define fontassert() passert(m.ro == 0,"readonly")

class PFont
{
public:
    typedef struct
    {
        const uint8_t * table;
        pixel_t height;
        pixel_t width;
        uint8_t ro;
        uint8_t bytesPerLine;
    } properties_t;

    PFont(const uint8_t * table, pixel_t heigth, pixel_t width);
    PFont(properties_t & prop);
    PFont(const properties_t & prop);

public:
    const uint8_t * character(char letter) const;
    uint32_t line(char letter, uint8_t line) const;
    uint32_t line(const uint8_t * character, uint8_t line) const;
    inline const uint8_t * table() const {return m.table;}
    inline pixel_t height() const {return m.height;}
    inline pixel_t width() const {return m.width;}
    inline uint8_t bytesPerLine() const {return m.bytesPerLine;}
    inline void setTable(const uint8_t *table) {fontassert();m.table = table;}
    inline void setWidth(pixel_t w) {fontassert();m.width = w;}
    inline void setHeight(pixel_t h) {fontassert();m.height = h;}


    static PFont sans16;
    static const properties_t font_sans_16_p;
    static PFont terminus16;
    static const properties_t font_terminus_16_p;
    static PFont terminus12;
    static const properties_t font_terminus_12_p;


private:

    properties_t & m;

};


}

#define DECL_FONT(name,table,width,height ,bytes_per_line) \
    piris::PFont::properties_t name = \
    {\
    table,height,width,1, bytes_per_line \
    }\

#endif // PFONT_H
