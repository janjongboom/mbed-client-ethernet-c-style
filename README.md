# mbed Client for Ethernet, C-style

For the library documentation, look [here](mbed-client-ethernet-c-style/README.md).

This repo contains experiments in creating a simpler mbed Client API. It consists of a library and two example applications.

## Setup (for FRDM-K64F)

1. Clone this repository
1. Go into example-app/ or quickstart-app/ folder
1. Get a [certificate](https://connector.mbed.com/#credentials) and store as source/security.h
1. Run `yt target frdm-k64f-gcc`
1. Run `yt build`
1. Drag the `.bin` file from `build/frdm-k64f-gcc/source/` onto your FRDM-K64F board

## Setup locally

1. Go into mbed-client-ethernet-c-style/
1. Run `sudo yt link`
1. Go into example-app/ or quickstart-app/ folder
1. Get a [certificate](https://connector.mbed.com/#credentials) and store as source/security.h
1. Run `yt target frdm-k64f-gcc`
1. Run `yt link mbed-client-ethernet-c-style`
1. Run `yt build`
1. Drag the `.bin` file from `build/frdm-k64f-gcc/source/` onto your FRDM-K64F board
