#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definicao da estrutura 'arquivo'
typedef struct arquivo {
    char *nome;
    char *permissao;
    int tamanho, ordem;
} Arquivo;

typedef struct arvore {
    Arquivo *raiz;
    struct arvore *direito;
    struct arvore *esquerdo;
} Arvore;

void inserir_arvore(Arvore **arvore, Arquivo *novo) {
    
    if (*arvore == NULL) {
        Arvore *novoNo = (Arvore *)malloc(sizeof(Arvore));
        novoNo->raiz = novo;
        novoNo->direito = NULL;
        novoNo->esquerdo = NULL;

    } else {
        int cmp = strcmp(novo->nome, (*arvore)->raiz->nome);
        if (cmp < 0) {
            inserir_arvore(&(*arvore)->esquerdo, novo);
        } else if(cmp > 0) {
            inserir_arvore(&(*arvore)->direito, novo);
        } else {
            if (strcmp((*arvore)->raiz->permissao, "rw") == 0) {
                free((*arvore)->raiz->nome);
                free((*arvore)->raiz->permissao);
                free((*arvore)->raiz);
                (*arvore)->raiz = novo;

            } else {
                free(novo->nome);
                free(novo->permissao);
                free(novo);               
            }
        }
    }
}

void EPD(Arvore *arvore, FILE *output) {
    // Percurso: esquerda -> raíz -> direita,

    if (arvore->esquerdo != NULL) {
        EPD(arvore->esquerdo, output);
    } else {
       // printf("%d %s %s %d bytes\n", arvore->esquerdo->ordem, arvore->esquerdo->)
    }
}

void PED(Arvore *raiz, FILE *output) {
    // Percurso: raíz -> esquerda -> direita
}

void EDP(Arvore *raiz, FILE *output) {
    // Percurso: esquerda -> direita -> raíz
}

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

    int qtd_arquivos;
    char linha[1000];

    fscanf(input, "%d", &qtd_arquivos);

    Arquivo *arquivos = (Arquivo *)malloc(qtd_arquivos * sizeof(Arquivo)); // Aloca memória para os arquivos
    Arvore *arvore = NULL;

    for (int i = 0; i < qtd_arquivos; i++) {
        // Alocar memória para as strings
        arquivos[i].nome = (char *)malloc(51 * sizeof(char));  // Ajuste o tamanho conforme necessário
        arquivos[i].permissao = (char *)malloc(3 * sizeof(char)); // Ajuste o tamanho conforme necessário
        fscanf(input, "%s %s %d", arquivos[i].nome, arquivos[i].permissao, &arquivos[i].tamanho);
        
            Arquivo *novoArquivo = (Arquivo *)malloc(sizeof(Arquivo));

            for (int j = 0; j < i; j++) {
                if (strcmp(novoArquivo->nome, arquivos[j].nome) == 0) {
                    if(strcmp(arquivos[j].permissao, "ro") == 0) {
                        free(novoArquivo);
                        break;
                    }
                }
            }

            novoArquivo->nome = strdup(arquivos[i].nome);
            novoArquivo->permissao = strdup(arquivos[i].permissao);
            novoArquivo->tamanho = arquivos[i].tamanho;
            novoArquivo->ordem = i;
            
            inserir_arvore(&arvore, novoArquivo);
            printf("Arquivo inserido: [%d] %s\n", novoArquivo->ordem, novoArquivo->nome);

        free(arquivos[i].nome);
        free(arquivos[i].permissao);
        }

    fclose(input);
    fclose(output);

    free(arquivos);

    return 0;
}