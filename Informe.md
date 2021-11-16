TP1 Sistemas Operativos 2 
### Ingeniería en Computación - FCEFyN - UNC - Lucas Antonio Salse
# Interprocess Communication

## Introducción 
En el siguiente trabajo práctico se busca se busca diseñar e implementar mecanismos de IPC (InterProcess Comunication),
los cuales nos permitirán poder diseñar e implementar un software capaz de cumnplir con los requerimientos propuestos,
haciendo uso de estos mecanismos presentados y explicados en la materia.

## Arquitectura del sistema
Se busca diseñar e implementar un software (desarrollado en lenguaje C), siguiendo el diagrama de la Figura 1, la cual describe un sistema utilizando el patrón [Publisher Suscriber][publisher].

![Diagrama de Componentes](/img/block_diagram.png)

*Figura 1: Diagrama de Componentes del sistema*

## Componentes del sistema
### Delivery Manager
Es un proceso que se encargua de recibir mensajes y enviarlos a todos los suscriptores validos (asociados a algún productor).
A este proceso, lo vamos a llamar _DeliveryManager_. El mismo debe contemplar lo siguiente:

-   Debe poseer una interfaz (CLI) que acepte únicamente los siguientes comandos:
    - `add <socket> <productor>` : Este comando agregará el socket a una lista correspondiente al servicio, para ser validado.
    - `delete <socket> <productor>`: Este comando borra el host como suscriptor dejando de enviarle mensajes.
    - `log <socket>`. Este comando comprime el log local del _DeliveryManager_ y lo envía a `<socket>`.
-	El _DeliveryManager_ debe validar que los hosts agregados sean aptos para recibir mensaje del tipo `<Mensaje>|<Checksum>`.	
-	Una vez validado el host, todos los mensajes que recibe el _DeliveryManager_ de un productor, deben ser enviados a los suscriptores correspondientes.
-	En caso que se desconecte un suscriptor, el servicio para los demás suscriptores no debe verse afectado. Luego de cinco segundos de mensajes fallados, debe ser eliminado de la lista. Si el suscriptor vuelve antes de esos 5 segundos, debe enviársele todos los mensajes en colados.
-	El _DeliveryManager_ debe loguear todos los mensajes enviados, tanto el origen como el destino, también se agrega o se elimina un suscriptor y cuando es validado. El formato del log debe `<datetime> <Mensaje>`.
### Productores
Se deben implementar tres productores:
- Un productor que envía un mensaje random con una tasa de X/segundos.
- Un productor que envía la memoria libre del Sistema, cada Y/segundos.
- Un productor que debe enviar load del sistema normalizado, cada Z/segundos.
Pueden elegir otro productor, justificándolo. X, Y y Z deben ser distintos.
### Suscriptores
- Puede existir hasta cinco mil suscriptores.
- Los suscriptores deben esperar que el _DeliveryManager_ los suscriba mediante el comando `add`, es decir, que lo suscriba.
- Los suscriptores deben validar el `checksum` de los mensajes recibidos y loguear el mensaje, para luego ser descartados.

## Diseño e implementación
En esta sección se buscará justificar la solución tomada para realizar el proyecto.
### Comunicación entre proceseos
Además de la comunicación entre los clientes con el delivery manager que se realiza a través de *sockets*, se debió elegir otro
mecanismo de comunicación entre los demás procesos, esto es entre cada uno de los productores con el delivery manager para
poder comunicarle el mensaje que estos generan y que luego serán retransmitidos solo a aquellos clientes (*sockets*) suscriptos.
Además la solución implementada busco realizar la interfaz (CLI) en un proceso distinto al delivery manager, es por esta 
razón que también se optó el correspondiente IPC teniendo en cuenta esta situación, dado que también se deberá comunicar
estos mensajes/comandos de la CLI al DM (Delivery Manager).

Para realizar estas comunicaciones entre los diferentes procesos se utilizó el sistema de colas de mensajes *UNIX*. La justificación
de esta implementación es que nos permiten manejar cada una de estas colas mediante file desciriptors los cuales van a ser muy
útiles al momento de que el delivery manager los administre y controle correctamente. En otras palabras el delivery manager
maneja los eventos producidos por medio de las colas de mensajes a través de eventos que se producen en estos file descriptors y
actúa en consecuencia a estos eventos ejecutando las acciones correspondientes.
Existe una cola de mensaje para cada uno de estos procesos (3 productores y 1 cli), por lo tanto tenemos 4 flie descritors distintos.
### Comunicación entre clientes y productores
La comunicación entre los clientes y los productores se realiza a través de sockets, los cuales son un método de IPC que permite intercambiar datos entre aplicaciones, ya sea en el mismo host (computadora) o en diferentes hosts conectados por una red.
La implementación se realizó siendo que cada uno de los clientes, realizan un *connect()* al DM (delivery manager) y este será el encargado de aceptar esa conexión (*accept()*), generando así un nuevo file descritpor correspondiente a dicha conexión. Esto permitirá que ese file descritpor de esa nueva conexión (de cada conexión de clientes que se produzca) sea agregado al manejo de fd mediante epoll del DM. Se uso una comunicación a través de sockets. 
- El dominio elegido fue IPv4 (AF_INET) porque permite la comunicación entre aplicaciones que se ejecutan en hosts conectados a través de una red de Protocolo de Internet versión 4 (IPv4).
- El tipo de socket elegido (SOCK_STREAM) dado que proporcionan un canal de comunicación de flujo de bytes confiable. Confiable significa que tenemos la garantía de que los datos transmitidos llegaran intactos a la aplicación receptora, exactamente como los transmitió el remitente (asumiendo que ni el enlace de red ni el receptor fallan), o que recibiremos una notificación de un probable falla en la transmisión.
### Delivery manager
El delivery manager es el server dedicado a recibir y coordinar ya sea las comunicaciones entre los clientes con los productores, como
así también los comandos ingresados por la CLI. Lo primero que se encarga el DM (Delivery manager) es crear las correspondientes
vías de comunicación con los productores y con la CLI mediante las colas de mensaje *UNIX*. Luego se crea un socket para la conexión
con los clientes como se explicó anteriormente y finalmente se utiliza la función epoll la cual nos permite manejar todos los eventos
que ocurran y realizar la correspondiente acción debida. Esto significa que todo file descriptor generado por las colas de mensajes o
la conexión con los clientes (mediante sockets) son agregados al epoll para su manejo adecuado. los eventos que pueden ocurrir en estos
file descritpor son:
- Recepción de mensajes por medio de las colas de mensajes por parte de los productores.
- Recepción de mensajes/comandos por medio de la CLI
- Comunicación de los clientes con el server para solicitar una conexión
- Caída de alguno de los clientes.
- Envió de los mensajes a los clientes que se encuentran asociados a algún productor.

Además se almacena a cada uno de los clientes que generaron una conexión exitosa en una lista. Esto nos permite saber que los clientes
que se encuentran en la lista su conexión con el DM está establecida. En caso de que algún cliente por alguna razón se caiga, el DM
recibe un mensaje de "exit" que le permite desencolar a dicho cliente de la lista.
Otra funcionalidad del DM es llevar un log con los eventos que se producen los cuales son los mensajes que se envían a los clientes,
de la forma origen (productor-n) al cliente (ip:puerto) junto con el mensaje enviado; las asociaciones correspondientes entre los
clientes con algún productor y las desuscripciones de los clientes con los productores.

### CLI
La CLI nos permite ingresar comandos mediante *stdin* que luego serán enviados al DM para ejecutar alguna acción las cuales son:
- **add <ip:puerto> <productorn>** siendo n valores entre 1 y 3 que representan a los 3 productores a los cuales se puede asociar.
- **delete <ip:puerto> <productorn>** siendo n valores entre 1 - 3 que son los 3 productores a los cuales podemos cancelar la suscb.
- **log <ip:puerto>** comando que envía el log del DM a dicho cliente.
Estos anteriores comandos deberán ingresarse de manera correcta si no, no serán enviados. Con manera correcta nos referimos
a que tanto las primeras palabras claves (add, delete o log) deben ser escritas en minúsculas y además respetar la cantidad
de elementos ingresados. Por ejemplo add 127.0.0.1:2345 sería un comando tomado como invalido dado que falta un elemento más
que es el productor al que deseamos asociar que debe estar ingresado de la forma: **productor1 o porductor2 o productor3**.
Se agregaron comandos a la cli los cuales permiten testear la cantidad de clientes conectados, esto fue hecho para testear que sea
posible que se conecten 5000 clientes, estos comandos son:
- **listar**: Se encarga de listar los clientes que se encuentran conectados, cual es su file descriptor, ip:puerto y a que productores
están asociados actualmente.
- **cantidad**: Nos permite saber cuántos clientes se encuentran conectados en el Delivery Manager.

## Funcionamiento
Se muestra el funcionamiento del software mediante ciertas capturas:
En una primera parte se ejecuta el DM llamado server y obtenemos la creación del mismos junto con las colas de mensajes y
el socket para la comunicación con los clientes:

![DM_run](/img_informe/dm_run.png)

*Figura 1: dm_run a la espera de conexiones*

Una vez que el DM (en este caso llamado server) se ejecuta se encuentra a la espera de aceptar conexiones, las cuales se pueden ver
en la siguiente imagen como se conectan de manera correcta dos clientes:

![DM_conexiones](/img_informe/dm_conexiones.png)

*Figura 2: El dm acepta dos conexiones de manera correcta*

Como se puede observar se tiene la información de los file descriptors de los clientes conectados como su ip:puierto, luego de que la conexión se haya realizado de manera correcta, es decir, que los paquetes ACK se hayan tanto enviado como recibido.
Luego cada uno de estos clientes van a ser capaces de ser asociados a uno o mas productores para recibir los mensajes correspondientes. A continuación se muestra la asociación del cliente1 a los 3 productores:

![add_clientes_productores](/img_informe/add_cliente_productres.png)

*Figura 3: Comando add a un cliente para asociar a 3 productores*

![mensajes_clientes](/img_informe/cliente_rcv.png)

*Figura 4: Mensajes recibidos por el cliente luego de la suscripción*

![dm_clientes](/img_informe/dm_conexiones.png)

*Figura 5: Lista de los clientes conectados y asociados a los productores*

Como se puede ver en la figura 4 la recepción de los mensajes muestra el mensaje de *comprobación exitosa*, esto es debido al envío del checksum MD5 el cual es enviado en cada mensaje de la siguiente manera: <mensaje|checksum> Luego de esto del lado de los clientes al recibir dicho mensaje realiza nuevamente la comprobación para verificar si efectivamente el mensaje enviado posee el mismo checksum y darlo como valido para mostrarlo.

Para el envío de archivo log.txt comprimido solo hace falta colocar en la cli el comando log <ip:puerto>, con esto se enviara el log de forma comprimido al cliente en cuestión, como se ve en las siguientes imagenes:

![cli_log](/img_informe/cli_log.png)

*Figura 6: Se ingresa la orden para enviar el log comprimido*

![log_send_and_recv](/img_informe/log_send_and_recv.png)

*Figura 7: Se peude ver como el log llega de manera correcta y comprimida*

En el caso de que algun cliente se desconecte mediante una señal de SIGINT automaticamente manda al DM un mensaje de exit para que sea quitado de la lista de clientes conectados como así tambien eliminar su file descriptor de epoll como se ve en la siguiente imagen:

![log_send_and_recv](/img_informe/client_close.png)

*Figura 8: Se ve el cierre del fd por parte del DM*

Finalmente se muestra la conexión del delivery manager con 5000 clientes a través del script de bash:

![log_send_and_recv](/img_informe/clientes_5000.png)

*Figura 9: Se ve que los 5000 clientes junto con sus file descriptores*

### Diagramas de secuecia del funcionamiento del proyecto
**Diagrama de secuencia entre el DM y los clientes**

![diagrama_conexion_cliente](/img_informe/diagrama_conexion_cliente.png)

*Figura 10: Diagrama conexión del DM con los clientes*

**Diagrama de secuencia entre el DM y los productores**

![diagrama_dm_productores](/img_informe/diagrama_dm_productores.png)

*Figura 11: Diagrama relación DM productores*

**Diagrama de secuencai entre el DM y la CLI**

![diagrama_dm_cli](/img_informe/diagrama_dm_cli.png)

*Figura 12: Diagrama relación DM con la CLI*


## Concluciones
Durante la realización del proyecto el principal problema que se enfrento fue definir el diseño e implementación del mismo. Una vez que se tuvo "certeza" de lo que se iba a utilizar para implementarlo, a medida que se avanzaba en su desarrollo fueron surgiendo distintos interrogantes sobre si la manera correcta de estar haciendolo era efectivamente la correcta. Una de las ventajas también fue la comunicación mediante los canales brindados por la cátedra (slack) donde se podían ver distintos puntos de vista lo cual fue de mucha ayuda. Así y todo se lograron cumplir y abarcar con practicamente todos los objetivos planteados.

## Referencias
---
Bibliografia:
- [The Linux Programming Interfaces](https://github.com/rangaeeeee/books-raspberrypimagazines/blob/18bf13c133ef0a78b2b60c4ff8635ac71b0843f1/The%20Linux%20Programming%20Interface-Michael%20Kerrisk.pdf)
- Filminas de clase
- Linux man page

Enlaces:
- [Como convertir Hash MD5 a String]( <https://stackoverflow.com/questions/7627723/how-to-create-a-md5-hash-of-a-string-in-c>)
- [Como obtener la garda de CPU](https://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-using-c)
- [Instalación de libreria openssl](https://www.howtoforge.com/tutorial/how-to-install-openssl-from-source-on-linux/)
- [Libreria zip ustilizada](https://libzip.org/documentation/)
---
