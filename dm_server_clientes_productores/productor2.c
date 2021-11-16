#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

#define QUEUEPATH "/cola_mensaje2"
#define TIME_SLEEP 5

typedef struct {
    char *mensaje  ; 
} mensaje_prod2;

mqd_t qd_prod2 ;

int memoria_disponible()        ;
char* conver_intTochar (char *) ;
void enviar_msg (mensaje_prod2) ;
/**
 * @brief Funcion principal que se encarga de obtener la memorai disponible y enviarla a traves de una cola de mensaje
 * en un intervalo de tiempo deterimnado
 * 
 * @return int 
 */
int main (){

    char  mensaje [100] ;
    char numero   [100] ;
    mensaje_prod2 mensaje_enviar ;

    while(1){

        memset(mensaje, '\0', sizeof(mensaje));
        strcpy(mensaje, "El valor de la memoria disponible es: ") ;
        strcat(mensaje, conver_intTochar(numero)) ;
        mensaje_enviar.mensaje = mensaje ;
        printf ("El mensaje a enviar es: %s \n", mensaje_enviar.mensaje) ;
        enviar_msg(mensaje_enviar) ;
        sleep(TIME_SLEEP) ;
        
    }

    return 0 ;
}


/**
 * @brief Funcion que devuelve la memoria disponible
 * 
 * @return int Se devuelve la memoria disponible
 */
int memoria_disponible(){

    int memoriaDisponible ;
    FILE* archivo ;
    char buffer[100] ;
    char* linea ;

    archivo = fopen("/proc/meminfo", "r") ;

    while(feof(archivo) == 0){
        fgets(buffer, 100, archivo) ;
        linea = strstr(buffer, "MemAvailable") ;

        if(linea != NULL){
            break ;
        }
    }
    fclose(archivo) ;
    sscanf(linea, "MemAvailable: %i", &memoriaDisponible) ;
    return memoriaDisponible ;
}
/**
 * @brief Funcion que convierte un entero (la memoria disponible) en un char puntero con formato
 * 
 * 
 * 
 * @param a Variable que tendra el formato final
 * @return char* Se devuevle el formato correspondiente
 */
char* conver_intTochar (char* a){
    int myInteger = memoria_disponible(); 
    sprintf( a, "%d %s", myInteger, "KB" ); 
    //printf( "%s\n", a ); 
    return a ;

}
/**
 * @brief Funcion encargadad de enviar el mensaje a traves de una cola de mensaje
 * 
 * @param sent_msg Mensaje a ser enviado
 */
void enviar_msg(mensaje_prod2 sent_msg){

    qd_prod2 = mq_open(QUEUEPATH, O_RDWR);
    if (qd_prod2 == -1){
        perror("Creating queue");
        exit(EXIT_FAILURE);
    }

    if (  mq_send(qd_prod2, sent_msg.mensaje, strlen(sent_msg.mensaje) + 1, (unsigned int) 1) == -1){
        perror("Sending");
        exit(EXIT_FAILURE);
    }

}




