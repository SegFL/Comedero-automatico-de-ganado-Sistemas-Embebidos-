



#include "FATFileSystem.h"
#include "platform/mbed_retarget.h"
#include "log.h"
#include "mbed.h"
#include "mbed.h"
#include "LittleFileSystem.h"
#include "FlashIAPBlockDevice.h"
#include "pc_serial_com.h"

#define BLOCK_SIZE 512
#define SECTOR_SIZE 512


typedef struct{

    char*uid;
    size_t duracion;
}data_t;

data_t* data_vector;







/*

void logInit(){

    // Inicializar el dispositivo de almacenamiento
    bd.init();

    // Montar el sistema de archivos
    int error = fs.mount(&bd);
    if (error) {
        printf("Error al montar el sistema de archivos: %d\n", error);
        return;
    }
    FILE* archivo=fopen("/fs/log.txt", "w");
    if(archivo == NULL){
        puts("no se pudo abrir el archivo");
    } else puts("se puedo crear");

    //abro archivo
    //leo y cargo todos los uid 
    //data_vector[0].uid=strndup("C3F3209B",10);
    //data_vector[1].uid=strndup("C7458501",10);
}



void logRead(){
    for(int i=0;i<2;i++){
        printf("UID:%s %d\n",data_vector[i].uid, data_vector[i].duracion);
    }
}
void logUpdate(const char*uid){
/*
    FILE* log=fopen("log.txt","r+");
    if(!log)
        return;
    
    fprintf(log,"UID:%s, fecha:%s\n",uid,dateAndTimeRead());
    fclose(log);

}


 void logWrite(const char* uid,int duracion){
     if(!uid)
        return;
     for(int i = 0;i<2;i++){
        if(!strcmp(uid,data_vector[i].uid)){
             data_vector[i].duracion+=duracion;
             return;
         }
     }   
 }
*/

 //=====[Libraries]=============================================================

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

LittleFileSystem fs( SPI3_MOSI, SPI3_MISO, SPI3_SCK, SPI3_CS );

FATFileSystem sdCardFileSystem("sd", &sd);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

bool sdCardInit()
{
    pcSerialComStringWrite("Looking for a filesystem in the SD card... \r\n");
    sdCardFileSystem.mount(&sd);
    DIR *sdCardListOfDirectories = opendir("/sd/");
    if ( sdCardListOfDirectories != NULL ) {
        pcSerialComStringWrite("Filesystem found in the SD card. \r\n");
        closedir(sdCardListOfDirectories);
        return true;
    } else {
        pcSerialComStringWrite("Filesystem not mounted. \r\n");
        pcSerialComStringWrite("Insert an SD card and ");
        pcSerialComStringWrite("reset the NUCLEO board.\r\n");
        return false;
    }
}

bool sdCardWriteFile( const char* fileName, const char* writeBuffer )
{
    char fileNameSD[SD_CARD_FILENAME_MAX_LENGTH+4] = "";
    
    fileNameSD[0] = '\0';
    strcat( fileNameSD, "/sd/" );
    strcat( fileNameSD, fileName );

    FILE *sdCardFilePointer = fopen( fileNameSD, "a" );

    if ( sdCardFilePointer != NULL ) {
        fprintf( sdCardFilePointer, "%s", writeBuffer );                       
        fclose( sdCardFilePointer );
        return true;
    } else {
        return false;
    }
}

bool sdCardReadFile( const char* fileName, char * readBuffer, int readBufferSize )
{
    char fileNameSD[SD_CARD_FILENAME_MAX_LENGTH+4] = "";
    int i;
    
    fileNameSD[0] = '\0';
    strcat( fileNameSD, "/sd/" );
    strcat( fileNameSD, fileName );
    
    FILE *sdCardFilePointer = fopen( fileNameSD, "r" );
    
    if ( sdCardFilePointer != NULL ) {
        pcSerialComStringWrite( "Opening file: " );
        pcSerialComStringWrite( fileNameSD );
        pcSerialComStringWrite( "\r\n" );

        i = 0;
        while ( ( !feof(sdCardFilePointer) ) && ( i < readBufferSize - 1 ) ) {
           fread( &readBuffer[i], 1, 1, sdCardFilePointer );
           i++;
        }
        readBuffer[i-1] = '\0';
        fclose( sdCardFilePointer );
        return true;
    } else {
        pcSerialComStringWrite( "File not found\r\n" );
        return false;
    }
}

bool sdCardListFiles( char* fileNamesBuffer, int fileNamesBufferSize )
{
    int NumberOfUsedBytesInBuffer = 0;
    struct dirent *sdCardDirectoryEntryPointer;

    DIR *sdCardListOfDirectories = opendir("/sd/");

    if ( sdCardListOfDirectories != NULL ) {
        pcSerialComStringWrite("Printing all filenames:\r\n");
        sdCardDirectoryEntryPointer = readdir(sdCardListOfDirectories);
        
        while ( ( sdCardDirectoryEntryPointer != NULL ) && 
                ( NumberOfUsedBytesInBuffer + strlen(sdCardDirectoryEntryPointer->d_name) < 
                    fileNamesBufferSize) ) {
            strcat( fileNamesBuffer, sdCardDirectoryEntryPointer->d_name );
            strcat( fileNamesBuffer, "\r\n" );
            NumberOfUsedBytesInBuffer = NumberOfUsedBytesInBuffer +
                                        strlen(sdCardDirectoryEntryPointer->d_name);
            sdCardDirectoryEntryPointer = readdir(sdCardListOfDirectories);
        }
        
        closedir(sdCardListOfDirectories);
        
        return true;
    } else {
        pcSerialComStringWrite("Insert an SD card and ");
        pcSerialComStringWrite("reset the NUCLEO board.\r\n");
        return false;
    }
}