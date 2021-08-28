#include <pigpio.h>
#include <stdio.h>
#include <time.h>
//sleep stuff:
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define IN1 2
#define IN2 3
// Pump should do about 83 ml per second (rated at 300 l/h)
#define mlCalib (((float) 1)/(300*1000/60/60))

int outs[] = {IN1,IN2};
int len1 = 2;
int ins[] = {};
int len2 = 0;

int setup(int *gpiOUTS, int len1, int *gpINSo, int len2) {
    printf("Starting application...\n");
    if (gpioInitialise() < 0) {
        printf("pigpio failed to initialize, exiting...\n");
        gpioTerminate();
        return 0;
    }
    printf("pigio initialization successfull!\n");
    for (int i = 0; i < len1; i++) gpioSetMode(*(gpiOUTS + i), PI_OUTPUT);
    for (int i = 0; i < len2; i++) gpioSetMode(*(gpINSo + i), PI_INPUT);
    printf("output and input pins setup!\n");
    return 1;
}

// functions for putting motor(s) controlled by pair(s) in pins[] in forward, reverse and stopping
void forw(int *pins, int len) {
    for (int i = 0; i < len; i+=2){
        gpioWrite(*(pins+i), 1);
        gpioWrite(*(pins+i+1), 0);
    }
}
void back(int *pins, int len) { // PUMP SHOULDN'T BE PUT IN REVERSE!
    for (int i = 0; i < len; i+=2){
        gpioWrite(*(pins+i), 0);
        gpioWrite(*(pins+i+1), 1);
    }
}
void stop(int *pins, int len) {
    for (int i = 0; i < len; i+=2){
        gpioWrite(*(pins+i), 0);
        gpioWrite(*(pins+i+1), 0);
    }
}

// starts (hopefully) pump(s) connected to "outs" and let's them pump x ml to y nr of plants
// returns the amount of time in seconds elapsed during function call
float waterXmlForYplants(float x, int y) {
    clock_t start = clock();
    clock_t end = start + (x*mlCalib * y * CLOCKS_PER_SEC);
    forw(outs, len1);           // start pump
    while (clock() < end) sleep(0.3); //for (int i = 0; i < 1000; i++);
    stop(outs, len1);           // stop it when x ml has been pumped to y nr of plants
    return ((float) clock() - start) / CLOCKS_PER_SEC;
}

// starts infinite loop that calls waterXmlForYplants once every 24h
void onceIn24H() {
    waterXmlForYplants(100,8);
    time_t start, end;
    float timeToWater = 0;
    while (1) {
        start = time(NULL);
        end = time(NULL);
        while (difftime(end, start) < ((float) 2*60) - timeToWater) { 
            end = time(NULL);     // wait till 24h have passed
            sleep(30);
        }
        timeToWater = waterXmlForYplants(100,8);
        printf("timeToWater: %f, ", timeToWater);
        printf("difftime(end, start): %f\n", difftime(end, start));
        fflush(stdout);
    }
}

int main() {    
    if (!setup(outs, len1, ins, len2)) return 0;
    
    int input = 2;
    while (input != 0) {                                // end program with 0
    scanf("%d", &input);    
    if (input == 1) forw(outs, len1);                   // start motor
    else if (input == 2) stop(outs, len1);              // stop
    else if (input == 3) waterXmlForYplants(100, 5);    // start calibration with 100 ml in 5 cups
    else if (input == 4) onceIn24H();                   // start 24h loop
    sleep(0.5);
    }
    
    printf("main() finished, returning...\n");
    gpioWrite(IN1, 0);
    gpioWrite(IN2, 0);
    gpioTerminate();
    return 0;
}
