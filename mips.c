#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>
#include "mips.h"


//funcoes principais

struct instrucao memReg(struct instrucao *mem, int pc){
  return mem[pc];
}

void memDados(int *memD, int endereco, int dado, int EscMem, int *saida){
 if(EscMem == 0){
   *saida = memD[endereco]; 
 } 
  else{
   memD[endereco] = dado;
  }
}

void ula(int valor1, int valor2, int *saida, int *flag, int ULAop){
  (*flag) = 0;
  switch(ULAop){

    case 0:
    if(valor1 + valor2 > 127 || valor1 + valor2 < -128){
    *flag = 1;
    }
    *saida = valor1 + valor2;
    break;

    case 2:
    if(valor1 - valor2 > 127 || valor1 - valor2 < -128){
    *flag = 1;
    }
    *saida = valor1 - valor2;
    break;

    case 4:
    if((valor1 & valor2) > 127 || (valor1 & valor2) < -128){
     *flag = 1;
    }
    *saida = valor1 & valor2;
    break;

    case 5:
    if((valor1 | valor2) > 127 || (valor1 | valor2) < -128){
    *flag = 1;
    }
    *saida = valor1 | valor2;
    break;

    case 1:
    if(valor1 == valor2){
    *flag = 1;
    *saida = 0;
    }
    break;

    }
}

void BancoRegistradores(int *registradores, int ReadEnd1, int ReadEnd2, int WriteEnd, int dado, int *saida1, int *saida2, int EscReg){
  if(saida1 != NULL && saida2 != NULL){
    *saida1 = registradores[ReadEnd1];
    *saida2 = registradores[ReadEnd2];
  }
  if(EscReg == 1){
     registradores[WriteEnd] = dado;
  }
}

struct controle *UC(struct controle *sinais, struct regiBI_ID *bits){
  switch(bits->inst->opcode){
    case 0:
      sinais->RegDst = 1;
      sinais->ULAOp = bits->inst->funct;
      sinais->ULAFonte = 0;
      sinais->DVC = 0;
      sinais->DVI = 0;
      sinais->EscMem = 0;
      sinais->EscReg = 1;
      sinais->MemParaReg = 1;
      break;

     case 1:
      sinais->RegDst = 1;
      sinais->ULAOp = bits->inst->funct;
      sinais->ULAFonte = 0;
      sinais->DVC = 0;
      sinais->DVI = 0;
      sinais->EscMem = 0;
      sinais->EscReg = 1;
      sinais->MemParaReg = 1;
      break;


    case 2:
      sinais->RegDst = 0;
      sinais->ULAOp = 0;
      sinais->ULAFonte = 0;
      sinais->DVC = 0;
      sinais->DVI = 1;
      sinais->EscMem = 0;
      sinais->EscReg = 0;
      sinais->MemParaReg = 0;
      break;

    case 4:
      sinais->RegDst = 0;
      sinais->ULAOp = 0;
      sinais->ULAFonte = 1;
      sinais->DVC = 0;
      sinais->DVI = 0;
      sinais->EscMem = 0;
      sinais->EscReg = 1;
      sinais->MemParaReg = 1;
      break;

    case 8:
      sinais->RegDst = 0;
      sinais->ULAOp = 1;
      sinais->ULAFonte = 0;
      sinais->DVC = 1;
      sinais->DVI = 0;
      sinais->EscMem = 0;
      sinais->EscReg = 0;
      sinais->MemParaReg = 0;
      break;

    case 11:
      sinais->RegDst = 0;
      sinais->ULAOp = 0;
      sinais->ULAFonte = 1;
      sinais->DVC = 0;
      sinais->DVI = 0;
      sinais->EscMem = 0;
      sinais->EscReg = 1;
      sinais->MemParaReg = 0;
      break;

    case 15:
      sinais->RegDst = 0;
      sinais->ULAOp = 0;
      sinais->ULAFonte = 0;
      sinais->DVC = 0;
      sinais->DVI = 0;
      sinais->EscMem = 1;
      sinais->EscReg = 0;
      sinais->MemParaReg = 1;
      break;
  }
  return sinais;
}

//funcoes de apoio

int carregarMemoria(char *nomeArquivo, struct instrucao *mem){

  int i=0;
  FILE *arquivo;
  arquivo = fopen(nomeArquivo, "r");

  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return 0;
  }

  while (fscanf(arquivo, "%16s", mem[i].instrucao) != EOF && i < 256) {
    //strncpy(op, mem[i].instrucao, 4);
    //mem[i].opcode=bi_dec(op);
    decodificarOpcode(mem, i);
    i++;
  }
  fclose(arquivo);
  return i;
}
void carregarDados(char *nomeArquivo, int *memD) {
    FILE *arquivo;
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para carregar.\n");
        return;
    }
    for (int i = 0; i < 256; i++) {
       fscanf(arquivo, "%d\n", &memD[i]); // Correção aqui
    }
    fclose(arquivo);
    printf("Dados carregados com sucesso do arquivo dados.dat.\n");
}

int bi_dec(char *bin){
  int i=strlen(bin);
  int value,deci=0,k=0;
  for(int j=i;j>0;j--){
    if(bin[j-1]=='0'){
      value=0;
    }
    else{
      value=1;
    }
  deci=deci+value*(pow(2,k));
  k++;
  }
  return(deci);
}

void decodificarOpcode(struct instrucao *mem, int n_instrucoes){

  char b11_9[4];
  char b8_6[4];
  char b5_3[4];
  char b5_0[7];
  char b0_6[8];
  char op[5];
  char funct[4];

  for(int i=0;i<3;i++){
    b11_9[i]=mem[n_instrucoes].instrucao[i+4];
    b8_6[i]=mem[n_instrucoes].instrucao[i+7];
    b5_3[i]=mem[n_instrucoes].instrucao[i+10];
    funct[i]=mem[n_instrucoes].instrucao[i+13];
    op[i]=mem[n_instrucoes].instrucao[i];
  }
  op[3]=mem[n_instrucoes].instrucao[3];

  for(int i=0;i<7;i++){
     b5_0[i]=mem[n_instrucoes].instrucao[i+10];
     b0_6[i]=mem[n_instrucoes].instrucao[i+9];
  }
  b0_6[6]=mem[n_instrucoes].instrucao[15];


  b11_9[3] = '\0';
  b8_6[3] = '\0';
  b5_3[3] = '\0';
  b5_0[6] = '\0';
  b0_6[7] = '\0';
  op[4] = '\0';
  funct[3] = '\0';

  mem[n_instrucoes].b0_6=bi_dec(b0_6);
  mem[n_instrucoes].b5_0=bi_dec(b5_0);
  mem[n_instrucoes].b8_6=bi_dec(b8_6);
  mem[n_instrucoes].b5_3=bi_dec(b5_3);
  mem[n_instrucoes].b11_9=bi_dec(b11_9);
  mem[n_instrucoes].opcode=bi_dec(op);
  mem[n_instrucoes].funct=bi_dec(funct);

}

struct controle * iniciarConrole(){
  struct controle *aux=(struct controle *)malloc(sizeof(struct controle));
  //aux->louD=0;
  aux->EscMem=0;
  //aux->IREsc=0;
  aux->RegDst=0;
  //aux->EscReg=0;
  aux->MemParaReg=0;
  aux->ULAFonte=0;
  //aux->ULAFonteDown=0;
  aux->ULAOp=0;

  //aux->branch=0;
  //aux->PCEsc=0;
  //aux->FontePC=0;
  return aux;
}
/*
int menu(struct controle *sinais, int *PC, struct regiS  *regis, int *registrador, int *mem, struct variaveis *var){

  char p;

  printf("\n\n================================================================\n");
  printf("\t\t\t    PIPELINE\n");
  printf("================================================================\n");
  printf("\t    PC: %i Instrução: %s Estado: %i\n\n", *PC, mem[*PC].instrucoes.instrucao, sinais->estado_atual);
  printf("\t    Instrução em Assembly: ");
  if(mem->d_i==1){
    traduzirInstrucao(mem, PC);
  }

  printf("\n");
  printf("\t\t (r) (RUN) Executar todo o arquivo    \n");
  printf("\t\t (e) (STEP) Executar uma linha        \n");
  printf("\t\t (b) (BACK) Voltar uma instrução      \n");
  printf("\t\t (v) Ver Estado                       \n");
  printf("\t\t (a) Ver Instrução Atual             \n");
  printf("\t\t (i) Ver registradores               \n");
  printf("\t\t (d) Ver Memória de Dados            \n");
  printf("\t\t (s) Ver Sinais                      \n");
  printf("\t\t (t) Ver Variáveis                   \n");
  printf("\t\t (c) Ver Registradores Temporários   \n");
  printf("\t\t (x) Sair                            \n");
  printf("================================================================\n");
  printf("\t\tSelecione: ");
  scanf("%s",&p);

  switch(p){
    case 'r':
      return 1;
      break;
    case 'e':
      return 0;
      break;

    case 'b':
    if (!isEmpty(pilha)) {
      fback(sinais, mem, PC, regitemp, registrador, pilha, 1);
    }
    else {
      printf("Nenhuma instrução para voltar\n");
    }
      return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;

    case 'v':
    verEstado(regis);
      return menu(sinais, PC, regis, registrador, mem, var);
      break;


    case 'a':
    verInstrucaoAtual(mem, *PC);
    //  verinstrucoes(mem,count,0, n_instrucoes);
      return menu(sinais, mem, PC, regitemp, registrador, pilha);
      break;



    case 'i':
      verReg(registrador);
      return menu(sinais, PC, regis, registrador, mem, var);
      break;
    case 'd':
      vermemoria(mem);
      return menu(sinais, PC, regis, registrador, mem, var);
      break;

    case 'c':
      verRegT(regis);
      return menu(sinais, PC, regis, registrador, mem, var);
      break;
    case 't':
      verVariaveis(var);
      return menu(sinais, PC, regis, registrador, mem, var);
      break;
    case 's':
      verSinais(sinais);
      return menu(sinais, PC, regis, registrador, mem, var);
      break;

    case 'x':
      printf("Programa finalizado\n");
      return 3;
      break;
    default:
      printf("Opção inválida\n");
      return menu(sinais, PC, regis, registrador, mem, var);
      break;

  }
  return 3;
}*/

int * iniciarRegi(){

  int *aux=(int *)malloc(8*sizeof(int));  
  for(int i=0; i<8; i++){
    aux[i] = 0;
  }
  return aux;

};

int * iniciarMemD(){

  int *aux=(int *)malloc(256*sizeof(int));  
  for(int i=0; i<256; i++){
    aux[i] = 0;
  }
  return aux;

};

struct regiS * iniciarRegiS(){

  struct regiS *aux=(struct regiS *)malloc(sizeof(struct regiS));
  aux->bi_di = (struct regiBI_ID *)malloc(sizeof(struct regiBI_ID));
  aux->di_ex = (struct regiDI_EX *)malloc(sizeof(struct regiDI_EX));
  aux->ex_mem = (struct regiEX_MEM *)malloc(sizeof(struct regiEX_MEM));
  aux->mem_er = (struct regiMEM_ER *)malloc(sizeof(struct regiMEM_ER));

  aux->bi_di->inst = (struct instrucao *)malloc(sizeof(struct instrucao));
  aux->di_ex->inst = (struct instrucao *)malloc(sizeof(struct instrucao));
  aux->ex_mem->inst = (struct instrucao *)malloc(sizeof(struct instrucao));
  aux->mem_er->inst = (struct instrucao *)malloc(sizeof(struct instrucao));

  aux->di_ex->sinais = (struct controle *)malloc(sizeof(struct controle));
  aux->ex_mem->sinais = (struct controle *)malloc(sizeof(struct controle));
  aux->mem_er->sinais = (struct controle *)malloc(sizeof(struct controle));

  aux->bi_di->var = inciarVariaveis();
  aux->di_ex->var = inciarVariaveis();
  aux->ex_mem->var = inciarVariaveis();
  aux->mem_er->var = inciarVariaveis();

  return aux;
};

struct variaveis * inciarVariaveis(){

      struct variaveis *aux = (struct variaveis *)malloc(sizeof(struct variaveis));
      aux->flag=(int *)malloc(sizeof(int));
      *aux->flag = 0;
      aux->saida1 = (int *)malloc(sizeof(int));
      *aux->saida1 = 0;
      aux->saida2 = (int *)malloc(sizeof(int));
      *aux->saida1 = 0;
      aux->ULA = (int *)malloc(sizeof(int));
      *aux->ULA = 0;
      aux->saidaMem = (int *)malloc(sizeof(int)); 
      *aux->saidaMem = 0;
      aux->muxDVC = 0;
      aux->muxDVI = 0;
      aux->muxloaD = 0;
      aux->muxRegDst = 0;
      aux->muxMemReg = 0;
      aux->muxULA = 0;
      return aux;

};

struct regiS * copy(struct regiS *regS2){

  struct regiS *aux = iniciarRegiS(); 

  *aux->bi_di->var =  *regS2->bi_di->var;
  *aux->bi_di->inst =  *regS2->bi_di->inst;
  aux->bi_di->pc =  regS2->bi_di->pc;

  aux->di_ex->A = regS2->di_ex->A;
  aux->di_ex->B = regS2->di_ex->B;
  aux->di_ex->pc = regS2->di_ex->pc;
  *aux->di_ex->inst = *regS2->di_ex->inst;
  *aux->di_ex->sinais = *regS2->di_ex->sinais;
  *aux->di_ex->var = *regS2->di_ex->var;

  *aux->ex_mem->inst = *regS2->ex_mem->inst;
  aux->ex_mem->saidaULA = regS2->ex_mem->saidaULA;
  aux->ex_mem->B = regS2->ex_mem->B;
  aux->ex_mem->muxRegDst = regS2->ex_mem->muxRegDst;
  *aux->ex_mem->sinais = *regS2->ex_mem->sinais;
  *aux->ex_mem->var = *regS2->ex_mem->var;
  aux->ex_mem->pc = regS2->ex_mem->pc;
  aux->ex_mem->flag = regS2->ex_mem->flag;

  aux->mem_er->pc = regS2->mem_er->pc;
  aux->mem_er->muxRegDst = regS2->mem_er->muxRegDst;
  aux->mem_er->saidaULA = regS2->mem_er->saidaULA;
  aux->mem_er->saidaMem = regS2->mem_er->saidaMem;
  *aux->mem_er->inst = *regS2->mem_er->inst;
  *aux->mem_er->sinais = *regS2->mem_er->sinais;
  *aux->mem_er->var = *regS2->mem_er->var;

  return aux;
};

void verEstado(struct regiS  *regis){
    printf("\nBI_DI [%s]", regis->bi_di->inst->instrucao);
    printf("\nDI EX [%s]", regis->di_ex->inst->instrucao);
    printf("\nEX_MEM [%s]", regis->ex_mem->inst->instrucao);
    printf("\nMEM_ER [%s]", regis->mem_er->inst->instrucao);
};

void verReg(int *registrador){
  for(int i=0;i<8;i++){
    printf("\nRegistrador %i: %i \n",i, registrador[i]);
  }
}

void vermemoria(int *mem){
  for(int i=0;i<256;i++){
    printf("\n[%i]: %i \n",i, mem[i]);
    }	
};

void verRegT(struct regiS  *regisT){

  printf("\n\nBI_DI \nInstrução: [%s]\nPC: %i", regisT->bi_di->inst->instrucao, regisT->bi_di->pc);
  printf("\n\nDI EX \nInstrução: [%s]\nPC: %i\nA: %i\nB: %i", regisT->di_ex->inst->instrucao, regisT->di_ex->pc, regisT->di_ex->A, regisT->di_ex->B);
  printf("\n\nEX_MEM\nInstrução: [%s]\nPC: %i\nSa´da ULA: %i\nB: %i\nRegistrador destino: %i", regisT->ex_mem->inst->instrucao, regisT->ex_mem->pc, regisT->ex_mem->saidaULA, regisT->ex_mem->B, regisT->ex_mem->muxRegDst);
  printf("\n\nMEM_ER\nInstrução: [%s]\nPC: %i\nRegistrador destino: %i", regisT->mem_er->inst->instrucao, regisT->mem_er->pc, regisT->mem_er->muxRegDst);

};

void verVariaveis(struct variaveis *var){
  printf("\nFlag: %i, \nmuxDVC: %i, \nmuxDVI: %i, \nmuxloaD: %i, \nmuxRegDst: %i, \nmuxMemReg: %i, \nmuxULA: %i, \nsaida1: %i, \nsaida2: %i, \nULA: %i, \nsaidaMem: %i", *var->flag, var->muxDVC, var->muxDVI, var->muxloaD, var->muxRegDst, var->muxMemReg, var->muxULA, *var->saida1, *var->saida2, *var->ULA, *var->saidaMem); 
};

void verSinais(struct controle *sinais){
  printf("\nmuxEscMem: %i, \nRegDst: %i, \nEscReg: %i, \nMemParaReg: %i, \nULAFonte: %i, \nULAOp: %i, \nDVI: %i, \nDVC: %i",sinais->EscMem, sinais->RegDst, sinais->EscReg, sinais->MemParaReg, sinais->ULAFonte, sinais->ULAOp, sinais->DVI, sinais->DVC);
};


void fback(int *registradores, int *memD, int *pc, struct controle *sinais, struct variaveis *var, struct regiS *regTemp, Pilha *pilha, int chose, int *countbeq){
  if(chose == 0){
    Nodo *aux = criaNodo(registradores, memD, pc, sinais, var, regTemp, countbeq);
    push(pilha, aux->estado);
    free(aux);
  } 
  else {
	*countbeq = pilha->top->estado->countdesvio;
    *pc = pilha->top->estado->pc;
    for(int i = 0; i<8; i++){
      registradores[i] = pilha->top->estado->registradores[i];
    }
    for(int i = 0; i<256; i++){
      memD[i] = pilha->top->estado->memD[i];
    }
    *sinais = pilha->top->estado->sinais;

  *var->flag = *pilha->top->estado->var.flag;
  var->muxDVC = pilha->top->estado->var.muxDVC;
  var->muxDVI = pilha->top->estado->var.muxDVI;
  var->muxloaD = pilha->top->estado->var.muxloaD;
  var->muxRegDst = pilha->top->estado->var.muxRegDst;
  var->muxMemReg = pilha->top->estado->var.muxMemReg;
  var->muxULA = pilha->top->estado->var.muxULA;
  *var->saida1 = *pilha->top->estado->var.saida1;
  *var->saida2 = *pilha->top->estado->var.saida2;
  *var->ULA = *pilha->top->estado->var.ULA;
  *var->saidaMem = *pilha->top->estado->var.saidaMem;

  regTemp->bi_di->pc = pilha->top->estado->regTemp->bi_di->pc;
  *regTemp->bi_di->inst = *pilha->top->estado->regTemp->bi_di->inst;

  regTemp->di_ex->A = pilha->top->estado->regTemp->di_ex->A;
  regTemp->di_ex->B = pilha->top->estado->regTemp->di_ex->B;
  regTemp->di_ex->pc = pilha->top->estado->regTemp->di_ex->pc;
  *regTemp->di_ex->sinais = *pilha->top->estado->regTemp->di_ex->sinais;
  *regTemp->di_ex->inst = *pilha->top->estado->regTemp->di_ex->inst;

  regTemp->ex_mem->DVC = pilha->top->estado->regTemp->ex_mem->DVC;
  regTemp->ex_mem->pc = pilha->top->estado->regTemp->ex_mem->pc;
  regTemp->ex_mem->saidaULA = pilha->top->estado->regTemp->ex_mem->saidaULA;
  regTemp->ex_mem->B = pilha->top->estado->regTemp->ex_mem->B;
  regTemp->ex_mem->muxRegDst = pilha->top->estado->regTemp->ex_mem->muxRegDst;  
  regTemp->ex_mem->flag = pilha->top->estado->regTemp->ex_mem->flag;
  *regTemp->ex_mem->sinais = *pilha->top->estado->regTemp->ex_mem->sinais;
  *regTemp->ex_mem->inst = *pilha->top->estado->regTemp->ex_mem->inst;


  regTemp->mem_er->pc = pilha->top->estado->regTemp->mem_er->pc;
  regTemp->mem_er->muxRegDst = pilha->top->estado->regTemp->mem_er->muxRegDst;
  regTemp->mem_er->saidaULA = pilha->top->estado->regTemp->mem_er->saidaULA;
  regTemp->mem_er->saidaMem = pilha->top->estado->regTemp->mem_er->saidaMem;
  *regTemp->mem_er->inst = *pilha->top->estado->regTemp->mem_er->inst;
  *regTemp->mem_er->sinais = *pilha->top->estado->regTemp->mem_er->sinais;

   pop(pilha);
  }
}

void push(Pilha* stack, back *estado) {
  Nodo* novonodo = (Nodo*)malloc(sizeof(Nodo));
  novonodo->estado = estado;
  novonodo->next = stack->top;
  stack->top = novonodo;
  stack->tam++;
}

void pop(Pilha *pilha) {
  if (pilha->tam == 0 ) {
      printf("Pilha vazia\n");
      exit(1);
  }
  pilha->top = pilha->top->next;
  pilha->tam--;
}

Nodo *criaNodo(int *registradores, int *memD, int *pc, struct controle *sinais, struct variaveis *var, struct regiS *regTemp, int *countBeq){

  Nodo *aux=(Nodo *)malloc(sizeof(Nodo));
  aux->estado = printn(registradores, memD, pc, sinais, var, regTemp, countBeq);

  return aux;
}

back *printn(int *registradores, int *memD, int *pc, struct controle *sinais, struct variaveis *var, struct regiS *regTemp, int *countBeq){
  back *aux=iniciarBack();

  aux->countdesvio = *countBeq;
  aux->pc = *pc;

  for(int i = 0; i<8; i++){
    aux->registradores[i] = registradores[i];
  }
  for(int i = 0; i<256; i++){
    aux->memD[i] = memD[i];
  }

  aux->sinais = *sinais;

  *aux->var.flag = *var->flag;
  aux->var.muxDVC = var->muxDVC;
  aux->var.muxDVI = var->muxDVI;
  aux->var.muxloaD = var->muxloaD;
  aux->var.muxRegDst = var->muxRegDst;
  aux->var.muxMemReg = var->muxMemReg;
  aux->var.muxULA = var->muxULA;
  *aux->var.saida1 = *var->saida1;
  *aux->var.saida2 = *var->saida2;
  *aux->var.ULA = *var->ULA;
  *aux->var.saidaMem = *var->saidaMem;

  aux->regTemp->bi_di->pc = regTemp->bi_di->pc;
  *aux->regTemp->bi_di->inst = *regTemp->bi_di->inst;

  aux->regTemp->di_ex->A = regTemp->di_ex->A;
  aux->regTemp->di_ex->B = regTemp->di_ex->B;
  aux->regTemp->di_ex->pc = regTemp->di_ex->pc;
  *aux->regTemp->di_ex->sinais = *regTemp->di_ex->sinais;
  *aux->regTemp->di_ex->inst = *regTemp->di_ex->inst;

  aux->regTemp->ex_mem->pc = regTemp->ex_mem->pc;
   aux->regTemp->ex_mem->DVC = regTemp->ex_mem->DVC;
  aux->regTemp->ex_mem->saidaULA = regTemp->ex_mem->saidaULA;
  aux->regTemp->ex_mem->B = regTemp->ex_mem->B;
  aux->regTemp->ex_mem->muxRegDst = regTemp->ex_mem->muxRegDst;  
  aux->regTemp->ex_mem->flag = regTemp->ex_mem->flag;
  *aux->regTemp->ex_mem->sinais= *regTemp->ex_mem->sinais;
  *aux->regTemp->ex_mem->inst = *regTemp->ex_mem->inst;


  aux->regTemp->mem_er->pc = regTemp->mem_er->pc;
  aux->regTemp->mem_er->muxRegDst = regTemp->mem_er->muxRegDst;
  aux->regTemp->mem_er->saidaULA = regTemp->mem_er->saidaULA;
  aux->regTemp->mem_er->saidaMem = regTemp->mem_er->saidaMem;
  *aux->regTemp->mem_er->inst = *regTemp->mem_er->inst;
  *aux->regTemp->mem_er->sinais = *regTemp->mem_er->sinais;


  return aux;
}

back * iniciarBack(){
   back *aux=(back *)malloc(sizeof(back));

    aux->registradores =  iniciarRegi();
    aux->memD = iniciarMemD();
    aux->var.flag = (int *)malloc(sizeof(int));
    aux->var.saida1 = (int *)malloc(sizeof(int));
    aux->var.saida2 = (int *)malloc(sizeof(int));
    aux->var.ULA = (int *)malloc(sizeof(int));
  aux->var.saidaMem = (int *)malloc(sizeof(int));
  aux->regTemp = iniciarRegiS();


   return aux;
}


void exibir_registradores(WINDOW *regwin, int *registrador) {
    box(regwin, 0, 0);
    mvwprintw(regwin, 0, 4, "BANCO DE REGISTRADORES");

    for (int i = 0; i < 8; i++) {
        mvwprintw(regwin, 2 + i, 5, "Registrador %d: %d", i, registrador[i]);
    }

    wrefresh(regwin);
}
void exibir_memoria(WINDOW *memwin, int *mem) {
   box(memwin, 0, 0);
    mvwprintw(memwin, 0, 4, "MEMORIA DE DADOS");

    for (int i = 0; i < 256; i++) {
        mvwprintw(memwin, (i / 38) + 2, (i % 40) * 7 + 1, " %d", mem[i]);
         //mvwprintw(memwin, 2 + i, 1, "%d: %d", i, mem[i]);
        /*if (i % 16 == 15) {
            wrefresh(memwin);
        }*/
    }
    wrefresh(memwin);
}
void exibir_regt(WINDOW *regtwin, struct regiS *regisT) {

    wclear(regtwin);
    box(regtwin, 0, 0);
    mvwprintw(regtwin, 0, 4, "REGISTRADORES TEMPORARIOS");

    mvwprintw(regtwin, 2, 2, "BI_DI");
    mvwprintw(regtwin, 3, 2, "Instrução: [%s]", regisT->bi_di->inst->instrucao);
    mvwprintw(regtwin, 4, 2, "PC: %i", regisT->bi_di->pc);

    mvwprintw(regtwin, 6, 2, "DI_EX");
    mvwprintw(regtwin, 7, 2, "Instrução: [%s]", regisT->di_ex->inst->instrucao);
    mvwprintw(regtwin, 8, 2, "PC: %i", regisT->di_ex->pc);
    mvwprintw(regtwin, 9, 2, "A: %i", regisT->di_ex->A);
    mvwprintw(regtwin, 10, 2, "B: %i", regisT->di_ex->B);

    mvwprintw(regtwin, 12, 2, "EX_MEM");
    mvwprintw(regtwin, 13, 2, "Instrução: [%s]", regisT->ex_mem->inst->instrucao);
    mvwprintw(regtwin, 14, 2, "PC: %i", regisT->ex_mem->pc);
    mvwprintw(regtwin, 15, 2, "Saída ULA: %i", regisT->ex_mem->saidaULA);
    mvwprintw(regtwin, 16, 2, "B: %i", regisT->ex_mem->B);
    mvwprintw(regtwin, 17, 2, "Registrador destino: %i", regisT->ex_mem->muxRegDst);

    mvwprintw(regtwin, 19, 2, "MEM_ER");
    mvwprintw(regtwin, 20, 2, "Instrução: [%s]", regisT->mem_er->inst->instrucao);
    mvwprintw(regtwin, 21, 2, "PC: %i", regisT->mem_er->pc);
    mvwprintw(regtwin, 22, 2, "Registrador destino: %i", regisT->mem_er->muxRegDst);

    wrefresh(regtwin);
}

void exibir_sinais(WINDOW *sinwin, struct controle *sinais) {

    wclear(sinwin);
    box(sinwin, 0, 0);
    mvwprintw(sinwin, 0, 2, "SINAIS DE CONTROLE");

    mvwprintw(sinwin, 2, 2, "Escreve Memoria: %i", sinais->EscMem);
    mvwprintw(sinwin, 3, 2, "Registrador Destino: %i", sinais->RegDst);
    mvwprintw(sinwin, 4, 2, "Escreve em Reg: %i", sinais->EscReg);
    mvwprintw(sinwin, 5, 2, "Memoria p/ Reg: %i", sinais->MemParaReg);
    mvwprintw(sinwin, 7, 2, "ULA Fonte: %i", sinais->ULAFonte);
    mvwprintw(sinwin, 8, 2, "ULA Op: %i", sinais->ULAOp);
    mvwprintw(sinwin, 9, 2, "DVI: %i", sinais->DVI);
    mvwprintw(sinwin, 10, 2, "DVC: %i", sinais->DVC);


    wrefresh(sinwin);
}

void exibir_atual(WINDOW *atuwin, int  *mem, int n_instrucoes) {

    wclear(atuwin);
    box(atuwin, 0, 0);
    mvwprintw(atuwin, 0, 1, "INSTRUCAO MIPS:");
   // mvwprintw(atuwin, 2, 2, "%i", );

    wrefresh(atuwin);
}

void exibir_pc(WINDOW *pcwin, int *PC){

    wclear(pcwin);
    box(pcwin, 0, 0);
    mvwprintw(pcwin, 1, 8, "PC: %i", *PC);

    wrefresh(pcwin);
}

int menu(struct controle *sinais, int *PC, struct regiS  *regis, int *registrador, int *mem, struct variaveis *var, Pilha *pilha, WINDOW *menuwin, WINDOW *memwin, struct instrucao *regmem, int n_instrucoes, int *countBeq){
  refresh();

  // *PC, mem[*PC].instrucoes.instrucao, sinais->estado_atual

    box(menuwin, 0, 0);
    keypad(menuwin, TRUE); // Habilita captura de teclas especiais
    wrefresh(menuwin);
    // Desenhando o cabeçalho
    mvwprintw(menuwin, 1, 2, "\t");
    mvwprintw(menuwin, 2, 2, "\t\t\t\t\t");
    mvwprintw(menuwin, 3, 2, "\t");
   // mvwprintw(menuwin, 5, 2, "\tPC: %i Instrução:  Estado: ", *PC);  // Valores de exemplo
  //  mvwprintw(menuwin, 7, 2, "\tInstrução em Assembly: ");
    mvwprintw(menuwin, 6, 14, "(r) (RUN) Executar todo o arquivo");
    mvwprintw(menuwin, 8, 14, "(e) (STEP) Executar uma linha");
    mvwprintw(menuwin, 10, 14, "(b) (BACK) Voltar uma instrução");
   // mvwprintw(menuwin, 12, 2, "\t(v) Ver Estado");
   // mvwprintw(menuwin, 13, 2, "\t(a) Ver Instrução Atual");
   // mvwprintw(menuwin, 14, 2, "\t(i) Ver Registradores");
   // mvwprintw(menuwin, 15, 2, "\t(d) Ver Memória de Dados");
   // mvwprintw(menuwin, 16, 2, "\t(i) Ver Todas as Instruções");
   // mvwprintw(menuwin, 17, 2, "\t(s) Ver Sinais");
   // mvwprintw(menuwin, 18, 2, "\t(t) Ver Variáveis");
   // mvwprintw(menuwin, 19, 2, "\t(c) Ver Registradores Temporários");
    mvwprintw(menuwin, 12, 14, "(m) Salvar .asm");
    mvwprintw(menuwin, 14, 14, "(t) Salvar .dat");
    mvwprintw(menuwin, 16, 14, "(x) Sair");
   // mvwprintw(menuwin, 21, 2, "\t");
    //mvwprintw(menuwin, 22, 2, "\tSelecione: p: ");
    wrefresh(menuwin);
    char op =  getch();

   if(op == '\n'){
    op = 'e';
    wrefresh(memwin);
  }
  switch(op){
    case 'r':
      return 1;
      break;
    case 'e':
      return 0;
      break;
    case 'b':
      if (pilha->tam != 0) {
        fback(registrador, mem, PC, sinais, var, regis, pilha, 1, countBeq);
        refresh();
       }
      else {
        printf("Nenhuma instrução para voltar\n");
      }
      return menu(sinais, PC, regis, registrador, mem, var, pilha, menuwin, memwin, regmem, n_instrucoes, countBeq);
      refresh();
      break;
    case 't':
      salvarDat(mem);

      return menu(sinais, PC, regis, registrador, mem, var, pilha, menuwin, memwin, regmem, n_instrucoes, countBeq);
      refresh();
      break;
    case 'm':
      salvarAsm(regmem, n_instrucoes);

      return menu(sinais, PC, regis, registrador, mem, var, pilha, menuwin, memwin, regmem, n_instrucoes, countBeq);
      refresh();
      break;

    case 'x':
      printf("Programa finalizado\n");
      return 3;
      break;

    default:
      //printf("Opção inválida\n");
      scanf("%c", &op);
      return menu(sinais, PC, regis, registrador, mem, var, pilha, menuwin, memwin, regmem, n_instrucoes, countBeq);
      refresh();
      break;

  }
  return 3;
}

void HazardControle(struct regiS *regTemp, struct instrucao inst, struct controle *sinais, int flag, int *countBeq){
  if((inst.opcode == 8 && flag == 1 && *countBeq == 0) || (inst.opcode == 2 && *countBeq == 0)){
    *sinais = zerarSinais();
    *regTemp->di_ex->sinais = zerarSinais();
    (*countBeq)++;
    return;
  }
  if(*countBeq == 1){
  *sinais = zerarSinais();
  *regTemp->di_ex->sinais = zerarSinais();
  *regTemp->ex_mem->sinais = zerarSinais();
  (*countBeq)++;
  return;
  }
  if(*countBeq == 2){
  // *sinais = zerarSinais();
  *regTemp->di_ex->sinais = zerarSinais();
  *regTemp->ex_mem->sinais = zerarSinais();
  *regTemp->mem_er->sinais = zerarSinais();
  (*countBeq)++;
  return;
  }
  if(*countBeq == 3){
    *countBeq = 0;
    return;
  }
}

struct controle zerarSinais(){
    struct controle sinais;
    sinais.RegDst = 0;
      sinais.ULAOp = 0;
      sinais.ULAFonte = 0;
      sinais.DVC = 0;
      sinais.DVI = 0;
      sinais.EscMem = 0;
      sinais.EscReg = 0;
      sinais.MemParaReg = 0;
      return sinais;
}

void salvarAsm(struct instrucao *mem, int n_instrucoes) {
  FILE *arquivoSaida;
  arquivoSaida = fopen("instrucoes.asm", "w");

  if (arquivoSaida == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return;
  }

  for (int i = 0; i < n_instrucoes; i++) {
      switch (mem[i].opcode) {
      case 0:
        switch (mem[i].funct) {
        case 0:
          fprintf(arquivoSaida, "add $%d, $%d, $%d\n", mem[i].b5_3,
                  mem[i].b11_9, mem[i].b8_6);
          break;
        case 2:
          fprintf(arquivoSaida, "sub $%d, $%d, $%d\n", mem[i].b5_3,
                  mem[i].b11_9, mem[i].b8_6);
          break;
        case 5:
          fprintf(arquivoSaida, "or $%d, $%d, $%d\n", mem[i].b5_3,
                  mem[i].b11_9, mem[i].b8_6);
          break;
        case 4:
          fprintf(arquivoSaida, "and $%d, $%d, $%d\n", mem[i].b5_3,
                  mem[i].b11_9, mem[i].b8_6);
          break;
        default:
          fprintf(arquivoSaida, "Instrução inválida");
          break;
        }
        break;
      case 4:
        fprintf(arquivoSaida, "addi $%d, $%d, %d\n", mem[i].b8_6,
                mem[i].b11_9, mem[i].b5_0);
        break;
      case 11:
        fprintf(arquivoSaida, "lw $%d, %d($%d)\n", mem[i].b8_6,
                mem[i].b5_0, mem[i].b11_9);
        break;
      case 15:
        fprintf(arquivoSaida, "sw $%d, %d($%d)\n", mem[i].b8_6,
                mem[i].b5_0, mem[i].b11_9);
        break;
      case 8:
        fprintf(arquivoSaida, "beq $%d, $%d, %d\n", mem[i].b8_6,
                mem[i].b11_9, mem[i].b5_0);
        break;
      case 2:
        fprintf(arquivoSaida, "j %d\n", mem[i].b0_6);
        break;
      default:
        break;
    }
  }
  fclose(arquivoSaida);
  printf("\nArquivo .asm salvo com sucesso.\n");
}

void salvarDat(int *memD){
  FILE *arquivo;
  arquivo = fopen("dados.txt", "w");
  if(arquivo == NULL){
    printf("Erro ao abrir o arquivo.\n");
    return;
  }
  for(int i = 0; i < 256; i++){
    fprintf(arquivo, "%d\n",memD[i]);
  }
  fclose(arquivo);
  printf("\n\nDados salvos com sucesso.\n");
}


void draw_textbox(WINDOW *win, int start_y, int start_x, int width, int height) {
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Text Box ");
    wrefresh(win);
}

void get_input(WINDOW *win, char *input, int len) {
    echo();
    mvwgetnstr(win, 1, 1, input, len);
    noecho();
}
void draw_pipeline_art(WINDOW *win) {
    int start_x = (getmaxx(win) - 62) / 2;
    mvwprintw(win, 5, start_x, "        _____  _  _____  ____  __      _  _    _  ____ ");
    mvwprintw(win, 6, start_x, "       |  __ || ||  __ ||  __|| |     | || |  | ||  __|");
    mvwprintw(win, 7, start_x, "       | |__ || || |__ || |__ | |     | || |\\ | || |__ ");
    mvwprintw(win, 8, start_x, "       |  ___|| ||  ___||  __|| |     | || | \\| ||  __|");
    mvwprintw(win, 9, start_x, "       | |    | || |    | |__ | |____ | || |  \\ || |__ ");
   mvwprintw(win, 10, start_x, "       |_|    |_||_|    |____||______||_||_|   \\||____|");
}

