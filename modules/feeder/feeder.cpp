//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "feeder.h"
#include "motor.h"

DigitalOut modomanual(LED1);
DigitalOut modofree(LED2);
DigitalOut modotiempo(LED3);

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================



//=====[Declaration and initialization of public global objects]===============

motor motorArray[] = {motor(PF_2, PE_3), motor(PH_0, PH_1)};

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


static feederStatus_t feederStatus;

struct tm startTime;//tiempo de inicio del evento en TIME MODE
int durationTime;


//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of public functions]===================================

void feederInit(){

    feederStatus=FEEDER_MANUAL_MODE;

}
void feederUpdate() {


    for (int i = 0; i < 2; i++) {

      switch (motorArray[i].read_state()) {
        case DIRECTION_1:
            if (motorArray[i].read() != DIRECTION_1) {
                motorArray[i].change_state(STOPPED);
            }
            break;

        case DIRECTION_2:
            if (motorArray[i].read() != DIRECTION_2) {
                 motorArray[i].change_state(STOPPED);
            }
            break;

        case STOPPED:
        default:
            if (motorArray[i].read() == DIRECTION_1) {
                motorArray[i].change_state(DIRECTION_1);
            }else if (motorArray[i].read() == DIRECTION_2) {
                motorArray[i].change_state(DIRECTION_2);
            }
            break;
        }
    }


//pruebas con leds
    switch(feederStatus){
        case   FEEDER_MANUAL_MODE:{
            modomanual.write(ON);
            modofree.write(OFF);
            modotiempo.write(OFF);
            break;
        }
                case     FEEDER_FREE_MODE:{
            modomanual.write(OFF);
            modofree.write(ON);
            modotiempo.write(OFF);
            break;
        }
                case     FEEDER_TIME_MODE:{
            modomanual.write(OFF);
            modofree.write(OFF);
            modotiempo.write(ON);
            break;
        }
    }

}

feederStatus_t feederStatusRead(){

    return feederStatus;

}

void feederStatusWrite(feederStatus_t status){

    feederStatus = status;

}

bool feederTimeSet( int year1, int month1, int day1, 
                       int hour1, int minute1, int second1, int duration){


    startTime.tm_year = year1 - 1900;
    startTime.tm_mon  = month1 - 1;
    startTime.tm_mday = day1;
    startTime.tm_hour = hour1;
    startTime.tm_min  = minute1;
    startTime.tm_sec  = second1;
    startTime.tm_isdst = -1;

    durationTime = duration;




}