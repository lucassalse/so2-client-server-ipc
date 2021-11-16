#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <openssl/md5.h>



#define TAM 256
#define PORTFILE 5555


int comprobar_checksum(char*) ;
char* get_md5(char *) ;
void sig_handler() ;

int sockfd ;
/**
 * @brief Funcion main donde el cliente se encarga de generar un connect al server, si es aceptado recibe un
 * mensaje de ACK, envia otro para establecer la conexion y se queda esperando recibir y comprobar mensajes.
 * 
 * @return int 
 */
int main(  ) {
	int n;
	int flag_connect_accpet = 0 ;
	int recibiendo = 1 ;
	char buffer[TAM];
	struct sigaction ctrlc;
  	ctrlc.sa_handler = sig_handler;
  	sigemptyset(&ctrlc.sa_mask);
  	ctrlc.sa_flags = 0;

  	if(sigaction(SIGINT, &ctrlc, NULL) == -1){
      perror("signal");
      exit(EXIT_FAILURE);
    }
	
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        perror("No se ha podido crear el socket: "); 
        exit(EXIT_FAILURE); 
    }
    // Estructura
    struct sockaddr_in * servaddrfile = calloc(1, sizeof (struct sockaddr_in));
    servaddrfile->sin_family = AF_INET;
    servaddrfile->sin_port = htons(PORTFILE);
    servaddrfile->sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr *)  servaddrfile, sizeof (struct sockaddr)) != 0) { 
        perror("Conexion con el servicio de archivos incorrecta: "); 
        exit(EXIT_FAILURE);
    }

	while(1) {
		/*Conexion aceptado y verificado por medio de los ACK*/
		if (flag_connect_accpet){

			while(1){
				memset( buffer, '\0', TAM );
				n = (int) recv(sockfd, buffer, sizeof(buffer), 0) ;
				if(strstr(buffer, "log")){ // Corroboramos si lo que se esta por recivir es un log o no
					ssize_t receivedbyte ;
					int bytes_perdidos ;
					
					strtok(buffer, "|");
  					char* bytes        = strtok(NULL, "\n");		
					int bytes_int = atoi(bytes) ;
					bytes_perdidos = bytes_int;
					FILE *zip_recv = fopen("zip_recv", "w+b");
					if(zip_recv == NULL){
						perror("No se a podido abrir el archivo: ");
						exit(EXIT_FAILURE);
					}
					memset( buffer, '\0', TAM );
					while (1){
						receivedbyte = recv(sockfd, buffer, sizeof(buffer), 0);
						if(receivedbyte == -1){
							perror("Error en la recepcion de la iso: ");
							exit(EXIT_FAILURE);
						}
						fwrite(buffer, sizeof(char), (size_t) receivedbyte, zip_recv);
						bytes_perdidos -= (int) receivedbyte;
						if(bytes_perdidos == 0){
							break;
						}
					}
					fclose(zip_recv);

				}
				else{ // si es que es un mensaje de un productor por estar suscripto
					comprobar_checksum(buffer) ;
				}
			}

		}
		/*Una vez efectuada la conexion esperemos el ACK*/
		printf("Eseperando ACK del server \n") ;
		while(recibiendo){
			memset( buffer, '\0', TAM );
			n = (int) recv(sockfd, buffer, sizeof(buffer), 0) ;
			if(strcmp(buffer, "ACK") == 0){
				printf ("Recibi: %s \n", buffer) ;
				printf("Voy a mandar mi ACK \n") ;
				recibiendo = 0 ;
			}		
		}
		/*Una vez recibido el ACK enviamos nuestro ACK para corroborar la conexion*/
		memset( buffer, '\0', TAM );
		n = (int) send( sockfd, "ACK", 4, 0 );
		if ( n < 0 ) {
			perror( "escritura en socket" );
			exit( 1 );
		}

		flag_connect_accpet = 1 ;
	}
	return 0;
}
/**
 * @brief Funcion encargada de comprobar si el checksum recibido es correcto.
 * 
 * @param mensaje_recibido Mensaje recibido sobre el cual se calculara el nuevo checksum
 * @return int Si la combrobacion fue exitosa se devuelve 1 de lo contrario 0
 */
int comprobar_checksum(char* mensaje_recibido){

	char *mensaje = strtok(mensaje_recibido, "|");
  	char* checksum = strtok(NULL, "\n");

	if(!strcmp(get_md5(mensaje), checksum)){
      printf("Comprobacion exitosa.\n");
	  printf("El mensaje es: %s \n", mensaje) ;
	  return 1 ;
    }
 	else{
    	printf("Comprobacion fallida.\n");
		return 0 ;
	}
	return 0 ;
}
/**
 * @brief Se obtiene el Md5 del mensaje.
 * 
 * @param mensaje_a_convertir Mensaje sobre el cual se realiza el md5
 * @return char* Se devuelve el md5
 */
char* get_md5(char * mensaje_a_convertir){
    unsigned char c[MD5_DIGEST_LENGTH];
    MD5_CTX md_context;
    MD5_Init(&md_context);
    MD5_Update(&md_context, mensaje_a_convertir, strlen(mensaje_a_convertir)) ;
    MD5_Final(c, &md_context);

    char *md5 = malloc(MD5_DIGEST_LENGTH * 2 + 1);

    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(&md5[i*2], "%02x", (uint) c[i]);
    }
    return md5 ;

}
/**
 * @brief Funcion handler que envia mensaje de exit al server, por ser finalizado el cliente.
 * 
 */
void sig_handler(){
  printf("Voy a mandar un exit \n");
  int n = (int) send( sockfd, "exit", 5, 0 );
	if ( n < 0 ) {
		perror( "control-c" );
		exit( 1 );
	}
  exit(EXIT_SUCCESS);
}