#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Estrutura dos servidores
int servidores_livres_contador;

typedef struct servidor {
	int id;
	int capacidade;
	int num_requisicoes;
	char **chave;
} servidor;

// Função para remover os espaços da string de entrada
char *rem_espaco(char *str) {
    if (str == NULL || *str == '\0') {
        return NULL;
    }
    char *destino = malloc(strlen(str) + 1); // Aloca memória para a nova string
    if (!destino) {
        return NULL; // Falha ao alocar memória
    }
    char *inicial = destino;
    while (*str) {
        if (*str != ' ') {
            *inicial++ = *str;
        }
        str++;
    }
    *inicial = '\0';
    return destino; // Retorna a nova string sem espaços
}

// Função para remover o primeiro caractere e a quebra de linha
char *limpar_impressao(char *str) {
    if (str == NULL || *str == '\0') {
        return NULL;
    }
    char *destino = malloc(strlen(str) + 1); // Aloca memória para a nova string
    if (!destino) {
        return NULL; // Falha ao alocar memória
    }
    char *inicial = destino;
    while (*str) {
        if (*str >= 32 && *str <= 126) {
            *inicial++ = *str;
        }
        str++;
    }
    *inicial = '\0';
    return destino; // Retorna a nova string limpa
}

// Função que cria a lista de servidores
servidor *criar_servidores (int qtd_servidores, int capacidade) {

	servidores_livres_contador = qtd_servidores;

	servidor *servidores = malloc((qtd_servidores) * sizeof(servidor));

	for (int i = 0; i < qtd_servidores; i++) {
		servidores[i].id = i;
		servidores[i].capacidade = capacidade;
		servidores[i].num_requisicoes = 0;

		servidores[i].chave = malloc(capacidade * sizeof(char *));
		for (int j = 0; j < capacidade; j++) {
			servidores[i].chave[j] = NULL;
		}
	}

return servidores;

}	

// Função checksum
int checksum_8(char chave[101]) {
	int8_t resultado = 0;

	for (int i = 0; chave[i] != '\0'; i++) {
		int8_t ascii = chave[i];
        //printf("Caractere: '%c' (ASCII: %u)\n", chave[i], ascii);
		resultado = resultado ^ ascii;
	}
	//printf("chave: %s\nresultado: %d\n", chave, resultado);
	return resultado;
}

// Função hash 1
int hash_1 (int checksum, int T){
    int resultado = (7919 * checksum) % T;
    return resultado;
}
// Função hash 2, que será usada para caso de colisão
int hash_2 (int checksum, int T) {
    int resultado = (104729 * checksum + 123) % T;
    //printf("checksum=%d, T=%d) = %d\n", checksum, T, resultado);
    return resultado;
}
// Função que lida com as colisões
int double_hash(int h1_x, int h2_x, int i, int checksum, int T) {
    if (T == 0) return 0;

   // int hash1 = h1_x(checksum, T);
    //int hash2 = h2_x(checksum, T);

    // Certificar de que hash2 não é zero para evitar problemas de divisão
    if (h2_x == 0) {
        h2_x = 1;
    }

    int result = (h1_x + (i * h2_x)) % T;

    if (result < 0) {
        result += T;
    }

   // printf("Checksum: %d, Hash1: %d, Hash2: %d, i: %d, Result: %d\n", checksum, hash1, hash2, i, result);

    return result;
}

void armazenar_chave(int id_servidor, int capacidade_max, servidor *lista_servidores, char chave[101]){

	char chave_limpa[101];
	strcpy(chave_limpa, limpar_impressao(chave));

	for (int i = 0; i < capacidade_max; i++) {
		if (lista_servidores[id_servidor].chave[i] == NULL) {
			lista_servidores[id_servidor].chave[i] = strdup(chave_limpa);
			break;
		}
	}
}

void imprimir_chaves (int primeira_chave, int id_servidor, servidor *lista_servidores, char chave[101], FILE *output) {

	char chave_limpa[101];
	strcpy(chave_limpa, limpar_impressao(chave));

	if (primeira_chave) {
		fprintf(output, "[S%d] %s\n", id_servidor, chave_limpa);
	} else {
		int i = 0;
		fprintf(output, "[S%d] ", id_servidor);
		while(lista_servidores[id_servidor].chave[i] != NULL) {
			if (i > 0) {
				fprintf(output, ", ");
			}
			fprintf(output, "%s ", lista_servidores[id_servidor].chave[i]);
			i++;
		}
		fprintf(output, "\n");
	}
}


// Função que verifica se algum servidor está livre
int checar_servidor (servidor *lista_servidores, int id_servidor) {
	
		if (lista_servidores[id_servidor].num_requisicoes < lista_servidores[id_servidor].capacidade) {
			return id_servidor;
	} else {
		return -1;
	}
}

// Função que verifica se todos os servidores estão ocupados
int servidores_livres(servidor *lista_servidores, int qtd_servidores) {

	for (int i = 0; i < qtd_servidores; i++) {
		if (lista_servidores[i].num_requisicoes < lista_servidores[i].capacidade) {
			return 1;
		} 
	}
	return 0;
}

int check_servidores_livre () {
	return servidores_livres_contador > 0;
}

// Função que distribui as requisições
int distribuir_requisicoes (char chave [101], servidor *todos_servidores, int capacidade, int qtd_servidores, FILE *input, FILE *output) {

	char chave_original[101];
	strcpy(chave_original, chave);	

	char chave_espacos[101];
	strcpy(chave_espacos, rem_espaco(chave));

	// calculando o checksum
	int8_t checksum = checksum_8(chave_espacos);
	//int id_servidor = h1_x(checksum, qtd_servidores);

	int h1_x = hash_1(checksum, qtd_servidores);
	int h2_x = hash_2(checksum, qtd_servidores);

	if (todos_servidores[h1_x].num_requisicoes < todos_servidores[h1_x].capacidade) {
		armazenar_chave(h1_x, capacidade, todos_servidores, chave_original);
		
		int primeira_chave = (todos_servidores[h1_x].num_requisicoes == 0);
		imprimir_chaves(primeira_chave, h1_x, todos_servidores, chave_original, output);
	    todos_servidores[h1_x].num_requisicoes++;

	    if (todos_servidores[h1_x].num_requisicoes == todos_servidores[h1_x].capacidade) {
	    	servidores_livres_contador--;
	    }

	} else {
		int tentativas = 1;
		while (check_servidores_livre()) {
			//printf("Chave sem espaço: %s\n", chave_espacos);
			//printf("checksum do teste: %d\n", checksum);
			//printf("Tentativas: %d\n", tentativas);
			int novo_id = double_hash(h1_x, h2_x, tentativas, checksum, qtd_servidores);
			//printf("Novo id: %d\n", novo_id);
			int check_servidor = checar_servidor(todos_servidores, novo_id);

			if (check_servidor != -1) {

				armazenar_chave(novo_id, capacidade, todos_servidores, chave_original);
				fprintf(output, "[S%d] -> [S%d]\n", h1_x, novo_id);
				int primeira_chave = (todos_servidores[novo_id].num_requisicoes == 0);
				
				imprimir_chaves(primeira_chave, novo_id, todos_servidores, chave_original, output);
			    todos_servidores[novo_id].num_requisicoes++;

			    if(todos_servidores[novo_id].num_requisicoes == todos_servidores[novo_id].capacidade) {
			    	servidores_livres_contador--;
			    }
				break;
			}
			tentativas++;
		}
	}
    return 0;	
}


// main
 
int main (int argc, char* argv[]) {

	if (argc != 3) {
		printf("Uso correto: %s arquivo_entrada arquivo_saida\n", argv[0]);
	 	return 1;
	}

char linha[101];

FILE* input = fopen(argv[1], "r");
FILE* output = fopen(argv[2], "w");

    // Verifica se o arquivo foi aberto com sucesso
    if (input == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }	

    int qtd_chaves;
    char chave[101];
    int qtd_servidores, capacidade;
 
  if (fgets(linha, sizeof(linha), input) != NULL) {
        //printf("Linha de configuração: %s\n", linha);
        if (sscanf(linha, "%d %d", &qtd_servidores, &capacidade) != 2) {
            printf("Erro ao ler o número de servidores e a capacidade.\n");
            return -1;
        }
    } else {
        printf("Erro ao ler a primeira linha do arquivo.\n");
        return -1;
    }
    //printf("Servidores: %d\nCapacidade: %d\n", qtd_servidores, capacidade);
    servidor *lista_servidores = criar_servidores(qtd_servidores, capacidade);
    if (fgets(linha, sizeof(linha), input) != NULL) {
        qtd_chaves = atoi(linha);
        //printf("Quantidade de chaves: %d\n", qtd_chaves);

        for (int i = 0; i < qtd_chaves; i++) {
            if (fgets(linha, sizeof(chave), input) != NULL) {
                    distribuir_requisicoes(linha, lista_servidores, capacidade, qtd_servidores, input, output);
                }
            }
        }
   

    fclose(input);
    fclose(output);

    return 0;
}