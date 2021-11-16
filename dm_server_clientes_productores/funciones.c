#include "funciones.h"

/**
 * @brief Funcion que crea un nuevo nodo para la lista con los parametros de su argumento.
 * 
 * @param cliente Parametro puntero Cliente con el cual se va a crear el nuevo nodo
 * @return Nodo* Devuelve el nodo creado con sus correspondientes paramentros
 */
Nodo* crear_nodo (Cliente* cliente){
    Nodo* nodo = (Nodo*) malloc (sizeof(Nodo)) ;
    nodo->cliente.id = cliente->id ;
    nodo->cliente.fd = cliente->fd ;
    nodo->cliente.prod1 = cliente->prod1 ;
    nodo->cliente.prod2 = cliente->prod2 ;
    nodo->cliente.prod3 = cliente->prod3 ;
    strcpy(nodo->cliente.direccion_puerto, cliente->direccion_puerto) ;
    nodo->siguiente = NULL ;
    return nodo ;
}

/**
 * @brief Funcion que elimina un nodo de la lista mediante un free()
 * 
 * @param nodo Nodo a eliminar
 */
void eliminar_nodo(Nodo* nodo){
    free (nodo) ;
}
/**
 * @brief Funcion que inserta un nodo al principio de la lista
 * 
 * @param lista Lista que contiene los clientes
 * @param cliente Cliente a ser insertado en la lista
 */
void insertar_principio(Lista* lista, Cliente* cliente){
    Nodo* nodo = crear_nodo(cliente) ;
    nodo->siguiente = lista->cabeza ;
    lista->cabeza = nodo ;
    lista->longitud++ ;
}
/**
 * @brief Funcion que inserta un nodo al final de la lista
 * 
 * @param lista Lista que contiene clientes donde se insertara el nuevo
 * @param cliente Cliente a ser insertado en la lista
 */
void insertar_final(Lista* lista, Cliente* cliente){
    Nodo* nodo = crear_nodo(cliente) ;
    if (lista->cabeza == NULL){
        lista->cabeza = nodo ;
    }
    else{
        Nodo* puntero = lista->cabeza ;
        while (puntero->siguiente){
            puntero = puntero->siguiente ;
        }
        puntero->siguiente = nodo ;
    }
    lista->longitud++ ;
}
/**
 * @brief Funcion encargada de mantener la cantidad de elementos en la lista
 * 
 * @param lista Lista de elementos
 * @return int Cantidad de elementos en la lista
 */
int contar(Lista* lista){
    return lista->longitud ;
}
/**
 * @brief Funcion que elimina el primer elemento de la lista
 * 
 * @param lista Lista de elementos
 */
void eliminar_principio(Lista* lista){
    if(lista->cabeza){
        Nodo* eliminado = lista->cabeza ;
        lista->cabeza = lista->cabeza->siguiente ;
        eliminar_nodo(eliminado) ;
        lista->longitud-- ;
    }
}
/**
 * @brief Funcion que me permite saber si la lista esta vacia
 * 
 * @param lista Lista de elementos
 * @return int 
 */
int esta_vacia(Lista* lista){
    return lista->cabeza == NULL ;
}
/**
 * @brief Funcion que se encarga de eliminar el ultimo elemento de la lista
 * 
 * @param lista Lista de elementos
 */
void eliminar_ultimo(Lista* lista){
    if (lista->cabeza){
        if(lista->cabeza->siguiente){
            Nodo* puntero = lista->cabeza ;
            while(puntero->siguiente->siguiente){
                puntero = puntero->siguiente ;
            }
            Nodo* eliminado = puntero->siguiente ;
            puntero->siguiente = NULL ;
            eliminar_nodo(eliminado) ;
            lista->longitud-- ;
        }
        else{
            Nodo* eliminado = lista->cabeza ;
            lista->cabeza = NULL ;
            eliminar_nodo (eliminado) ;
            lista->longitud-- ;
        }
    }
}
/**
 * @brief Funcion encargada de eliminar un cliente en especifico de la lista
 * 
 * @param lista Lista de clientes
 * @param fd fd del cliente que se desea eliminar
 */
void eliminar_cliente(Lista* lista, int fd){
    if (lista->cabeza != NULL){
        Nodo* aux_borrar ;
        Nodo* anterior = NULL ;
        aux_borrar = lista->cabeza ;
    
        while((aux_borrar != NULL) && (aux_borrar->cliente.fd != fd) ){
            anterior = aux_borrar ;
            aux_borrar = aux_borrar->siguiente ;
        }
        if (aux_borrar == NULL){
            printf("No existe el elemento en la lista \n") ;
        }
        else if(anterior == NULL){
            lista->cabeza = lista->cabeza->siguiente ;
            eliminar_nodo (aux_borrar) ;
            lista->longitud-- ;
        }
        else{
            anterior->siguiente = aux_borrar->siguiente ;
            eliminar_nodo (aux_borrar) ;
            lista->longitud -- ;
        }
    }
}
/**
 * @brief Funcion encargada de recorrer la lista y enviar a todos los clientes suscriptos el mensaje del productor1
 * 
 * @param lista Lista de clientes
 * @param mensaje Mensaje a enviar
 */
void recorrer_mostar(Lista* lista, char* mensaje){
    Nodo* puntero = lista->cabeza ;
    while(puntero != NULL){   
        //mostrar_nodo(puntero) ;
        send_mensaje(puntero, mensaje) ;
        puntero = puntero->siguiente ;

    }
}
/**
 * @brief Funcion encargada de recorrer la lista y enviar a todos los clientes suscriptos el mensaje del productor2
 * 
 * @param lista Lista de clientes
 * @param mensaje Mensaje a enviar
 */
void recorrer_mostar2(Lista* lista, char* mensaje){
    Nodo* puntero = lista->cabeza ;
    while(puntero != NULL){
        
        //mostrar_nodo(puntero) ;
        send_mensaje2(puntero, mensaje) ;
        puntero = puntero->siguiente ;

    }
}
/**
 * @brief Funcion encargada de recorrer la lista y enviar a todos los clientes suscriptos el mensaje del productor3
 * 
 * @param lista Lista de clientes
 * @param mensaje Mensaje a enviar
 */
void recorrer_mostar3(Lista* lista, char* mensaje){
    Nodo* puntero = lista->cabeza ;
    while(puntero != NULL){
        
        //mostrar_nodo(puntero) ;
        send_mensaje3(puntero, mensaje) ;
        puntero = puntero->siguiente ;

    }
}
/**
 * @brief Funcion encargada de eliminar solamente un nodo de la lista
 * 
 * @param lista Lista de clientes
 * @param fd fd que se desea eliminar
 */
void recorrer_eliminar(Lista* lista, int fd){
    Nodo* puntero = lista->cabeza ;
    while(puntero != NULL){
        
        if(puntero->cliente.fd == fd){
            eliminar_nodo (puntero) ;
        }
        puntero = puntero->siguiente ;
    }
}
/**
 * @brief Funcion que me permite obtener mediante impresion de consola los atributos de un determinado nodo
 * 
 * @param nodo Nodo que sea desea imprimir (Obtener sus argumentos)
 */
void mostrar_nodo(Nodo* nodo){
    printf("La id del cliente es: %d \n", nodo->cliente.id) ;
    printf("El fd del cliente es: %d \n", nodo->cliente.fd) ;
    printf("La falg productor1 es: %d \n", nodo->cliente.prod1) ;
    printf("La falg productor2 es: %d \n", nodo->cliente.prod2) ;
    printf("La falg productor3 es: %d \n", nodo->cliente.prod3) ;
}
/**
 * @brief Funcion que verifica y envia el mensaje del productor1 a los clientes que corresponda
 * 
 * @param nodo Nodo que posee datos del cliente para determinar si recibira el envio
 * @param mensaje Mensaje a enviar
 */
void send_mensaje(Nodo* nodo, char* mensaje){
    if (nodo->cliente.prod1 == 1){

        char a[500] ;
        int m = (int) send( nodo->cliente.fd, get_menasje_md5(mensaje, a), strlen(get_menasje_md5(mensaje, a)), 0) ;
        if ( m < 0 ){
            perror( "No se ha podido enviar un mensaje al cliente: " );
        }
        escribir_log(nodo, mensaje, 1) ;
    }

}
/**
 * @brief Funcion que verifica y envia el mensaje del productor2 a los clientes que corresponda
 * 
 * @param nodo Nodo cliente
 * @param mensaje Mensaje a enviar
 */
void send_mensaje2(Nodo* nodo, char* mensaje){
    if (nodo->cliente.prod2 == 1){
        char a[500] ;
        int m = (int) send( nodo->cliente.fd, get_menasje_md5(mensaje, a), strlen(get_menasje_md5(mensaje, a)), 0) ;
        if ( m < 0 ){
            perror( "No se ha podido enviar un mensaje al cliente: " );
        }
        escribir_log(nodo, mensaje, 2) ;
    }
}
/**
 * @brief Funcion que verifica y envia el mensaje del productor3 a los clientes que corresponda
 * 
 * @param nodo Nodo cliente
 * @param mensaje Mensaje a enviar
 */
void send_mensaje3(Nodo* nodo, char* mensaje){
    if (nodo->cliente.prod3 == 1){
        char a[500] ;
        int m = (int) send( nodo->cliente.fd, get_menasje_md5(mensaje, a), strlen(get_menasje_md5(mensaje, a)), 0) ;
        if ( m < 0 ){
            perror( "No se ha podido enviar un mensaje al cliente: " );
        }
        escribir_log(nodo, mensaje, 3) ;
    }
}
/**
 * @brief Funcion que recorre y determina el comando obtenido por la cli mediante la cola de mensajes, ejecutando su orden
 * 
 * @param lista Lista de clientes
 * @param mensaje Mensaje recibido por la cli
 */
void recorrer_comandos(Lista* lista, char* mensaje){
    Nodo* puntero = lista->cabeza ;
    while(puntero != NULL){      
        if(set_comandos(puntero, mensaje, lista)){
            break ;
        }
        puntero = puntero->siguiente ;
    }
}
/**
 * @brief Setea los clientes con algun productor, de manera correspondiente
 * 
 * @param nodo Nodo cliente
 * @param mensaje Mensaje obtenido por la cli
 */
int set_comandos(Nodo* nodo, char* mensaje, Lista* lista){
    
    //printf("El valor antes de copiar del nodo es: %s \n", nodo->cliente.direccion_puerto) ;
    if(strstr(mensaje, "add")){
        if(strstr( mensaje, nodo->cliente.direccion_puerto) ){
            int flag_productor = flagProductor(mensaje) ;
            switch (flag_productor)
            {
            case 1:
                nodo->cliente.prod1 = 1 ;
                escribir_log(nodo, mensaje, 4) ;
                break;
            case 2:
                nodo->cliente.prod2 = 1 ;
                escribir_log(nodo, mensaje, 5) ;
                break ;
            case 3:
                nodo->cliente.prod3 = 1 ;
                escribir_log(nodo, mensaje, 6) ;
                break ;
            default:
                break;
            }
        }
        else{
            //printf("El cliente no se encontro en la lista para hacer el add \n") ;
        }


    }
    if(strstr(mensaje, "delete")){
        if(strstr( mensaje, nodo->cliente.direccion_puerto) ){
            int flag_productor = flagProductor(mensaje) ;
            switch (flag_productor){
            case 1:
                nodo->cliente.prod1 = 0 ;
                escribir_log(nodo, mensaje, 7) ;
                break;
            case 2:
                nodo->cliente.prod2 = 0 ;
                escribir_log(nodo, mensaje, 8) ;
                break ;
            case 3:
                nodo->cliente.prod3 = 0 ;
                escribir_log(nodo, mensaje, 9) ;
                break ;
            default:
                break;
            }

        }
        else{
            //printf("El cliente no se encontro en la lista para hacer el delete \n") ;
        }

    }
    if(strstr(mensaje, "log")){
        if(strstr( mensaje, nodo->cliente.direccion_puerto) ){
            comprimir_log() ;
            size_t bytes = (size_t) bytes_envio() ; // Obtenemos los bytes del zip
            char buffer [100] ;
            sprintf(buffer,"%s|%ld", "log", bytes ) ;
            int m = (int) send( nodo->cliente.fd, buffer, strlen(buffer), 0) ;
            if ( m < 0 ){
            perror( "No se ha podido enviar un mensaje al cliente: " );
            }
            send_log(nodo) ;
            
        }
        else{
            //printf("El cliente no se encontro en la lista para hacer el log \n") ;
        }

    }
    if(strstr(mensaje, "listar")){
        
        recorrer_listar(lista) ;

    }
     if(strstr(mensaje, "cantidad")){
        
        printf("Cantidad de clientes conectados: %d \n", lista->longitud) ;
        return 1 ;
    }
    return 0 ;
}
/**
 * @brief Funcion que obtiene un int el cual representa a cual productor asociar a un cliente.
 * 
 * @param mensaje Mensaje obtenido por la cli
 * @return int Productor al que se debe asociar un cliente. 1 = productor1; 2 = productor2; 3 = productor3
 */
int flagProductor(char* mensaje){
    
    if (strstr(mensaje,"productor1")){
        return 1 ;
    }
     if (strstr(mensaje,"productor2")){
        return 2 ;
    }
     if (strstr(mensaje,"productor3")){
        return 3 ;
    }
    return 0 ;


}
/**
 * @brief Funcion encargada de escribir un archvio log con los sucesos correspondientes
 * 
 * @param nodo Nodo cliente (Posee la informacion del cliente de destino)
 * @param mensaje Mensaje a ser enviado a un cliente
 * @param productor_flag Determina que productor fue el que envio el mensaje
 */
void escribir_log (Nodo* nodo, char* mensaje, int productor_flag){

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char datetime[100];
    strftime(datetime, sizeof(datetime)-1, "%d %m %Y %H:%M", t);

    FILE * flujo = fopen("log.txt", "a") ;
    if(flujo == NULL){
        perror("Error en la escritura del log \n") ;
    }
    else{

        switch (productor_flag)
        {
        case 1:
            fprintf(flujo, "%s El mensaje a enviar es: %s   Desde productor1 a %s \n", datetime, mensaje, nodo->cliente.direccion_puerto) ;
            break;
        case 2:
            fprintf(flujo, "%s El mensaje a enviar es: %s   Desde productor2 a %s \n", datetime, mensaje, nodo->cliente.direccion_puerto) ;
            break ;
        case 3:
            fprintf(flujo, "%s El mensaje a enviar es: %s   Desde productor3 a %s \n", datetime, mensaje, nodo->cliente.direccion_puerto) ;
            break ;
        case 4:
            fprintf(flujo, "%s Se agrega el suscriptor %s al productor1 de manera exitosa \n", datetime, nodo->cliente.direccion_puerto) ;
            break ;
        case 5:
            fprintf(flujo, "%s Se agrega el suscriptor %s al productor2 de manera exitosa \n", datetime, nodo->cliente.direccion_puerto) ;
            break ;
        case 6:
            fprintf(flujo, "%s Se agrega el suscriptor %s al productor3 de manera exitosa \n", datetime, nodo->cliente.direccion_puerto) ;
            break ;
        case 7:
            fprintf(flujo, "%s Se elimina el suscriptor %s del productor1 de manera exitosa \n", datetime, nodo->cliente.direccion_puerto) ;
            break ;
        case 8:
            fprintf(flujo, "%s Se elimina el suscriptor %s del productor2 de manera exitosa \n", datetime, nodo->cliente.direccion_puerto) ;
            break ;
        case 9:
            fprintf(flujo, "%s Se elimina el suscriptor %s del productor3 de manera exitosa \n", datetime, nodo->cliente.direccion_puerto) ;
            break ;
        default:
            break;
        }
    }
    fflush (flujo);
    fclose(flujo) ;


}
/**
 * @brief Obtiene el hash mediante md5 de un mensaje
 * 
 * @param mensaje_a_convertir Mensaje a convertir mediante md5
 * @param a Valor donde se guardara el formato correspondiente pedido <mensaje>|chechsum
 * @return char* Se retorna el formato correspondiente de mensaje y checksum
 */
char* get_menasje_md5(char * mensaje_a_convertir, char *a){
    unsigned char c[MD5_DIGEST_LENGTH];
    MD5_CTX md_context;

    //char a[250] ;

    MD5_Init(&md_context);
    MD5_Update(&md_context, mensaje_a_convertir, strlen(mensaje_a_convertir)) ;
    MD5_Final(c, &md_context);

    char *md5 = malloc(MD5_DIGEST_LENGTH * 2 + 1);

    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(&md5[i*2], "%02x", (uint) c[i]);
    }
    sprintf(a, "%s|%s", mensaje_a_convertir, md5);

    return a ;
}

/**
 * @brief Funcion encargada de recorrer y listar todos los clientes conectados con sus atributos
 * 
 * @param lista Lista de clientes
 */
void recorrer_listar(Lista* lista){
    Nodo* puntero = lista->cabeza ;
    while(puntero != NULL){   
        mostrar_nodo(puntero) ;
        puntero = puntero->siguiente ;
    }
    printf("Cantidad de clientes conectados: %d \n", lista->longitud) ;
}

/**
 * @brief Funcion encargada de comprimir el log del server
 * 
 */
void comprimir_log(){
    int errorp;
    /* creamos el zip file */
    struct zip *zip_file = zip_open("log_log.zip", ZIP_CREATE | ZIP_TRUNCATE, &errorp);
    if(zip_file == NULL) {
        printf("error creating zip file %i\n", errorp);
    }

    int fd = open("log.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
    }

    off_t bytes = lseek(fd, 0, SEEK_END);
    if (bytes == -1) {
        perror("lseek");
    }
    char logs_buf[bytes];

    if(lseek(fd, 0, SEEK_SET) == -1) { 
        perror("lseek");
    }

    if (read(fd, logs_buf, (size_t) bytes) == -1){ 
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("close");
    }

    zip_source_t *zip_source = zip_source_buffer(zip_file,logs_buf, (zip_uint64_t) bytes, 0);
    if(zip_source == NULL) {
        printf("error when creating zip data source from buffer");
    }

    if (zip_file_add(zip_file,"log.txt",zip_source,ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) == -1) {
        printf("error when adding file to zip\n");
    }

    if (zip_close(zip_file) == -1) {
        printf("error when closing zip archive");
    }

}
/**
 * @brief Funcion encargada de enviar al cliente correspondiete el archivo log comprimido
 * 
 * @param nodo Nodo que posee la informacion del cliente al cual enviar
 */
void send_log(Nodo* nodo){
 
  //printf("%s \n", "  DM: Sending log...");

  int logfd = open("log_log.zip", O_RDONLY);
    if (logfd == -1) {
        perror("open log_log.zip");
    }

    off_t bytes = lseek(logfd, 0, SEEK_END);
    if (bytes == -1) {
        perror("lseek");
    }

    if(lseek(logfd, 0, SEEK_SET) == -1) {
        perror("lseek");
    }

  size_t to_send = (size_t) bytes;
  ssize_t sent;
  off_t offset = 0;

  while(((sent = sendfile(nodo->cliente.fd, logfd, &offset, to_send)) > 0)
        && (to_send > 0))
  {
    to_send -= (size_t) sent;
  }
  close(logfd);
}

/**
 * @brief Funcion encargada de obtner los bytes del .zip para su envio
 * 
 * @return off_t Bytes del envio del zip
 */
off_t bytes_envio (){

    int logfd = open("log_log.zip", O_RDONLY);
    if (logfd == -1) {
        perror("open log_log.zip");
    }
    
    off_t bytes = lseek(logfd, 0, SEEK_END);
    if (bytes == -1) {
        perror("lseek");
    }
    
    close(logfd);
    return bytes ;
}