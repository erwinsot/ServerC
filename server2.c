
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include "hashs.c"

/** Puerto  */
#define PORT       6000

/** Número máximo de hijos */
#define MAX_CHILDS 3

/** Longitud del buffer  */
#define BUFFERSIZE 512


struct buscador{
    float id;
    float tiempo;
};

int AtiendeCliente(int socket, struct sockaddr_in addr);
int DemasiadosClientes(int socket, struct sockaddr_in addr);
void error(int code, char *err);
void reloj(int loop);

int main(int argv, char** argc){

    int socket_host;
    struct sockaddr_in client_addr;
    struct sockaddr_in my_addr;
    struct timeval tv;      /* Para el timeout del accept */
    socklen_t size_addr = 0;
    int socket_client;
    fd_set rfds;        /* Conjunto de descriptores a vigilar */
    int childcount=0;
    int exitcode;

    int childpid;
    int pidstatus;

    int activated=1;
    int loop=0;
    socket_host = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_host == -1)
      error(1, "No puedo inicializar el socket");
   
    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY ;

   
    if( bind( socket_host, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
      error(2, "El puerto está en uso"); /* Error al hacer el bind() */

    if(listen( socket_host, 10) == -1 )
      error(3, "No puedo escuchar en el puerto especificado");

    size_addr = sizeof(struct sockaddr_in);


    while(activated)
      {
    reloj(loop);
    /* select() se carga el valor de rfds */
    FD_ZERO(&rfds);
    FD_SET(socket_host, &rfds);

    /* select() se carga el valor de tv */
    tv.tv_sec = 0;
    tv.tv_usec = 500000;    /* Tiempo de espera */
   
    if (select(socket_host+1, &rfds, NULL, NULL, &tv))
      {
        if((socket_client = accept( socket_host, (struct sockaddr*)&client_addr, &size_addr))!= -1)
          {
        loop=-1;        /* Para reiniciar el mensaje de Esperando conexión... */
        printf("\nSe ha conectado %s por su puerto %d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
        switch ( childpid=fork() )
          {
          case -1:  /* Error */
            error(4, "No se puede crear el proceso hijo");
            break;
          case 0:   /* Somos proceso hijo */
            if (childcount<MAX_CHILDS)
              exitcode=AtiendeCliente(socket_client, client_addr);
            else
              exitcode=DemasiadosClientes(socket_client, client_addr);

            exit(exitcode); /* Código de salida */
          default:  /* Somos proceso padre */
            childcount++; /* Acabamos de tener un hijo */
            close(socket_client); /* Nuestro hijo se las apaña con el cliente que
                         entró, para nosotros ya no existe. */
            break;
          }
          }
        else
          fprintf(stderr, "ERROR AL ACEPTAR LA CONEXIÓN\n");
      }

    /* Miramos si se ha cerrado algún hijo últimamente */
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
            activated=0;
          }
          }
      }
    loop++;
    }

    close(socket_host);

    return 0;
}

    /* No usamos addr, pero lo dejamos para el futuro */
int DemasiadosClientes(int socket, struct sockaddr_in addr)
{
    char buffer[BUFFERSIZE];
    int bytecount;

    memset(buffer, 0, BUFFERSIZE);
   
    sprintf(buffer, "Demasiados clientes conectados. Por favor, espere unos minutos\n");

    if((bytecount = send(socket, buffer, strlen(buffer), 0))== -1)
      error(6, "No puedo enviar información");
   
    close(socket);

    return 0;
}

int AtiendeCliente(int socket, struct sockaddr_in addr)
{

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
    
    recv (socket,RecvBuff,sizeof(RecvBuff), 0);
    memcpy(indice,RecvBuff,1);
    memcpy(valor,RecvBuff+1,sizeof(RecvBuff));
   
    switch(atoi(indice)){
      case 1:
      
       Lector.origen= atof(valor);        
        break; 
      case 2:
      
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