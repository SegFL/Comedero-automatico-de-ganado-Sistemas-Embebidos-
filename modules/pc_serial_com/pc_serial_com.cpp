//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_com.h"
#include "log.h"
#include "rfid.h"
#include "ble_com.h"
#include "feeder.h"
#include "date_and_time.h"



//=====[Declaration of private defines]========================================

#define INTERFAZ_BLE_AND_PC
//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_COMMANDS,
    PC_SERIAL_SETTING_DATE,
    PC_SERIAL_SETTING_FEEDER_STATUS,
    PC_SERIAL_SETTING_FEEDER_TIME,
    PC_SERIAL_LOADING_NEW_UID
} pcSerialComMode_t;


// estructura con las variables para configurar la hora
struct dateAndTime{
     char year[5] = "";
	char month[3] = "";
	char day[3] = "";
	char hour[3] = "";
	char minute[3] = "";
	char second[3] = "";
	char duration[3]="";
};



//Un tipo de dato que utiliza por funciones para saber que campo de la fecha y hora se esta modificando
typedef enum {  SETTING_YEAR,
                SETTING_MONTH,
                SETTING_DAY,
                SETTING_HOUR,
                SETTING_MINUTE,
                SETTING_SECOND,
		        SETTING_DURATION,
                SETTING_DESACTIVATE,
 } setting_date_and_time_status_t;


//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;
static int feederManualModeDirection =0;
//Variable que utiliza pcSerialComCommandUpdate para saber si se esta actualizando el dia y la hora
static bool settingDateAndTimer=false;
setting_date_and_time_status_t date_and_time_status = SETTING_DESACTIVATE;


//=====[Declarations (prototypes) of private functions]========================

static void pcSerialComStringRead( char* str, int strLength );

static void pcSerialComCommandUpdate( const char receivedChar );
static void availableCommands();

static void commandShowCurrentFeederStatus();
static void commandSetFeederStatus(const char receivedChar);
static void commandSetFeederTime(const char receivedChar);
static void commandShowFeederTime();

static void commandSetFeederDirection(char receivedChar);



static void commandSetDateAndTime(const char receivedChar);
static void commandShowDateAndTime();
static void commandLoadUid();


//=====[Implementations of public functions]===================================

void pcSerialComInit()
{
    availableCommands();
}
//Lee un caracter de la pc o por BLE
char pcSerialComCharRead()
{
    char receivedChar = '\0';

    #ifdef INTERFAZ_PC

    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    #endif

    #ifdef INTERFAZ_BLE_AND_PC

    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    if(receivedChar=='\0'){
            receivedChar = bleComCharRead(); 
    }
    #endif

    
    return receivedChar;
}

void pcSerialComStringWrite( const char* str )
{
    

    #ifdef INTERFAZ_PC
    uartUsb.write( str, strlen(str) );
    #endif

    #ifdef INTERFAZ_BLE_AND_PC

    bleComStringWrite(str);
    uartUsb.write( str, strlen(str) );

    #endif
}

void pcSerialComUpdate()
{
  char receivedChar = pcSerialComCharRead();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
                 break;
            case PC_SERIAL_SETTING_DATE:
                commandSetDateAndTime( receivedChar );
                break;
            case PC_SERIAL_SETTING_FEEDER_STATUS:
                commandSetFeederStatus(receivedChar);
                break;
            case PC_SERIAL_SETTING_FEEDER_TIME:
                commandSetFeederTime(receivedChar);
                break;
            case PC_SERIAL_LOADING_NEW_UID:
                commandLoadUid();
                 break;
            default:
                pcSerialComMode = PC_SERIAL_COMMANDS;
                break;
        }
    }    
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
    switch (receivedChar) {
        case 'q':case 'Q': commandShowCurrentFeederStatus(); break;
        case 'w':case 'W': commandSetFeederStatus('\0'); break;
        case 'e':case 'E': commandShowDateAndTime(); break;
        case 'r':case 'R': commandSetDateAndTime('\0'); break;
        case 't':case 'T': commandSetFeederTime('\0');break;
        case 'y':case 'Y': commandShowFeederTime();break;
        case 'u':case 'U': logRead();break;
        case 'i': case 'I':commandLoadUid();break;
        case 'z': case 'x':case 'c':case 'Z':case 'X':case 'C': commandSetFeederDirection(receivedChar);break;
        
        default: availableCommands(); break;
    } 
}

int commandGetFeederDirection(){
    return feederManualModeDirection;
}
void commandSetFeederDirection(char receivedChar){

    switch(receivedChar){
        case 'z':  case 'Z': {
            feederManualModeDirection=-1;
            break;
        }
        case 'c':  case 'C': {
           feederManualModeDirection=1;
            break;
        }
        case 'x':  case 'X': {
            feederManualModeDirection=0;
            break;
        }
        default:
            break;

    }

}

static void availableCommands(){
    pcSerialComStringWrite( "Available commands:\r\n" );

    pcSerialComStringWrite( "Press 'q' to get the feeder state\r\n" );
    pcSerialComStringWrite( "Press 'w' to set the feeder state\r\n" );
    pcSerialComStringWrite( "Press 'e' ShowDateAndTime\r\n" );
    pcSerialComStringWrite( "Press 'r' SetDateAndTime\r\n" );
    pcSerialComStringWrite( "Press 't' SetFeederTime\r\n" );
    pcSerialComStringWrite( "Press 'y' GetFeederTime\r\n" );
    pcSerialComStringWrite( "Press 'u' ShowLog\r\n" );
    pcSerialComStringWrite( "Press 'i' Add Entry To Log\r\n" );
    pcSerialComStringWrite( "Press 'Z','X','C' to move de motors (ONLY IN MANUAL MODE)\r\n" );


    pcSerialComStringWrite( "\r\n" );
}
static void commandShowCurrentFeederState(){
    feederStatus_t estate =  feederStatusRead();

    switch(estate){
        case FEEDER_FREE_MODE: {
            pcSerialComStringWrite( "Feeder en modo FREE MODE:\r\n" );
            break;
        }
        case FEEDER_MANUAL_MODE:{
            pcSerialComStringWrite( "Feeder en modo MANUAL MODE:\r\n" );
            break;

        }
        case FEEDER_TIME_MODE:{
            pcSerialComStringWrite( "Feeder en modo TIME MODE:\r\n" );
        }
        default:{break;}
    }
    pcSerialComStringWrite( "\r\n" );
}



static void commandShowDateAndTime()
{
    char str[100] = "";
    sprintf ( str, "Date and Time = %s", dateAndTimeRead() );
    pcSerialComStringWrite( str );
    pcSerialComStringWrite("\r\n");
}


static void commandSetFeederStatus(char receivedChar)
{
    feederStatus_t mode = FEEDER_MANUAL_MODE;

    if(receivedChar=='\0'){
        pcSerialComMode=PC_SERIAL_SETTING_FEEDER_STATUS;
        pcSerialComStringWrite( "Press '1' to set the feeder state to MANUAL MODE\r\n" );
        pcSerialComStringWrite( "Press '2' to set the feeder state to FREE MODE\r\n" );    
        pcSerialComStringWrite( "Press '3' to set the feeder state to TIME MODE\r\n" );
        pcSerialComStringWrite("\r\n");
    }else {
        switch(receivedChar){
            case '1':{
                mode=FEEDER_MANUAL_MODE;
                break;
            }
           case '2':{
                mode=FEEDER_FREE_MODE;
                break;
            }
            case '3':{
                 mode=FEEDER_TIME_MODE;
                break;
            }
            default:{
                pcSerialComMode=PC_SERIAL_COMMANDS;
                pcSerialComStringWrite( "Wrong key pressed\r\n" );
                return;
            }
        }
        //aca termina el switch
        pcSerialComMode=PC_SERIAL_COMMANDS;
        feederStatusWrite(mode);
        commandShowCurrentFeederStatus();


    }
    
}
void commandShowCurrentFeederStatus(){

    switch(feederStatusRead()){

        case FEEDER_MANUAL_MODE:{
            pcSerialComStringWrite( "Manual mode\r\n" );
            break;
        }
        case FEEDER_FREE_MODE:{
            pcSerialComStringWrite( "Free mode\r\n" );
            break;
        }
        case FEEDER_TIME_MODE:{
            pcSerialComStringWrite( "Time mode\r\n" );
            break;
        }
        default:{
            pcSerialComStringWrite( "Error no mode selected\r\n" );
            break;
        }
    }
}   


static void commandSetDateAndTime(const char receivedChar)
{

    static dateAndTime dat;
    static int indice;


    if(receivedChar =='\0') {
        pcSerialComStringWrite("\r\nType four digits for the current year (YYYY): ");
        pcSerialComMode=PC_SERIAL_SETTING_DATE;
        date_and_time_status = SETTING_YEAR;
        indice = 0;
        return;
    }

  
/*Cambios para que no sea bloquiante:Semana 4*/
//
        switch (date_and_time_status) {
            case SETTING_YEAR: {
                if (indice < 4) {
                    dat.year[indice] = receivedChar;
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
                        dat.month[indice] = receivedChar;
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
                        dat.day[indice] = receivedChar;
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
                    dat.hour[indice] = receivedChar;
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
                    dat.minute[indice] = receivedChar;
                    indice++;
                }
                if (indice == 2) {
                    dat.minute[2] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_SECOND;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(
                        "Type two digits for the current seconds (00-59): ");
                }        
            } break;

            case SETTING_SECOND: {
                if (indice < 2) {
                    dat.second[indice] = receivedChar;
                    indice++;
                }
                if (indice == 2) {
                    dat.second[2] = '\0';
                    indice = 0;
                    dateAndTimeWrite(atoi(dat.year), atoi(dat.month), atoi(dat.day), atoi(dat.hour),
                            atoi(dat.minute), atoi(dat.second));
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite("Date and time has been set\r\n");
                    commandShowDateAndTime();
                    date_and_time_status = SETTING_DESACTIVATE;
                    pcSerialComMode=PC_SERIAL_COMMANDS;
                }    
            } break;
    
            case SETTING_DESACTIVATE: {
                pcSerialComMode=PC_SERIAL_COMMANDS;
            } break;
        }
}

static void commandSetFeederTime(const char receivedChar){

    static dateAndTime dat;
    static int indice;

    if(feederStatusRead()!=FEEDER_TIME_MODE){
            pcSerialComStringWrite("\r\nThe feeder is not in TIME MODE ");
            return;
    }
    //En caso de que el caracter sea nulo no hago nada
    if(receivedChar =='\0') {
        pcSerialComStringWrite("\r\nType four digits for the current year(feeder) (YYYY): ");
        pcSerialComMode=PC_SERIAL_SETTING_FEEDER_TIME;
        date_and_time_status = SETTING_YEAR;
        indice = 0;
        return;
    }


        switch (date_and_time_status) {
            case SETTING_YEAR: {
                if (indice < 4) {
                    dat.year[indice] = receivedChar;
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
                        dat.month[indice] = receivedChar;
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
                        dat.day[indice] = receivedChar;
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
                    dat.hour[indice] = receivedChar;
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
                    dat.minute[indice] = receivedChar;
                    indice++;

                }
                if (indice == 2) {
                    dat.minute[2] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_SECOND;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(
                        "Type two digits for the current seconds (00-59): ");
                }        
            } break;
            case SETTING_SECOND: {
                if (indice < 2) {
                    dat.second[indice] = receivedChar;
                    indice++;
                }
                if (indice == 2) {
                    dat.second[2] = '\0';
                    indice = 0;
                    pcSerialComStringWrite("\r\n");
			pcSerialComStringWrite("Type the duration of the event\r\n");
                    date_and_time_status = SETTING_DURATION;
                }    
            } break;
	    
	    case SETTING_DURATION:{
		if (indice < 2) {
                    dat.duration[indice] = receivedChar;
                    indice++;
                }
                if (indice == 2) {
                    dat.second[2] = '\0';
                    indice = 0;
		            if(feederTimeSet(atoi(dat.year), atoi(dat.month), atoi(dat.day), atoi  (dat.hour),
                     atoi(dat.minute), atoi(dat.second),atoi(dat.duration))==false){
                     pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(" The time is invalid\r\n");
                    commandShowFeederTime();
                     }
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(" The time and duaration has been set\r\n");
                    commandShowFeederTime();
	
                    date_and_time_status = SETTING_DESACTIVATE;
		    pcSerialComMode=PC_SERIAL_COMMANDS;
                }    
	    }break;
            case SETTING_DESACTIVATE: {
                pcSerialComMode=PC_SERIAL_COMMANDS;
            } break;
        }
    
}

static void commandShowFeederTime()
{
     if(feederStatusRead()!=FEEDER_TIME_MODE){
            pcSerialComStringWrite("\r\nThe feeder is not in TIME MODE ");
            return;
     }
    char str[100] = "";
    sprintf ( str, "Feeder time = %s",feederTimeRead() );
    pcSerialComStringWrite( str );
    pcSerialComStringWrite("\r\n");
}
static void commandLoadUid(){

    if(pcSerialComMode!=PC_SERIAL_LOADING_NEW_UID){

        pcSerialComMode=PC_SERIAL_LOADING_NEW_UID;
        pcSerialComStringWrite("\r\nIntrudce a new RFID tag");
    }else{
        pcSerialComMode=PC_SERIAL_COMMANDS;         
        pcSerialComStringWrite("\r\nFinished introducing new tags ");
    }
    
}

bool pcSerialComStateNewUid(){

    return pcSerialComMode==PC_SERIAL_LOADING_NEW_UID?true:false;

}