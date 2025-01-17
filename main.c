#include "mips.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    int height, width;
    getmaxyx(stdscr, height, width);

    char inst_file[20] = "t.txt";
    char data_file[20] = "dados.dat";
    int *memD = iniciarMemD();

    while (1) {
        WINDOW *startwin = newwin(20, 50, (height / 2) - 10, (width / 2) - 25);
        box(startwin, 0, 0);
        keypad(startwin, TRUE);
        refresh();
        wrefresh(startwin);

        mvwprintw(startwin, 3, 15, "PIPELINE");
        mvwprintw(startwin, 6, 20, "  ______");
        mvwprintw(startwin, 7, 20, " /     /|");
        mvwprintw(startwin, 8, 20, "/_____/ |");
        mvwprintw(startwin, 9, 20, "|_____| |");
        mvwprintw(startwin, 10, 20, "| (o) | |");
        mvwprintw(startwin, 11, 20, "|_____|/ ");
        mvwprintw(startwin, 13, 5, inst_file);
        mvwprintw(startwin, 14, 5, data_file);
        mvwprintw(startwin, 15, 5, "[T para editar INSTRUCOES]");
        mvwprintw(startwin, 16, 5, "[D para editar DADOS]");
        mvwprintw(startwin, 18, 15, "[Pressione ENTER para Iniciar]");
        wrefresh(startwin);

        char ch = wgetch(startwin);
        if (ch == '\n') {
            break;
        } else if (ch == 'T' || ch == 't' || ch == 'D' || ch == 'd') {
            char file_name[20];
            strcpy(file_name, (ch == 'T' || ch == 't') ? inst_file : data_file);
            curs_set(TRUE); 
            WINDOW *inputwin = newwin(3, 40, (height / 2) + 10, (width / 2) - 20);
            draw_textbox(inputwin, 1, 1, 38, 1);
            mvwprintw(inputwin, 0, 2, " Editando %s ", file_name);
            mvwprintw(inputwin, 1, 1, "digite_");
            wrefresh(inputwin);

                char new_file_name[20];
            get_input(inputwin, new_file_name, 19);
            
            if (ch == 'T' || ch == 't') {
                strcpy(inst_file, new_file_name);
                mvwprintw(inputwin, 2, 2, "Arquivo editado com sucesso!");

            } else if (ch == 'D' || ch == 'd') {
                strcpy(data_file, new_file_name);
                mvwprintw(inputwin, 2, 2, "Arquivo editado com sucesso!");

            }
            wrefresh(inputwin);
            napms(2000);
            delwin(inputwin);
        }
        delwin(startwin);
        clear();
        refresh();
    }

    int *pc = (int *)malloc(sizeof(int));
    *pc = 0;

    Pilha *pilha = (Pilha *)malloc(sizeof(Pilha));

    struct instrucao *regmem = (struct instrucao *)malloc(256 * sizeof(struct instrucao));

    int *registradores = iniciarRegi();
    struct controle *sinais = iniciarConrole();
    struct regiS *regS1 = iniciarRegiS();
    struct regiS *regS2 = iniciarRegiS();
    struct variaveis *var = inciarVariaveis();

    int op = 0;
    int *countBeq = (int *)malloc(sizeof(int));
    *countBeq = 0;

    int n_instrucoes = carregarMemoria(inst_file, regmem);
    carregarDados(data_file, memD);

    curs_set(FALSE); 
    WINDOW *menuwin = newwin(24, 62, (height / 2) - 12, (width / 2) - 31);
    WINDOW *regwin = newwin(11, 30, height - 11, 1);
    WINDOW *regtwin = newwin(25, 40, (height / 2) - 12, 1); 
    WINDOW *memwin = newwin(11, 120, height - 11, (width / 2) - 60); 
    WINDOW *sinwin = newwin(12, 30, (height / 2), width - 31);
    WINDOW *pcwin = newwin(3, 25, (height / 2) - 7, width - 29); 
	WINDOW *atuwin = newwin(3, 30, (height / 2) - 4, width - 31);

    draw_pipeline_art(stdscr);
    refresh();


    do {
        fback(registradores, memD, pc, sinais, var, regS1, pilha, 0, countBeq);
        
        if(op == 1){
			exibir_registradores(regwin, registradores);
            exibir_regt(regtwin, regS1);
            exibir_memoria(memwin, memD);
            exibir_sinais(sinwin, sinais);
            exibir_pc(pcwin, pc);
            exibir_atual(atuwin, regmem, n_instrucoes, pc);


            wrefresh(regwin);
            wrefresh(regtwin);
            wrefresh(memwin);
            wrefresh(sinwin);
            wrefresh(pcwin);
            wrefresh(atuwin);
		}

        if (*(pc) == (5 + n_instrucoes)) {
            mvwprintw(menuwin, 21, 2, "Todas as instruções foram executadas");
            op = 0;
        }

        if (op != 1) {
            exibir_registradores(regwin, registradores);
            exibir_regt(regtwin, regS1);
            exibir_memoria(memwin, memD);
            exibir_sinais(sinwin, sinais);
            exibir_pc(pcwin, pc);
            exibir_atual(atuwin, regmem, n_instrucoes, pc);

            op = menu(sinais, pc, regS1, registradores, memD, var, pilha, menuwin, memwin, regmem, n_instrucoes, countBeq, regwin, regtwin, sinwin, pcwin, atuwin);
            wrefresh(regwin);
            wrefresh(regtwin);
            wrefresh(memwin);
            wrefresh(sinwin);
            wrefresh(pcwin);
            wrefresh(atuwin);
            if (op == 3) {
                break;
            }
        }

        // Atualiza todas as janelas novamente após o menu
        wrefresh(regwin);
        wrefresh(regtwin);
        wrefresh(memwin);
        wrefresh(sinwin);
        wrefresh(pcwin);
        wrefresh(atuwin);
    //////

    *pc = var->muxDVI;

    *regS2->bi_di->inst = memReg(regmem, *pc);
    ;
    regS2->bi_di->pc = *pc + 1;

    //////////BI/DI

    UC(sinais, regS1->bi_di);

    BancoRegistradores(registradores, regS1->bi_di->inst->b11_9, regS1->bi_di->inst->b8_6, 
    regS1->mem_er->muxRegDst, var->muxMemReg, var->saida1, var->saida2, regS1->mem_er->sinais->EscReg);

    *regS2->di_ex->inst = *regS1->bi_di->inst;
    *regS2->di_ex->sinais = *sinais;
    regS2->di_ex->pc = regS1->bi_di->pc;
    regS2->di_ex->A = *var->saida1;
    regS2->di_ex->B = *var->saida2;

    //////////DI/EX

    if (regS1->di_ex->sinais->RegDst == 0) {
      var->muxRegDst = regS1->di_ex->inst->b8_6;
    } 
    else {
      var->muxRegDst = regS1->di_ex->inst->b5_3;
    }

    if (regS1->di_ex->sinais->ULAFonte == 1) {
      var->muxULA = regS1->di_ex->inst->b5_0;
    } 
    else {
      var->muxULA = regS1->di_ex->B;
    }

    ula(regS1->di_ex->A, var->muxULA, var->ULA, var->flag,
        regS1->di_ex->sinais->ULAOp);

    HazardControle(regS2, *regS1->di_ex->inst, sinais, *var->flag, countBeq);

    *regS2->ex_mem->sinais = *regS1->di_ex->sinais;
    *regS2->ex_mem->inst = *regS1->di_ex->inst;
    regS2->ex_mem->pc = regS1->di_ex->pc;
    regS2->ex_mem->saidaULA = *var->ULA;
    regS2->ex_mem->muxRegDst = var->muxRegDst;
    regS2->ex_mem->B = regS1->di_ex->B;
    regS2->ex_mem->flag = *var->flag;
    
    //////////EX/MEM
    memDados(memD, regS1->ex_mem->saidaULA, regS1->ex_mem->B, regS1->ex_mem->sinais->EscMem, var->saidaMem);

    if ((regS1->ex_mem->flag + regS1->ex_mem->sinais->DVC) == 2) {
      var->muxDVC = 1;
    }
    else{
	  var->muxDVC = 0;
	}
    if (var->muxDVC == 1) {
      regS1->ex_mem->DVC = regS1->ex_mem->pc + regS1->ex_mem->inst->b5_0;
    } 
    else {
      regS1->ex_mem->DVC = *pc + 1;
    }

    if (regS1->ex_mem->sinais->DVI == 0) {
      var->muxDVI = regS1->ex_mem->DVC;
    } 
    else {
      var->muxDVI = regS1->ex_mem->inst->b0_6;
    }

    *regS2->mem_er->inst = *regS1->ex_mem->inst;
    *regS2->mem_er->sinais = *regS1->ex_mem->sinais;
    regS2->mem_er->pc = regS1->ex_mem->pc;
    regS2->mem_er->saidaULA = regS1->ex_mem->saidaULA;
    regS2->mem_er->muxRegDst = regS1->ex_mem->muxRegDst;
    regS2->mem_er->saidaMem = *var->saidaMem;

    //////////MEM/ER

    if (regS1->mem_er->sinais->MemParaReg == 1) {
      var->muxMemReg = regS1->mem_er->saidaULA;
    } 
    else {
      var->muxMemReg = regS1->mem_er->saidaMem;
    }

    BancoRegistradores(registradores, 0, 0, regS1->mem_er->muxRegDst, var->muxMemReg, NULL, NULL, regS1->mem_er->sinais->EscReg);

    ////////////////

    regS1 = copy(regS2);

    //////////

  } while (1);
  
   WINDOW *fimwin = newwin(20, 50, (height / 2) - 10, (width / 2) - 25);
        box(fimwin, 0, 0);
        keypad(fimwin, TRUE);
        refresh();
        wrefresh(fimwin);

        mvwprintw(fimwin, 3, 15, "FIM");
        mvwprintw(fimwin, 6, 20, "  ______");
        mvwprintw(fimwin, 7, 20, " /     /|");
        mvwprintw(fimwin, 8, 20, "/_____/ |");
        mvwprintw(fimwin, 9, 20, "|_____| |");
        mvwprintw(fimwin, 10, 20, "| (o) | |");
        mvwprintw(fimwin, 11, 20, "|_____|/ ");
        wrefresh(fimwin);
        napms(4000);
        
  endwin();
  return 0;
}
