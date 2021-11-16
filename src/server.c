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
#include <arpa/inet.h>

#include "funciones.h"
#include <sys/resource.h>
#include <sys/time.h>

//#include "funciones.h"

typedef struct Estado_fd{
    int rd ;
    int wr ;
} Estado_fd;

#define TAM 256
#define PORT 5555
#define MAX_EVENTS 5100
#define BUFFER_SIZE 500
#define QUEUEPATH1 "/cola_mensaje1"
#define QUEUEPATH2 "/cola_mensaje2"
#define QUEUEPATH3 "/cola_mensaje3"
#define QUEUEPATHCLI "/cola_mensaje_cli"
#define MSG_SIZE 150


mqd_t qda_prod1 ;
mqd_t qda_prod2 ;
mqd_t qda_prod3 ;
mqd_t qda_cli ;
struct mq_attr queue_atributes10  = {0};
struct mq_attr queue_atributes2  = {0};
struct mq_attr queue_atributes3  = {0} ;
struct mq_attr queue_atributes_cli  = {0} ;
//int fd_set_blocking(int, int) ;
const Estado_fd FD_ESTADO_R  =   {.rd = 1, .wr = 0} ;
const Estado_fd FD_ESTADO_W  =   {.rd = 0, .wr = 1} ;
const Estado_fd FD_ESTADO_RW =   {.rd = 1, .wr = 1} ;
const Estado_fd FD_ESTADO_EXIT = {.rd = 0, .wr = 0} ;



Estado_fd cliente_listo_rcv(int) ;
Estado_fd client_conecction(int , struct sockaddr_in*, Lista*) ;
void crear_colas() ;
char* conver_addres (struct sockaddr_in*, char*) ;
void add_fds(struct epoll_event, struct epoll_event, int) ;

int epollfd ;
/**
 * @brief Funcion main en donde se aceptan las conexiones porovenientes de los clientes, como asi tambien los mensajes obtenidos
 * mediante las colas de mensajes por parte de los productres como de la cli. Todo manejado mediante epoll y flie descritpors asociados.
 * 
 * @return int 
 */
int main( ) {

    struct rlimit limits = {.rlim_cur = 5100, .rlim_max = 6000} ;
    
    setrlimit(RLIMIT_NOFILE, &limits) ;
    
    int sockfd ;
    int n;
    int conn_sock, nfds ;
    struct epoll_event ev, events[MAX_EVENTS], even_mq;

    char recv_msg1[MSG_SIZE] ;
    char recv_msg2[MSG_SIZE] ;
    char recv_msg3[MSG_SIZE] ;
    char recv_msg_cli[MSG_SIZE] ;
    unsigned int prio = 1;

    Lista clientes ;
    clientes.cabeza = NULL ;
    clientes.longitud = 0 ;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) { 
        perror("socket creation failed...\n"); 
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
    }

   // Estructura del server
    struct sockaddr_in* server = calloc(1, sizeof (struct sockaddr_in));
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT);
    server->sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr *) server, sizeof (struct sockaddr)) == -1){
        perror("Error en bind: ");
        exit(EXIT_FAILURE);
    }

    printf( "Proceso: %d - socket disponible: %d\n", getpid(), ntohs(server->sin_port) );

    if(listen(sockfd, 5100) == -1){ // 1024
        perror("Error en listen: ");
        exit(EXIT_FAILURE);
    }

    crear_colas() ;

    epollfd = epoll_create1(0);
    if (epollfd == -1){
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    add_fds(ev, even_mq, sockfd) ;
   
	while( 1 ) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
            if (nfds == -1) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
        for (n = 0; n < nfds; ++n) {
            if(events[n].events == EPOLLERR){ 
                perror("EPOLLERR \n") ;
                close(epollfd) ;
                return 0 ;
            }
            // Vemos si se produjo un evento en el sockfd para poder aceptar la conexion
            if (events[n].data.fd == sockfd){
                printf("Se produjo una conexion con un cliente \n") ;
                // Estructura  del cliente
                struct sockaddr_in* client = calloc(1, sizeof (struct sockaddr_in));
                int32_t lenght_client = (int32_t) sizeof (struct sockaddr_in);

                conn_sock = accept(sockfd, (struct sockaddr *) client, (socklen_t *) &lenght_client);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                if (conn_sock > MAX_EVENTS){
                    perror ("fd mayores a la cantidad maxima") ;
                }
                printf("Se creo el conn_sock correctamente \n") ;
                printf("El fd del server es: %d y el del connect es: %d\n", sockfd, conn_sock) ;
                printf("El valor del puerto del cliente es: %d \n", ntohs(client->sin_port) ) ;
                printf("El valor del ip del cliente es: %s \n", inet_ntoa(client->sin_addr) ) ;

                Estado_fd estado = client_conecction(conn_sock, client, &clientes) ;
                struct epoll_event event ;
                event.data.fd = conn_sock ;
                if (estado.rd){
                    event.events = EPOLLIN ;
                }
                if (estado.wr){
                    event.events = EPOLLOUT ;
                }
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &event) == -1){
                    close(epollfd) ;
                    close(conn_sock) ;
                    close(sockfd) ;
                    perror("epoll_ctl: conn_sock nuevo event");
                    exit(EXIT_FAILURE);
                }
            }
            // Si es fd prod1
            if(events[n].data.fd == qda_prod1){
                if (mq_receive(qda_prod1, recv_msg1, MSG_SIZE, &prio) == -1 ){
                    perror("Receiving");
                    exit(EXIT_FAILURE);
                }
                recorrer_mostar(&clientes, recv_msg1) ;
                bzero(recv_msg1, MSG_SIZE);
            }
            // Si es fd prod2
            if(events[n].data.fd == qda_prod2){
                if (mq_receive(qda_prod2, recv_msg2, MSG_SIZE, &prio) == -1 ){
                    perror("Receiving");
                    exit(EXIT_FAILURE);
                }
                recorrer_mostar2(&clientes, recv_msg2) ;
                bzero(recv_msg1, MSG_SIZE);
            }
            // Si es fd prod3
            if(events[n].data.fd == qda_prod3){
                if (mq_receive(qda_prod3, recv_msg3, MSG_SIZE, &prio) == -1 ){
                    perror("Receiving");
                    exit(EXIT_FAILURE);
                }
                recorrer_mostar3(&clientes, recv_msg3) ;
                bzero(recv_msg1, MSG_SIZE);
            }
            // Si es fd cli      
            if(events[n].data.fd == qda_cli){
                if (mq_receive(qda_cli, recv_msg_cli, MSG_SIZE, &prio) == -1 ){
                    perror("Receiving");
                    exit(EXIT_FAILURE);
                }
                recorrer_comandos(&clientes, recv_msg_cli) ;
                bzero(recv_msg_cli, MSG_SIZE);
            }
            // Si no fue el socket de la conexion o las colas fueron los socket de los clientes
            if(events[n].events == EPOLLIN && events[n].data.fd != sockfd && events[n].data.fd != qda_prod1
                 && events[n].data.fd != qda_prod2 && events[n].data.fd != qda_prod3 && events[n].data.fd != qda_cli){
                
                int fd = events[n].data.fd ; 
                Estado_fd estado = cliente_listo_rcv(fd) ;
                struct epoll_event event ;
                event.data.fd = fd ;
                if(estado.rd){
                    event.events = EPOLLIN ;
                }
                if(estado.wr){
                    event.events = EPOLLOUT ;
                }

                if(estado.rd == 0 && estado.wr == 0){ // En caso que se desconecte un cliente
                    event.events = 0 ;
                }
                if (event.events == 0){
                    printf ("Socket %d closes \n", fd) ;
                    eliminar_cliente(&clientes, fd) ;
                    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1){
                        perror("EPOLL_CTL_DEL") ;
                    }
                    close(fd) ;
                }
                else{
                     if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event) == -1){
                        perror("EPOLL_CTL_MOD") ;
                    }
                }
            }                             
        }
	}
	return 0; 
} 




/** 
 * Set a file descriptor to blocking or non-blocking mode.
 *
 * @param fd The file descriptor
 * @param blocking 0:non-blocking mode, 1:blocking mode
 *
 * @return 1:success, 0:failure.
 **/

int fd_set_blocking(int fd, int blocking) {
    // Save the current flags
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return 0;

    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags) != -1;
}
/**
 * @brief Se crea un nuevo cliente dado a que sea aceptado una nueva conexion con el server. Ademas se envia un ACK
 * 
 * @param conn_sock fd de la nueva conexion
 * @param client Datos del cliente de la nueva conexion
 * @param lista Lista de clientes donde se almacenara la nueva conexion
 * @return Estado_fd Se retorna un estado correspondiente si el cliente se encontrara asociado a algun evento
 */

Estado_fd client_conecction(int conn_sock, struct sockaddr_in* client, Lista* lista){


    char a [100] ;
    int id = ntohs(client-> sin_port) ;
    Cliente cliente ;

    cliente.id = id ;
    cliente.fd = conn_sock ;
    cliente.prod1 = 0 ;
    cliente.prod2 = 0 ;
    cliente.prod3 = 0 ;
    strcpy(cliente.direccion_puerto, conver_addres(client, a)) ;
    printf("El cliente tiene su direccion_puerto: %s \n", cliente.direccion_puerto) ;
    insertar_final(lista, &cliente) ;
    int m = (int) send( conn_sock, "ACK", 3, 0) ;
    if ( m < 0 ){
        perror( "No se ha podido enviar un mensaje al cliente: " );
    }
    return FD_ESTADO_R ;
} 
/**
 * @brief Funcion encargada de obtener una respuesta de los clientes, en caso de ser un ACK la conexion fue exitosa y
 *        se encunetra establecida lista para ser asociada a algun productor.
 * 
 * @param fd file descriptor del cliente que se comunico
 * @return Estado_fd Se retorna un estado correspondiente si el cliente se encontrara asociado a algun evento
 */

Estado_fd cliente_listo_rcv(int fd){
    char buffer [BUFFER_SIZE] ;
    recv(fd, buffer, sizeof(buffer), 0) ;

    if(strcmp(buffer, "exit") == 0){
        printf ("El cliente se cerro \n") ;
        return FD_ESTADO_EXIT ;
    }
    if (strcmp(buffer, "ACK") == 0){
        printf ("Recibi: %s \n", buffer) ;
        return FD_ESTADO_R ;
    }
    else{ // No hubo conexion se espera el ACK
        return FD_ESTADO_R ;
    }
    return FD_ESTADO_R ;
}
/**
 * @brief Funcion encargada de crear las colas de mensajes con los respectivos productores y la cli.
 * 
 */

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

    queue_atributes_cli.mq_maxmsg  = 10 ;
    queue_atributes_cli.mq_msgsize = MSG_SIZE ;
    qda_cli = mq_open(QUEUEPATHCLI, O_RDWR | O_CREAT, 0666, &queue_atributes_cli) ;
    if (qda_cli == -1){
        perror("Error al crear/unirse a la cola en el servidor principal: ");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Funcion que convierte la direccion ip y puerto del cliente en un solo formato para ser almacenada en la lista.
 * 
 * @param client Cliente sobre el que se obtienen los datos de su direccion y puerto
 * @param a Arreglo de char que obtendra el formato correspondiente de ip:puerto
 * @return char* Se retorno un arreglo de char con el formato ip:puerto
 */
char* conver_addres (struct sockaddr_in* client, char* a){
    int myInteger = ntohs(client->sin_port);
    char* addres_port = inet_ntoa(client->sin_addr) ; 
    sprintf(a, "%s:%d", addres_port, myInteger); 

    return a ;
}

/**
 * @brief Funcion que se encarga de agregar los fd tanto del socket como de las colas de mensaje al contro epoll
 * 
 * @param ev Estructura de evento para el fd del socket
 * @param even_mq Estructura de evento para los fd de las colas de mensaje
 * @param sockfd fd del socket
 */
void add_fds(struct epoll_event ev, struct epoll_event even_mq, int sockfd){

 // Se agregan los flie descriptor del socket y de las colas de mensaje
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
            perror("epoll_ctl: listen_sock");
            exit(EXIT_FAILURE);
        }
    
    even_mq.events = EPOLLIN ;
    even_mq.data.fd = qda_prod1 ;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, qda_prod1, &even_mq) == -1) {
            perror("epoll_ctl_ADD: mq_1");
            exit(EXIT_FAILURE);
        }
    
    even_mq.events = EPOLLIN ; 
    even_mq.data.fd = qda_prod2 ;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, qda_prod2, &even_mq) == -1) {
            perror("epoll_ctl_ADD: mq_2");
            exit(EXIT_FAILURE);
        }
    
    even_mq.events = EPOLLIN ; 
    even_mq.data.fd = qda_prod3 ;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, qda_prod3, &even_mq) == -1) {
            perror("epoll_ctl_ADD: mq_3");
            exit(EXIT_FAILURE);
        }
    
    even_mq.events = EPOLLIN ;
    even_mq.data.fd = qda_cli ;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, qda_cli, &even_mq) == -1) {
            perror("epoll_ctl_ADD: qda_cli");
            exit(EXIT_FAILURE);
        }
}
