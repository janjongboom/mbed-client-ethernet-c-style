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
    int count = mbed_client_get_int("3200/5501");
    mbed_client_set("3200/5501", ++count);
}

void do_blink(std::vector<uint32_t>* pattern, uint16_t position) {
    // blink the LED
    green = !green;

    // up the position, if we reached the end of the vector
    if (position >= pattern->size()) {
        // free memory, and exit this function
        delete pattern;
        return;
    }

    // how long do we need to wait before the next blink?
    uint32_t delay_ms = pattern->at(position);

    // we create a FunctionPointer to this same function
    FunctionPointer2<void, std::vector<uint32_t>*, uint16_t> fp(&do_blink);
    // and invoke it after `delay_ms` (upping position)
    minar::Scheduler::postCallback(fp.bind(pattern, ++position)).delay(minar::milliseconds(delay_ms));
}

void blink_led(void*) {
    // transform the pattern (500:500:500) into a vector of uint32_t's, so we know how to blink
    std::stringstream pattern(mbed_client_get("3201/5853"));
    std::vector<uint32_t>* v = new std::vector<uint32_t>;
    std::string item;
    while (std::getline(pattern, item, ':')) {
        v->push_back(atoi((const char*)item.c_str()));
    }

    do_blink(v, 0);
}

void app_start(int, char **) {
    red = blue = green = 1; // turn LEDs off first

    // button click count: 3200 (digital input) / 5501 (counter). Only reading allowed from cloud.
    mbed_client_define_resource("3200/5501", 0, M2MBase::GET_ALLOWED, true);
    // LED blink pattern: 3201 (digital output) / 5853 (pattern), default value set as well
    mbed_client_define_resource("3201/5853", string("500:500:500:500:500:500:500"), M2MBase::GET_PUT_ALLOWED, true);
    // LED blink function: 3201 (digital output) / 5850 (execute)
    mbed_client_define_function("3201/5850", &blink_led);

    bool setup = mbed_client_setup();
    if (!setup) {
        red = 0; // put RED on when setup failed
        return;
    }
    mbed_client_on_registered(&registered);
    mbed_client_on_unregistered(&unregistered);

    btn2.fall(&click_btn2);
}
