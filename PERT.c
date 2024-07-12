#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void marcar(int** g, int linha1, int linha2, int valor) {
    g[linha1][linha2] = valor;
}

int** determinaTamanho(FILE *f, int *numLinhas) {
    char linha[50];
    int linhas = 0;

    while (fgets(linha, sizeof(linha), f)) {
        linhas++;
    }

    int **matriz = (int **)malloc(linhas * sizeof(int *));
    if (matriz == NULL) {
        perror("Erro ao alocar memória para as linhas");
        return NULL;
    }
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)calloc(linhas, sizeof(int));
        if (matriz[i] == NULL) {
            perror("Erro ao alocar memória para as colunas");
            for (int j = 0; j < i; j++) {
                free(matriz[j]);
            }
            free(matriz);
            return NULL;
        }
    }
    *numLinhas = linhas;

    rewind(f);
    return matriz;
}

void mudarValoresColunas(int** mat, int col, int n) {
    for (int i = 0; i < n; i++) {
        if (mat[i][col] != 0) {
            mat[i][col] = mat[col][col];
        }
    }
}

int** marcarValorMatriz(FILE *f, int *numLinhas) {
    int n;
    int **g = determinaTamanho(f, &n);
    if (g == NULL) {
        perror("Erro ao alocar memória");
        return NULL;
    }

    char linha[50];
    int valor, linha1, linha2;
    char d[] = ",";
    char *token;

    while (fgets(linha, sizeof(linha), f)) {
        token = strtok(linha, d);
        if (token == NULL) {
            perror("Erro ao ler token");
            return NULL;
        }
        linha1 = token[0] - 'a';

        token = strtok(NULL, d);
        if (token == NULL) {
            perror("Erro ao ler token");
            return NULL;
        }
        if (token[0] >= '0' && token[0] <= '9') {
            valor = atoi(token);
            marcar(g, linha1, linha1, valor);
        }

        while ((token = strtok(NULL, d)) != NULL) {
            linha2 = token[0] - 'a';
            marcar(g, linha1, linha2, valor);
        }
    }

    for (int i = 0; i < n; i++) {
        mudarValoresColunas(g, i, n);
    }
    *numLinhas = n;
    return g;
}

void determina(int** mat, int n, int linha, int* verificado) {
    if (linha < n) {
        if (!verificado[linha]) {
            int total = 0;
            for (int j = 0; j < n; j++) {
                if (j < linha) {
                    if (mat[linha][j] > total)
                        total = mat[linha][j];
                } else if (j > linha) {
                    if (mat[linha][j] != 0 && !verificado[j]) {
                        determina(mat, n, j, verificado);
                        if (mat[j][j] > total) {
                            total = mat[j][j];
                        }
                    }
                }
            }
            mat[linha][linha] += total;
            mudarValoresColunas(mat, linha, n);
            verificado[linha] = 1;
        }
    }
}

int percorreLinhaMaior(int** g,int n,int linha){
    int i;
    int maior=0;
    for(i=0;i<n;i++){
        if(g[linha][i] != 0 && g[linha][i] > maior && i != linha){
            maior = g[linha][i];
        }
    }
    return maior;
}

int procuraProximaLinha(int** g,int n,int maior,int linha){
    int proxLinha;
    for(int i=0;i<n;i++){
        if(g[linha][i] == maior){
            proxLinha = i;
        }
    }
    if(proxLinha == 0){
        return -1;
    }else{
        return proxLinha;
    }
}

char* caminhoCritico(int** g, int n, int linha, int* qtd,int* valorMinimo) {
    int maior;
    int proxLinha;
    int* verificado = (int*)calloc(n, sizeof(int));
    int idx = 0;
    *valorMinimo = g[n-1][n-1];
    verificado[n-1] = 1;
    (*qtd)++;
    while (linha >= 0) {
        maior = percorreLinhaMaior(g, n, linha);
        proxLinha = procuraProximaLinha(g, n, maior, linha);
            for(int j=0;j<n;j++){
                if(g[linha][j] == maior && !verificado[j]){
                        (*qtd)++;
                        verificado[j] = 1;
                }
            }
        linha = proxLinha;
    }

    char* caminho = (char*)malloc((*qtd) * sizeof(char));
    int indx = 0;
    for (int i = 0; i < n; i++) {
        if (verificado[i]) {
            caminho[indx++] = i + 'a';
        }
    }

    free(verificado);
    return caminho;
}
int main() {
    FILE *f = fopen("arquivo.txt", "r");
    if (f == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    int n;
    int **g = marcarValorMatriz(f, &n);
    if (g == NULL) {
        return 1;
    }

    int* verificado = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        verificado[i] = 0;
    }

    for (int i = 0; i < n; i++) {
        determina(g, n, i, verificado);
    }

    int valorMinimo;
    int tamCritico;
    char* caminho = caminhoCritico(g, n, n-1, &tamCritico,&valorMinimo);
    printf("Tempo minimo para realização de tarefas: %d", valorMinimo);
    printf("\nCaminho crítico:\n");
    for (int i = 0; i < tamCritico; i++) {
        printf("%c ", caminho[i]);
    }
    printf("\n");

    for (int i = 0; i < n; i++) {
        free(g[i]);
    }
    free(caminho);
    free(g);
    free(verificado);
    fclose(f);
    return 0;
}
