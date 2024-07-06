#include <ncurses.h>

struct instrucao{
    char instrucao[17];
    int funct;
    int opcode;
    int b11_9;
    int b8_6;
    int b5_3;
    int b5_0;
    int b0_6;
};

struct controle{
  //int louD;
  int EscMem;
  //int IREsc;
  int RegDst;
  int EscReg;
  int MemParaReg;
  int ULAFonte;
  //int ULAFonteDown;
  int ULAOp;
  //int branch;
  //int PCEsc;
  //int FontePC;
  int DVI;
  int DVC;
  };

struct regiBI_ID{

  int pc;
  struct instrucao *inst;
  struct variaveis *var;

};

struct regiDI_EX{

  int A;
  int B;
  int pc;
  struct instrucao *inst;
  struct controle *sinais;
  struct variaveis *var;

};

struct regiEX_MEM{

  int pc;
  struct instrucao *inst;
  int saidaULA;
  int B;
  int muxRegDst;
  int flag;
  struct controle *sinais;
  struct variaveis *var;

};

struct regiMEM_ER{

  int pc;
  int muxRegDst;
  int saidaULA;
  int saidaMem;
  struct instrucao *inst;
  struct controle *sinais;
  struct variaveis *var;

};

struct regiS{

  struct regiBI_ID *bi_di;
  struct regiDI_EX *di_ex;
  struct regiEX_MEM *ex_mem;
  struct regiMEM_ER *mem_er;
 struct variaveis *var;

};

struct variaveis{

    int *flag;
    int muxDVC;
    int muxDVI;
    int muxloaD;
    int muxRegDst;
    int muxMemReg;
    int muxULA;
    int *saida1;
    int *saida2;
    int *ULA;
    int *saidaMem; 

};

typedef struct{
	
  int *registradores;
  int *memD;
  struct instrucao *inst;
  int pc;
  struct controle sinais;
  struct variaveis var;
  struct regiS regTemp;

}back;

typedef struct Node {
    back *estado;
    struct Node *next;
} Nodo;

typedef struct {
    Nodo *top;
    int tam;
} Pilha;

//funcoes principais

struct instrucao memReg(struct instrucao *mem, int pc);
void memDados(int *memD, int endereco, int dado, int EscMem, int *saida);

void ula(int valor1, int valor2, int *saida, int *flag, int ULAop);

void BancoRegistradores(int *registradores, int ReadEnd1, int ReadEnd2, int WriteEnd, int dado, int *saida1, int *saida2, int EscReg);

struct controle *UC(struct controle *sinais, struct regiBI_ID *bits);

//funcoes de apoio

void carregarMemoria(char *nomeArquivo, struct instrucao *mem);

int bi_dec(char *bin);

void decodificarOpcode(struct instrucao *mem, int n_instrucoes); 

struct controle * iniciarConrole();

int menu(struct controle *sinais, int *PC, struct regiS  *regis, int *registrador, int *mem, struct variaveis *var, Pilha *pilha, WINDOW *menuwin, WINDOW *memwin);

int * iniciarRegi();
int * iniciarMemD();
struct regiS * iniciarRegiS();
struct variaveis * inciarVariaveis();
struct regiS * copy(struct regiS *regS2);
void verEstado(struct regiS  *regis);
void verReg(int *registrador);
void vermemoria(int *mem);
void verRegT(struct regiS  *regiST);
void verVariaveis(struct variaveis *var);
void verSinais(struct controle *sinais);

back * iniciarBack();

void push(Pilha* stack, back *estado);
void pop(Pilha* pilha);
Nodo *criaNodo(int *registradores, int *memD, int *pc, struct controle *sinais, struct variaveis *var, struct regiS *regTemp);
void fback(int *registradores, int *memD, int *pc, struct controle *sinais, struct variaveis *var, struct regiS *regTemp, Pilha *pilha, int chose);
back *printn(int *registradores, int *memD, int *pc, struct controle *sinais, struct variaveis *var, struct regiS *regTemp);
int isEmpty(Pilha* pilha);
back * iniciarBack();

void exibir_registradores(WINDOW *regwin, int *registrador);
void exibir_memoria(WINDOW *memwin, int *mem);
void exibir_regt(WINDOW *regtwin, struct regiS  *regisT);
