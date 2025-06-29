#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct bet {
    char* code;
    int* numbers;
    int hits;
} Bet;

// Verificar se o número da aposta está nos números sorteados
int verify_bet (int number, int low, int high, int* drawn) {

    int mid = low + (high - low) / 2;

    if (low > high) {
        return 0;
    }
    
    if (drawn[mid] == number) {
        return 1;
    } else {
        if (drawn[mid] < number) {
            return verify_bet(number, mid + 1, high, drawn);
        } else {
            return verify_bet(number, low, mid - 1, drawn);
        }
    }
}

int esquerdo (int index) {
    return 2 * index + 1;
}

int direito(int index) {
    return 2 * index + 2;
}

void trocar(Bet *vetor, int pai, int index) {

    Bet aux = vetor[pai];
    vetor[pai] = vetor[index];
    vetor[index] = aux;
}

void heapify_max (Bet *vetor, int tamanho, int index) {

    int maior = index;
    int e = esquerdo(index);
    int d = direito(index);

    if (e < tamanho && vetor[e].hits > vetor[maior].hits) {
        maior = e;
    }

    if (d < tamanho && vetor[d].hits > vetor[maior].hits) {
        maior = d;
    }

    if (maior != index) {
        trocar(vetor, maior, index);
        heapify_max(vetor, tamanho, maior);
    }
}

void heapify_min (Bet *vetor, int tamanho, int index) {

    int menor = index;
    int e = esquerdo(index);
    int d = direito(index);

    if (e < tamanho && vetor[e].hits < vetor[menor].hits) {
        menor = e;
    }

    if (d < tamanho && vetor[d].hits < vetor[menor].hits) {
        menor = d;
    }

    if (menor != index) {
        trocar(vetor, menor, index);
        heapify_min(vetor, tamanho, menor);
    }
}

void winner (Bet* allBets, int qtd_bets, int award, FILE* output) {

    int winners = 1;
    int awardFinal = 0;
    
    // Verificando se existem mais ganhadores do prêmio
    for (int i = 1; i < qtd_bets; i++) {
        if (allBets[i].hits == allBets[0].hits) {
            winners++;
       }
    }

    awardFinal = award/winners;

     fprintf(output, "[%d:%d:%d]\n", winners, allBets[0].hits, awardFinal);

    fprintf(output, "%s\n", allBets[0].code);  // Primeiro vencedor
        for (int i = 1; i < qtd_bets; i++) {
            if (allBets[i].hits == allBets[0].hits) {
                fprintf(output, "%s\n", allBets[i].code);
            }
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

    int qtd_bets = 0, award = 0, awardMin = 0; // quantidade de apostas e o prêmio em reais
    int drawn[10]; // vetor dos números sorteados  

    fscanf(input, "%d", &award);
    fscanf(input, "%d", &qtd_bets);

    int awardMax = awardMin = award/2;

    for (int i = 0; i < 10; i++) {
        fscanf(input, "%d", &drawn[i]);
    }   

    Bet allBets[qtd_bets];

    for (int i = 0; i < qtd_bets; i++) {
        allBets[i].numbers = (int*) malloc(15 * sizeof(int));
        allBets[i].code = (char*) malloc(100 * sizeof(char));
        allBets[i].hits = 0;
        
        fscanf(input, "%32s", allBets[i].code);

        for (int j = 0; j < 15; j++) {
            fscanf(input, " %d", &allBets[i].numbers[j]);
            
            int hit = verify_bet(allBets[i].numbers[j], 0, 10, drawn);
            if (hit == 1) {
                allBets[i].hits++;
            }
        }
    }

    // Aplicando o heapify máximo
    for (int i = 0; i < qtd_bets; i++) {
        heapify_max(allBets, qtd_bets, 0);
    }

    winner(allBets, qtd_bets, awardMax, output);

    // Aplicando o heapify mínimo
        for (int i = 0; i < qtd_bets; i++) {
        heapify_min(allBets, qtd_bets, 0);
    }

    winner(allBets, qtd_bets, awardMax, output);

   // Liberar a memória para cada aposta
    for (int i = 0; i < qtd_bets; i++) {
        free(allBets[i].numbers);
        free(allBets[i].code);
    }

    fclose(input);
    fclose(output);
} // Fim da main