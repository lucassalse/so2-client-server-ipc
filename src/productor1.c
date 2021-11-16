#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

#define QUEUEPATH "/cola_mensaje1"
#define TIME_SLEEP 10

static char* msg[] = {"Productor1 mensaje randm1" ,"Productor1 mensaje randm2" , "Productor1 mensaje randm3", "Productor1 mensaje randm4"} ;

typedef struct {
    char *mensaje ; 
} mensaje_prod1;

char* generar_mensaje_random() ;
void enviar_msg (mensaje_prod1) ;

mqd_t qd_prod1 ;
/**
 * @brief Main principal que se encarga de mandar obtener un mensaje random y enviarlo por medio de una cola de mensaje
 * cada un cierto tiempo
 * 
 * @return int 
 */
int main (){

    while(1){
        mensaje_prod1 mensaje ;
        mensaje.mensaje = generar_mensaje_random() ;

        printf("El mensaje en la estructura es: %s \n", mensaje.mensaje) ;
        enviar_msg(mensaje) ;
        
        sleep(TIME_SLEEP) ;
    
    }
    return 0 ;
}
/**
 * @brief Funcion que devuelve un mensaje random de un arreglode char
 * 
 * @return char* Mensaje random
 */
char* generar_mensaje_random(){

    //int num_alt = 0 + rand() / (RAND_MAX / (3 - 0 + 1) + 1) ;
    char* random = msg[0 + rand() / (RAND_MAX / 4)] ;

    return random ;
}
/**
 * @brief Funcion encargada de mandar un mensaje a traves de la cola de mensaje
 * 
 * @param sent_msg Mensaje a ser enviado
 */
void enviar_msg(mensaje_prod1 sent_msg){

    qd_prod1 = mq_open(QUEUEPATH, O_RDWR);
    if (qd_prod1 == -1){
        perror("Creating queue");
        exit(EXIT_FAILURE);
    }

    if (  mq_send(qd_prod1, sent_msg.mensaje, strlen(sent_msg.mensaje) + 1, (unsigned int) 1) == -1){
        perror("Sending");
        exit(EXIT_FAILURE);
    }

}




