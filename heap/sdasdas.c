#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct aposta {
    char* codigo;
    uint8_t numeros[15];
    uint8_t acertos;
} Aposta;

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

    uint8_t qtd_apostas = 0, premio = 0;
    uint8_t sorteados[10];


    fscanf(input, "%d", &qtd_apostas);
    fscanf(input, "%d", &premio);

    for (int i = 0; i < 10; i++) {
        fscanf(input, "%d", &sorteados[i]);
    }   

    for (int i = 0; i < qtd_apostas; i++) {
        Aposta *aposta = (Aposta*) malloc(sizeof(Aposta)); 
        aposta->codigo = (char*) malloc(100 * sizeof(char));
        
        fscanf(input, "%32s", aposta->codigo);
        printf("Codigo: %s\n", aposta->codigo);

        for (int j = 0; j < 15; j++) {
            fscanf(input, "%d", &aposta->numeros[j]);
        }
    }

} // Fim da main