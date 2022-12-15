
TRABALHO PRÁTICO - ETAPA 2

. Esta etapa tem como objetivo implementar algoritmos de acesso a registros de 
um arquivo por meio de índices armazenados em disco. 

. Implementar índices baseados em Árvores B+ ou Hash Dinâmico. 

. SEJA UM ARQUIVO COM REGISTROS DE TAMANHO FIXO NO SEGUINTE FORMATO: 
	
	REGISTRO(50)  
	. NSEQ   INT(4)   
	. TEXT   CHAR(46) 

	Considere dois índices: 
	1) Chave do índice: NSEQ
           Chave primária 
           Alternativa 1 (Entrada de dados no índice é: o próprio Registro)

	2) Chave do índice: TEXT
	   Chave duplicada
	   Alternativa 3 (Entrada de dados no índice é: chave + lista de RID)

. PEDE-SE
	ESCOLHA UMA ESTRUTURA DE DADOS (ÁRVORE B+ OU HASH DINÂMICO) E IMPLEMENTE
	AS OPERAÇÕES DE BUSCA, INSERÇÃO E REMOÇÃO DE REGISTROS TENDO COMO 
	ENTRADA UMA CHAVE

. OBS: OS FORMATOS DE REGISTROS E PÁGINAS DOS ARQUIVOS DE DADOS E ÍNDICES 
DEVEM SEGUIR A DESCRIÇÃO NO LIVRO-TEXTO OU DOS SLIDES APRESENTADOS EM 
SALA DE AULA. 

-------
O QUE ENTREGAR E O VALOR DE CADA ITEM ENTREGUE

I) Um aluno do grupo deve entregar, pelo MS Teams, o código fonte [Valor 2 pontos]

II) Um aluno do grupo deve entregar, pelo MS Teams, um exemplo de cada 
    arquivo de índice e de dados contendo, no máximo, 100KB em cada arquivo [Valor 2 pontos].
   Este aluno deve indicar os componentes do grupo.

III) Todos os alunos do grupo devem entregar, de forma individual, uma descrição das 
    estrutura de dados implementadas, ou seja, um documento descrevendo o formato da página
    e dos registros armazenados em disco para todas as estruturas de dados e índices. 
    No relatório o aluno deve indicar a referências aos formatos utilizados (slide do professor
    ou seção do livro texto em que se baseou para definir os formatos) [Valor 3]. 

IV) a critério do professor poderá ser solicitada uma apresentação de execução do código para
    auxiliar as avaliações dos itens acima.

------
---