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
#include <regex.h>
#include <math.h>

//Puerto de mi servidor
#define PORT 9666
#define BACKLOG_SIZE 500 //cantidad de conexiones que va a poder almazenar
#define MAX_PATH 4096 //max cantidad de caraceteres en el path
#define MAX_TOKENS 15 

//Tamaño del buffer para recibir mensjaes por parte de un cliente
//#define BUFSIZ 2048

//Bitwise----------------------------------------------------------------------------------------
#define BITWISE_32UNOS 4294967295 //Número para trabajar con bitwise, equivalente 4 octetos todos con unos. o 2^32-1
//--------------------------------------------------------------------------------------------

//EStructuras para los sockets
typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;


enum PRIMITIVA{
    BROADCAST = 1,
    NETWORK_NUMBER = 2,
    HOSTS_RANGE = 3,
    RANDOM_SUBNETS = 4
};

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
pthread_mutex_t lock_primitivas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Mask = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tokenizer_lock = PTHREAD_MUTEX_INITIALIZER;
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

*/

unsigned long int ipToDecimal(char * ip){
	
	char *token;
    token = strtok(ip, ".");
    
    unsigned long int a = atol(token);
    a = a << 24;
    token = strtok(NULL, ".");
    
    unsigned long int b = atol(token);
    b = b << 16;
    token = strtok(NULL, ".");
    
    unsigned long int c = atol(token);
    c = c << 8;
    token = strtok(NULL, ".");
    
    unsigned long int d = atol(token);
    //d = d << 8;
    
    a = a | b | c | d;
    return a;
}

char* decimalToIp(unsigned long int ip){
    //COnvierto una numero en decimal, a su valor en ip.
    
    struct in_addr ip_addr;
    ip_addr.s_addr = ip;
    
    char *token;
    token = strtok(inet_ntoa(ip_addr), ".");

    char *pAddr;
    pAddr=(char*)malloc(sizeof(char)*100);
    memset(pAddr,0,strlen(pAddr));


    char* a;
    char* b;
    char* c;
    char* d;
    a=(char*)malloc(sizeof(char)*20);
    b=(char*)malloc(sizeof(char)*20);
    c=(char*)malloc(sizeof(char)*20);
    d=(char*)malloc(sizeof(char)*20);

    strcpy(a, token);
    //strcat(a, ".");
    token = strtok(NULL, ".");
    strcpy(b, token);
    strcat(b, ".");
    token = strtok(NULL, ".");
    strcpy(c, token);
    strcat(c, ".");
    token = strtok(NULL, ".");
    strcpy(d, token);
    strcat(d, ".");

    strcat(pAddr, d);
    strcat(pAddr, c);
    strcat(pAddr, b);
    strcat(pAddr, a);

    return pAddr;
}

int extractInt(char* cidr){
    //ESto para extraer el mumero del texto /29
	char resp[5];;
    strcpy(resp, cidr);
	
    char *p;
	p = resp;
	while (*p != '\0') {
		if (*p == '/') *p = ' ';
		p++;
	}
    return atoi(resp);
}

char ** tokenizer(char * buffer){
    pthread_mutex_lock(&tokenizer_lock);
    //Creo un array para guardar los tokens
    char **tokens;
    //Variable que guarda cada token
    char *token;
    
    // Guardo memoria a los tokens
    tokens = malloc(MAX_TOKENS * sizeof(char*));

    token = strtok(buffer, " ");
    
    int i = 0;
    while (token != NULL){
        tokens[i] = malloc((100 + 1) * sizeof(char));
        strcpy(tokens[i], token);
        token = strtok(NULL, " ");
        if (token == NULL){break;}
        i = i + 1;
    }
    
    //for(int i = 0; i < 6; i++) {
    //    tokens[i] = malloc((100 + 1) * sizeof(char));
    //    strcpy(tokens[i], token);
    //    token = strtok(NULL, " ");
    //    }
    return tokens; 
}

int numeroPrimitiva(char * buffer){
    pthread_mutex_lock(&lock_primitivas);
    regex_t regex;

    int broadcast;
    int network_number;
    int host_range;
    int random_subnets;

    broadcast = regcomp(&regex,"GET BROADCAST IP [0-9]+.[0-9]+.[0-9]+.[0-9]+ MASK ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+)",REG_EXTENDED);
    broadcast = regexec(&regex, buffer, 0, NULL, 0);

    network_number = regcomp(&regex,"GET NETWORK NUMBER IP [0-9]+.[0-9]+.[0-9]+.[0-9]+ MASK ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+)",REG_EXTENDED);
    network_number = regexec(&regex, buffer, 0, NULL, 0);

    host_range = regcomp(&regex,"GET HOSTS RANGE IP [0-9]+.[0-9]+.[0-9]+.[0-9]+ MASK ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+)",REG_EXTENDED);
    host_range = regexec(&regex, buffer, 0, NULL, 0);

    random_subnets = regcomp(&regex,"GET RANDOM SUBNETS NETWORK NUMBER [0-9]+.[0-9]+.[0-9]+.[0-9]+ MASK ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+) NUMBER [0-9]+ SIZE ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+)",REG_EXTENDED);
    random_subnets = regexec(&regex, buffer, 0, NULL, 0);


    if (broadcast == 0) {
        return 1;
	}else if (network_number == 0){
        return 2;
    }else if (host_range == 0){
        return 3;
    }else if (random_subnets == 0){
        return 4;
    }else{
        return -1;
    }
}

//main() {
//    uint32_t ip = 4286578688;
//    struct in_addr ip_addr;
//    ip_addr.s_addr = ip;
//    printf("The IP address is %s\n", inet_ntoa(ip_addr));
//}

//int ipToInt(){}
//char * IntToIp{}

void * handleMessage(int* p_client_socket){
    int client_socket = *p_client_socket;
    free(p_client_socket);
    
    //Buffer para almacenar los mensajaes
    char buffer[BUFSIZ];
    memset(buffer,0,strlen(buffer));


    size_t bytes_read;
    int msgsize = 0;
    char actualpath[MAX_PATH+1];
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(client_socket,&fds);
    char numberString[4096];

    

    //lee la informacion escrita por el cliente y guarda en buffer
    while((bytes_read = read(client_socket, buffer+msgsize, sizeof(buffer)-msgsize-1)) > 0 ) {
        msgsize += bytes_read;
        if (msgsize > BUFSIZ-1 || buffer[msgsize-1] == '\n') break;
    }
    if(bytes_read==-1){
        printf("error con la info recibida");
        exit(1);
    }
    buffer[msgsize-1] = 0; //se termina el msg en null y se remueve el \n
    printf("%s\n",buffer);

    int primitiva = numeroPrimitiva(buffer);
    pthread_mutex_unlock(&lock_primitivas);
    //printf("Primitiva es : %d\n", primitiva);

    char **tokens_broadcast;
    tokens_broadcast = tokenizer(buffer);
    pthread_mutex_unlock(&tokenizer_lock);

    if (primitiva == BROADCAST){

        //GUardo los tokens
        if (strstr(tokens_broadcast[5], "/") != NULL) {            
            //Calculo el número de prefijo de la máscara si es /29
            unsigned long int prefijo = extractInt(tokens_broadcast[5]);

            //Calculo el número de prefijo de la máscara si es /29
            //printf("%ld\n", prefijo);
            unsigned long int mask = pow(2,prefijo) -1;
            mask = mask << (32-prefijo);
            
            //Extraigo el número de 32 bits que representa el ip
            unsigned long int ipDec = ipToDecimal(tokens_broadcast[3]);

            //Realizo la operacion para sacar el broadcast
            unsigned long int reverseMask = BITWISE_32UNOS;
            reverseMask = reverseMask ^ mask;
            unsigned long int broadcast = ipDec | reverseMask;

            printf("%s\n",decimalToIp(broadcast));
        
        //La mascara viene en formato 255.255.255.255
        }else{
            //Convierto la máscara a decimal
            unsigned long int mask = ipToDecimal(tokens_broadcast[5]);

            //Convierto ip a decimal
            unsigned long int ipDec = ipToDecimal(tokens_broadcast[3]);

            //Realizo la operacion para sacar el broadcast
            unsigned long int reverseMask = BITWISE_32UNOS;
            reverseMask = reverseMask ^ mask;
            unsigned long int broadcast = ipDec | reverseMask;

            printf("%s\n",decimalToIp(broadcast));
        }

    }else if (primitiva == NETWORK_NUMBER){
        
        if (strstr(tokens_broadcast[6], "/") != NULL) {
            //Calculo el número de prefijo de la máscara si es /entero
            unsigned long int prefijo = extractInt(tokens_broadcast[6]);
            //Extraigo el número de 32 bits que representa el ip
            unsigned long int ipDec = ipToDecimal(tokens_broadcast[4]);

            unsigned long int mask = pow(2,prefijo) -1;
            mask = mask << (32-prefijo);

            //Calculo de numero de red
            unsigned long int networkNumber = ipDec & mask;
            printf("NUmero de red: %s\n", decimalToIp(networkNumber));

        }else{
            //Convierto la máscara a decimal
            unsigned long int mask = ipToDecimal(tokens_broadcast[6]);
            //Convierto ip a decimal
            unsigned long int ipDec = ipToDecimal(tokens_broadcast[4]);

            unsigned long int networkNumber = ipDec & mask;
            printf("NUmero de red: %s\n", decimalToIp(networkNumber));
        }
        
    }else if (primitiva == HOSTS_RANGE){
        
    }else if (primitiva == RANDOM_SUBNETS){
        
    }else{
        printf("\nError no existe el comando");
    }
    
    close(client_socket);
    printf("cerrando conexion\n");
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
        
    }

    return 0;
}