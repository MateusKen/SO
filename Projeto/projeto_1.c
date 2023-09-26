/*
Nome: Erik Samuel Viana Hsu
TIA: 32265921

Nome: Mateus Kenzo Iochimoto
TIA: 32216289

Nome: Rodrigo Machado de Assis Oliveira de Lima
TIA: 32234678

Nome: Thiago Shihan Cardoso Toma
TIA: 32210744
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

float saldoA;
float saldoB;


void *transfAB(void *args){
  float valorAB = *(float *)args;

  //Lê o próprio saldo
  FILE *arqA = fopen("saldoA.txt", "w");
  if (arqA == NULL){
    printf("Erro ao abrir o arquivo saldoA.txt");
  }

  fscanf(arqA, "%f", &saldoA);
  float saldo_novoA = saldoA - valorAB;  //desconta o valor do saldo de A
  fprintf(arqA, "%.2f", saldo_novoA);  //atualiza o saldo de A
  fclose(arqA);


  
  //Transfere para o cliente B
  FILE *arqB = fopen("saldoB.txt", "w");
  if (arqB == NULL){
    printf("Erro ao abrir o arquivo saldoB.txt");
  }

  fscanf(arqB, "%f", &saldoB);
  float saldo_novoB = saldoB + valorAB;  //saldo de B + valor que A mandou
  fprintf(arqB, "%.2f", saldo_novoB); //atualiza saldo de B em saldoB.txt
  fclose(arqB);
  sleep(3);
  return NULL;
}


void *transfBA(void *args){
  float valorBA = *(float *)args;

  //Lê o próprio saldo
  FILE *arqB = fopen("saldoB.txt", "w");
  if (arqB == NULL){
    printf("Erro ao abrir o arquivo saldoB.txt");
  }
  fscanf(arqB, "%f", &saldoB);
  float saldo_novoB = saldoB - valorBA;  //desconta o valor do saldo de B
  fprintf(arqB, "%.2f", saldo_novoB);  // atualiza o saldo de B
  fclose(arqB);

  
  //Transfere para o cliente A
  FILE *arqA = fopen("saldoA.txt", "w");
  if (arqA == NULL){
    printf("Erro ao abrir o arquivo saldoA.txt");
  }

  fscanf(arqA, "%f", &saldoA);
  float saldo_novoA = saldoA + valorBA;  //saldo de A + valor que B mandou
  fprintf(arqA, "%.2f", saldo_novoA); //atualiza saldo de A em saldoA.txt
  fclose(arqA);
  sleep(3);
  return NULL;
}

int main(void) {
  //declarando valores de transferencias
  float valorAB = 50;  //50 reais de A para B
  float valorBA = 200; //200 reais de B para A
  
  //ponteiros para os arquivos de saldoA.txt e saldoB.txt
  FILE *arquivoA;
  FILE *arquivoB;   
  
  //abertura dos arquivos
  arquivoA = fopen("saldoA.txt", "r");
  arquivoB = fopen("saldoB.txt", "r");
  if(arquivoA == NULL || arquivoB == NULL){
    printf("Erro ao abrir os arquivos");
  }
  
  //ler os saldos dos clientes
  fscanf(arquivoA, "%f", &saldoA);
  fscanf(arquivoB, "%f", &saldoB);

  //fechando os arquivos
  fclose(arquivoA);
  fclose(arquivoB);
  
  //criando threads
  pthread_t clienteA; 
  pthread_t clienteB;
  pthread_create(&clienteA, NULL, transfAB, &valorAB);
  pthread_create(&clienteB, NULL, transfBA, &valorBA);

  sleep(3);
  
  return 0;
}
