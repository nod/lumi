```
 _                 _
| |_   _ _ __ ___ (_)
| | | | | '_ ` _ \| |
| | |_| | | | | | | |
|_|\__,_|_| |_| |_|_|

```

# lumi

Some quick and ugly code to run on a `wemos d1 r2` that connects to your local
wifi and then runs a simple webserver on port 80 to control a strand of ws2801.

## build env

This uses `platformio.org` to compile the code for the wemos.

- `virtualenv virt`
- `source virt/bin/activate`
- `pip install -r reqs.pip`

## config

edit `src/local.h` and set your wifi ssid, etc

## building and loading

Plugin the wemos board to your usb and try: `pio run -t upload`

On Mac Mojave the usb controller drivers are already there.

