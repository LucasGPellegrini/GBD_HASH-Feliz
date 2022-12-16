#include "hash.h"
#include <stdio.h>

int main(){
    int op = -1;

    Hash h;
    struct registro aux;
    printf("%d", CRT_HASH(&h, 2, "arquivo"));

    while(op != 0){
        printf("Entre com a opcao: ");
        scanf("%d", &op);
        switch(op){

            case 1:
                printf("Entre com a registro:\n");
                scanf("%u", &aux.nseq);
                scanf("%u", &aux.text);
                printf("%d", INST_HASH(h, &aux));
                break;

            case 2:
                printf("Imprimir hash!\n");

                printf("%d", PRNT_HASH(h));
                break;
        }

        fflush(stdin);
    }

    return 0;
}