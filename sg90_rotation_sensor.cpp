#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>
#include <iostream>

#define LED_1 12
#define SENSOR_SIA 25
#define SENSOR_SIB 24
#define SENSOR_SW 19 // Knob button
#define SERVO_PIN 4

void cleanup(){
    gpioInitialise();
    gpioSetMode(LED_1, PI_OUTPUT);
    gpioWrite(LED_1, 0);
    gpioTerminate();
    printf("Bye\n");
}
void rotateServo(int pulse_time){
    for ( int i = 0 ; i < 4 ; i++ ){
        gpioWrite(SERVO_PIN, 1);
        usleep(pulse_time);
        gpioWrite(SERVO_PIN, 0);
        usleep(10000 - pulse_time);
    }

}
int main(){
    atexit(cleanup);
    printf("Hello\n");   
    gpioTerminate();
    if (gpioInitialise()<0) exit(1);
    gpioSetMode(LED_1, PI_OUTPUT);
    gpioSetMode(SENSOR_SIA, PI_INPUT);
    gpioSetMode(SENSOR_SIB, PI_INPUT);
    gpioSetMode(SENSOR_SW, PI_INPUT);
    gpioSetMode(SERVO_PIN, PI_OUTPUT);

    int a[2] = { 1, 1 };
    int b[2] = { 1, 1 };
    int i = 0;
    bool is_during_change = 0;
    int pulse_time = 1000; // at 180 and 2600 goes a bit insane
    int pulse_change = 250;

    while ( 1 ) {
        i++;
        a[0] = gpioRead(SENSOR_SIA);
        a[1] = gpioRead(SENSOR_SIB);

        if ( !gpioRead(SENSOR_SW)){
            gpioWrite(LED_1, 1);
            usleep(250000);
            break;
        }else{
            gpioWrite(LED_1, 0);
        }

        if ( !a[0] == b[0] || !a[1] == b[1] ){
            if ( i < 10 ){ i = 0; continue; }
            if ( a[0] == 1 && a[1] == 1){
                if( b[0] == 1 && b[1] == 0){
                    if ( pulse_time < 180 ){
                        std::cout << "Left maxed " << pulse_time << std::endl;
                    }
                    else{
                        std::cout << "Left  " << pulse_time << std::endl;
                        pulse_time = pulse_time - pulse_change;
                        rotateServo(pulse_time);
                    }
                }
                else if( b[0] == 0 && b[1] == 1){
                    if ( pulse_time > 2600 ){
                        std::cout << "Right maxed " << pulse_time << std::endl;
                    }
                    else{
                        std::cout << "Right " << pulse_time <<  std::endl;
                        pulse_time = pulse_time + pulse_change;
                        rotateServo(pulse_time);
                    }
                }
                else{
                    std::cout << "Error" << std::endl;
                }
 //           std::cout << a[0] << "|" << a[1] << i << std::endl;
            i = 0;
            usleep(10000 - pulse_time);
            }
        }
        b[0] = a[0];
        b[1] = a[1];
        usleep(1000); // longer and shorter sleep fucks up readings a bit, probably possible to tune it in tandem with *i* variable 
    }
    gpioTerminate();
    return 0;
}
