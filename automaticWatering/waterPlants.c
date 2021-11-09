#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Below are calibration values.
#define tubeLenghtCM 600
#define tubeInnerRadiusCM 0.3
#define tubeVolume (3.14 * tubeInnerRadiusCM*tubeInnerRadiusCM * tubeLenghtCM)
// Pump should do about 83 ml per second (rated at 300 l/h)
#define testedRating 1.8
#define mlCalibration ((((float) 1)/(300*1000/60/60))/testedRating)
// The raspberry pi's pins used as output pins
#define OUT1 2
#define OUT2 3

int outs[] = {OUT1,OUT2};
int len1 = 2;
int ins[] = {};
int len2 = 0;

// setup() attempts to initializes the pigpio library and prepares all input and output pins
int setup(int *gpiOUTS, int len1, int *gpINSo, int len2) {
    printf("Starting application... ");
    if (gpioInitialise() < 0) {
        printf("pigpio failed to initialize, exiting...\n");
        gpioTerminate();
        return 0;
    }
    for (int i = 0; i < len1; i++) gpioSetMode(*(gpiOUTS + i), PI_OUTPUT);
    for (int i = 0; i < len2; i++) gpioSetMode(*(gpINSo + i), PI_INPUT);
    printf("output and input pins setup! ");
    return 1;
}

// functions for putting motor(s) controlled by pair(s) in pins[] in forward/reverse and stopping
void pump(int *pins, int len) {
    for (int i = 0; i < len; i+=2){
        gpioWrite(*(pins+i), 1);
        gpioWrite(*(pins+i+1), 0);
    }
}
void stop(int *pins, int len) {
    for (int i = 0; i < len; i+=2){
        gpioWrite(*(pins+i), 0);
        gpioWrite(*(pins+i+1), 0);
    }
}

// starts (hopefully) pump(s) connected to "outs" and let's them pump "ml" of water to "plants" number of plants
// returns the amount of time in seconds (float) elapsed during function call
float waterXmlForYplants(float ml, int plants) {
    clock_t start = clock();
    clock_t end = start 
                + (ml*mlCalibration * plants * CLOCKS_PER_SEC);
                // + (tubeVolume * mlCalibration * CLOCKS_PER_SEC);     // If the tube empties after pumping, this is the time needed to refill it
    pump(outs, len1);           // start pump
    while (clock() < end) sleep(0.3); // wait for pump to finish
    stop(outs, len1);           // stop it when ml has been pumped to "plants" number of plants
    return ((float) clock() - start) / CLOCKS_PER_SEC;
}

// REQUIRES passing two args:
// 1. ml to water each plant
// 2. nr of plants to water
int main(int argc, char **argv) {
    if (argc != 3) return 0;  // if not enough args passed, return
    if (!setup(outs, len1, ins, len2)) return 0;
    float ml = float(atof(argv[1]));
    int plants = atoi(argv[2]);
    float secondsElapsed = waterXmlForYplants(ml, plants);	// crashes if args can't be converted to floats!
    float actual_ml = secondsElapsed / mlCalibration;

    printf("\nsecondsElapsed: %.*f, actual_ml: %.*f, mlCalibration: %.*f\n", 1, secondsElapsed, 5, actual_ml, 5, mlCalibration);

    // From here: http://www.cplusplus.com/reference/ctime/localtime/
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    int decimalsPrinted = 1;
    FILE *f;
    f = fopen("water.log", "a+");
    fprintf(f, "%.*f ml for %d plants (total: %.*f) at: %s", decimalsPrinted, ml, plants, decimalsPrinted, actual_ml, asctime(timeinfo));

    printf("main() finished, returning...\n");
    gpioWrite(OUT1, 0);
    gpioWrite(OUT2, 0);
    gpioTerminate();
    return 0;
}
