//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_com.h"





#include "feeder.h"
#include "date_and_time.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_COMMANDS,
    PC_SERIAL_SETTING_DATE,
    PC_SERIAL_SETTING_FEEDER_STATUS,
    PC_SERIAL_SETTING_FEEDER_TIME
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

//Variable que utiliza pcSerialComCommandUpdate para saber si se esta actualizando el dia y la hora
//static bool settingDateAndTimer=false;
setting_date_and_time_status_t date_and_time_status = SETTING_DESACTIVATE;


//=====[Declarations (prototypes) of private functions]========================

static void pcSerialComStringRead( char* str, int strLength );

static void pcSerialComCommandUpdate( char receivedChar );
static void availableCommands();

static void commandShowCurrentFeederStatus();
static void commandSetFeederStatus(char);
static void commandSetFeederTime(const char charReceived);

static bool commandReadDateAndTime(const char charReceived,struct dateAndTime*);


static void commandSetDateAndTime(const char charReceived);
static void commandShowDateAndTime();


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
            case PC_SERIAL_SETTING_DATE:
                commandSetDateAndTime( receivedChar );
            break;

            case PC_SERIAL_SETTING_FEEDER_STATUS:  
                commandSetFeederStatus(receivedChar);
            case PC_SERIAL_SETTING_FEEDER_TIME:
                commandSetFeederTime(receivedChar);

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
        case '1': commandShowCurrentFeederStatus(); break;
        case '2': commandSetFeederStatus('\0'); break;
        case '3': commandShowDateAndTime(); break;
        case '4': commandSetDateAndTime('\0'); break;
        default: availableCommands(); break;
    } 
}

static void availableCommands(){
    pcSerialComStringWrite( "Available commands:\r\n" );

    pcSerialComStringWrite( "Press '1' to get the feeder state\r\n" );
    pcSerialComStringWrite( "Press '2' to set the feeder state\r\n" );
    pcSerialComStringWrite( "Press '3' ShowDateAndTime\r\n" );
    pcSerialComStringWrite( "Press '4' SetDateAndTime\r\n" );

    pcSerialComStringWrite( "\r\n" );
}
static void commandShowCurrentFeederState(){


    switch(feederStatusRead()){
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


static void commandSetDateAndTime(const char charReceived)
{

    //Si ya estoy seteando la fecha no hago nada ya que puede ser otra funcion la que este aceptando caracterres
    if(pcSerialComMode==PC_SERIAL_SETTING_DATE)
        return;
    pcSerialComMode=PC_SERIAL_SETTING_DATE;

    static dateAndTime dat;

    //si la funcion no devuelve true significa que sigue aceptando caracteres por lo tanto no actualizo los valores de tiempo
    if(commandReadDateAndTime(charReceived,&dat)==true){

            dateAndTimeWrite(atoi(dat.year), atoi(dat.month), atoi(dat.day), atoi(dat.hour),atoi(dat.minute), atoi(dat.second));

            //si seteo la fecha significa que termine por lo que vuevlo al modo normal
            pcSerialComMode = PC_SERIAL_COMMANDS;
    }

}

static void commandShowDateAndTime()
{
    char str[100] = "";
    sprintf ( str, "Date and Time = %s", dateAndTimeRead() );
    pcSerialComStringWrite( str );
    pcSerialComStringWrite("\r\n");
}


static void commandSetFeederStatus(char charReceived)
{
    feederStatus_t mode = FEEDER_MANUAL_MODE;

    if(charReceived=='\0'){
        pcSerialComMode=PC_SERIAL_SETTING_FEEDER_STATUS;
        pcSerialComStringWrite( "Press '1' to set the feeder state to MANUAL MODE\r\n" );
        pcSerialComStringWrite( "Press '2' to set the feeder state to FREE MODE\r\n" );    
        pcSerialComStringWrite( "Press '3' to set the feeder state to TIME MODE\r\n" );
        pcSerialComStringWrite("\r\n");
    }else {
        switch(charReceived){
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
        commandShowCurrentFeederState();


    }
    
}


static void commandSetFeederTime(const char charReceived){

    //Verifico de que el feeder esta en TIME MODE
    if(feederStatusRead()!=FEEDER_TIME_MODE){
        return;
    }

    //Si ya estoy seteando la fecha no hago nada ya que puede ser otra funcion la que este aceptando caracterres
    if(pcSerialComMode==PC_SERIAL_SETTING_DATE)
        return;
    pcSerialComMode=PC_SERIAL_SETTING_DATE;

    static dateAndTime dat1;//Tiempo en el cual se prenden los motores
    static dateAndTime dat2;//Tiempo que los motores permanecen prendidos

    //si la funcion no devuelve true significa que sigue aceptando caracteres por lo tanto no actualizo los valores de tiempo
    if(commandReadDateAndTime(charReceived,&dat1)==true){

        if(commandReadDateAndTime(charReceived,&dat2)==true){


            dateAndTimeWrite(atoi(dat.year), atoi(dat.month), atoi(dat.day), atoi(dat.hour),atoi(dat.minute), atoi(dat.second));

            //si seteo la fecha significa que termine por lo que vuevlo al modo normal
            pcSerialComMode = PC_SERIAL_COMMANDS;
            
        }



    }

}













//Funcion geneirca que lee una fecha de la consola
//devuelve false si tadavia no termino de leer y true si lo hizo. Recive como parametro un caracter leido de la consola y un puntero a un variable datAndTime a modificar
static bool commandReadDateAndTime(const char charReceived,dateAndTime* dat)
{

    static int indice;

    //En caso de que el caracter sea nulo significa que tengo que emepzar a aceptar caracteres
    if(charReceived =='\0') {
        pcSerialComMode = PC_SERIAL_SETTING_DATE;   //Cambio de estado el modulo
          pcSerialComStringWrite("\r\nType four digits for the year (YYYY): ");
        //settingDateAndTimer = true;
        date_and_time_status = SETTING_YEAR;
        indice = 0;
        return false;
    }

  
/*Cambios para que no sea bloquiante:Semana 4*/
//
        switch (date_and_time_status) {
            case SETTING_YEAR: {
                if (indice < 4) {
                    dat->year[indice] = charReceived;
                    indice++;
                }
                if (indice == 4) {
                    dat->year[4] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_MONTH;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(
                        "Type two digits for the month (01-12): ");
                }        
            } break;

            case SETTING_MONTH: {
                    if (indice < 2) {
                        dat->month[indice] = charReceived;
                        indice++;
                    }
                    if (indice == 2) {
                    dat->month[2] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_DAY;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite("Type two digits for the day (01-31): ");
                    }
            } break;

            case SETTING_DAY: {
                    if (indice < 2) {
                        dat->day[indice] = charReceived;
                        indice++;
                    }
                    if (indice == 2) {
                        dat->day[2] = '\0';
                        indice = 0;
                        date_and_time_status = SETTING_HOUR;
                        pcSerialComStringWrite("\r\n");
                        pcSerialComStringWrite(
                            "Type two digits for the hour (00-23): ");
                    }    
            } break;

            case SETTING_HOUR: {
                if (indice < 2) {
                    dat->hour[indice] = charReceived;
                    indice++;
                }
                if (indice == 2) {
                    dat->hour[2] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_MINUTE;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(
                     "Type two digits for the minutes (00-59): ");
                }
            } break;

            case SETTING_MINUTE: {
                if (indice < 2) {
                    dat->second[indice] = charReceived;
                    indice++;

                }
                if (indice == 2) {
                    dat->second[2] = '\0';
                    indice = 0;
                    date_and_time_status = SETTING_SECOND;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite(
                        "Type two digits for the seconds (00-59): ");
                }        
            } break;

            case SETTING_SECOND: {
                if (indice < 2) {
                    dat->second[indice] = charReceived;
                    indice++;
                }
                if (indice == 2) {
                    dat->second[2] = '\0';
                    indice = 0;
                    //settingDateAndTimer = false; // pongo en falso la variable para saber si se estacambiando la fecha y hora
                    date_and_time_status = SETTING_DESACTIVATE;
                    pcSerialComStringWrite("\r\n");
                    pcSerialComStringWrite("Date and time has been set\r\n");
                    return true;
                }    
            } break;
    
            case SETTING_DESACTIVATE: {
                pcSerialComMode = PC_SERIAL_COMMANDS;//si termine de setear la fecha vuelvo al modo comun
                indice = 0;
            } break;
        }

        return false;
}