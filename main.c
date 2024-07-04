#include <stdio.h>
#include <stdlib.h>
#include "mips.h"
#include <ncurses.h>

int main(){
   initscr();            // Inicia o modo ncurses
    cbreak();             // Desativa o buffering de linha
    noecho();             // Desativa a exibição dos caracteres digitados
    curs_set(FALSE);      // Oculta o cursor

    int height, width;
    getmaxyx(stdscr, height, width);

    WINDOW *startwin = newwin(20, 50, (height / 2) - 10, (width / 2) - 25);
    box(startwin, 0, 0);
    keypad(startwin, TRUE); // Habilita captura de teclas especiais
    refresh();
    wrefresh(startwin);

    mvwprintw(startwin, 3, 15, "PIPELINE");
  	mvwprintw(startwin, 6, 20, "  ______");
  	mvwprintw(startwin, 7, 20, " /     /|");
  	mvwprintw(startwin, 8, 20, "/_____/ |");
  	mvwprintw(startwin, 9, 20, "|_____| |");
  	mvwprintw(startwin, 10, 20, "| (o) | |");
  	mvwprintw(startwin, 11, 20, "|_____|/ ");
  	mvwprintw(startwin, 15, 15, "[Pressione ENTER para Iniciar]");

             // E
  int *pc = (int *)malloc(sizeof(int));
  *pc = 0;
  
  struct instrucao *regmem = (struct instrucao*)malloc(256*sizeof(struct instrucao));
  Pilha *pilha = (Pilha *)malloc(sizeof(Pilha));
  int *registradores = iniciarRegi();
  
  int *memD = iniciarMemD();

  struct controle *sinais = iniciarConrole();

  struct regiS *regS1 = iniciarRegiS();
  struct regiS *regS2 = iniciarRegiS();
  
  struct variaveis *var = inciarVariaveis();
  
  int op = 0;
  
  //////////
  carregarMemoria("instrucoes.txt", regmem);
  do{
     wrefresh(startwin);
        int ch = wgetch(startwin);
        if (ch == '\n') {  // Verifica se o usuário pressionou Enter
            delwin(startwin); // Deleta a janela inicial
             WINDOW *menuwin = newwin(22, 62, (height / 2) - 11, (width / 2) - 31);
              char p = menuview(menuwin);
    if(op!=1){ 
      

            // Criar janelas fixas para o menu, registradores e memória
           
          //  WINDOW *regwin = newwin(10, 30, 1, 1);
          //  WINDOW *memwin = newwin(10, 30, 12, 1);
          //  WINDOW *instmem = newwin(10, 120, 44, 30);

                //exibir_registradores(regwin);
              //  exibir_memoria(memwin);
               // exibir_inst(instmem);
               
               // if (p == 'x') {
                    //break;  // Encerra o programa ao pressionar 'x'
              //  }

              op=menu(sinais, pc, regS1, registradores, memD, var, pilha, p);
                  if(op == 3){
              break;
              }

            // Deleta as janelas antes de encerrar
           
            //delwin(menuwin);
          
           // delwin(regwin);
           // delwin(memwin);
           // delwin(instmem);
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
    var->muxULA = *var->saida2;
  }

  ula(*var->saida1, var->muxULA, var->ULA , var->flag, regS1->di_ex->sinais->ULAOp);
  
  *regS2->ex_mem->sinais = *regS1->di_ex->sinais;
  *regS2->ex_mem->inst = *regS1->di_ex->inst;
  regS2->ex_mem->pc = regS1->di_ex->pc;
  regS2->ex_mem->saidaULA = *var->ULA;
  regS2->ex_mem->muxRegDst = var->muxRegDst;
  
  //////////EX/MEM
  
  memDados(memD, *var->ULA, *var->saida2, regS1->ex_mem->sinais->EscMem, var->saidaMem);

  if((*var->flag + regS1->ex_mem->sinais->DVC) == 2){
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
  
  //////////MEM/ER

  if(regS1->mem_er->sinais->MemParaReg == 1){
    var->muxMemReg = regS1->mem_er->saidaULA;
  }
  else{
    var->muxMemReg = *var->saidaMem;
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
	printf("\n\nBanco de Registradores\n");
	verReg(registradores);

  fback(registradores, memD, regS1->bi_di->inst,  pc, sinais, var, regS1,  pilha, 0);
  //////////
    endwin(); 
        
  }while(1);

return 0;
}
