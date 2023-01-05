#ifndef REGISTRO_H_INCLUDED
#define REGISTRO_H_INCLUDED

#pragma pack(1)

#define TEXT_SIZE 46

#include <limits.h>
#define ENTRY_MAX INT_MAX

typedef unsigned int entry_number_t;

struct registro{
	entry_number_t nseq;
	char text[TEXT_SIZE];
};

typedef struct registro* Registro;

#endif
