#include "menu.h"

char d1[4] = {1,3,5,2};
char d2[1];
char d3[32] = "bdaciii";
char d4[32] = "budakovja";
char d5[3];

DECL_SCREEN(setup_time, "Time",4,d1);
DECL_SCREEN(time,"Source",1,d2);
DECL_SCREEN(wifi_ssid,"SSID",32,d3);
DECL_SCREEN(wifi_pass,"Password",32,d4);
DECL_SCREEN(luminance,"Luminance",3,d5);

static const char * combo1[] = {"Manual", "DCF", "Wifi"};
static const Screen::combo_t c1 = {3, combo1};

Screen * createScreens()
{
    setup_time.pairNext(&time);
    time.pairNext(&wifi_ssid);
    wifi_ssid.pairNext(&wifi_pass);
    wifi_pass.pairNext(&luminance);

    luminance.pairNext(&setup_time);

    time.setCombo(&c1);

    return &setup_time;
}
