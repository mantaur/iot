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