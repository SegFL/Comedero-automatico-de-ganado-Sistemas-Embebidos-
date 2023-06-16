



#include "log.h"
#include "mbed.h"
#include "aux_functions.h"
#include "pc_serial_com.h"
#include "stdlib.h"


typedef struct{

    char*uid;
    size_t duracion;
}data_t;

data_t** data_vector=NULL;
size_t cantidad=0;




 bool logAdd(const char* uid);
 void logInit();
 void logUpdate(const char*uid);
 void logDelete(const char* uid);




void logInit(){

    
    //leo y cargo todos los uid 
    //data_vector[0].uid=strndup("C3F3209B",10);
    //data_vector[1].uid=strndup("C7458501",10);
}



void logRead(){
    if(!data_vector){
        pcSerialComStringWrite("The log is empty\n");
        return;

    }

    for(int i=0;i<2;i++){
        printf("UID:%s %d\n",data_vector[i]->uid, data_vector[i]->duracion);
    }

}
void logUpdate(const char*uid){

    //fprintf(log,"UID:%s, fecha:%s\n",uid,dateAndTimeRead());
    
}

void logWrite(const char* uid,int duracion){
     
     if(!uid||!data_vector)
        return;
     for(int i = 0;i<cantidad;i++){
        if(!strcmp(uid,data_vector[i]->uid)){
             data_vector[i]->duracion+=duracion;
             return;
         }
     } 
 }

bool logAdd(char*uid){

    if(!uid||!data_vector)
        return false;
    puts("entre a logAdd con algo no nulo");
    
    data_t* data_aux=(data_t*)calloc(1,sizeof(data_t));
    if(!data_aux)
        return false;
    if(!(data_aux->uid=strndup(uid,10))){
        free(data_aux);
        return false;
    }
    data_t** vector_aux=(data_t**)realloc(data_vector,sizeof(data_t*)*(cantidad+1));
    if(!vector_aux){
        free(data_aux->uid);
        free(data_aux);
        return false;  
    }
    data_vector=vector_aux;
    data_vector[cantidad+1]=data_aux;
    cantidad++;
    return true;

    
}
void logDelete(const char* uid){

 
 }

 //Busca si existe el uid
bool logExist(const char* uid){
     
     if(!uid)
        return true;
     for(int i=0;i<cantidad;i++){
         if(!strcmp(uid,data_vector[i]->uid))
            return true;
     }
     return false;
 }
