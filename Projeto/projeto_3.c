#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

float saldoA;
float saldoB;

sem_t semA, semB,semT;


void *transfAB(void *args){
  float valorAB = *(float *)args;
  sem_wait(&semT); // Travamento de semaforo Auxiliar para evitar deadlock
  sem_wait(&semA);
  printf("\nCliente A travando saldo A\n");
  sleep(2);
  sem_wait(&semB);
  printf("Cliente A travando saldo B\n");
  //Lê o próprio saldo
  FILE *arqA = fopen("saldoA.txt", "w");
  if (arqA == NULL){
    printf("Erro ao abrir o arquivo saldoA.txt");
  }

  
  fscanf(arqA, "%f", &saldoA);
  float saldo_novoA = saldoA - valorAB;  //desconta o valor do saldo de A
  fprintf(arqA, "%.2f", saldo_novoA);  //atualiza o saldo de A
  fclose(arqA);

  printf("Valor de transferencia debitado do saldo A\n");
  
  //Transfere para o cliente B
  FILE *arqB = fopen("saldoB.txt", "w");
  if (arqB == NULL){
    printf("Erro ao abrir o arquivo saldoB.txt");
  }

  
  fscanf(arqB, "%f", &saldoB);
  float saldo_novoB = saldoB + valorAB;  //saldo de B + valor que A mandou

  fprintf(arqB, "%.2f", saldo_novoB); //atualiza saldo de B em saldoB.txt
  fclose(arqB);

  printf("Valor de transferencia creditado ao saldo B\n");
  
  sem_post(&semB);
  printf("Cliente A destravando saldo B\n");
  sem_post(&semA);
  printf("Cliente A destravando saldo A\n\n");
  sem_post(&semT);
  pthread_exit(NULL);
  sleep(2);
  return NULL;
}


void *transfBA(void *args){
  float valorBA = *(float *)args;
  sem_wait(&semT); // Travamento de semaforo Auxiliar para evitar deadlock
  sem_wait(&semB);
  printf("\nCliente B travando saldo B\n");
  sleep(2);
  sem_wait(&semA);
  printf("Cliente B travando saldo A\n");
  //Lê o próprio saldo
  FILE *arqB = fopen("saldoB.txt", "r+");
  if (arqB == NULL){
    printf("Erro ao abrir o arquivo saldoB.txt");
  }

  fscanf(arqB, "%f", &saldoB);
  float saldo_novoB = saldoB - valorBA;  //desconta o valor do saldo de B
  fprintf(arqB, "%.2f", saldo_novoB);  // atualiza o saldo de B
  fclose(arqB);

  printf("Valor de transferencia debitado do saldo B\n");
  //Transfere para o cliente A
  FILE *arqA = fopen("saldoA.txt", "w");
  if (arqA == NULL){
    printf("Erro ao abrir o arquivo saldoA.txt");
  }
  
  fscanf(arqA, "%f", &saldoA);
  float saldo_novoA = saldoA + valorBA;  //saldo de A + valor que B mandou
  fprintf(arqA, "%.2f", saldo_novoA); //atualiza saldo de A em saldoA.txt
  fclose(arqA);

  printf("Valor de transferencia creditado ao saldo A\n");
  
  sem_post(&semA);
  printf("Cliente B destravando saldo A\n");
  sem_post(&semB);
  printf("Cliente B destravando saldo B\n\n");
  sem_post(&semT);
  pthread_exit(NULL);
  sleep(2);
  return NULL;
}



int main(void) {
   //inicializar semaforos
  sem_init(&semA, 0, 1);
  sem_init(&semB, 0, 1);
  sem_init(&semT, 0, 1);
  
  //declarando valores de transferencias
  float valorAB = 50;  //50 reais de A para B
  float valorBA = 100; //200 reais de B para A
  
  //ponteiros para os arquivos de saldoA.txt e saldoB.txt
  FILE *arquivoA;
  FILE *arquivoB;  
  //declarando variável do tipo pthread
  pthread_t clienteA; 
  pthread_t clienteB; 
  
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
  pthread_create(&clienteA, NULL, transfAB, &valorAB);
  pthread_create(&clienteB, NULL, transfBA, &valorBA);

  pthread_join(clienteA, NULL);
  pthread_join(clienteB, NULL);

  sleep(5);

  return 0;
}