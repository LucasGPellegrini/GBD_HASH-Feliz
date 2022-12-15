#include "hash.h"
#include <stdio.h>
#include <stdlib.h>

// Funcao hash (h(key) mod N, com h(key) = key)
bucket_t _HASH_FUNCTION(entry_number_t chave, bucket_t N){
    return chave % N;
}

int CRT_HASH(Hash hash, depth_t pg_inicial, char* hdir){
    if(hash == NULL || pg_inicial < 0) return 0;

    hash = malloc(sizeof(struct hash));
    if(hash == NULL) return 0;

    directory_size_t dr_size = 1;

    for(depth_t i = 0; i < pg_inicial; i++)
        dr_size *= 2;

    hash->dr_size = dr_size;
    hash->bucket_size = dr_size;
    hash->bucket_number = hash->dr_size;
    hash->pg = pg_inicial;

    hash->dr = malloc(hash->dr_size * sizeof(struct directory_entry));
    if(hash->dr == NULL){
        free(hash);
        hash = NULL;
        return 0;
    }

    for(directory_size_t i = 0; i < hash->dr_size; i++){
        hash->dr[i].pl = hash->pg;
        hash->dr[i].bucket = _HASH_FUNCTION(i, hash->pg);
    }

    hash->fp = fopen(hdir, "wb");
    if(hash->fp == NULL){
        free(hash->dr);
        free(hash);
        hash = NULL;
        return 0;
    }

    strcpy(&hash->fname, hdir);

    rewind(hash->fp);

    struct registro reg;
    reg.nseq = 0;
    strcpy(&reg->text, "\0");

    for(directory_size_t i = 0; i < hash->dr_size; i++){
        if(fwrite(&reg, sizeof(struct registro), hash->dr_size, hash->fp)
        != hash->dr_size * sizeof(struct registro)){
            fclose(hash->fp);
            free(hash->dr);
            free(hash);
            hash = NULL;
            return 0;
        }
    }

    fclose(hash->fp);

    return 1;
}

int _NVLD_HASH(Hash hash){
    return(hash == NULL || hash->dr == NULL || hash->pg < 0 || hash->fp == NULL);
}

// entry_number_t _PG_LST_BITS(Hash hash){
//     entry_number_t pg_ult_bits = ENTRY_MAX;
//     pg_ult_bits <<= hash->pg;
//     pg_ult_bits = ~pg_ult_bits;
//     return pg_ult_bits;
// }

int SRCH_HASH(Hash hash, entry_number_t chave, Registro reg){
    if(_NVLD_HASH(hash) || !chave || reg == NULL) return 0;

    // Abre o arquivo hash
    hash->fp = fopen(hash->fname, "rb");
    if(hash->fp == NULL) return 0;

    bucket_t bucket = _HASH_FUNCTION(chave, hash->bucket_number);

    //entry_number_t pg_ult_bits = _PG_LST_BITS(hash);

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
    hash->fp = fopen(hash->fname, "wb");
    if(hash->fp == NULL) return 0;

    bucket_t bucket = _HASH_FUNCTION(reg->nseq, hash->bucket_number);

    // Achar bucket com essa hash
    fseek(hash->fp, bucket, SEEK_SET);    

    // Vai a procura de um slot vazio no bucket da hash (nseq == 0)
    bucket_size_t i;
    for(i = 0; i < hash->bucket_size; i++){
        if(!fread(reg, sizeof(struct registro), 1, hash->fp)) return 0;
        if(reg->nseq == 0) break;
    }

    // Bucket nao-cheio (insercao tranquila)
    if(i != hash->bucket_size){
        fseek(hash->fp, -(long int)sizeof(struct registro), SEEK_CUR);
        fwrite(reg, sizeof(struct registro), 1, hash->fp);
    }else{

        struct registro buffer[hash->bucket_size];

        // Nao precisa duplicar diretorio
        if(hash->pg > hash->dr[bucket].pl){
            // Proximo indice que aponta pro mesmo bucket cheio
            bucket_t bucket_duplicado = (i << (hash->pg - 1)) + i; // Conferir conta
            hash->dr[i].pl++;
            hash->dr[bucket_duplicado].pl++;

            fseek(hash->fp, hash->dr[i].bucket, SEEK_SET);
            for(bucket_size_t j = 0; j < hash->bucket_size; j++){
                if(!fread(aux, sizeof(struct registro), 1, hash->fp)) return 0;

            }
        }
        // Precisa duplicar diretorio
        else{

        }

        // Bucket cheio, tem que duplicar diretorio
        directory_t novo_dr = realloc(hash->dr, 2 * hash->dr_size);
        if(novo_dr == NULL) return 0;

        hash->dr = novo_dr;
        hash->dr_size *= 2;
        hash->pg++;

        struct registro aux;
        entry_number_t zero = 0;

        // Transfere a 1a metade dos slots do bucket para o novo bucket criado
        for(bucket_size_t i = 0; i < hash->bucket_size / 2; i++){
            fseek(hash->fp, hash->dr[bucket].bucket + i * sizeof(struct registro), SEEK_SET);
            fread(&aux, sizeof(struct registro), 1, hash->fp);
            
            fseek(hash->fp, -(long int)sizeof(struct registro), SEEK_CUR);
            fwrite(&zero, sizeof(entry_number_t), 1, hash->fp);

            fseek(hash->fp, 0, SEEK_END);
            fwrite(&aux, sizeof(struct registro), 1, hash->fp);
        }
        
        fwrite(reg, sizeof(struct registro), 1, hash->fp);

        // Aumenta pl do bucket encontrado (pois teve split dele)
        hash->dr[bucket].pl++;
        // Ambos com o mesmo pl (o bucket criado tem o mesmo que o splitado)
        hash->dr[hash->bucket_number].pl = hash->dr[bucket].pl;
        // Aponta o diretorio novo para o bucket novo criado
        hash->dr[hash->bucket_number].bucket = hash->bucket_number;
        // Aumentou +1 bucket
        hash->bucket_number++;

        // Aponta os outros diretorios para os buckets normais
        entry_number_t pg_ult_bits = _PG_LST_BITS(hash);

        for(directory_size_t i = hash->dr_size / 2; i < hash->dr_size; i++){
            // Se esta no diretorio do bucket criado no fim, passa (ja resolvido acima)
            if(i && pg_ult_bits == bucket) continue;

            hash->dr[i].pl = hash->dr[i / hash->bucket_size].pl;
            hash->dr[i].bucket = _HASH_FUNCTION(i, hash->bucket_size);
        }
    }

    return 1;
}

int PRNT_HASH(Hash hash){
    if(_NVLD_HASH(hash)) return 0;

    printf("Tamanho diretorio = %u\n", hash->dr_size);
    printf("Buckets instanciados = %u\n", hash->bucket_number);
    printf("Numero de registro por bucket = %u\n", hash->bucket_size);
    printf("Profundidade global = %u\n", hash->pg);

    printf("Conteudo do diretorio:\n\n");    

    struct registro aux;

    for(directory_size_t i = 0; i < hash->dr_size; i++){
        printf("Bucket = %u | Pl = %u :", hash->dr[i].bucket, hash->dr[i].pl);

        fseek(hash->fp, hash->dr[i].bucket, SEEK_SET);

        for(bucket_size_t j = 0; j < hash->bucket_size; j++){
            fread(&aux, sizeof(struct registro), 1, hash->fp);
            printf("Nseq = %d - Text = %s", aux.nseq, aux.text);
        }
    }

    return 1;
}