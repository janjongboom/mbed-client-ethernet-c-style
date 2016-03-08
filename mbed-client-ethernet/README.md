# mbed Client for Ethernet (C-style)

This is an [mbed Client](https://www.mbed.com/en/development/software/mbed-client/) library which connects devices to [mbed Device Connector](https://connector.mbed.com/) over Ethernet. The API is a lot simpler than the API of the normal mbed Client library, and is meant to add mbed Client support to any application in a few lines of C code, without needing to restructure your application.

## Set up

First install the library through [yotta](http://yotta.mbed.com):

```bash
$ yt install mbed-client-ethernet-c-style
```

Now you can connect to mbed Device Connector via:

```cpp
#include "security.h" // get this from https://connector.mbed.com/#credentials
#include "mbed-client-ethernet-c-style/client.h"

void registered() {
    printf("Connected to mbed Device Connector!\r\n");
}

void app_start(int, char **) {
    bool setup = mbed_client_setup();
    if (!setup) {
        printf("Setup failed (e.g. couldn't get IP address. Check serial output.\r\n");
        return;
    }
    mbed_client_on_registered(&registered);
}
```

## Storing data in Connector

Data is identified by routes (like URLs). A route is a string of two segments. First we define the route in our `app_start`, set whether the data is readable and/or writeable, and set a default value. Afterwards you can read/write from the resource in your normal functions.

```cpp
#include "security.h"
#include "mbed-client-ethernet-c-style/client.h"

InterruptIn btn2(SW2);

void click_btn2() {
    int count = mbed_client_get_int("button/clickcount");
    mbed_client_set("button/clickcount", ++count);
}

void app_start(int, char **) {
    // can read & write to this resource
    mbed_client_define_resource("button/clickcount", 0, M2MBase::GET_PUT_ALLOWED, true);

    // ... SETUP LOGIC HERE ...

    btn2.fall(&click_btn2);
}
```

In mbed Device Connector the click count is now exposed under `button/0/clickcount`.

## Calling functions from Connector

If you want to call a function on your device, we can create a route and set a callback function. When a `POST` comes in from mbed Device Connector the function will be executed.

```cpp
#include "security.h"
#include "mbed-client-ethernet-c-style/client.h"

DigitalOut green(LED_GREEN);

void toggle_led(void*) {
    green = !green;
}

void app_start(int, char **) {
    mbed_client_define_function("led/toggle", &toggle_led);

    // ... SETUP LOGIC HERE ...
}
```

## Full example application

Here's a full example application that works with the mbed Device Connector QuickStart applications for [node.js](https://github.com/ARMmbed/mbed-connector-api-node-quickstart) and [Python](https://github.com/ARMmbed/mbed-connector-api-python-quickstart). It runs on the FRDM-K64F board.

```cpp
#include "security.h"
#include "mbed-client-ethernet-c-style/client.h"

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
```