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
    struct documento *proximo;
} impressora;

// Estrutura dos documentos impressos por cada impressora
typedef struct impresso {
    char nome[51];
    int num_paginas;
    struct impresso *proximo;
} impresso;

// Estrutura da fila de documentos (ordem de chegada)
typedef struct fila {
    documento *primeiro;
    documento *ultimo;
} fila;

// Estrutura da pilha (saída de documentos)
typedef struct documento_concluido {
    char nome[51];
    int num_paginas;
    struct documento_concluido *proximo;
} documento_concluido;

typedef struct {
    char nome[51];
    impresso *impressos;
} historico_impressora;

// Iniciar a fila 
void criar_fila (fila *fila) {
	fila->primeiro = NULL;
	fila->ultimo = NULL;
}

// Função que cria um novo documento
documento *criar_doc(char*nome, int num_paginas, fila *fila_docs, FILE *input) {

    char linha[120];
    int num_documentos;
    documento* novo_doc = (documento*)malloc(sizeof(documento));

    // Lê o número total de documentos
    if (fgets(linha, sizeof(linha), input) != NULL) {
        num_documentos = atoi(linha);
    } else {
        printf("Erro 00: erro ao ler o número de documentos\n");
    }

    // Lê os documentos e adiciona à fila
    while (fgets(linha, sizeof(linha), input) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';    
       if (sscanf(linha, "%50s %d", nome, &num_paginas) == 2) {
        } else {
            printf("Erro 01: erro ao ler o documento: %s\n", linha);
        }
    if (sscanf(linha, "%50s %d", nome, &num_paginas) == 2) {	
    strncpy(novo_doc->nome, nome, 50);
    novo_doc->nome[50] = '\0';
    novo_doc->num_paginas = num_paginas;
    novo_doc->tempo_impressao = num_paginas;
    novo_doc->proximo = NULL;

    } else {
        printf("Erro 02: erro ao ler o documento\n");
     }
    } // while
 return novo_doc;    
} // fim do procedimento de criar documento

// Função para ler as impressoras e colocar em um array
impressora* gerenciar_impressoras(FILE *input, int *num_impressoras) {
    char linha[120];

	// Le o total de impressoras
    if (fgets(linha, sizeof(linha), input) != NULL) {
        *num_impressoras = atoi(linha);
    }

    impressora *impressoras = (impressora *)malloc(*num_impressoras * sizeof(impressora));
    // Lê os dados das impressoras e salva no array
    for (int i = 0; i < *num_impressoras; i++) {
        if (fgets(linha, sizeof(linha), input) != NULL) {
            linha[strcspn(linha, "\n")] = '\0';
            strncpy((impressoras)[i].nome, linha, 50);
            (impressoras)[i].nome[50] = '\0';
    		(impressoras)[i].tempo_conclusao = 0;
            (impressoras)[i].proximo = NULL;
    	}
    }
    return impressoras;
}

// Função que adc documentos a pilha
void adicionar_pilha(documento_concluido**topo, char *nome, int num_paginas) {
    documento_concluido *novo_doc = (documento_concluido *)malloc(sizeof(documento_concluido));
    strncpy(novo_doc->nome, nome, 50);
    novo_doc->nome[50] = '\0';
    novo_doc->num_paginas = num_paginas;
    novo_doc->proximo = *topo;
    *topo = novo_doc;
}

// Função para verificar se algum documento ainda está sendo impresso
int algum_documento_em_impressao(impressora *lista_impressoras, int num_impressoras) {
    for (int i = 0; i < num_impressoras; i++) {
        if (lista_impressoras[i].tempo_conclusao > 0) {
            return 1;
        }
    }
    return 0;
}

void imprimir_historico(FILE *output, historico_impressora *historico, impressora *lista_impressoras, int num_impressoras, int impressora_id) {
        if (historico[impressora_id].impressos != NULL) {
            fprintf(output, "[%s] ", lista_impressoras[impressora_id].nome);
            impresso *atual = historico[impressora_id].impressos;
            while (atual != NULL) {
                fprintf(output, "%s -%dp", atual->nome, atual->num_paginas);
                if (atual->proximo != NULL) {
                    fprintf(output, ", ");
                }
                atual = atual->proximo;
            }
            fprintf(output, "\n");
        }
    }

void imprimir_pilha(FILE *output, documento_concluido *topo) {
    documento_concluido *atual = topo;
    while (atual != NULL) {
        fprintf(output, "%s -%dp\n", atual->nome, atual->num_paginas);
        documento_concluido *temp = atual;
        atual = atual->proximo;
        free(temp);
    }
}

void gerenciar_impressao(FILE *input, impressora *lista_impressoras, historico_impressora *historico, fila *fila_documentos, int num_impressoras, FILE *output) {
    char linha[120];
    char nome[51];
    int num_paginas;
    int num_documentos;
    documento_concluido *pilha_concluidos = NULL;

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
            documento *novo_doc = (documento *)malloc(sizeof(documento));
            strncpy(novo_doc->nome, nome, 50);
            novo_doc->nome[50] = '\0';
            novo_doc->num_paginas = num_paginas;
            novo_doc->tempo_impressao = num_paginas;
            novo_doc->proximo = NULL;

            // Adiciona o documento à fila
            if (fila_documentos->primeiro == NULL) {
                fila_documentos->primeiro = novo_doc;
                fila_documentos->ultimo = novo_doc;
            } else {
                fila_documentos->ultimo->proximo = novo_doc;
                fila_documentos->ultimo = novo_doc;
            }
        } else {
            printf("Erro ao ler o documento: %s\n", linha);
        }
    }

     // Verificar se há impressora disponíveis
    int tempo = 1;
    while (fila_documentos->primeiro != NULL || algum_documento_em_impressao(lista_impressoras, num_impressoras)) {
        // Verificar e processar documentos nas impressoras
        for (int i = 0; i < num_impressoras; i++) {
            if (lista_impressoras[i].tempo_conclusao == 0 && fila_documentos->primeiro != NULL) {
                // Atualizando a fila de documentos
                documento *atual = fila_documentos->primeiro;
                fila_documentos->primeiro = fila_documentos->primeiro->proximo;
                lista_impressoras[i].tempo_conclusao = atual->num_paginas;

                // Criando o histórico da impressora
                impresso *novo_impresso = (impresso *)malloc(sizeof(impresso));
                strncpy(novo_impresso->nome, atual->nome, 50);
                novo_impresso->nome[50] = '\0';
                novo_impresso->num_paginas = atual->num_paginas;
                novo_impresso->proximo = historico[i].impressos;
                historico[i].impressos = novo_impresso;

                documento_concluido *novo_concluido = (documento_concluido *)malloc(sizeof(documento_concluido));
                strncpy(novo_concluido->nome, atual->nome, 50);
                novo_concluido->nome[50] = '\0';
                novo_concluido->num_paginas = atual->num_paginas;
                novo_concluido->proximo = pilha_concluidos;
                pilha_concluidos = novo_concluido;               
                imprimir_historico(output, historico, lista_impressoras, num_impressoras, i);
                free(atual);
            }
        }
        // Reduzir o tempo de conclusão de todas as impressoras para simular a passagem do tempo
        for (int i = 0; i < num_impressoras; i++) {
            if (lista_impressoras[i].tempo_conclusao > 0) {
                lista_impressoras[i].tempo_conclusao--;
            }
        }
        tempo++;
    }
    int total_paginas = 0;
    for (int i = 0; i < num_impressoras; i++) {
        impresso *atual = historico[i].impressos;
        while (atual != NULL) {
            total_paginas += atual->num_paginas;
            atual = atual->proximo;
        } 
    }
    fprintf(output, "%d p\n", total_paginas);
  
     documento_concluido *atual_concluido = pilha_concluidos;
    while (atual_concluido != NULL) {
        fprintf(output, "%s -%dp\n", atual_concluido->nome, atual_concluido->num_paginas);
        documento_concluido *temp = atual_concluido;
        atual_concluido = atual_concluido->proximo;
        free(temp);
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


    // Inicia a fila nula
    fila fila_docs;
    criar_fila(&fila_docs);

  // Gerenciar impressoras
    int num_impressoras;
	impressora *lista_impressoras = gerenciar_impressoras(input, &num_impressoras);

    historico_impressora *historico = malloc(num_impressoras * sizeof(historico_impressora));
    for (int i = 0; i < num_impressoras; i++) {
        strncpy(historico[i].nome, lista_impressoras[i].nome, 50);
        historico[i].nome[50] = '\0';
        historico[i].impressos = NULL;
    }

  // Gerenciar impressoras
    gerenciar_impressao(input, lista_impressoras, historico, &fila_docs, num_impressoras, output);

  fclose(input);
  fclose(output);

    for (int i = 0; i < num_impressoras; i++) {
        // Não há memória adicional alocada para cada impressora
    }
    free(lista_impressoras);

    for (int i = 0; i < num_impressoras; i++) {
        impresso *atual = historico[i].impressos;
        while (atual != NULL) {
            impresso *temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
    free(historico);

    documento *atual_doc = fila_docs.primeiro;
    while (atual_doc != NULL) {
        documento *temp = atual_doc;
        atual_doc = atual_doc->proximo;
        free(temp);
    }

 return 0;
 }

