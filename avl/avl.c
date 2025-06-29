#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Definição da estrutura palavra
typedef struct palavra {
    char* nome;
    int qtd_sinonimos;
    char** sinonimos;
} Palavra;

// Definição da estrutura do nó da árvore AVL
typedef struct No {
    char palavra[31];            // Palavra (até 30 caracteres)
    char sinonimos[10][31];      // Lista de sinônimos (até 10 palavras, cada uma com 30 caracteres)
    int num_sinonimos;           // Número de sinônimos
    struct No *esquerda;         // Subárvore esquerda
    struct No *direita;          // Subárvore direita
    int altura;                  // Altura do nó
} No;

// Definição da estrutura da árvore AVL
typedef struct {
    No* raiz;
} AVL;

// Função para criar a árvore AVL
AVL* criarArvore() {
    AVL* avl = (AVL*) malloc(sizeof(AVL));
    avl->raiz = NULL;
    return avl;
}

int altura(No *no) {
    if (no == NULL)
        return 0;
    return no->altura;
}

int fator_balanceamento(No *no) {
    if (no == NULL)
        return 0;
    return altura(no->esquerda) - altura(no->direita);
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

No* rotacao_direita(No *y) {
    No *x = y->esquerda;
    No *T2 = x->direita;

    // Realiza a rotação
    x->direita = y;
    y->esquerda = T2;

    // Atualiza as alturas
    y->altura = max(altura(y->esquerda), altura(y->direita)) + 1;
    x->altura = max(altura(x->esquerda), altura(x->direita)) + 1;

    return x; // Novo nó raiz
}

No* rotacao_esquerda(No *x) {
    No *y = x->direita;
    No *T2 = y->esquerda;

    // Realiza a rotação
    y->esquerda = x;
    x->direita = T2;

    // Atualiza as alturas
    x->altura = max(altura(x->esquerda), altura(x->direita)) + 1;
    y->altura = max(altura(y->esquerda), altura(y->direita)) + 1;

    return y; // Novo nó raiz
}

No* inserir(No* no, char palavra[], char sinonimos[][31], int num_sinonimos) {
    if (no == NULL) {
        No *novoNo = (No*) malloc(sizeof(No));
        strcpy(novoNo->palavra, palavra);
        novoNo->num_sinonimos = num_sinonimos;
        for (int i = 0; i < num_sinonimos; i++) {
            strcpy(novoNo->sinonimos[i], sinonimos[i]);
        }
        novoNo->esquerda = novoNo->direita = NULL;
        novoNo->altura = 1;
        return novoNo;
    }

    if (strcmp(palavra, no->palavra) < 0) {
        no->esquerda = inserir(no->esquerda, palavra, sinonimos, num_sinonimos);
    } else if (strcmp(palavra, no->palavra) > 0) {
        no->direita = inserir(no->direita, palavra, sinonimos, num_sinonimos);
    } else {
        return no; // Evita duplicação
    }

    no->altura = 1 + max(altura(no->esquerda), altura(no->direita));
    int balance = fator_balanceamento(no);

    if (balance > 1 && strcmp(palavra, no->esquerda->palavra) < 0)
        return rotacao_direita(no);

    if (balance < -1 && strcmp(palavra, no->direita->palavra) > 0)
        return rotacao_esquerda(no);

    if (balance > 1 && strcmp(palavra, no->esquerda->palavra) > 0) {
        no->esquerda = rotacao_esquerda(no->esquerda);
        return rotacao_direita(no);
    }

    if (balance < -1 && strcmp(palavra, no->direita->palavra) < 0) {
        no->direita = rotacao_direita(no->direita);
        return rotacao_esquerda(no);
    }

    return no;
}

No* busca(No *no, char palavra[], FILE *output) {
    if (no == NULL) {
        fprintf(output, "?]");
        return NULL;
    }

    if (strcmp(palavra, no->palavra) == 0) {
        fprintf(output, "%s", no->palavra);  // Não imprime seta para a última palavra
        return no;
    }

    if (strcmp(palavra, no->palavra) < 0) {
        fprintf(output, "%s->", no->palavra);  // Imprime a seta corretamente
        return busca(no->esquerda, palavra, output);
    } else {
        fprintf(output, "%s->", no->palavra);  // Imprime a seta corretamente
        return busca(no->direita, palavra, output);
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

    int qtd_palavras;
    fscanf(input, "%d", &qtd_palavras);

    AVL *arvore = criarArvore();

    // Inserir palavras e sinônimos na árvore AVL
    for (int i = 0; i < qtd_palavras; i++) {
        char palavra[31];
        int num_sinonimos;
        fscanf(input, "%s %d", palavra, &num_sinonimos);

        char sinonimos[10][31];
        for (int j = 0; j < num_sinonimos; j++) {
            fscanf(input, "%s", sinonimos[j]);
        }

        arvore->raiz = inserir(arvore->raiz, palavra, sinonimos, num_sinonimos);
    }

    int qtd_consultas;
    fscanf(input, "%d", &qtd_consultas);

    // Realizar consultas e imprimir resultados
    for (int i = 0; i < qtd_consultas; i++) {
        char consulta[31];
        fprintf(output, "[");
        fscanf(input, "%s", consulta);

        No *resultado = busca(arvore->raiz, consulta, output);
        
        if (resultado != NULL) {
            // Finaliza o percurso dentro dos colchetes
            fprintf(output, "]\n");

            // Listar sinônimos sem colchetes e com vírgula
            for (int j = 0; j < resultado->num_sinonimos; j++) {
                fprintf(output, "%s%s", resultado->sinonimos[j], 
                        j == resultado->num_sinonimos - 1 ? "\n" : ",");
            }
        } else {
            // Caso a palavra não seja encontrada
            fprintf(output, "\n-\n");
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}

