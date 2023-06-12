//=====[#include guards - begin]===============================================

#ifndef _FEEDER_H_
#define _FEEDER_H_


//=====[Declaration of public defines]=========================================

#define MOTOR1_PIN1 PF_12
#define MOTOR1_PIN2 PF_13

#define MOTOR2_PIN1 PF_14
#define MOTOR2_PIN2 PF_15

//=====[Declaration of public data types]======================================

typedef enum {
    FEEDER_MANUAL_MODE,
    FEEDER_FREE_MODE,
    FEEDER_TIME_MODE
} feederStatus_t;



//=====[Declarations (prototypes) of public functions]=========================

void feederInit();
void feederUpdate();


feederStatus_t feederStatusRead();
void feederStatusWrite(feederStatus_t);

char* feederTimeRead();
void feederTimeSet( int year1, int month1, int day1, 
                       int hour1, int minute1, int second1, int duration);
void manualModeUpdate(char receivedChar);
void feederFreeModeInit(const char* uid);




//=====[#include guards - end]=================================================

#endif // _FEEDER_H_
