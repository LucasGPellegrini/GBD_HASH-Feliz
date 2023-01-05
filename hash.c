#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REG 1024

directory_size_t _CALC_N(depth_t p){
    depth_t np = 1;

    for(depth_t i = 0; i < p; i++)
        np *= 2;

    return np;
}

// Funcao hash (h(key) mod N, com h(key) = key)
directory_size_t _HASH_FUNCTION(entry_number_t chave, directory_size_t N){
    return chave % N;
}

int CRT_HASH(Hash* hash_ptr, depth_t pg_inicial, char* hdir){
    if(hash_ptr == NULL || pg_inicial <= 0 || hdir == NULL) return 0;

    *hash_ptr = malloc(sizeof(struct hash));
    if(*hash_ptr == NULL) return 0;

    Hash hash = *hash_ptr;

    directory_size_t dr_size = _CALC_N(pg_inicial);

    hash->dr_size = dr_size;
    hash->bucket_size = dr_size;
    hash->bucket_number = dr_size;
    hash->pg = pg_inicial;

    hash->dr = malloc(hash->dr_size * sizeof(struct directory_entry));
    if(hash->dr == NULL){
        free(hash);
        hash = NULL;
        return 0;
    }

    hash->fname = malloc(MAX_REG * sizeof(char));
    if(hash->fname == NULL){
        free(hash);
        hash = NULL;
        return 0;
    }

    strcpy(hash->fname, hdir);
    hash->fp = fopen(hash->fname, "w+");
    if(hash->fp == NULL){
        free(hash->dr);
        free(hash);
        hash = NULL;
        return 0;
    }

    rewind(hash->fp);
    long int fp_pointer;

    struct registro reg;
    reg.nseq = 0;

    for(directory_size_t i = 0; i < hash->dr_size; i++){
        fp_pointer = ftell(hash->fp);
        for(directory_size_t j = 0; j < hash->bucket_size; j++){
            if(!fwrite(&reg, sizeof(struct registro), 1, hash->fp)){
                fclose(hash->fp);
                free(hash->dr);
                free(hash);
                hash = NULL;
                return 0;
            }
        }
        hash->dr[i].pl = hash->pg;
        hash->dr[i].bucket = fp_pointer;
    }

    fclose(hash->fp);
    PERSISTE_DIR(hash);

    return 1;
}

int _NVLD_HASH(Hash hash){
    return(hash == NULL || hash->dr == NULL || hash->pg <= 0 || hash->fname == NULL);
}

int SRCH_HASH(Hash hash, entry_number_t chave, Registro reg){
    if(_NVLD_HASH(hash) || chave == 0 || reg == NULL) return 0;

    // Abre o arquivo hash
    hash->fp = fopen(hash->fname, "r");
    if(hash->fp == NULL) return 0;

    directory_size_t bucket = _HASH_FUNCTION(chave, hash->dr_size);

    fseek(hash->fp, hash->dr[bucket].bucket, SEEK_SET);

    for(bucket_size_t i = 0; i < hash->bucket_size; i++){
        if(!fread(reg, sizeof(struct registro), 1, hash->fp)) return 0;
        // Se eh chave, achou o registro, que jah estah em reg
        if(reg->nseq == chave) return 1;
    }
    
    fclose(hash->fp);

    return 0;
}

int INST_HASH(Hash hash, Registro reg){
    if(_NVLD_HASH(hash) || reg == NULL) return 0;

    // Abre o arquivo hash
    hash->fp = fopen(hash->fname, "r+");
    if(hash->fp == NULL) return 0;

    directory_size_t bucket = _HASH_FUNCTION(reg->nseq, hash->dr_size);

    // Achar bucket com essa hash
    fseek(hash->fp, hash->dr[bucket].bucket, SEEK_SET);    

    struct registro aux;

    // Vai a procura de um slot vazio no bucket original da hash (nseq == 0)
    bucket_size_t original;
    for(original = 0; original < hash->bucket_size; original++){
        fread(&aux, sizeof(struct registro), 1, hash->fp);
        if(aux.nseq == 0) break;
    }

    // Bucket original nao-cheio (insercao tranquila)
    if(original != hash->bucket_size){
        fseek(hash->fp, -(long int)sizeof(struct registro), SEEK_CUR);
        fwrite(reg, sizeof(struct registro), 1, hash->fp);
    }else{

        // Caso contrario, bucket original cheio

        struct registro buffer[hash->bucket_size];
        aux.nseq = 0;

        // Precisa duplicar diretorio (pg == pl)
        if(hash->pg == hash->dr[bucket].pl){
            // Bucket cheio, tem que duplicar diretorio
		    hash->dr_size *= 2;
            hash->pg++;

            hash->dr = (directory_t) realloc(hash->dr, hash->dr_size * sizeof(struct directory_entry));
            if(hash->dr == NULL) return 0;

            
            //Copia a primeira metade para a segunda metade
            for(directory_size_t i = 0; i < hash->dr_size / 2; i++){
                hash->dr[hash->dr_size / 2 + i].pl = hash->dr[i].pl;
                hash->dr[hash->dr_size / 2 + i].bucket = hash->dr[i].bucket;
            }
        }
        
        // Se precisou duplicar diretorio, fez isso no if acima
        // Se nao precisou duplicar, nao passou pelo if e eh soh
        // Arrumar o diretorio (criar novo bucket no final)

        // Proximo indice que aponta pro mesmo bucket original cheio
        bucket_t bucket_duplicado = bucket + hash->dr_size / 2;
        hash->dr[bucket].pl++;

        // Arruma as info do bucket duplicado (novo)
        // Ele tem a mesma pl nova de pg nova
        // Ele estah no final do arquivo
        hash->dr[bucket_duplicado].pl++;
        fseek(hash->fp, 0, SEEK_END);
        hash->dr[bucket_duplicado].bucket = ftell(hash->fp);
        hash->bucket_number ++;
        

        // Vai para o bucket original
        fseek(hash->fp, hash->dr[bucket].bucket, SEEK_SET);

        // Pega todos elementos do bucket original e arruma eles
        for(bucket_size_t j = 0; j < hash->bucket_size; j++){
            fread(&buffer[j], sizeof(struct registro), 1, hash->fp);
            // Registro faz ainda parte do bucket original
            if(hash->dr[_HASH_FUNCTION(buffer[j].nseq, hash->dr_size)].bucket == hash->dr[bucket].bucket) buffer[j].nseq = 0;
            else{
                // Caso contrario, registro tem que ser colocado no bucket duplicado
                // Portanto, abaixo, registro eh apagado do bucket original
                fseek(hash->fp, -(long int)sizeof(struct registro), SEEK_CUR);
                // Limpa registro (nseq = 0) do bucket original
                fwrite(&aux, sizeof(struct registro), 1, hash->fp);
            }
        }

        // Vai para o bucket duplicado
        fseek(hash->fp, hash->dr[bucket_duplicado].bucket, SEEK_SET);

        // Escreve o buffer no bucket duplicado (cria no final do arquivo)
        bucket_size_t qtd = 0;
        for(bucket_size_t j = 0; j < hash->bucket_size; j++){
            fwrite(&buffer[j], sizeof(struct registro), 1, hash->fp);
            qtd++;
        }
        
        // Registro da insercao cai no bucket original
        if(_HASH_FUNCTION(reg->nseq, hash->dr_size) == bucket){
            fseek(hash->fp, hash->dr[bucket].bucket, SEEK_SET);
            for(original = 0; original < hash->bucket_size; original++){
                fread(&aux, sizeof(struct registro), 1, hash->fp);
                if(aux.nseq == 0) break;
            }

            if(original != hash->bucket_size){
                fseek(hash->fp, -(long int)sizeof(struct registro), SEEK_CUR);
                fwrite(reg, sizeof(struct registro), 1, hash->fp);
            }
        }
        // Registro cai no novo bucket
        else{
            fseek(hash->fp, hash->dr[bucket_duplicado].bucket, SEEK_SET);
            for(original = 0; original < hash->bucket_size; original++){
                fread(&aux, sizeof(struct registro), 1, hash->fp);
                if(aux.nseq == 0) break;
            }

            if(original != hash->bucket_size){
                fseek(hash->fp, -(long int)sizeof(struct registro), SEEK_CUR);
                fwrite(reg, sizeof(struct registro), 1, hash->fp);
                qtd++;
            }
        }

        // Como criou um bucket no final, bucket_number++
        hash->bucket_number++;
    }
	
    fclose(hash->fp);
    return 1;
}

int RMV_HASH(Hash hash, entry_number_t chave, Registro reg){
    if(_NVLD_HASH(hash) || reg == NULL) return 0;

    // Abre o arquivo hash
    hash->fp = fopen(hash->fname, "r+");
    if(hash->fp == NULL) return 0;

    bucket_t bucket = _HASH_FUNCTION(chave, hash->dr_size);

    fseek(hash->fp, hash->dr[bucket].bucket, SEEK_SET);

    struct registro aux;
    aux.nseq = 0;

    for(bucket_size_t i = 0; i < hash->bucket_size; i++){
        if(!fread(reg, sizeof(struct registro), 1, hash->fp)) {
        	fclose(hash->fp);
    		return 0;
        }

        // Se eh chave, achou o registro, que jah estah em reg
        if(reg->nseq == chave){
            fseek(hash->fp, -(long int)sizeof(struct registro), SEEK_CUR);
            fwrite(&aux, sizeof(struct registro), 1, hash->fp);
            fclose(hash->fp);
            return 1;
        }
    }
    
    fclose(hash->fp);
    return 0;
}

int PRNT_HASH(Hash hash){
    if(_NVLD_HASH(hash)) return 0;

    // Abre o arquivo hash
    hash->fp = fopen(hash->fname, "r");
    if(hash->fp == NULL) return 0;

    printf("Tamanho diretorio = %u\n", hash->dr_size);
    printf("Buckets instanciados = %ld\n", hash->bucket_number);
    printf("Numero de registro por bucket = %u\n", hash->bucket_size);
    printf("Profundidade global = %u\n", hash->pg);

    printf("Conteudo do diretorio:\n\n");    

    struct registro aux;

    for(directory_size_t i = 0; i < hash->dr_size; i++){
        printf("(B%ld, %u):", hash->dr[i].bucket, hash->dr[i].pl);

        fseek(hash->fp, hash->dr[i].bucket, SEEK_SET);

        for(bucket_size_t j = 0; j < hash->bucket_size; j++){
            fread(&aux, sizeof(struct registro), 1, hash->fp);

            if(aux.nseq != 0) printf(" <%u, %s> |", aux.nseq, aux.text);
            else printf(" <%u> |", aux.nseq);
        }

        printf("\n\n");
    }

    fclose(hash->fp);

    return 1;
}

int PERSISTE_DIR(Hash hash) {
    FILE *dir = fopen("dir", "w+");
    if (!dir) return 0;

    fwrite(&hash->dr_size, sizeof(directory_size_t), 1, dir); 
    fwrite(&hash->bucket_number, sizeof(bucket_t), 1, dir);
    fwrite(&hash->bucket_size, sizeof(bucket_size_t), 1, dir); 
    fwrite(&hash->pg, sizeof(depth_t), 1, dir);
    fwrite(&hash->dr, sizeof(directory_t) * hash->dr_size, 1, dir);
    

    fclose(dir);
    return 1;
}
