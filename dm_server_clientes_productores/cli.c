#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

#define QUEUEPATH "/cola_mensaje_cli"

#define BUFFER 256
#define SIZE2   20

typedef struct {
    char *mensaje  ; 
} mensaje_cli;

mqd_t qd_cli ;

int parsear (char *palabras[], char *cadena) ;
void enviar_msg(mensaje_cli sent_msg) ;
/**
 * @brief Funcion main encargada de obtener por stdin un mensaje, corroborar que sea correcto (formato correcto)
 * y enviarlo al dm por medio de la cola de mensaje
 * 
 * @return int 
 */
int main(){

    int cantidad_palabras ;
    char entrada[BUFFER] ;
    char mensaje_teclado[BUFFER];
    char *trims[3] ;
    mensaje_cli mensaje_enviar ;

    while(1){
        printf("Ingrese un comando de manera correcta > ") ;
        if (fgets (entrada, BUFFER, stdin) == NULL){
            perror ("error en fgets");
        }
        strcpy(mensaje_teclado, entrada) ;
        cantidad_palabras = parsear(trims, entrada) ;
        if (strcmp(trims[0], "add") == 0 && cantidad_palabras == 3 &&( (strcmp(trims[2], "productor1")== 0)
             || (strcmp(trims[2] , "productor2") == 0) || (strcmp(trims[2] , "productor3") == 0) )  ){
            mensaje_enviar.mensaje = mensaje_teclado ;
            enviar_msg(mensaje_enviar) ;
        }  
        if (strcmp(trims[0], "delete") == 0 && cantidad_palabras == 3 &&( (strcmp(trims[2], "productor1")== 0)
                || (strcmp(trims[2] , "productor2") == 0) || (strcmp(trims[2] , "productor3") == 0) ) ){
            mensaje_enviar.mensaje = mensaje_teclado ;
            enviar_msg(mensaje_enviar) ;
        }        
        if (strcmp(trims[0], "log") == 0 && cantidad_palabras == 2){
            mensaje_enviar.mensaje = mensaje_teclado ;
            enviar_msg(mensaje_enviar) ;
        }               
        if (strcmp(trims[0], "listar") == 0 && cantidad_palabras == 1){
            mensaje_enviar.mensaje = mensaje_teclado ;
            enviar_msg(mensaje_enviar) ;
        }              
        if (strcmp(trims[0], "cantidad") == 0 && cantidad_palabras == 1){
            mensaje_enviar.mensaje = mensaje_teclado ;
            printf("Voy a enviar cantidad conexiones \n") ;
            enviar_msg(mensaje_enviar) ;
        } 
    }
    return 0 ;
}

/**
 * @brief Lee la cadena ingresada por el teclado, divide la cadena en una serie de tokens y se guardan en
 * palabras[] y devuelve la cantidad de divisiones obtenidas.
 * @param palabras Puntero arreglo a los argumentos obtenidos.
 * @param cadena Cadena ingresada por el usuario
 * @return Número de divisiones encontradas en la cadena.
 */
int parsear (char *palabras[], char *cadena){
  int count = 0;
  char *delimiters = " \n";
  palabras[0] = strtok (cadena, delimiters);
  for (count = 1; count < 3; count++)
    {
      palabras[count] = strtok (NULL, delimiters);
      //Encuentro null salgo
      if (palabras[count] == NULL)
        break;
    }
  return count;
}
/**
 * @brief Funcion encargada de mandar el mensaje pasado por paramentro por medio de la cola de mensaje.
 * 
 * @param sent_msg Mensaje a enviar
 */
void enviar_msg(mensaje_cli sent_msg){

    qd_cli = mq_open(QUEUEPATH, O_RDWR);
    if (qd_cli == -1){
        perror("Creating queue");
        exit(EXIT_FAILURE);
    }
    if (  mq_send(qd_cli, sent_msg.mensaje, strlen(sent_msg.mensaje) + 1, (unsigned int) 1) == -1){
        perror("Sending");
        exit(EXIT_FAILURE);
    }
}
