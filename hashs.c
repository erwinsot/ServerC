#include <stdio.h>
struct mistru{
  float origen;
  float destino;
  float hora;
} ;

typedef struct  estru mistru;

long int myhash(struct mistru hasher){
    long int hash2=0;
    int inicial=(int) hasher.origen; //(int)valores[0];
    int destino= (int) hasher.destino;  //(int)valores[1];
    int hora=(int) hasher.hora; //valores[2];


  if(inicial==0 ){
    inicial=1;
  }
  if(destino==0){
    destino=1;
  }

  int top=inicial*230400;
  int down=(inicial-1)*230400;
  int topdes=destino+1*192;
  int downdes=destino*192;
  int tophora=hora*8;

  int diff=top-down;

  hash2= hash2 % 144;
  int hash=down+downdes+tophora;


//  printf("el hash es---------------------------> %li\n",hash);

  return hash;

}