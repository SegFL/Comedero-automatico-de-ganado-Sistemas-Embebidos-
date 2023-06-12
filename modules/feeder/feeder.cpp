//=====[Libraries]=============================================================


#include "mbed.h"
#include "arm_book_lib.h"

#include "feeder.h"
#include "motor.h"
#include "stdlib.h"
#include "string.h"
#include "non_blocking_delay.h"

#include "time.h"
#include "date_and_time.h"


#define _PROBANDO_MOTORES


#ifdef _PROBANDO_PINES
DigitalOut d1(LED1);
DigitalOut s(LED2);
DigitalOut d2(LED3);
#endif  

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

motor motorArray[] = {motor(MOTOR1_PIN1 , MOTOR1_PIN2), motor(MOTOR2_PIN1, MOTOR2_PIN2)};

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


static feederStatus_t feederStatus;
static nonBlockingDelay_t freeModeDelay;
struct tm startTime;//tiempo de inicio del evento en TIME MODE
int durationTime;


//=====[Declarations (prototypes) of private functions]========================


static bool validate_uid(const char* uid);

static void feederTimeModeUpdate();
static void feederFreeModeUpdate();


//=====[Implementations of public functions]===================================

void feederInit(){

    feederStatus=FEEDER_MANUAL_MODE;

}
void feederUpdate() {

    switch(feederStatus){
        case FEEDER_MANUAL_MODE:{
            //----No hago nada
        }break;
        case FEEDER_FREE_MODE:{
            feederFreeModeUpdate();
        }break;
        case FEEDER_TIME_MODE:{
            feederTimeModeUpdate();
        }
        default:{

        }
    }
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

#ifdef _PROBANDO_MOTORES
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

#ifdef _PROBANDO_PINES
#define _PROBANDO_PINES
//pruebas con leds
    switch(motorArray[0].read()){
        case DIRECTION_1:{
            d1.write(ON);
            d2.write(OFF);
            s.write(OFF);

            break;
        }
        case DIRECTION_2:{
            d2.write(ON);
            d1.write(OFF);
            s.write(OFF);
            break;
        }
        case STOPPED:{
            s.write(ON);
            d1.write(OFF);
            d2.write(OFF);
            break;
        }
    }
#endif 



#ifdef _PROBANDO_MODOS
#define _PROBANDO_MODOS
//pruebas con PINES
    switch(feederStatus){
        case FEEDER_MANUAL_MODE:{
            modomanual.write(ON);
            modofree.write(OFF);
            modotiempo.write(OFF);
            break;
        }
        case FEEDER_FREE_MODE:{
            modomanual.write(OFF);
            modofree.write(ON);
            modotiempo.write(OFF);
            break;
        }
        case FEEDER_TIME_MODE:{
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

    if(feederStatus!=FEEDER_TIME_MODE){
        return NULL;
    }
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

    strcpy(cadena, asctime(&startTime));
    strcat(cadena, "Duration:");
    sprintf(buffer, "%d", durationTime);
    strcat(cadena, buffer);

    free(buffer); // Liberamos la memoria asignada para el buffer

    return cadena;

}



void feederTimeModeUpdate(){

    if(feederStatus!=FEEDER_TIME_MODE)
        return;
    struct tm date;
    strftime(dateAndTimeRead(),50,"%Y-%m-%d %H:%M:%S",&date);
    //printf("%f\n",difftime(mktime(&date),mktime(&startTime)));

        if(difftime(mktime(&date),mktime(&startTime))<0){
            
            for (int i = 0; i < 2; i++) {
                motorArray[i].write(DIRECTION_1);
             }

        }else{

            for (int i = 0; i < 2; i++) {
                motorArray[i].write(STOPPED);
            }
        }

    

}

//Cambia el la direccion de los motores(el valor se actualiza al ejecutar la funcion feederUpdate())
void manualModeUpdate(const char receivedChar){


    motorDirection_t d=STOPPED;
    if(feederStatus!=FEEDER_MANUAL_MODE){
        return;
    }
    switch(receivedChar){
        case 'z':case 'Z':  d=DIRECTION_1;  break;
        case 'x':case 'X':  d=STOPPED;  break;
        case 'c':case 'C':  d=DIRECTION_2;  break;
    }

    for (int i = 0; i < 2; i++) {
        motorArray[i].write(d);
    }


}

void feederFreeModeUpdate(){
    //Luego de haber esperado el delay de 2s freno los motores, si se llama devuelta al Init se reinicia el contador.
    if(nonBlockingDelayRead(&freeModeDelay)==true){
            for (int i = 0; i < 2; i++) {
                 motorArray[i].write(STOPPED);
            }   
    }

}
void feederFreeModeInit(const char* uid){
    if(!uid)
        return;
    if(feederStatus!=FEEDER_FREE_MODE)
        return;
    
    printf("%s\n",uid);

    if(validate_uid(uid)==true){

        for (int i = 0; i < 2; i++) {
            motorArray[i].write(DIRECTION_1);
        }
        nonBlockingDelayInit(&freeModeDelay,2000);

    }
    
}

bool validate_uid(const char* uid){
    if(!strcmp(uid,"C3F3209B")||!strcmp(uid,"C7458501"))
        return true;
    return false;
}