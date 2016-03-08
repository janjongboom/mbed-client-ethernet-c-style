# mbed Client for Ethernet (C-style)

This is an [mbed Client](https://www.mbed.com/en/development/software/mbed-client/) library for mbed OS, which connects devices to [mbed Device Connector](https://connector.mbed.com/) over Ethernet. The API is a lot simpler than the API of the normal mbed Client library, and is meant to add mbed Client support to any application in a few lines of C code, without needing to restructure your application.

Porting this repo over to do 6LoWPAN should be trivial.

* [Setup](#setup)
* [Storing data in Connector](#storing-data-in-connector)
* [Calling functions from Connector](#calling-functions-from-connector)
* [Full example application](#full-example-application)

## Setup

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

### Options

`mbed_client_setup` takes a `struct MbedClientOptions` as optional first parameter which you can use to configure the library. You can get the default options through `mbed_client_get_default_options()`, and override the ones you want. The options are:

|Parameter|Description|Default Value|
|---|---|---|
|Manufacturer|LWM2M Device Manufacturer|Manufacturer_String
|Type|LWM2M Device Type|DeviceType_String|
|ModelNumber|LWM2M Model Number|ModelNumber_String|
|SerialNumber|LWM2M Serial Number|SerialNumber_String|
|DeviceType|Device Connector Type|test|
|SocketMode|Mode to connect (UDP or TCP)|M2MInterface::UDP|
|ServerAddress|Address of Connector instance you want to use|coap://api.connector.mbed.com:5684|

To override a setting do something like:

```cpp
struct MbedClientOptions opts = mbed_client_get_default_options();
opts.DeviceType = "temperature";
mbed_client_setup(opts);
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

You can also reference class members via:

```cpp
LedResource* led = new LedResource();
mbed_client_define_function("led/toggle", execute_callback(led, &LedResource::toggle_led);
```

Or use lambdas (mbed OS supports C++11):

```cpp
mbed_client_define_function("led/toggle", ([] (void*) -> void { green = !green; }));
```

## Full example application

Here's a full example application that works with the mbed Device Connector QuickStart applications for [node.js](https://github.com/ARMmbed/mbed-connector-api-node-quickstart) and [Python](https://github.com/ARMmbed/mbed-connector-api-python-quickstart). It runs on the FRDM-K64F board.

[Quickstart application](../quickstart-app)
