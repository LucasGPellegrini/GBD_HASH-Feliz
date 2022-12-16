#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hash.h"

// checa para saber se é windows OS
// _WIN32 macro
#ifdef _WIN32
    #define limpar "cls"
  
// checa para saber se é linux OS 
// __linux__ macro
#elif __linux__
    #define limpar "clear"
  
#endif

#define MAXOP 6

// Preambulo :)

int main()
{
	struct registro reg;
    Hash h;
	int op;
    depth_t pg_inicial;
    entry_number_t chave;

	do {
		printf("LISTA DE OPCOES:\n\n");
		printf(" [1] Criar Hash.\n");
		printf(" [2] Procurar Registro.\n");
		printf(" [3] Inserir Registro.\n");
		printf(" [4] Remover Registro.\n");
		printf(" [5] Imprimir Hash.\n");
		printf(" [6] Sair.\n");
		printf("\nDigite uma das opcoes: ");

		scanf("%d", &op);
		system(limpar);
		setbuf(stdin, NULL);
		
		switch(op){			
			case 1:
				printf("Entre com a profundidade global inicial: ");
                scanf("%u", &pg_inicial);

                if(CRT_HASH(&h, pg_inicial, "arquivo"))
                    printf("Hash criado com sucesso!\n\n");
                else
                    printf("Houve algum erro!\n\n");
                break;

			case 2:
				printf("Entre com a chave: ");
                scanf("%u", &chave);
                if(SRCH_HASH(h, chave, &reg))
                    printf("Registro encontrado: <%u, %s>!\n\n", reg.nseq, reg.text);
                else
                    printf("Registro nao encontrado!\n\n");
                
	 			break;

            case 3:
                printf("Entre com o registro.\n");
                printf("Entre com a chave: ");
                scanf("%u", &reg.nseq);
                printf("Entre com o texto: ");
                scanf("%s", &reg.text);

                if(INST_HASH(h, &reg))
                    printf("Registro inserido com sucesso!\n\n");
                else
                    printf("Registro nao inserido!\n\n");

                break;

			case 4:
				printf("Entre com a chave: ");
                scanf("%u", &chave);
                if(RMV_HASH(h, chave, &reg))
                    printf("Registro removido: <%u, %s>!\n\n", reg.nseq, reg.text);
                else
                    printf("Registro nao encontrado!\n\n");
                
	 			break;

            case 5:
                PRNT_HASH(h);
                break;

			case MAXOP:
				printf("Encerrando o programa.\n");
				
				//esperando n segundos antes de encerrar o programa
				int n = 3;
				clock_t inicio = clock();
				while((clock() - inicio) * 1000 / CLOCKS_PER_SEC < n * 1000);		
				break;

		}
				
	} while(op != MAXOP);

	system(limpar);
	return 0;
}
