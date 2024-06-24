#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mips.h"

int main(){
  back *reserva=malloc(sizeof(back));
  int *n_instrucoes = (int *)malloc(sizeof(int));
  dados *memoria2=malloc(sizeof(dados));
  Memoria mem[256];
  int *count = malloc(sizeof(int));
  int *registrador=malloc(8*sizeof(int));
  iniciarReg(registrador);
  *count = 0;
  *n_instrucoes = 0;
  inicializarMemoriaDados(memoria2);
  char *nome=malloc(sizeof(char)*51);
  printf("Qual o nome do arquivo de instruções? ");
  fgets(nome, 51, stdin);
  if (nome[strlen(nome)-1] == '\n'){
    nome[strlen(nome)-1] = '\0';
  }
  carregarMemoria(nome, mem, count, n_instrucoes);
  char p;
  do{


  printf("\n================================================================\n");
  printf("\t\t\t    PIPELINE\n");
  printf("================================================================\n");
//  printf("\t    PC: %i Instrução: %s Estado: %i\n\n", *PC, mem[*PC].instrucoes.instrucao, sinais->estado_atual);
//  printf("\t    Instrução em Assembly: ");
//  if(mem->d_i==1){
//    traduzirInstrucao(mem, PC);
//  }
  printf("\n");
  printf("\t\t (r) (RUN) Executar todo o arquivo    \n");
  printf("\t\t (e) (STEP) Executar uma linha        \n");
  printf("\t\t (b) (BACK) Voltar uma instrução      \n");
  printf("\t\t (v) Ver Registradores               \n");
  printf("\t\t (a) Ver Instrução Atual             \n");
  printf("\t\t (i) Ver Todas as Instruções         \n");
  printf("\t\t (d) Ver Memória de Dados            \n");
  printf("\t\t (s) Salvar .asm                     \n");
  printf("\t\t (t) Salvar .dat                     \n");
//  printf("\t\t (c) Carregar .dat                   \n");
  printf("\t\t (x) Sair                            \n");
  printf("================================================================\n");
  printf("\t\tSelecione: ");
  scanf("%s",&p);

  switch(p){
    case 'r':
      //return 1;
      break;
    case 'e':
    //  return 0;
      break;
    case 'b':
  //  if (!isEmpty(pilha)) {
    //  fback(sinais, mem, PC, regitemp, registrador, pilha, 1);
  //  }
  //  else {
  //    printf("Nenhuma instrução para voltar\n");
  //  }
  //    return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
    case 'v':
    //  verReg(registrador);struct nodo_Pilha * criaNodo(){
  //  verReg(registrador, regitemp, sinais);

  //    return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
    case 'a':
  //  verInstrucaoAtual(mem, *PC);
    //  verinstrucoes(mem,count,0, n_instrucoes);
  //    return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
    case 'i':
  //  verInstrucoesTodas(mem, *PC);
      //verinstrucoes(mem, count,1, n_instrucoes);
    //  return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
    case 'd':
    //  vermemoriadados(mem, PC);
    //  return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
    case 's':
    //  salvarAsm(mem);
    //  return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
    case 't':
    //  salvarDados(mem);
    //  return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
    case 'c':
    //  carregarDados(memoria2);
    //  return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
    case 'x':
      printf("Programa finalizado\n");
    //  return 3;
      break;
    default:
      printf("Opção inválida\n");
    //  return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;
  }
  //return 3;
}while(p!='x');
}
