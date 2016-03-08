# simple-mbed-client-ethernet

Experiments in creating a simpler mbed Client API

## Setup

```bash
git clone git@github.com:ARMmbed/simple-mbed-client-ethernet.git
cd simple-mbed-client-ethernet/mbed-client-ethernet
sudo yt link
cd ../example-app
yt target frdm-k64f-gcc
yt link mbed-client-ethernet
yt install
```

Now get a certificate and put it in source/security.h

```
yt build
cp build/frdm-k64f-gcc/source/*.bin /Volumes/MBED
```