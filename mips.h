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

//funcoes principais

struct instrucao memReg(struct instrucao *mem, int pc);
int memDados(int *memD, int endereco, int dado, int EscMem, int *saida);

void ula(int valor1, int valor2, int *saida, int *flag, int ULAop);

void BancoRegistradores(int *registradores, int ReadEnd1, int ReadEnd2, int WriteEnd, int dado, int *saida1, int *saida2, int EscReg);

struct controle *UC(struct controle *sinais, struct regiBI_ID *bits);

//funcoes de apoio

void carregarMemoria(char *nomeArquivo, struct instrucao *mem);

int bi_dec(char *bin);

void decodificarOpcode(struct instrucao *mem, int n_instrucoes); 

struct controle * iniciarConrole();

int menu(struct controle *sinais, int *PC, struct regiS  *regis, int *registrador, int *mem, struct variaveis *var);

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
