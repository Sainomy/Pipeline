#include <stdio.h>
#include <stdlib.h>
#include "mips.h"
//#include <ncurses.h>

int main(){
  int *pc = (int *)malloc(sizeof(int));
  *pc = 0;
  
  struct instrucao *regmem = (struct instrucao*)malloc(256*sizeof(struct instrucao));
  
  int *registradores = iniciarRegi();
  
  int *memD = iniciarMemD();

  struct controle *sinais = iniciarConrole();

  struct regiS *regS1 = iniciarRegiS();
  struct regiS *regS2 = iniciarRegiS();
  
  struct variaveis *var = inciarVariaveis();
  
  int op = 0;
  
  //////////
  carregarMemoria("t.txt", regmem);
  do{
    if(op!=1){ 
        op=menu(sinais, pc, regS1, registradores, memD, var);
        if(op == 3){
          break;
        }
    }

  //////
 
  *pc = var->muxDVI;

  *regS2->bi_di->inst =   memReg(regmem, *pc);;
  regS2->bi_di->pc = *pc +1;
  
  
  //////////BI/DI
  
  UC(sinais, regS1->bi_di);

  BancoRegistradores(registradores, regS1->bi_di->inst->b11_9, regS1->bi_di->inst->b8_6, regS1->mem_er->muxRegDst,  var->muxMemReg, var->saida1, var->saida2, regS1->mem_er->sinais->EscReg);
  
  *regS2->di_ex->inst = *regS1->bi_di->inst;
  *regS2->di_ex->sinais = *sinais;
  regS2->di_ex->pc = regS1->bi_di->pc;
  regS2->di_ex->A = *var->saida1;
  regS2->di_ex->B = *var->saida2;
  
  
  //////////DI/EX 
  
  if(regS1->di_ex->sinais->RegDst == 0){
    var->muxRegDst = regS1->di_ex->inst->b8_6;
  }
  else{
    var->muxRegDst = regS1->di_ex->inst->b5_3;
  }

  if(regS1->di_ex->sinais->ULAFonte == 1){
    var->muxULA = regS1->di_ex->inst->b5_0;
  }
  else{
    var->muxULA = regS1->di_ex->B;
  }

  ula(regS1->di_ex->A, var->muxULA, var->ULA , var->flag, regS1->di_ex->sinais->ULAOp);
  
  *regS2->ex_mem->sinais = *regS1->di_ex->sinais;
  *regS2->ex_mem->inst = *regS1->di_ex->inst;
  regS2->ex_mem->pc = regS1->di_ex->pc;
  regS2->ex_mem->saidaULA = *var->ULA;
  regS2->ex_mem->muxRegDst = var->muxRegDst;
  regS2->ex_mem->B = regS1->di_ex->B;
  regS2->ex_mem->flag = *var->flag;
  //////////EX/MEM
  
  memDados(memD, regS1->ex_mem->saidaULA,  regS1->ex_mem->B , regS1->ex_mem->sinais->EscMem, var->saidaMem);

  if((regS1->ex_mem->flag + regS1->ex_mem->sinais->DVC) == 2){
    var->muxDVC = 1;
  }
  if(var->muxDVC == 1){
    var->muxDVC = *pc + 1 + regS1->ex_mem->inst->b5_0;
  }
  else{
    var->muxDVC = *pc + 1;
  }

  if(regS1->ex_mem->sinais->DVI == 0){
    var->muxDVI = var->muxDVC;
  }
  else{
    var->muxDVI = /* complemeta com pc + 1 */ regS1->ex_mem->inst->b0_6;
  }
  
  *regS2->mem_er->inst = *regS1->ex_mem->inst;
  *regS2->mem_er->sinais = *regS1->ex_mem->sinais;
  regS2->mem_er->pc = regS1->ex_mem->pc;
  regS2->mem_er->saidaULA = regS1->ex_mem->saidaULA;
  regS2->mem_er->muxRegDst = regS1->ex_mem->muxRegDst; 
  regS2->mem_er->saidaMem = *var->saidaMem;
  
  //////////MEM/ER

  if(regS1->mem_er->sinais->MemParaReg == 1){
    var->muxMemReg = regS1->mem_er->saidaULA;
  }
  else{
    var->muxMemReg = regS1->mem_er->saidaMem;
  }
  
  BancoRegistradores(registradores, 0, 0, regS1->mem_er->muxRegDst,  var->muxMemReg, NULL, NULL, regS1->mem_er->sinais->EscReg);
  
  ////////////////
  
  regS1 = copy(regS2);

	printf("\n\nDados ao fim do ciclo");
	printf("\n\nRegistradores Temporários\n");
	verRegT(regS1);
	printf("\n\nVariáveis\n");
	verVariaveis(var);
	printf("\n\nSinais\n");
	verSinais(sinais);
	printf("\n\nPC %i\n", *pc);
	printf("\n\nBanco de Registradores\n");
	verReg(registradores);

  //////////
    
  }while(1);

return 0;
}
