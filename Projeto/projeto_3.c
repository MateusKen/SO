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
#include <semaphore.h>

float saldoA;
float saldoB;

sem_t semA, semB,semT;


void *transfAB(void *args){
  float valorAB = *(float *)args;
  sem_wait(&semT); // Travamento de semaforo Auxiliar para evitar deadlock
  sem_wait(&semA); //Travamento recurso saldo A
  printf("\nCliente A travando saldo A\n");
  sleep(2);
  sem_wait(&semB); //Travamento recurso saldo B
  printf("Cliente A travando saldo B\n");
  
  //Lê o próprio saldo
  FILE *arqA = fopen("saldoA.txt", "r");
  if (arqA == NULL){
    printf("Erro ao abrir o arquivo saldoA.txt");
  }

  
  fscanf(arqA, "%f", &saldoA);
  fclose(arqA);
  float saldo_novoA = saldoA - valorAB;  //desconta o valor do saldo de A
  
  //Atualiza o saldo de A
  arqA = fopen("saldoA.txt", "w");
  if(arqA == NULL){
    printf("Erro ao abrir o arquivo saldoA.txt");
  }
  fprintf(arqA, "%.2f", saldo_novoA);
  fclose(arqA);

  printf("Valor de transferencia (R$%.2f) debitado do saldo A\nNovo saldo A: R$%.2f\n",valorAB,saldo_novoA);
  
  //Transfere para o cliente B
  FILE *arqB = fopen("saldoB.txt", "r");
  if (arqB == NULL){
    printf("Erro ao abrir o arquivo saldoB.txt");
  }
  fscanf(arqB, "%f", &saldoB);
  fclose(arqB);
  float saldo_novoB = saldoB + valorAB;  //saldo de B + valor que A mandou

  //Atualiza o saldo de B
  arqB = fopen("saldoB.txt", "w");
  if (arqB == NULL){
    printf("Erro ao abrir o arquivo saldoB.txt");
  }
  fprintf(arqB, "%.2f", saldo_novoB); //atualiza saldo de B em saldoB.txt
  fclose(arqB);

  printf("Valor de transferencia (R$%.2f) creditado ao saldo B\nNovo saldo B: R$%.2f\n",valorAB,saldo_novoB);
  
  sem_post(&semB); //Destrava recurso saldo B
  printf("Cliente A destravando saldo B\n");
  sem_post(&semA); //Destrava recurso saldo A
  printf("Cliente A destravando saldo A\n\n");
  sem_post(&semT); //Destrava semaforo auxiliar
  pthread_exit(NULL);
  sleep(2);
  return NULL;
}


void *transfBA(void *args){
  float valorBA = *(float *)args;
  sem_wait(&semT); // Travamento de semaforo Auxiliar para evitar deadlock
  sem_wait(&semB); //Travamento recurso saldo B
  printf("\nCliente B travando saldo B\n");
  sleep(2);
  sem_wait(&semA); //Travamento recurso saldo A
  printf("Cliente B travando saldo A\n");
  
  //Lê o próprio saldo de B
  FILE *arqB = fopen("saldoB.txt", "r");
  if (arqB == NULL){
    printf("Erro ao abrir o arquivo saldoB.txt");
  }
  fscanf(arqB, "%f", &saldoB);
  fclose(arqB);
  float saldo_novoB = saldoB - valorBA;  //desconta o valor do saldo de B

  //Atualiza o saldo de B
  arqB = fopen("saldoB.txt", "w");
  if (arqB == NULL){
    printf("Erro ao abrir o arquivo saldoB.txt");
  }
  fprintf(arqB, "%.2f", saldo_novoB);
  fclose(arqB);

  printf("Valor de transferencia (R$%.2f) debitado do saldo B\nNovo saldo B: R$%.2f\n",valorBA,saldo_novoB);
  
  //Transfere para o cliente A - pega o saldo de A
  FILE *arqA = fopen("saldoA.txt", "r");
  if (arqA == NULL){
    printf("Erro ao abrir o arquivo saldoA.txt");
  }
  fscanf(arqA, "%f", &saldoA);
  fclose(arqA);
  float saldo_novoA = saldoA + valorBA;  //saldo de A + valor que B mandou

  //Atualiza o saldo de A
  arqA = fopen("saldoA.txt", "w");
  if (arqA == NULL){
    printf("Erro ao abrir o arquivo saldoA.txt");
  }
  fprintf(arqA, "%.2f", saldo_novoA);
  fclose(arqA);

  printf("Valor de transferencia (R$%.2f) creditado ao saldo A\nNovo saldo A: R$%.2f\n",valorBA,saldo_novoA);
  
  sem_post(&semA); //Destrava recurso saldo A
  printf("Cliente B destravando saldo A\n");
  sem_post(&semB); //Destrava recurso saldo B
  printf("Cliente B destravando saldo B\n\n");
  sem_post(&semT); // Destrava semaforo auxiliar
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
  float valorAB = 300;  //valor em reais de A para B
  float valorBA = 100; //valor em reais de B para A
  
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

  // Print Saldo das contas
  printf("Saldo A:R$ %.2f\n",saldoA);
  printf("Saldo B:R$ %.2f\n",saldoB);
  
  //criando threads
  pthread_create(&clienteA, NULL, transfAB, &valorAB);
  pthread_create(&clienteB, NULL, transfBA, &valorBA);

  pthread_join(clienteA, NULL);
  pthread_join(clienteB, NULL);

  sleep(5);
  return 0;
}
