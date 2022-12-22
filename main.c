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

    int FLAG = 0;
    FILE *dir;
    dir = fopen("dir", "r+");
    if (dir != NULL) {
    	FLAG = 1;

    	directory_size_t tam;
    	fread(&tam, sizeof(directory_size_t), 1, dir); 
    	bucket_t bn;
	    fread(&bn, sizeof(bucket_t), 1, dir);
	    bucket_size_t bs;
	    fread(&bs, sizeof(bucket_size_t), 1, dir);
	    depth_t depth;
	    fread(&depth, sizeof(depth_t), 1, dir);
	    directory_t dir_t;
	    fread(&dir_t, sizeof(directory_t), 1, dir);

	   FLAG = RECUPERA_DIR(&h, tam, bs, bn, depth, dir_t, "arquivo");
    }

	do {
		printf("=-=-=LISTA DE OPCOES:-=-=-\n\n");
		if (FLAG == 1) printf("(Já Existe um hash Criado)\n");
		printf(" [1] Criar Hash.\n");
		printf(" [2] Procurar Registro.\n");
		printf(" [3] Inserir Registro.\n");
		printf(" [4] Remover Registro.\n");
		printf(" [5] Imprimir Hash.\n");
		printf(" [6] Sair.\n");
		printf("\n Digite uma das opcoes: ");

		scanf("%d", &op);
		system(limpar);
		setbuf(stdin, NULL);
		
		switch(op){			
			case 1:
				printf("Entre com a profundidade global inicial: ");
                scanf("%u", &pg_inicial);

                if(CRT_HASH(&h, pg_inicial, "arquivo")){
                    printf("Hash criado com sucesso!\n\n");
                	FLAG = 1;
                }
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
                if(!PRNT_HASH(h)) printf("Houve erro ao imprimir Hash!\n\n");
                break;

			case MAXOP:
				PERSISTE_DIR(h);
				printf("Encerrando o programa.\n");
				
				//esperando n segundos antes de encerrar o programa
				int n = 1;
				clock_t inicio = clock();
				while((clock() - inicio) * 1000 / CLOCKS_PER_SEC < n * 1000);		
				break;

		}
				
	} while(op != MAXOP);

	system(limpar);
	return 0;
}
