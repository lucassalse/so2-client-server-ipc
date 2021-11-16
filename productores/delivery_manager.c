#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>


#define QUEUEPATH1 "/cola_mensaje1"
#define QUEUEPATH2 "/cola_mensaje2"
#define QUEUEPATH3 "/cola_mensaje3"
#define MSG_SIZE 150

mqd_t qda_prod1 ;
mqd_t qda_prod2 ;
mqd_t qda_prod3 ;
struct mq_attr queue_atributes10  = {0};
struct mq_attr queue_atributes2  = {0};
struct mq_attr queue_atributes3  = {0} ;

void crear_colas()         ;
long mensajes_encolados1() ; 
long mensajes_encolados2() ;
long mensajes_encolados3() ;

long mensajes_encolados10() ;

int main(){
   
    char recv_msg1[MSG_SIZE] ;
    char recv_msg2[MSG_SIZE] ;
    char recv_msg3[MSG_SIZE] ;
    unsigned int prio = 1;

    crear_colas() ;

    /*
    printf("El campo cola_1 mq_msgsize es: %ld \n",  mensajes_encolados1()  ) ;
    printf("El campo cola_1 mq_curmsgs es: %ld \n",  mensajes_encolados10() ) ;
    printf("El campo cola_2 mq_curmsgs es: %ld \n",  mensajes_encolados2() ) ;
    sleep(10) ;
    */



    //printf("El campo mq_msgsize es: %ld \n",  mensajes_encolados10()  ) ;
    
    //printf("El campo cola_1 mq_curmsgs es: %ld \n",  mensajes_encolados1() ) ;
    //printf("El campo cola_2 mq_curmsgs es: %ld \n",  mensajes_encolados2() ) ;

    while(1){

        if(mensajes_encolados10() > 0 ){
            if (mq_receive(qda_prod1, recv_msg1, MSG_SIZE, &prio) == -1 ){
                perror("Receiving");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", recv_msg1);
            bzero(recv_msg1, MSG_SIZE);
            printf("El campo cola1 mq_curmsgs es: %ld \n",  mensajes_encolados1() ) ;
        }

        if(mensajes_encolados2() > 0){
            if (mq_receive(qda_prod2, recv_msg2, MSG_SIZE, &prio) == -1 ){
                perror("Receiving");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", recv_msg2);
            bzero(recv_msg2, MSG_SIZE);
            printf("El campo cola_2 mq_curmsgs es: %ld \n",  mensajes_encolados2() ) ;
        }

        if(mensajes_encolados3() > 0){
            if (mq_receive(qda_prod3, recv_msg3, MSG_SIZE, &prio) == -1 ){
                perror("Receiving");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", recv_msg3);
            bzero(recv_msg3, MSG_SIZE);
            printf("El campo cola_3 mq_curmsgs es: %ld \n",  mensajes_encolados3() ) ;
        }
               
    }

}

void crear_colas(){

    queue_atributes10.mq_maxmsg  = 10 ;
    queue_atributes10.mq_msgsize = MSG_SIZE ;

    qda_prod1 = mq_open(QUEUEPATH1, O_RDWR | O_CREAT, 0666, &queue_atributes10) ;
    if (qda_prod1 == -1){
        perror("Error al crear/unirse a la cola en el servidor principal: ");
        exit(EXIT_FAILURE);
    }
    
    queue_atributes2.mq_maxmsg  = 10 ;
    queue_atributes2.mq_msgsize = MSG_SIZE ;
    qda_prod2 = mq_open(QUEUEPATH2, O_RDWR | O_CREAT, 0666, &queue_atributes2) ;
    if (qda_prod2 == -1){
        perror("Error al crear/unirse a la cola en el servidor principal: ");
        exit(EXIT_FAILURE);
    }

    queue_atributes3.mq_maxmsg  = 10 ;
    queue_atributes3.mq_msgsize = MSG_SIZE ;
    qda_prod3 = mq_open(QUEUEPATH3, O_RDWR | O_CREAT, 0666, &queue_atributes3) ;
    if (qda_prod3 == -1){
        perror("Error al crear/unirse a la cola en el servidor principal: ");
        exit(EXIT_FAILURE);
    }
}

long mensajes_encolados1(){

    if (mq_getattr(qda_prod1, &queue_atributes10) == -1){
        perror("Error de estructura cola 1 ");
        exit(EXIT_FAILURE);
    }

    return queue_atributes10.mq_curmsgs ;

}

long mensajes_encolados2(){

    if (mq_getattr(qda_prod2, &queue_atributes2) == -1){
        perror("Error de estructura cola 2 ");
        exit(EXIT_FAILURE);
    }

    return queue_atributes2.mq_curmsgs ;

}

long mensajes_encolados3(){

    if (mq_getattr(qda_prod3, &queue_atributes3) == -1){
        perror("Error de estructura cola 2 ");
        exit(EXIT_FAILURE);
    }

    return queue_atributes3.mq_curmsgs ;

}

long mensajes_encolados10(){

    if (mq_getattr(qda_prod1, &queue_atributes10) == -1){
        perror("Error de estructura cola 1 ");
        exit(EXIT_FAILURE);
    }

    return queue_atributes10.mq_msgsize ;

}