#include "security.h"
#include "mbed-client-ethernet/client.h"

DigitalOut blue(LED_BLUE);
DigitalOut red(LED_RED);
DigitalOut green(LED_GREEN);
InterruptIn btn2(SW2);

void registered() {
    blue = 0; // Connected to Connector? Put blue LED on.
}
void unregistered() {
    blue = 1; // Disconnected? Put blue LED off.
}

void click_btn2() {
    int count = mbed_client_get_int("button/clickcount");
    mbed_client_set("button/clickcount", ++count);
}

static void toggle_led(void*) {
    green = !green;
}

void app_start(int, char **) {
    red = blue = green = 1; // turn LEDs off first

    mbed_client_define_resource("button/clickcount", 0, M2MBase::GET_PUT_ALLOWED, true);
    mbed_client_define_resource("button/test", string("Hoi"), M2MBase::GET_ALLOWED, false);
    mbed_client_define_function("led/toggle", &toggle_led);

    bool setup = mbed_client_setup();
    if (!setup) {
        red = 0; // put RED on when setup failed
        return;
    }
    mbed_client_on_registered(&registered);
    mbed_client_on_unregistered(&unregistered);

    btn2.fall(&click_btn2);
}
