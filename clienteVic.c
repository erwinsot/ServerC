#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/socket.h>
#include <stdlib.h>
#include<arpa/inet.h>
#include <string.h>
#include<netinet/in.h>
#include<netdb.h>
#define BUFFERSIZE 512
char myBuffer[BUFFERSIZE];


int menu();

char SendBuff[512],RecvBuff[512];

int main(int argc, char *argv[]){   
  int conn_socket;
  struct sockaddr_in server;
  struct hostent *hp;
   


  //Obtenemos la IP del servidor... en este caso
  // localhost indica nuestra propia máquina...
  hp=(struct hostent *)gethostbyname("localhost");

  if(!hp){
    printf("No se ha encontrado servidor...\n");
    return -1;    
  }

  // Creamos el socket...
  conn_socket=socket(AF_INET,SOCK_STREAM, 0);
  if(conn_socket<0) {
    printf("Error al crear socket\n");
    return -1;
    
  }
   
  memset(&server, 0, sizeof(server)) ;
  memcpy(&server.sin_addr, hp->h_addr_list[0], hp->h_length);
  //server.sin_addr.s_addr=INADDR_ANY;
  server.sin_family = hp->h_addrtype;
  server.sin_port = htons(6000);

  // Nos conectamos con el servidor...
  if(connect(conn_socket,(struct sockaddr *)&server,sizeof(server))<0){
    printf("Fallo al conectarse con el servidor\n");
    close(conn_socket);
    return -1;
  }
  printf("Conexión establecida con: %s\n", inet_ntoa(server.sin_addr));

    int buc = 0;
    float float1 = 0;
    float float2 = 0;
    float float3 = 0;
    int selec;

   while (buc == 0){
       selec = menu();
       switch(selec){
         case 1:
           do{
              printf("Ingrese el ID de origen:  ");
              if(fgets(myBuffer,BUFFERSIZE,stdin)) {float1 = (float) atoi(myBuffer); //toma de forma segura caracteres
              strcat(strcpy(SendBuff, "1"),myBuffer);
              printf("leido como %f \n",float1);
              send(conn_socket,SendBuff,sizeof(SendBuff),0);
              }
              }
               while (float1<1 || float1>1160); 
                break;
         case 2:
           do{
              printf("Ingrese el ID de destino:  ");
              if(fgets(myBuffer,BUFFERSIZE,stdin)) {float2 = (float) atoi(myBuffer); //toma de forma segura caracteres
              
              strcat(strcpy(SendBuff, "2"),myBuffer);
              printf("leido como %f \n",float2);
              send(conn_socket,SendBuff,sizeof(SendBuff),0);
              }
              } while (float2<1 || float2>1160); 
         break;
         case 3:
           do{
              printf("Ingrese hora del dia:  ");
              if(fgets(myBuffer,BUFFERSIZE,stdin)){float3 = (float) atoi(myBuffer); //toma de forma segura caracteres
                if( myBuffer[0] != '0'  && float3 == 0) float3 = -1; //evita que lineas vacias y basura sea leido como cero
              }
              strcat(strcpy(SendBuff, "3"),myBuffer);
              printf("leido como %f \n",float3);
              send(conn_socket,SendBuff,sizeof(SendBuff),0);
              } while (float3<0 || float3>23);  
                  break;
         case 4:
            printf("\n Con datos %f %f %f, buscando ...\n",float1,float2,float3);
            //instancia un struct para pasarle a backend
            send(conn_socket,"4",sizeof(char),0);
            recv(conn_socket,RecvBuff,sizeof(RecvBuff),0);
            printf(" El tiempo de viaje medio es: %s\n",RecvBuff);
            break;
         case 5:
          send(conn_socket,"5",sizeof(char),0);
          return 0;      
         
         default: printf("Escoja una opción valida\n");
       }
  
    }  
  
  
  // Cerramos el socket y liberamos la DLL de sockets
  close(conn_socket);  
  return EXIT_SUCCESS;
} 

int menu(){
  int selec;
  printf("\n\nIngrese una opción y pulse enter: \n\n");
  printf("1. Ingresar origen\n");
  printf("2. Ingresar destino\n");
  printf("3. Ingresar hora\n");
  printf("4. Buscar tiempo de viaje medio\n");
  printf("5. Salir\n");
 
  if(fgets(myBuffer,BUFFERSIZE,stdin)){
     selec = atoi(myBuffer); //toma de forma segura caracteres 
  }
  return selec;
}