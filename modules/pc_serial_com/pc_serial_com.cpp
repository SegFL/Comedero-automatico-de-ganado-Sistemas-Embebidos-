//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_com.h"


#include "date_and_time.h"
#include "gas_sensor.h"
#include "event_log.h"
#include "motor.h"
#include "gate.h"
#include "motion_sensor.h"
#include "alarm.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_COMMANDS,
    PC_SERIAL_GET_CODE,
    PC_SERIAL_SAVE_NEW_CODE,
} pcSerialComMode_t;

// estructura con las variables para configurar la hora
struct dateAndTime{
     char year[5] = "";
	char month[3] = "";
	char day[3] = "";
	char hour[3] = "";
	char minute[3] = "";
	char second[3] = "";
};


//Un tipo de dato que utiliza la funcion commandSetDateAndTime para saber que campo de la fecha y hora se esta modificando
typedef enum {  SETTING_YEAR,
                SETTING_MONTH,
                SETTING_DAY,
                SETTING_HOUR,
                SETTING_MINUTE,
                SETTING_SECOND,
                SETTING_DESACTIVATE,
 } setting_date_and_time_status_t;


//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
static bool codeComplete = false;
static int numberOfCodeChars = 0;


static bool settingDateAndTimer=false;      //Variable que utiliza pcSerialComCommandUpdate para saber si se esta actualizando el dia y la hora
setting_date_and_time_status_t date_and_time_status = SETTING_DESACTIVATE;

//=====[Declarations (prototypes) of private functions]========================

static void pcSerialComStringRead( char* str, int strLength );

static void pcSerialComGetCodeUpdate( char receivedChar );
static void pcSerialComSaveNewCodeUpdate( char receivedChar );

static void pcSerialComCommandUpdate( char receivedChar );

static void availableCommands();

static void commandEnterCodeSequence();
static void commandEnterNewCode();

static void commandSetDateAndTime(const char charReceived);
static void commandShowDateAndTime();
static void commandShowStoredEvents();
static void commandShowCurrentMotorState();

static void commandMotionSensorActivate();
static void commandMotionSensorDeactivate();

//=====[Implementations of public functions]===================================

void pcSerialComInit()
{
    availableCommands();
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    return receivedChar;
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.write( str, strlen(str) );
}

void pcSerialComUpdate()
{
    char receivedChar = pcSerialComCharRead();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
            break;

            case PC_SERIAL_GET_CODE:
                pcSerialComGetCodeUpdate( receivedChar );
            break;

            case PC_SERIAL_SAVE_NEW_CODE:
                pcSerialComSaveNewCodeUpdate( receivedChar );
            break;
            default:
                pcSerialComMode = PC_SERIAL_COMMANDS;
            break;
        }
    }    
}

bool pcSerialComCodeCompleteRead()
{
    return codeComplete;
}

void pcSerialComCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void pcSerialComStringRead( char* str, int strLength )
{
    int strIndex;
    for ( strIndex = 0; strIndex < strLength; strIndex++) {
        uartUsb.read( &str[strIndex] , 1 );
        uartUsb.write( &str[strIndex] ,1 );
    }
    str[strLength]='\0';
}




static void pcSerialComCommandUpdate( char receivedChar )
{

    if(settingDateAndTimer==true) {
        commandSetDateAndTime(receivedChar);
        return;
    }
    switch (receivedChar) {
        case 's': case 'S': commandSetDateAndTime('\0'); break;
        case 't': case 'T': commandShowDateAndTime(); break;
        case 'e': case 'E': commandShowStoredEvents(); break;
        case 'm': case 'M': commandShowCurrentMotorState(); break;
        default: availableCommands(); break;
    } 
}

static void availableCommands()
{
    pcSerialComStringWrite( "Available commands:\r\n" );
    pcSerialComStringWrite( "Press 's' or 'S' to set the date and time\r\n" );
    pcSerialComStringWrite( "Press 't' or 'T' to get the date and time\r\n" );
    pcSerialComStringWrite( "Press 'e' or 'E' to get the stored events\r\n" );
    pcSerialComStringWrite( "Press 'm' or 'M' to show the motor status\r\n" );
    pcSerialComStringWrite( "\r\n" );
}










static void commandSetDateAndTime(const char charReceived)
{

    static dateAndTime dat;
    static int indice;

    //En caso de que el caracter sea nulo no hago nada
    if(charReceived =='\0') {
        puts("Entro a :commandSetDateAndTime por primera vez\n");
          pcSerialComStringWrite("\r\nType four digits for the current year (YYYY): ");
        settingDateAndTimer = true;
        date_and_time_status = SETTING_YEAR;
        indice = 0;
         return;
    }

  
/*Cambios para que no sea bloquiante:Semana 4*/
//
        switch (date_and_time_status) {
            case SETTING_YEAR: {
                if (indice < 4) {
                    dat.year[indice] = charReceived;
                    indice++;
                }
                if (indice == 4) {
                    dat.year[4] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_MONTH;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(
                        "Type two digits for the current month (01-12): ");
                }        
            } break;

            case SETTING_MONTH: {
                    if (indice < 2) {
                        dat.month[indice] = charReceived;
                        indice++;
                    }
                    if (indice == 2) {
                    dat.month[2] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_DAY;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite("Type two digits for the current day (01-31): ");
                    }
            } break;

            case SETTING_DAY: {
                    if (indice < 2) {
                        dat.day[indice] = charReceived;
                        indice++;
                    }
                    if (indice == 2) {
                        dat.day[2] = '\0';
                        indice = 0;
                        date_and_time_status = SETTING_HOUR;
                        pcSerialComStringWrite("\r\n");
                        pcSerialComStringWrite(
                            "Type two digits for the current hour (00-23): ");
                    }    
            } break;

            case SETTING_HOUR: {
                if (indice < 2) {
                    dat.hour[indice] = charReceived;
                    indice++;
                }
                if (indice == 2) {
                    dat.hour[2] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_MINUTE;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(
                     "Type two digits for the current minutes (00-59): ");
                }
            } break;

            case SETTING_MINUTE: {
                if (indice < 2) {
                    dat.second[indice] = charReceived;
                    indice++;

                }
                if (indice == 2) {
                    dat.second[2] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_SECOND;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(
                        "Type two digits for the current seconds (00-59): ");
                }        
            } break;

            case SETTING_SECOND: {
                if (indice < 2) {
                    dat.second[indice] = charReceived;
                    indice++;
                }
                if (indice == 2) {
                    dat.second[2] = '\0';
                    dateAndTimeWrite(atoi(dat.year), atoi(dat.month), atoi(dat.day), atoi(dat.hour),
                            atoi(dat.minute), atoi(dat.second));
                    indice = 0;
                    settingDateAndTimer = false; // pongo en falso la variable para saber si se estacambiando la fecha y hora
                    date_and_time_status = SETTING_DESACTIVATE;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite("Date and time has been set\r\n");
                }    
            } break;
    
            case SETTING_DESACTIVATE: {
                indice = 0;
            } break;
        }

}

static void commandShowDateAndTime()
{
    char str[100] = "";
    sprintf ( str, "Date and Time = %s", dateAndTimeRead() );
    pcSerialComStringWrite( str );
    pcSerialComStringWrite("\r\n");
}



static void commandShowCurrentMotorState()
{
    switch ( motorDirectionRead() ) {
        case STOPPED: 
            pcSerialComStringWrite( "The motor is stopped\r\n"); break;
        case DIRECTION_1: 
            pcSerialComStringWrite( "The motor is turning in direction 1\r\n"); break;
        case DIRECTION_2: 
            pcSerialComStringWrite( "The motor is turning in direction 2\r\n"); break;
    }
}

static void commandShowCurrentMotorsState()
{

for(int i = 0; i < NUMBER_OF_MOTORS;i++ ){
    switch ( motorDirectionRead(i) ) {
        case GATE_CLOSED: pcSerialComStringWrite( "The gate is closed\r\n"); break;
        case GATE_OPEN: pcSerialComStringWrite( "The gate is open\r\n"); break;
        case GATE_OPENING: pcSerialComStringWrite( "The gate is opening\r\n"); break;
        case GATE_CLOSING: pcSerialComStringWrite( "The gate is closing\r\n"); break;
    }
}
    
}


