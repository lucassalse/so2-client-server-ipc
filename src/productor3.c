#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

#define QUEUEPATH "/cola_mensaje3"
#define TIME_SLEEP 4

typedef struct {
    char *mensaje  ; 
} mensaje_prod3;

mqd_t qd_prod3 ;

int get_jiffies_init () ;
int get_work_jiffies_init () ;
float load_system (int*, int*, int*, int*) ;
char* conver_intTochar (char*, float* )    ;
void enviar_msg(mensaje_prod3) ;

int main (){

    char mensaje  [100] ;
    char numero   [100] ;
    mensaje_prod3 mensaje_enviar ;

    int total_jiffies_init ;
    int work_jiffies_init ; 
    int total_jiffies_current ;
    int work_jiffies_current ;

    float carga ;
    
    total_jiffies_init = get_jiffies_init () ;
    work_jiffies_init = get_work_jiffies_init() ;

    //printf("El valor de total_jiffies_init en el main es: %d \n", total_jiffies_init) ;
    //printf("El valor de work_jiffies_init en el main es: %d \n", work_jiffies_init) ;

    while(1){
        sleep(TIME_SLEEP) ;
        total_jiffies_current = get_jiffies_init() ;
        work_jiffies_current = get_work_jiffies_init() ;

        //printf("El valor de total_jiffies_current en el main es: %d \n", total_jiffies_current) ;
        //printf("El valor de work_jiffies_current en el main es: %d \n", work_jiffies_current) ;

        carga = load_system (&work_jiffies_current, &work_jiffies_init, &total_jiffies_current, &total_jiffies_init) ;
        printf ("La carga del sistema es: %f \n", carga) ;

        memset(mensaje, '\0', sizeof(mensaje));
        strcpy(mensaje, "La carga del sistema normalizado es: ") ;
        strcat(mensaje, conver_intTochar (numero, &carga)) ;
        mensaje_enviar.mensaje = mensaje ;
        printf ("El mensaje a enviar es: %s \n", mensaje_enviar.mensaje) ;
        enviar_msg(mensaje_enviar) ;
    }

    return 0 ;
}
/**
 * @brief Funcion que se encarga de obtener los juffies iniciales para el calculo de la carga de cpu
 * 
 * @return int Devuelve el valor de los juffies iniciales
 */
int get_jiffies_init (){

    int valor1 ;
    int valor2 ;
    int valor3 ;
    int valor4 ;
    int valor5 ;
    int valor6 ;
    int valor7 ;
    int valor8 ;
    int valor9 ;
    int valor10 ;

    int total_jiffies_init ;

    FILE* archivo ;
    char buffer[100] ;
    char* linea ;

    archivo = fopen("/proc/stat", "r") ;
    if(archivo == NULL){
        perror("No se a podido abrir el archivo: ");
        return 0;
    }

    while(feof(archivo) == 0){
        fgets(buffer, 100, archivo) ;
        linea = strstr(buffer, "cpu") ;

        if(linea != NULL){
            break ;
        }
    }

    fclose(archivo) ;

    sscanf(linea, "cpu %d %d %d %d %d %d %d %d %d %d", &valor1, &valor2, &valor3, &valor4, &valor5, &valor6, &valor7, &valor8, &valor9, &valor10) ;

    total_jiffies_init = valor1 + valor2 + valor3 + valor4 + valor5 +valor6 +valor7 + valor8 + valor9 + valor10 ;
    //printf("La linea es: %s \n", linea) ;
    return total_jiffies_init ;

}
/**
 * @brief Funcion que obitene los jiffies iniciales (ultimos 3 valores)
 * 
 * @return int Devuelve el calculo de los jiffies
 */
int get_work_jiffies_init (){

    int valor1 ;
    int valor2 ;
    int valor3 ;

    int work_jiffies_init ; 

    FILE* archivo ;
    char buffer[100] ;
    char* linea ;

    archivo = fopen("/proc/stat", "r") ;
    if(archivo == NULL){
        perror("No se a podido abrir el archivo: ");
        return 0;
    }

    while(feof(archivo) == 0){
        fgets(buffer, 100, archivo) ;
        linea = strstr(buffer, "cpu") ;

        if(linea != NULL){
            break ;
        }
    }

    fclose(archivo) ;

    sscanf(linea, "cpu %d %d %d", &valor1, &valor2, &valor3) ;
    work_jiffies_init  = valor1 + valor2 + valor3 ;

    return work_jiffies_init ;
}
/**
 * @brief Funcion que se encarga de obtener el formato correspondiente para ser enviado
 * 
 * @param a Variable donde almacenamos el mensaje con formato
 * @param valor Valor de la carga obtenida
 * @return char* Formato final del mensaje a enviar
 */
char* conver_intTochar (char* a, float* valor){ 
    sprintf( a, "%.2f %s", *valor, "%" ); 
    //printf( "a es: %s \n", a ); 
    return a ;

}
/**
 * @brief Funcion encargada de obtener la la carga del sistema
 * 
 * @param work_jiffies_current 
 * @param work_jiffies_init 
 * @param total_jiffies_current 
 * @param total_jiffies_init 
 * @return float Se retorna la carga del sistema
 */
float load_system (int* work_jiffies_current, int* work_jiffies_init, int* total_jiffies_current, int* total_jiffies_init){

    float work_over_period ;
    float total_over_period ;
    float load_system ;

    work_over_period = (float)  *work_jiffies_current -  (float) *work_jiffies_init ;
    total_over_period = (float) *total_jiffies_current - (float) *total_jiffies_init ;

    load_system = (work_over_period / total_over_period) * 100 ;

    //printf ("La carga del sistema es: %f \n", load_system) ;
    return load_system ;



}
/**
 * @brief Funcion encargada de enviar el mensaje de estre productor (carga del sistema) por medio de la cola de mensaje
 * 
 * @param sent_msg Mensaje a enviar
 */
void enviar_msg(mensaje_prod3 sent_msg){

    qd_prod3 = mq_open(QUEUEPATH, O_RDWR);
    if (qd_prod3 == -1){
        perror("Creating queue");
        exit(EXIT_FAILURE);
    }

    if (  mq_send(qd_prod3, sent_msg.mensaje, strlen(sent_msg.mensaje) + 1, (unsigned int) 1) == -1){
        perror("Sending");
        exit(EXIT_FAILURE);
    }

}