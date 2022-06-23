#include <stdio.h>
#include "hashs.c"
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
# include <stdlib.h>


struct buscador{
    float id;
    float tiempo;

};
struct mistru{
  float origen;
  float destino;
  float hora;
} ;

long int timeUber(float valores[4])
{  

  FILE *fp; 
  float * array = malloc(4 * sizeof(float));
  struct buscador miBuscador;
  struct mistru Lector; 
  
  
  int id=myhash(valores);
  //busca en archivo bin
  fp=fopen("tabla_datos.bin","rb");
  fseek(fp,id,SEEK_SET); //se ubica en la linea que el hash apunta
  fread(&miBuscador,sizeof(miBuscador),1,fp);  // lee en fp el tiempo correspondiente 
  printf("%f",miBuscador.tiempo);
  //respuesta a solicitud
  if(miBuscador.id==valores[0] && valores[0] != 0){
    return miBuscador.tiempo;
    
  }
  else{
    
    miBuscador.tiempo = -1.0f;
    // printf("N/A \n");
    return -1.0f; //
      }  
 
fclose(fp);
free(array);
return 0;
}