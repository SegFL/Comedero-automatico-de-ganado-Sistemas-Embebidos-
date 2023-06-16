//=====[Libraries]=============================================================


#include "mbed.h"
#include "arm_book_lib.h"

#include "feeder.h"
#include "motor.h"
#include "stdlib.h"
#include "string.h"
#include "non_blocking_delay.h"
#include "log.h"
#include "rfid.h"
#include "pc_serial_com.h"

#include "date_and_time.h"
#include "stdio.h"
#include "aux_functions.h"


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

//estructura que lleva un registro de quien y cuando se activo un motor
typedef struct{
    struct timeval initTime;
    feederStatus_t feederStatus;
    char* uid;
    bool state=false;
}motor_info_t;

typedef struct{
    struct timeval counter;
    char* uid;
   // bool state=false;
}feeder_info_t;

//=====[Declaration and initialization of public global objects]===============

motor motorArray[] = {motor(MOTOR1_PIN1 , MOTOR1_PIN2), motor(MOTOR2_PIN1, MOTOR2_PIN2)};

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


static feederStatus_t feederStatus;
static nonBlockingDelay_t freeModeDelay;
static nonBlockingDelay_t timeModeDelay;
struct tm startTime;//tiempo de inicio del evento en TIME MODE
int durationTime;
bool feederTimeStateSet = false;
static motor_info_t motor_info[2];
static feederStatus_t previousState;


//=====[Declarations (prototypes) of private functions]========================




static void feederTimeModeUpdate();
static void feederFreeModeUpdate();
static void feederManualModeUpdate();
static void motorInfoInit(motor_info_t* d,const char*uid);
static void motorInfoEnd(motor_info_t* d);


//=====[Implementations of public functions]===================================

void feederInit(){

    feederStatus=FEEDER_MANUAL_MODE;

}
void feederUpdate() {

    switch(feederStatus){
        case FEEDER_MANUAL_MODE:{
            feederManualModeUpdate();
        }break;
        case FEEDER_FREE_MODE:{
            feederFreeModeUpdate();
        }break;
        case FEEDER_TIME_MODE:{
            if(feederTimeStateSet==false)  //Si no estan un tiempo no hago nada
                break;
            feederTimeModeUpdate();
        }
        case FEEDER_NEW_UID:{
            
            if(logAdd(rfidGetUid())==true){
                feederStatus=previousState;
            }

            break;   
        }
        default:{break; }
    }
    //Me fijo si tengo que cambiar el estado a FEEDER_NEW_UID
    if( pcSerialComStateNewUid()==true){
    //Podria implementar un delay para que salga autoamticamente desp de x tiempo
     //previousState=feederStatus;
    feederStatus=FEEDER_NEW_UID;
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
    for (int i = 0; i < 2; i++) {
        motorArray[i].write(STOPPED);
   }

}

bool feederTimeSet( int year1, int month1, int day1, 
                       int hour1, int minute1, int second1, int duration){

    
    struct tm aux;
    aux.tm_year = year1 - 1900;
    aux.tm_mon  = month1 - 1;
    aux.tm_mday = day1;
    aux.tm_hour = hour1;
    aux.tm_min  = minute1;
    aux.tm_sec  = second1;
    aux.tm_isdst = -1;

    struct timeval date;
    gettimeofday(&date, NULL);
    if(date.tv_sec-mktime(&aux)<0){
        return false;
    }


    startTime.tm_year = year1 - 1900;
    startTime.tm_mon  = month1 - 1;
    startTime.tm_mday = day1;
    startTime.tm_hour = hour1;
    startTime.tm_min  = minute1;
    startTime.tm_sec  = second1;
    startTime.tm_isdst = -1;
    durationTime = duration;
    feederTimeStateSet=true;
    return true;

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

    cadena=strndup( asctime(&startTime),20);
    strcat(cadena, "Duration:");
    sprintf(buffer, "%d", durationTime);
    strcat(cadena, buffer);

    free(buffer); // Liberamos la memoria asignada para el buffer

    return cadena;

}



void feederTimeModeUpdate(){

    if(feederStatus!=FEEDER_TIME_MODE||feederTimeStateSet==false)
        return;

    struct timeval date;
    if(gettimeofday(&date, NULL))
        printf("%s\n","error");
    
    //printf("StartTime:%u\n",mktime(&startTime));
   // printf("Date:%u\n",date.tv_sec);
    //printf("Diff:%u\n",mktime(&startTime)-date.tv_sec);


     if(date.tv_sec>mktime(&startTime)){
            // printf("%s\n","entre");
            //Vuelvo a cargar el tiempo
            startTime.tm_mday+=1;
            nonBlockingDelayInit(&timeModeDelay,1000*durationTime);


            for (int i = 0; i < 2; i++) {
                motorArray[i].write(DIRECTION_1);
            }
        }else if(nonBlockingDelayRead(&timeModeDelay)==true){
            for (int i = 0; i < 2; i++) {
                motorArray[i].write(STOPPED);
            }
        }

    

}


 //Luego de haber esperado el delay de 1s freno los motores, si se llama devuelta al feederFreeModeInit se reinicia el contador.
void feederFreeModeUpdate(){

    if(feederStatus!=FEEDER_FREE_MODE)
        return;
   
    if(nonBlockingDelayRead(&freeModeDelay)==true){
            for (int i = 0; i < 2; i++) {
                 motorArray[i].write(STOPPED);
                 if(motor_info[i].state==true){
                     motorInfoEnd(&motor_info[i]);
                }
                 
            }  

    }

}
//uid es el identificador unico RFID
bool feederFreeModeInit(char* uid){
    if(!uid)
        return false;
    if(feederStatus!=FEEDER_FREE_MODE){//si no estoy en free mode elimino el string y no hago nada
        free(uid);
        return false;
    }

    
    printf("%s\n",uid);

    if(logExist(uid)==true){

        //Me fijo si tengo que prender el motor

        for (int i = 0; i < 2; i++) {
            motorArray[i].write(DIRECTION_1);
            //Escribo en que momento se encendio el motor y con que uid
            motorInfoInit(&motor_info[i],uid);
        }
        nonBlockingDelayInit(&freeModeDelay,1000);
        free(uid);
        return true;

    }
    free(uid);
    return false;
    
}

 void motorInfoInit(motor_info_t* d,const char*uid){
    if(!d||!uid)
       return;
    if(d->state==true)//Si ya esta inicializado no hago nada
        return;
    struct timeval date;
    gettimeofday(&date, NULL);
    d->initTime=date;
    free(d->uid);
    d->uid=strndup(uid,10);
    d->state=true;
 }
 static void motorInfoEnd(motor_info_t* d){
     if(!d)
        return;
    if(d->state==false)
            return;
    struct timeval date;
    gettimeofday(&date, NULL);
    //printf("%s:%d\n",d->uid,date.tv_sec-d->initTime.tv_sec);
//resto el tiempo en el cual se incio y el tiempo final
    logWrite(d->uid,date.tv_sec-d->initTime.tv_sec);
    free(d->uid);
    d->uid=NULL;
    d->state=false;
    

 }








void feederManualModeUpdate(){
    motorDirection_t d=STOPPED;

    if(feederStatus!=FEEDER_MANUAL_MODE){
        return;
    }
    int dir= commandGetFeederDirection();
    switch(dir){
        case -1:  d=DIRECTION_1;  break;
        case 0:  d=STOPPED;  break;
        case +1:  d=DIRECTION_2;  break;
    }

    for (int i = 0; i < 2; i++) {
        motorArray[i].write(d);
    }


}