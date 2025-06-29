#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estrutura dos documentos
typedef struct documento {
    int num_paginas;
    char nome[51]; 
    int tempo_impressao;
    struct documento *proximo;
} documento;

typedef struct impressora {
	int tempo_conclusao;
	char nome[51];
} impressora;

// Estrutura da fila (ordem de chegada)
typedef struct fila {
    documento *primeiro;
    documento *ultimo;
} fila;

// Estrutura da pilha (saída de documentos)
typedef struct pilha {
    documento *topo;
} pilha;

// Iniciar a fila 
void criar_fila (fila *fila) {
	fila->primeiro = NULL;
	fila->ultimo = NULL;
}

// Função que inicializa a pilha
void inicia_pilha(pilha *p) {
    p->topo = NULL;
}

// Função que cria um novo documento
documento *criar_doc(char*nome, int num_paginas) {

    documento* novo_doc = (documento*)malloc(sizeof(documento));	
    strncpy(novo_doc->nome, nome, 50);
    novo_doc->nome[50] = '\0';
    novo_doc->num_paginas = num_paginas;
    novo_doc->tempo_impressao = num_paginas;
    novo_doc->proximo = NULL;
    
 return novo_doc;    
}

// Função que adiciona um novo doc na fila de impressão 
void adc_documento(fila *fila, char *nome, int num_paginas) {

    documento* novo_doc = criar_doc(nome, num_paginas);

    // se a fila estiver vazia
    if (fila->primeiro == NULL) {
    	fila->primeiro = novo_doc;
    	fila->ultimo = novo_doc;
    } else {
    	fila->ultimo->proximo = novo_doc;
    	fila->ultimo = novo_doc;;
    }
}

// Função para ler as impressoras
void gerenciar_impressoras(FILE *input, impressora **total_impressoras, int *num_impressoras) {
    char linha[120];
	
	// Le o total de impressoras
    if (fgets(linha, sizeof(linha), input) != NULL) {
        *num_impressoras = atoi(linha);
    }

	*total_impressoras = malloc(*num_impressoras * sizeof(impressora));
    // Lê os nomes das impressoras e salva no array
    for (int i = 0; i < *num_impressoras; i++) {
        if (fgets(linha, sizeof(linha), input) != NULL) {
            linha[strcspn(linha, "\n")] = '\0';
            strncpy((*total_impressoras)[i].nome, linha, 50);
            (*total_impressoras)[i].nome[50] = '\0';
    		(*total_impressoras)[i].tempo_conclusao = 0;
    	}
    }
}

// Função para ler os documentos e criar uma fila
void ler_documentos(FILE *input, fila *fila_docs) {
    char linha[120];
    char nome[51];
    int num_paginas;
    int num_documentos;

    // Lê o número total de documentos
    if (fgets(linha, sizeof(linha), input) != NULL) {
        num_documentos = atoi(linha);
    } else {
        printf("Erro ao ler o número de documentos\n");
        return;
    }

    // Lê os documentos e adiciona à fila
    while (fgets(linha, sizeof(linha), input) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';

        // Lê o nome e o número de páginas do documento
        if (sscanf(linha, "%50s %d", nome, &num_paginas) == 2) {
            adc_documento(fila_docs, nome, num_paginas);
        } else {
            printf("Erro ao ler o documento: %s\n", linha);
        }
    }
}

// Função que gerencia as impressões

void atribuir_docs (fila *fila_docs, impressora *total_impressoras, int num_impressoras) {

	while (fila_docs->primeiro != NULL) {
		int impressora_menor = 0;
		for (int i = 1; i < num_impressoras; i++) {
			if (total_impressoras[i].tempo_conclusao < total_impressoras[impressora_menor].tempo_conclusao) {				
				impressora_menor = i;
			}
		}

	documento *doc_atual = fila_docs->primeiro;
	fila_docs->primeiro = fila_docs->primeiro->proximo;
	total_impressoras[impressora_menor].tempo_conclusao += doc_atual->tempo_impressao;

	free(doc_atual);

	printf("Documento atribuído à impressora %s, tempo de conclusão atualizado para %d\n",
    total_impressoras[impressora_menor].nome, total_impressoras[impressora_menor].tempo_conclusao);	

	}
}
	


// Main
int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Uso correto: %s arquivo_entrada arquivo_saida\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");

    if (input == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }
    if (output == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(input);
        return 1;
    }

    // Variáveis para a chamada das funções de gerenciamento
    int num_impressoras;
    impressora *total_impressoras = NULL;

    // Inicia a fila nula
    fila fila_docs;
    criar_fila(&fila_docs);

  // Gerenciar impressoras
	gerenciar_impressoras(input, &total_impressoras, &num_impressoras);
  // Gerenciar documentos
	ler_documentos(input, &fila_docs);
  // Gerenciar as impressões
	atribuir_docs(&fila_docs, total_impressoras, num_impressoras);

  fclose(input);
  fclose(output);

  free(total_impressoras);

 return 0;
 }
