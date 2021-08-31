# Automatic watering
Holds code related to automatic watering system.

## Compiling:
Compilation of waterPlants.c is done with the following command 
```bash
g++ -lpigpio waterPlants.c -o water.exe
```
The -lpigpio flag tells the compiler to include the pigpio library and must as such be installed. Check [this documentation](http://abyz.me.uk/rpi/pigpio/download.html) for instructions on how to install and use it!

## Editing crontab
The crontab setup changes when the program is run, and how much water is dispensed. It can be edited with:
```bash
crontab -e
```

## Product list
### * Pump:
A Decdeal 12V 4.8W pump was used, bought [here](https://www.amazon.se/gp/product/B07B4F1TN4/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)

### * Computer
A Raspberry Pi 3 A + is used as the brains

### * Motor board
A Neuftech LN298 "double H-bridge" (mine arrived with one side dead so more like a single H-bridge...) motorboard bought [here](https://www.amazon.se/gp/product/B01KBTNHS6/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1)

### * Plumbing
6 meters of 8mm pvc pipe (inner diameter 6mm) ([bought locally](https://www.bauhaus.se/pvc-slang-gron-8x1mm)), 6 T-cross connections and ballpoint pen tips and/or bike presto tube valve hats with a hole drilled through them restricts the water flow for my setup enough to make it feed water ~ish evenly (accross up to 7 tested).
