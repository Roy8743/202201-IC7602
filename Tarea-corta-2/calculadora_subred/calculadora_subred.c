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
#include <time.h>

//Puerto de mi servidor
#define PORT 9666
#define BACKLOG_SIZE 500 //cantidad de conexiones que va a poder almazenar
#define MAX_PATH 4096 //max cantidad de caraceteres en el path
#define MAX_TOKENS 15 

#define uint unsigned int

//Tamaño del buffer para recibir mensjaes por parte de un cliente
//#define BUFSIZ 2048

//Bitwise----------------------------------------------------------------------------------------
#define BITWISE_32UNOS 4294967295 //Número para trabajar con bitwise, equivalente 4 octetos todos con unos. o 2^32-1
//--------------------------------------------------------------------------------------------

//EStructuras para los sockets
typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;


enum PRIMITIVA{
    NINGUN_CASO = 5,
    BROADCAST = 1,
    NETWORK_NUMBER = 2,
    HOSTS_RANGE = 3,
    RANDOM_SUBNETS = 4,
    NOT_COMMAND_FOUND = -1,
    EXIT = -2,
    BAD_IP = -3,
    BAD_MASK = -4
};

//Estructuras de datos utilizadas
struct TOKENS{
    char **lista;
    int largo;
}typedef TOKENS;


//Candados para proteccion de zonas de código



unsigned long int ipToDecimal(char * ipR){
	
    char * ip;
    ip = (char*)malloc(sizeof(char)*100);
    memset(ip,0,100);

    memcpy(ip, ipR, 100);

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



int extractInt(char* cidr_mask){
    //Recibe una mascar en notacion ciddr /xx
    //Retorna el numero entero despues de la barra inclinada
    char * cidr;
    cidr = (char*)malloc(sizeof(char)*10);
    memcpy(cidr, cidr_mask, 10);

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



TOKENS *tokenizer(char * buffer){
    //Creo un array para guardar los tokens
    char **tokens;
    int largo = 0;
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
        largo ++;
    }
    
    //for(int i = 0; i < 6; i++) {
    //    tokens[i] = malloc((100 + 1) * sizeof(char));
    //    strcpy(tokens[i], token);
    //    token = strtok(NULL, " ");
    //    }
    TOKENS *T;
    T = (TOKENS *) malloc(sizeof(TOKENS));
    T->lista = tokens;
    T->largo = largo;
    return T; 
}

int check(char* ipOrMask){
    //Esta función me revisa los rangos de bytes de una ip o una mascara
    char *ip;
    ip = (char*)malloc(sizeof(char)*100);
    memcpy(ip, ipOrMask, 100);

    char *token;
    token = strtok(ip, ".");

    for (int i = 0; i<4; i++){
        if (atoi(token) > 255){
            return -1;
        }
        token = strtok(NULL, ".");
    }
    return 1;
}

int checkIpAndMask(char **tokens, int largoListaTokens){
    //Esta fucnión revisa si los rangos de la ip y la máscara son correctos
    //Recibe el request por parte del usuario

    int i = 0;

    while (i<largoListaTokens){

        if (strcmp(tokens[i], "IP") == 0){
            // 1 indica que hay error 0 que no lo hay
            if (check(tokens[i+1]) == -1){
                return BAD_IP;
            }

        }else if (strcmp(tokens[i], "MASK") == 0){

            //Pregunto si la mascara es del tipo scdir
            if (strstr(tokens[i+1], "/") != NULL){
                //Calculo el número de prefijo de la máscara si es /29
                unsigned long int prefijo = extractInt(tokens[i + 1]);

                if (prefijo > 32){
                    return BAD_MASK;
                }

            }else{

                if (check(tokens[i+1]) == -1){
                    return BAD_MASK;
                }
            }

        }
        
        i = i + 1;
        
    }
    return 1;
}

int numeroPrimitiva(char * buffer){
    regex_t regex;

    int broadcast;
    int network_number;
    int host_range;
    int random_subnets;
    int exit;

    broadcast = regcomp(&regex,"GET BROADCAST IP [0-9]+.[0-9]+.[0-9]+.[0-9]+ MASK ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+)",REG_EXTENDED);
    broadcast = regexec(&regex, buffer, 0, NULL, 0);

    network_number = regcomp(&regex,"GET NETWORK NUMBER IP [0-9]+.[0-9]+.[0-9]+.[0-9]+ MASK ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+)",REG_EXTENDED);
    network_number = regexec(&regex, buffer, 0, NULL, 0);

    host_range = regcomp(&regex,"GET HOSTS RANGE IP [0-9]+.[0-9]+.[0-9]+.[0-9]+ MASK ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+)",REG_EXTENDED);
    host_range = regexec(&regex, buffer, 0, NULL, 0);

    random_subnets = regcomp(&regex,"GET RANDOM SUBNETS NETWORK NUMBER [0-9]+.[0-9]+.[0-9]+.[0-9]+ MASK ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+) NUMBER [0-9]+ SIZE ([0-9]+.[0-9]+.[0-9]+.[0-9]+|/[0-9]+)",REG_EXTENDED);
    random_subnets = regexec(&regex, buffer, 0, NULL, 0);

    exit = regcomp(&regex,"exit|EXIT",REG_EXTENDED);
    exit = regexec(&regex, buffer, 0, NULL, 0);


    if (broadcast == 0) {
        return 1;
	}else if (network_number == 0){
        return 2;
    }else if (host_range == 0){
        return 3;
    }else if (random_subnets == 0){
        return 4;
    }else if (exit == 0){
        return -2;
    }else{
        return -1;
    }
}



char * usableIpRange(unsigned long int ip, unsigned long int mask){
    //recibo el ip y las mascara en decimal

    //Calculo el networkNumber
    unsigned long int networkAddress = ip & mask;

    //Calculo el broadcast
    unsigned long int reverseMask = BITWISE_32UNOS;
    reverseMask = reverseMask ^ mask;
    unsigned long int broadcast = ip | reverseMask;

    //Convierto el networkAddress y el broadcast a texto
    char * networkAddressText;
    char * broadcastText;
    networkAddressText=(char*)malloc(sizeof(char)*200);
    broadcastText=(char*)malloc(sizeof(char)*200);
    strcpy(networkAddressText, decimalToIp(networkAddress));
    strcpy(broadcastText, decimalToIp(broadcast));

    //Almacenar el rango de ip
    char *ipRange;
    ipRange=(char*)malloc(sizeof(char)*200);
    memset(ipRange,0,strlen(ipRange));

    char *save_token_networkAddress, *save_token_broadcast;
    char *token_networkAddress, *token_broadcast;

    token_networkAddress = strtok_r(networkAddressText, ".", &save_token_networkAddress);
    token_broadcast = strtok_r(broadcastText, ".", &save_token_broadcast);

    for(int i = 0; i<3; i++){
        if (strcmp(token_networkAddress, token_broadcast) == 0){
            strcat(ipRange,token_networkAddress);
            strcat(ipRange,".");
        }else{
            strcat(ipRange,"{");
            strcat(ipRange,token_networkAddress);
            strcat(ipRange,"-");
            strcat(ipRange,token_broadcast);
            strcat(ipRange,"}");
            strcat(ipRange,".");
        }
        token_networkAddress = strtok_r(NULL, ".", &save_token_networkAddress);
        token_broadcast = strtok_r(NULL, ".", &save_token_broadcast);
    }

    strcat(ipRange,"{");
    int rango1 = atoi(token_networkAddress);
    rango1 = rango1 + 1;
    char ran1[10];
    sprintf(ran1, "%d", rango1);
    strcat(ipRange,ran1);
    strcat(ipRange,"-");
    int rango2 = atoi(token_broadcast);
    rango2 = rango2 - 1;
    char ran2[10];
    sprintf(ran2, "%d", rango2);
    strcat(ipRange,ran2);
    strcat(ipRange,"}");

    return ipRange;
}

unsigned long int getMask(char *mask){
    //Para numero aleatorio

    char *maskText;
    maskText = (char*)malloc(sizeof(char)*200);
    memcpy(maskText, mask, 200);

    if (strstr(maskText, "/") != NULL) {
                //Calculo el número de prefijo de la máscara si es /entero
                unsigned long int prefijo = extractInt(maskText);

                unsigned long int mask = pow(2,prefijo) -1;
                mask = mask << (32-prefijo);
                return mask;

            }else{
                //Convierto la máscara a decimal
                unsigned long int mask = ipToDecimal(maskText);
                return mask;
            }
}

int decimalToCidr(unsigned int i){

     i = i - ((i >> 1) & 0x55555555); // add pairs of bits
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333); // quads
     i = (i + (i >> 4)) & 0x0F0F0F0F; // groups of 8
     return (i * 0x01010101) >> 24;  // horizontal sum of bytes
}

void * handleMessage(int* p_client_socket){
    int client_socket = *p_client_socket;
    free(p_client_socket);
    
    //Buffer para almacenar los mensajaes
    char buffer[BUFSIZ];
    memset(buffer,0,strlen(buffer));


    size_t bytes_read;
    int msgsize = 0;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(client_socket,&fds);

    while (1){
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
        printf("Primitiva es : %d\n", primitiva);

        char **tokens_broadcast;
        TOKENS *T;
        T = tokenizer(buffer);
        tokens_broadcast = T->lista;
        int largoListaTokens = T->largo; 

        
        if (primitiva >= 0){
            //Checkeo que la ip y la mascara estén bien o sea dentro del rango permitido por ipv4
            int check = checkIpAndMask(tokens_broadcast, largoListaTokens);
            if (check == BAD_IP){
                send(client_socket, "Error: IP no permitida", strlen("Error: IP no permitida"),0);
                send(client_socket, "\n", strlen("\n"),0);
                primitiva = NINGUN_CASO;
            }else if (check == BAD_MASK){
                send(client_socket, "Error: Mascara no permitida", strlen("Error: Mascara no permitida"),0);
                send(client_socket, "\n", strlen("\n"),0);
                primitiva = NINGUN_CASO;
            }
        }
         
        
        if (primitiva == BROADCAST){
            
            //Tomo la máscara y la convierto a decimal
            unsigned long int mask = getMask(tokens_broadcast[5]);
            //Extraigo el número de 32 bits que representa el ip
            unsigned long int ipDec = ipToDecimal(tokens_broadcast[3]);

            //Revierto la máscara
            unsigned long int reverseMask = BITWISE_32UNOS;
            reverseMask = reverseMask ^ mask;

            //Hago la operación para el broadcast
            unsigned long int broadcast = ipDec | reverseMask; 
            
            char * broadcastText = decimalToIp(broadcast);
            //Le mando al cliente la respuesta
            send(client_socket, broadcastText, strlen(broadcastText), 0);
            send(client_socket, "\n", strlen("\n"),0);

            
        }else if (primitiva == NETWORK_NUMBER){
            //Tambien conocido como network address
            
            //Tomo la máscara y la convierto a decimal
            unsigned long int mask = getMask(tokens_broadcast[6]);

            //Obtengo el ip
            unsigned long int ipDec = ipToDecimal(tokens_broadcast[4]);

            //Calculo de numero de red
            unsigned long int networkNumber = ipDec & mask;
            char *networkNumberText = decimalToIp(networkNumber);
            //Le mando al cliente la respuesta
            send(client_socket,networkNumberText, strlen(networkNumberText),0);
            send(client_socket, "\n", strlen("\n"),0);

            
        }else if (primitiva == HOSTS_RANGE){
             //Tomo la máscara y la convierto a decimal
            unsigned long int mask = getMask(tokens_broadcast[6]);
            //Extraigo el número de 32 bits que representa el ip
            unsigned long int ipDec = ipToDecimal(tokens_broadcast[4]);
            char * hostRange = usableIpRange(ipDec, mask);

            //Le mando al cliente la respuesta
            send(client_socket,hostRange, strlen(hostRange),0);
            send(client_socket, "\n", strlen("\n"),0);
            

        }else if (primitiva == RANDOM_SUBNETS){
            //Convierto ip a decimal
            unsigned long int ipDec = ipToDecimal(tokens_broadcast[5]);
            //Convierto la máscara 1 a decimal
            unsigned long int mask1 = getMask(tokens_broadcast[7]);
            //Convierto la máscara 2 a decimal
            unsigned long int mask2 = getMask(tokens_broadcast[11]);
            //printf("MASK: %ld\n", mask2 >> (32 - 8));

            //Tomo la cantidad de de redes
            char *eptr;
            unsigned long int cantRedes;
            cantRedes = strtoul(tokens_broadcast[9], &eptr, cantRedes);
            

            //Paso 1 And entre la ip y ña máscara (obtener el numero de red)
            unsigned long int networkNumber = ipDec & mask1;

            //Paso 2 obtener el ip maximo con ipDec | not (mask1)
            unsigned int netMaskNot = ~mask1;
            unsigned long int upRange = networkNumber | netMaskNot;

            //Genero los numeros aleatorios
            unsigned long int aleatorio = (rand () % (upRange-networkNumber+1)) + networkNumber; 
            

            for(int i = 1; i<=cantRedes; i++){
                //Guardo el ciddr para la respuesta
                char cidr[5];
                int cidrNumber = decimalToCidr(mask2);
                sprintf(cidr, "%d", cidrNumber);

                unsigned long int ipAleatoria = aleatorio & mask2;

                char *ip = decimalToIp(ipAleatoria);
                send(client_socket, ip, strlen(ip), 0);
                send(client_socket, "/", 1, 0);
                send(client_socket, cidr, 5, 0);
                send(client_socket, "\n", strlen("\n"),0);

                aleatorio = rand () % (upRange-networkNumber+1) + networkNumber;
                memset(cidr, 0, 5);
            }

            cantRedes = 0;
            


        }else if (primitiva == EXIT){
            close(client_socket);
            printf("cerrando conexion\n");
            break;

        }else if (primitiva == NOT_COMMAND_FOUND){
            char Resp[] = "Error: El comando no existe, revise que esté bien escrito";
            send(client_socket, Resp, strlen(Resp),0);
            send(client_socket, "\n", strlen("\n"),0);
        }
        memset(buffer,0,strlen(buffer));
        bytes_read = 0;
        msgsize = 0;
        //close(client_socket);
        //printf("cerrando conexion\n");
    }
}


int main(int argc, char **argv){
     srand (time(NULL));

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