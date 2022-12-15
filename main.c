#include "hash.h"
#include <stdio.h>

int main(){
    int op = -1;

    Hash h;
    struct registro aux;
    CRT_HASH(h, 4);

    while(op != 0){
        scanf("%d", &op);
        switch(op){

            case 1:
                printf("Entre com a registro:\n");
                scanf("%u", &aux.nseq);
                scanf("%u", &aux.text);
                INST_HASH(h, &aux);
                break;

            case 2:
                printf("Imprimir hash!\n");

                PRNT_HASH(h);
                break;
        }
        fflush(stdin);
    }

    return 0;
}