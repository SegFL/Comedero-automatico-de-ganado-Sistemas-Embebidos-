//=====[Libraries]=============================================================


#include "mbed.h"
#include "arm_book_lib.h"

#include "feeder.h"
#include "motor.h"
#include "stdlib.h"
#include "string.h"

#ifdef _PROBANDO_MODOS
DigitalOut modomanual(LED1);
DigitalOut modofree(LED2);
DigitalOut modotiempo(LED3);
#endif 

#ifdef _PROBANDO_MOTORES
DigitalOut motor1izq(LED1);
DigitalOut motor1stop(LED2);
DigitalOut motor1der(LED3);
#endif
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

#ifdef _PROBANDO_MOTOR
switch(motorArray[0].read()){
    case DIRECTION_1:{
        motor1izq.write(ON);
        motor1stop.write(OFF);
        motor1der.write(OFF);
    }break;
    case DIRECTION_2:{
        motor1izq.write(OFF);
        motor1stop.write(OFF);
        motor1der.write(ON);
    }break;
    case STOPPED:{
        motor1izq.write(OFF);
        motor1stop.write(ON);
        motor1der.write(OFF);
    }break;
}
#endif

#ifdef _PROBANDO_MODOS
#define _PROBANDO_MODOS
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
#endif 

}

feederStatus_t feederStatusRead(){

    return feederStatus;

}

void feederStatusWrite(feederStatus_t status){

    feederStatus = status;

}

void feederTimeSet( int year1, int month1, int day1, 
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

char* feederTimeRead(){

    time_t epochSeconds;
    epochSeconds = time(NULL);

    char* buffer = (char*)malloc(10 * sizeof(char)); // Buffer para convertir el entero a cadena
    if (buffer == NULL) {
        return NULL;
    }

    char* cadena = (char*)malloc(100 * sizeof(char));
    if (cadena == NULL) {
        return NULL;
    }

    strcpy(cadena, ctime(&epochSeconds));
    strcat(cadena, "Duration:");
    sprintf(buffer, "%d", durationTime);
    strcat(cadena, buffer);

    free(buffer); // Liberamos la memoria asignada para el buffer

    return cadena;

}

/*
char* feederTimeRead(){

    char buffer[10];
    char cadena[100];
    time_t epochSeconds;
    epochSeconds = time(NULL);
    strcat(cadena,ctime(&epochSeconds));
    strcat(cadena,"Duration:");
    strcat(cadena,itoa(durationTime,buffer,DECIMAL));
    return cadena;    
}

*/