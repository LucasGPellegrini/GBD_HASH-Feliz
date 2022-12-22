#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "registro.h"
#include "size.h"
#include <stdio.h>

typedef entry_number_t directory_size_t;
typedef long int bucket_t;
typedef entry_number_t bucket_size_t;
typedef entry_number_t depth_t;

struct directory_entry{
    bucket_t bucket;
    depth_t pl;
};

typedef struct directory_entry* directory_t;

struct hash{
    directory_t dr; // diretorio do indice hash
    directory_size_t dr_size; // tamanho do diretorio

    bucket_t bucket_number; // numero de buckets instanciados
    bucket_size_t bucket_size; // numero de reg por bucket

    depth_t pg; // profundidade global
    
    FILE* fp; // ponteiro de arquivo em hash
    char* fname; // nome do arquivo em hash
};

typedef struct hash* Hash;

int CRT_HASH(Hash* hash_ptr, depth_t pg_inicial, char* hdir);
int SRCH_HASH(Hash hash, entry_number_t chave, Registro reg);
int INST_HASH(Hash hash, Registro reg);
int RMV_HASH(Hash hash, entry_number_t chave, Registro reg);
int PRNT_HASH(Hash hash);
int PERSISTE_DIR(Hash hash);
int RECUPERA_DIR(Hash *hash_ptr, directory_size_t tam, bucket_size_t bs, bucket_t bn, depth_t depth, directory_t dir_t, char* hdir);

#endif