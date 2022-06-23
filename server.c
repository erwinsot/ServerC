#include <stdio.h>
#include<sys/types.h>
//#include <winsock2.h>
#include <string.h>
//#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
//#include <windows.h>
#include <unistd.h>
#include<sys/socket.h>
#include <stdlib.h>
#include<arpa/inet.h>
#include <string.h>
#include "hashs.c"
#include<netinet/in.h>
#include<netdb.h>
#include <sys/types.h>
#include <sys/wait.h>


int AtiendeCliente(int socket, struct sockaddr_in addr);
int DemasiadosClientes(int socket, struct sockaddr_in addr);
void reloj(int loop);
void error(int code, char *err);

struct buscador{
    float id;
    float tiempo;
}; 
  

int main(int argc, char *argv[]){
  
  //WSADATA wsaData;
  int conn_socket,comm_socket;
  int  comunicacion;
  struct sockaddr_in server;
  struct sockaddr_in client;
  struct hostent *hp;
  int resp,stsize;
  int childpid;
  int exitcode;
  int childcount=0;
  int pidstatus;
  int loop=0;
  
  //Inicializamos la DLL de sockets
  //resp=WSAStartup(MAKEWORD(1,0),&wsaData);
  //if(resp){
    //printf("Error al inicializar socket\n");
    //getchar();return resp;
  //}
  
  //Obtenemos la IP que usará nuestro servidor... 
  // en este caso localhost indica nuestra propia máquina...
  hp=(struct hostent *)gethostbyname("localhost");

  if(!hp){
    printf("No se ha encontrado servidor...\n");
    getchar();
  }

  // Creamos el socket...
  conn_socket=socket(AF_INET,SOCK_STREAM, 0);
  if(conn_socket==-1) {
    printf("Error al crear socket\n");
    getchar();
  }
  
  memset(&server, 0, sizeof(server)) ;
  //memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_addr.s_addr=INADDR_ANY;
  server.sin_family = hp->h_addrtype;
  server.sin_port = htons(6000);

  // Asociamos ip y puerto al socket
  resp=bind(conn_socket, (struct sockaddr *)&server, sizeof(server));
  if(resp<0){
    printf("Error al asociar puerto e ip al socket\n");
    close(conn_socket);
    getchar();
  }

  if(listen(conn_socket, 32)==-1){
    printf("Error al habilitar conexiones entrantes\n");
    close(conn_socket);
    getchar();
  }
  stsize=sizeof(struct sockaddr);

  while(1){
    reloj(loop);
     if((comm_socket= accept( conn_socket, (struct sockaddr*)&client, &stsize))!= -1)
     {
      printf("Conexión entrante desde: %s\n", inet_ntoa(client.sin_addr));
      switch (childpid=fork())
      {
      case -1:
        error(4, "No se puede crear el proceso hijo");
        break;
      case 0:
        printf("entre al hijo");
        if (childcount<32){
          printf("entre a llamar la fucnion");
          exitcode=AtiendeCliente(conn_socket, client);}
        else
          exitcode=DemasiadosClientes(conn_socket, client);
      
      default:
            childcount++; /* Acabamos de tener un hijo */
            close(conn_socket);
        break;
      }


     }
     else
     {
      printf("Error al aceptar conexión entrante\n");
      close(conn_socket);
     }

     childpid=waitpid(0, &pidstatus, WNOHANG);
       if (childpid>0)
      {
        childcount--;   /* Se acaba de morir un hijo */

        /* Muchas veces nos dará 0 si no se ha muerto ningún hijo, o -1 si no tenemos hijos
         con errno=10 (No child process). Así nos quitamos esos mensajes*/

        if (WIFEXITED(pidstatus))
          {

        /* Tal vez querremos mirar algo cuando se ha cerrado un hijo correctamente */
        if (WEXITSTATUS(pidstatus)==99)
          {
            printf("\nSe ha pedido el cierre del programa\n");
            
          }
          }
      }
      loop++;     
          
  }
 close(conn_socket);

    




              
 
  
  return (EXIT_SUCCESS);
} 
int AtiendeCliente(int socket, struct sockaddr_in addr){
  char SendBuff[512],RecvBuff[512];
  FILE *fp; 
  char *buf;
  buf=(char *)malloc(100*sizeof(char));
  getcwd(buf,100);  
  char senbuff2[512];
  char indice[512];
  char valor [512];
  struct buscador miBuscador;
  struct mistru Lector;   
  int id;  

  while (1) 
  {
    printf("netree a la funcpons llamar");
    recv (socket,RecvBuff,sizeof(RecvBuff), 0);
    memcpy(indice,RecvBuff,1);
    memcpy(valor,RecvBuff+1,sizeof(RecvBuff));
    printf("el indecesw es %f", indice);

    switch(atoi(indice)){
      case 1:
      printf("ssssss %f",valor);
       Lector.origen= atof(valor);        
        break; 
      case 2:
      printf("entew a 2");
        Lector.destino= atof(valor);       
        break;
      case 3:
        Lector.hora= atof(valor);        
        break;
      case 4:        
        id=myhash(Lector);       
        fp=fopen("tabla_datos.bin","rb");
        fseek(fp,id,SEEK_SET); //se ubica en la linea que el hash apunta
        fread(&miBuscador,sizeof(miBuscador),1,fp); // lee en fp el tiempo correspondiente         
        snprintf(SendBuff, sizeof(SendBuff), "%f", miBuscador.tiempo);
        send(socket,SendBuff,sizeof(SendBuff),0);        
        break;
      case 5:
        return 0;
      default:       
       return 0;
    }  
    
  } 

  return 0;
}

int DemasiadosClientes(int socket, struct sockaddr_in addr)
{
    char buffer[512];
    int bytecount;

    memset(buffer, 0, 512);
   
    sprintf(buffer, "Demasiados clientes conectados. Por favor, espere unos minutos\n");

    if((bytecount = send(socket, buffer, strlen(buffer), 0))== -1)
      error(6, "No puedo enviar información");
   
    close(socket);

    return 0;
}

void reloj(int loop)
{
  if (loop==0)
    printf("[SERVIDOR] Esperando conexión  ");

  printf("\033[1D");        /* Introducimos código ANSI para retroceder 2 caracteres */
  switch (loop%4)
    {
    case 0: printf("|"); break;
    case 1: printf("/"); break;
    case 2: printf("-"); break;
    case 3: printf("\\"); break;
    default:            /* No debemos estar aquí */
      break;
    }

  fflush(stdout);       /* Actualizamos la pantalla */
}
void error(int code, char *err)
{
  char *msg=(char*)malloc(strlen(err)+14);
  sprintf(msg, "Error %d: %s\n", code, err);
  fprintf(stderr, msg);
  exit(1);
}
 // Aceptamos conexiones entrantes
  //printf("Esperando conexiones entrantes... \n");
  //stsize=sizeof(struct sockaddr);
  //comm_socket=accept(conn_socket,(struct sockaddr *)&client,&stsize);
  //if(comm_socket==-1){
    //printf("Error al aceptar conexión entrante\n");
    //close(conn_socket);
    
  //}
  //printf("Conexión entrante desde: %s\n", inet_ntoa(client.sin_addr));
              
  // Como no vamos a aceptar más conexiones cerramos el socket escucha
  //close(conn_socket);
 
  
  //getchar();          

  // Cerramos el socket de la comunicacion
  //close(comm_socket);  
  // Cerramos liberia winsock