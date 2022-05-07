#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <wait.h>
#include <fcntl.h> 
#include <sys/types.h>

//Puerto de mi servidor
#define PORT 9666
#define BACKLOG_SIZE 500 //cantidad de conexiones que va a poder almazenar
#define MAX_PATH 4096 //max cantidad de caraceteres en el path

//Tamaño del buffer para recibir mensjaes por parte de un cliente
//#define BUFSIZ 2048

//EStructuras para los sockets
typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;



//Estructuras de datos utilizadas
struct Header{
    char *key;
    char *value;
}typedef Header;

struct Request{
    char *path;
    char *method;
    Header *headers;
    int headersLength;
}typedef Request;

struct Response{
    long int contentLength;
    char *status;
    FILE *fp;
    Header *headers;
    int size;
    int type;
}typedef Response;


//Candados para proteccion de zonas de código
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/*

Request *newRequest(){
    Request *r;
    r = (Request *) malloc(sizeof(Request));
    r->path = (char *) malloc(sizeof(char) * (MAX_PATH - 1));
    r->method = (char *) malloc(sizeof(char) * 25);
    r->headersLength = 0;
    r->headers = (Header *) malloc(sizeof(Header) * 35);
    return r;
}

Request *parseMessage(char *buffer){
    char actualpath[MAX_PATH+1];
    Request *r = newRequest();
    //
      //home/jonder/Escritorio/Git/Sistemas_operativos_proyecto_3/datos/
      //home/meta/CLionProjects/Sistemas_operativos_proyecto_3/datos/
      //home/estudiante/Escritorio/Sistemas_operativos_proyecto_3/datos/
     //
    strreplace(buffer,"/","/home/estudiante/Escritorio/Sistemas_operativos_proyecto_3/datos/");
    // Separa el request por header
    char * headerToken = strtok(strdup(buffer), "\r\n");
    // Separo el method y path
    char *tempHeader = strdup(headerToken);
    //strcpy(tempHeader, headerToken);
    char* token = strtok(tempHeader, " ");
    strncpy(r->method, token, sizeof(r->method) - 1);
    r->method[sizeof(r->method)] = '\0';
    token = strtok(NULL, " ");
    if(realpath(token,actualpath)==NULL){
        printf("Error(bad path): %s\n",token);
        return NULL;
    }
    strcpy(r->path,actualpath);
    r->headersLength = 0;
    // Loop para separar cada header
    char **headersRaw;
    headersRaw = (char **) malloc(sizeof(char*)*35);
    for (int i = 0; i < 35; ++i) {
        headersRaw[i] = (char *) malloc(sizeof(char)*150);
    }
    headerToken = strtok(buffer, "\r\n");
    headerToken = strtok(NULL, "\r\n");
    while( headerToken != NULL ) {
        //printf("[%s]\n", headerToken);
        strcpy(headersRaw[r->headersLength],headerToken);
        r->headersLength++;
        headerToken = strtok(NULL, "\r\n");
    }
    for (int i = 0; i < r->headersLength; ++i) {
        Header *header = newHeader(headersRaw[i]);
        r->headers[i] = *header;
    }
    return r;
}

void * handleMessage(int* p_client_socket){
    int client_socket = *p_client_socket;
    //free(p_client_socket);
    char buffer[BUFSIZ];

    memset(buffer,0,strlen(buffer));
    size_t bytes_read;
    
    int msgsize = 0;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(client_socket,&fds);
    char numberString[4096];

    //esperamos 1 segundo si aun no hay nada en el socket de lectura
    //while(select(FD_SETSIZE,&fds,NULL,NULL,NULL)==0){
        //sleep(1);
    //}
    msgsize=0;
    //lee al cliente y obtiene el nombre del archivo
    while((bytes_read = read(client_socket, buffer+msgsize, sizeof(buffer)-msgsize-1)) > 0 ) {
        msgsize += bytes_read;
        if (msgsize > BUFSIZ-1 || buffer[msgsize-1] == '\n') break;
    }
    if(bytes_read==-1){
        printf("error con la info recibida");
        exit(1);
    }
    buffer[msgsize-1] = 0; //se termina el msg en null y se remueve el \n
    printf("Buffer:[%s]\n",buffer);

    //Proceso lo que me envió el cliente
    Request *r = parseMessage(buffer);


    if(r==NULL){
        close(client_socket);
        return NULL;
    }
    toString(r);
    fflush(stdout);

    //Para responder al cliente
    Response *response;
    
    //Veo en que metodo cae
    if (!strcmp(r->method,"GET")){
        print("Hola");
        //response = executeGet(r,buffer,&client_socket);
        if(response == NULL){
            close(client_socket);
            return NULL;
        }
    }else{
        
        //TODO Not Implemented response
         
        close(client_socket);
        return NULL;
    }

    while(response->size > 0){
        writeResponse(response->type,response,&client_socket);
        response->size = 0;
        //printf("write :  %ld\n", n);
    }
    //writeResponse(response->type,response,&client_socket);

    freeRequest(r);
    freeResponse(response);
    close(client_socket);
    printf("cerrando conexion\n");
}

*/

void * handleMessage(int* p_client_socket){
    printf("Bienvenido nuevo cliente jejejjeje");
    while (1)
    { 
    }
    
}

int main(int argc, char **argv){


    int server;
    unsigned int addr_size;
    SA_IN direccionServer, client_addr;


    //Limpiemoas la estructura
    memset(&direccionServer, 0, sizeof(direccionServer));

    //Se establece el objeto socket
    server = socket(AF_INET , SOCK_STREAM , 0);

    //AF_INET es una familia de direcciones
    direccionServer.sin_family = AF_INET;
    //INADDR_ANY = en realidad es la IP especial 0.0.0.0
    //direccionServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    direccionServer.sin_addr.s_addr = INADDR_ANY;
    //Este es el puerto del servidor
    direccionServer.sin_port = htons(PORT);

    //Esto es para no esperar despues de matar el servidor
    int activado = 1;
    setsockopt(server,SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

    if (bind(server,(SA*)&direccionServer, sizeof(direccionServer)) == -1){
        perror("Falló el bind del socket");
        return 1;
    }
    
    if (listen(server,BACKLOG_SIZE) == -1) {
        perror("Falló el listen del socket");
        return 1;
    }
    
    //pthread_mutex_lock(&lock);
    while (1) {
        int client_socket;
        printf("Esperando conexiones\n");
        //esperando clientes
        addr_size = sizeof(SA_IN);
        client_socket = accept(server, (void*)&client_addr, &addr_size);
        printf("Nuevo cliente\n");

        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;

        pthread_t thread;
        pthread_create(&thread, NULL, (void *) handleMessage, pclient);


        //Recibo el mensaje del cliente
        printf("%s: ", "Todo bien hasta acá");
        //handleMessage(pclient); 
        
    }

    return 0;
}