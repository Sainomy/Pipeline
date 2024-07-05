#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mips.h"
#include <ncurses.h>


//funcoes principais

struct instrucao memReg(struct instrucao *mem, int pc){
  return mem[pc];
}

int memDados(int *memD, int endereco, int dado, int EscMem, int *saida){
 if(EscMem == 0){
   return memD[endereco]; 
 } 
  else{
   memD[endereco] = dado;
   return *memD;
  }
}

void ula(int valor1, int valor2, int *saida, int *flag, int ULAop){

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

	  case 6:
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

void carregarMemoria(char *nomeArquivo, struct instrucao *mem){
  
  int i=0;
  FILE *arquivo;
  arquivo = fopen(nomeArquivo, "r");

  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return;
  }

  while (fscanf(arquivo, "%16s", mem[i].instrucao) != EOF && i < 256) {
    //strncpy(op, mem[i].instrucao, 4);
    //mem[i].opcode=bi_dec(op);
    decodificarOpcode(mem, i);
    i++;
  }
  fclose(arquivo);
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

int menu(struct controle *sinais, int *PC, struct regiS  *regis, int *registrador, int *mem, struct variaveis *var, Pilha *pilha, WINDOW *menuwin, WINDOW *memwin){
  refresh();

  //*PC, mem[*PC].instrucoes.instrucao, sinais->estado_atual
 
  	box(menuwin, 0, 0);
    keypad(menuwin, TRUE); // Habilita captura de teclas especiais
    wrefresh(menuwin);
    // Desenhando o cabeçalho
    mvwprintw(menuwin, 1, 2, "\t");
    mvwprintw(menuwin, 2, 2, "\t\t\tPIPELINE\t\t");
    mvwprintw(menuwin, 3, 2, "\t");
    mvwprintw(menuwin, 5, 2, "\tPC: %i Instrução:  Estado: ", *PC);  // Valores de exemplo
    mvwprintw(menuwin, 7, 2, "\tInstrução em Assembly:");
    mvwprintw(menuwin, 9, 2, "\t(r) (RUN) Executar todo o arquivo");
    mvwprintw(menuwin, 10, 2, "\t(e) (STEP) Executar uma linha");
    mvwprintw(menuwin, 11, 2, "\t(b) (BACK) Voltar uma instrução");
   // mvwprintw(menuwin, 12, 2, "\t(v) Ver Estado");
   // mvwprintw(menuwin, 13, 2, "\t(a) Ver Instrução Atual");
   // mvwprintw(menuwin, 14, 2, "\t(i) Ver Registradores");
   // mvwprintw(menuwin, 15, 2, "\t(d) Ver Memória de Dados");
   // mvwprintw(menuwin, 16, 2, "\t(i) Ver Todas as Instruções");
  //. mvwprintw(menuwin, 17, 2, "\t(s) Ver Sinais");
   // mvwprintw(menuwin, 18, 2, "\t(t) Ver Variáveis");
   // mvwprintw(menuwin, 19, 2, "\t(c) Ver Registradores Temporários");
    mvwprintw(menuwin, 12, 2, "\t(m) Salvar .asm");
    mvwprintw(menuwin, 13, 2, "\t(l) Salvar .dat");
    mvwprintw(menuwin, 14, 2, "\t(x) Sair");
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
      if (!isEmpty(pilha)) {
        fback(registrador, mem, regis->bi_di->inst,  PC, sinais, var, regis,  pilha, 1);
        refresh();
      }
      else {
        printf("Nenhuma instrução para voltar\n");
      }
      return menu(sinais, PC, regis, registrador, mem, var, pilha, menuwin, memwin);
      refresh();
      break;
    case 't':
      //salvarDat();
      
      return menu(sinais, PC, regis, registrador, mem, var, pilha, menuwin, memwin);
      refresh();
      break;
    case 's':
      //salvarAsm();
      
      return menu(sinais, PC, regis, registrador, mem, var, pilha, menuwin, memwin);
      refresh();
      break;
   
    case 'x':
      printf("Programa finalizado\n");
      return 3;
      break;
      
    default:
      //printf("Opção inválida\n");
      scanf("%c", &op);
      return menu(sinais, PC, regis, registrador, mem, var, pilha, menuwin, memwin);
      refresh();
      break;
    
  }
  return 3;
}

int * iniciarRegi(){

  int *aux=(int *)malloc(8*sizeof(int));  
  for(int i=0; i<8; i++){
    aux[i] = 0;
  }
  return aux;

};

void exibir_registradores(WINDOW *regwin, int *registrador) {
    box(regwin, 0, 0);
    mvwprintw(regwin, 1, 1, "Banco de Registradores");
    
    for (int i = 0; i < 8; i++) {
        mvwprintw(regwin, 2 + i, 1, "Registrador %d: %d", i, registrador[i]);
    }

    wrefresh(regwin);
}
void exibir_memoria(WINDOW *memwin, int *mem) {
   box(memwin, 0, 0);
    mvwprintw(memwin, 1, 1, "Memória de Dados");
    
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
    mvwprintw(regtwin, 1, 1, "Registradores Temporários");

    mvwprintw(regtwin, 2, 1, "BI_DI");
    mvwprintw(regtwin, 3, 1, "Instrução: [%s]", regisT->bi_di->inst->instrucao);
    mvwprintw(regtwin, 4, 1, "PC: %i", regisT->bi_di->pc);

    mvwprintw(regtwin, 6, 1, "DI_EX");
    mvwprintw(regtwin, 7, 1, "Instrução: [%s]", regisT->di_ex->inst->instrucao);
    mvwprintw(regtwin, 8, 1, "PC: %i", regisT->di_ex->pc);
    mvwprintw(regtwin, 9, 1, "A: %i", regisT->di_ex->A);
    mvwprintw(regtwin, 10, 1, "B: %i", regisT->di_ex->B);

    mvwprintw(regtwin, 12, 1, "EX_MEM");
    mvwprintw(regtwin, 13, 1, "Instrução: [%s]", regisT->ex_mem->inst->instrucao);
    mvwprintw(regtwin, 14, 1, "PC: %i", regisT->ex_mem->pc);
    mvwprintw(regtwin, 15, 1, "Saída ULA: %i", regisT->ex_mem->saidaULA);
    mvwprintw(regtwin, 16, 1, "B: %i", regisT->ex_mem->B);
    mvwprintw(regtwin, 17, 1, "Registrador destino: %i", regisT->ex_mem->muxRegDst);

    mvwprintw(regtwin, 19, 1, "MEM_ER");
    mvwprintw(regtwin, 20, 1, "Instrução: [%s]", regisT->mem_er->inst->instrucao);
    mvwprintw(regtwin, 21, 1, "PC: %i", regisT->mem_er->pc);
    mvwprintw(regtwin, 22, 1, "Registrador destino: %i", regisT->mem_er->muxRegDst);

    wrefresh(regtwin);
}
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
		printw("\nBI_DI [%s]", regis->bi_di->inst->instrucao);
		printw("\nDI EX [%s]", regis->di_ex->inst->instrucao);
		printw("\nEX_MEM [%s]", regis->ex_mem->inst->instrucao);
		printw("\nMEM_ER [%s]", regis->mem_er->inst->instrucao);
};

void verReg(int *registrador){
  for(int i=0;i<8;i++){
    printw("\nRegistrador %i: %i \n",i, registrador[i]);
  }
}

void vermemoria(int *mem){
	for(int i=0;i<256;i++){
		printw("\n[%i]: %i \n",i, mem[i]);
	  }	
};

void verRegT(struct regiS  *regisT){
	
	printw("\n\nBI_DI \nInstrução: [%s]\nPC: %i", regisT->bi_di->inst->instrucao, regisT->bi_di->pc);
	printw("\n\nDI EX \nInstrução: [%s]\nPC: %i\nA: %i\nB: %i", regisT->di_ex->inst->instrucao, regisT->di_ex->pc, regisT->di_ex->A, regisT->di_ex->B);
	printw("\n\nEX_MEM\nInstrução: [%s]\nPC: %i\nSa´da ULA: %i\nB: %i\nRegistrador destino: %i", regisT->ex_mem->inst->instrucao, regisT->ex_mem->pc, regisT->ex_mem->saidaULA, regisT->ex_mem->B, regisT->ex_mem->muxRegDst);
	printw("\n\nMEM_ER\nInstrução: [%s]\nPC: %i\nRegistrador destino: %i", regisT->mem_er->inst->instrucao, regisT->mem_er->pc, regisT->mem_er->muxRegDst);
	
};

void verVariaveis(struct variaveis *var){
	printw("\nFlag: %i, \nmuxDVC: %i, \nmuxDVI: %i, \nmuxloaD: %i, \nmuxRegDst: %i, \nmuxMemReg: %i, \nmuxULA: %i, \nsaida1: %i, \nsaida2: %i, \nULA: %i, \nsaidaMem: %i", *var->flag, var->muxDVC, var->muxDVI, var->muxloaD, var->muxRegDst, var->muxMemReg, var->muxULA, *var->saida1, *var->saida2, *var->ULA, *var->saidaMem); 
};

void verSinais(struct controle *sinais){
	printw("\nmuxEscMem: %i, \nRegDst: %i, \nEscReg: %i, \nMemParaReg: %i, \nULAFonte: %i, \nULAOp: %i, \nDVI: %i, \nDVC: %i",sinais->EscMem, sinais->RegDst, sinais->EscReg, sinais->MemParaReg, sinais->ULAFonte, sinais->ULAOp, sinais->DVI, sinais->DVC);
};

back *printn(int *registradores, int *memD, struct instrucao *inst, int pc, struct controle sinais, struct variaveis *var, struct regiS *regTemp){
  back *aux=(back *)malloc(sizeof(back));
  aux->pc = pc;
  aux->registradores = registradores;
  aux->memD = memD;
  aux->inst = inst;
  aux->sinais = sinais;
  aux->var = *var;
  aux->regTemp = *regTemp;
  return aux;
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
  Nodo *temp = pilha->top;
  pilha->top = pilha->top->next;
  free(temp->estado);
  free(temp);
  pilha->tam--;
}

Nodo *criaNodo(int *registradores, int *memD, struct instrucao *inst, int *pc, struct controle *sinais, struct variaveis *var, struct regiS *regTemp){

    Nodo *aux=(Nodo *)malloc(sizeof(Nodo));
    aux->estado = printn(registradores, memD, inst, *pc, *sinais, var, regTemp);

    return aux;
}


void fback(int *registradores, int *memD, struct instrucao *inst, int *pc, struct controle *sinais, struct variaveis *var, struct regiS *regTemp, Pilha *pilha, int chose){

  if(chose == 0 ){
    Nodo *aux;
    aux=criaNodo(registradores, memD, inst, pc, sinais, var, regTemp);
    push(pilha, aux->estado);
    free(aux);
  }
  else{
    *pc = pilha->top->estado->pc;
    pop(pilha);
    }
}

int isEmpty(Pilha* pilha) {
  return pilha->top == NULL;
}
