#ifndef TP1_LISTA_H
#define TP1_LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <mqueue.h>
#include <openssl/md5.h>
//#include "zip.h"
//#include <zip.h>
#include <zip.h>
#include <sys/sendfile.h>



typedef struct Cliente{
    int id ;
    int fd ;
    int prod1 ;
    int prod2 ;
    int prod3 ;
    //char *buffer ;
    char direccion_puerto [100] ;
} Cliente;

typedef struct Nodo{
    Cliente cliente;
    struct Nodo* siguiente ;
} Nodo;


typedef struct Lista{
    Nodo* cabeza ;
    int longitud ;
} Lista ;

void eliminar_ultimo(Lista*) ;
int esta_vacia(Lista*) ;
void eliminar_principio(Lista*) ;
int contar(Lista*) ;
void insertar_final(Lista*, Cliente*) ;
void insertar_principio(Lista*, Cliente*) ;
void eliminar_nodo(Nodo*) ;
Nodo* crear_nodo (Cliente*) ;
//void recorrer_mostar(Lista*) ;
void recorrer_mostar(Lista*, char*) ;
void recorrer_mostar2(Lista*, char*) ;
void recorrer_mostar3(Lista*, char*) ;
void mostrar_nodo(Nodo*) ;
void send_mensaje(Nodo*, char*) ;
void send_mensaje2(Nodo*, char*) ;
void send_mensaje3(Nodo*, char*) ;

void recorrer_comandos(Lista*, char*) ;
int set_comandos(Nodo*, char*, Lista*) ;
int flagProductor(char*) ;
void escribir_log (Nodo*, char*, int) ;
char* get_menasje_md5(char *, char *) ;
void recorrer_eliminar(Lista*, int) ;
void eliminar_cliente(Lista*, int) ;
void read_log(char*) ;
void recorrer_listar(Lista*) ;
void comprimir_log() ;
off_t bytes_envio () ;
void send_log(Nodo*) ;

#endif