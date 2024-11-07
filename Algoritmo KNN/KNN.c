#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define k 1
#define w 3
#define h 1 

int numLinhas;

int contarLinhas(const char *filePath) {
    FILE *arquivo = fopen(filePath, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    int linhas = 0;
    char ch;
    while ((ch = fgetc(arquivo)) != EOF) {
        if (ch == '\n') {
            linhas++;
        }
    }

    fclose(arquivo);

    return linhas + 1; // Adiciona 1 se o arquivo não estiver vazio e não terminar com uma nova linha 
}

int lerArquivo(const char *filePath, float *array, int tamanhoMax) {
    FILE *arquivo = fopen(filePath, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    int i = 0;
    while (i < tamanhoMax && fscanf(arquivo, "%f", &array[i]) == 1) {
        i++;
    }

    fclose(arquivo);
    return i;  // Retorna o número de elementos lidos
}

void salvarArrayEmArquivo(const float array[], int tamanho, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // Abre o arquivo em modo de escrita

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    for (int i = 0; i < tamanho; i++) {
        fprintf(arquivo, "%.2f\n", array[i]); // Escreve cada número com duas casas decimais em uma nova linha
    }

    fclose(arquivo); // Fecha o arquivo
    printf("Array salvo com sucesso em %s\n", nomeArquivo);
}

float** montaX(float *vetor, int tamanho) {
    int linhas = tamanho-w;
    float **Matriz = malloc((linhas) * sizeof(float*)); // Aloca espaço para as linhas

    for (int i = 0; i < linhas; i++) {
        Matriz[i] = malloc(w * sizeof(float)); // Aloca espaço para cada coluna
    }

    int indice = 0;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < w; j++) {
            Matriz[i][j] = vetor[indice];
            indice++;
        }
        indice -= 2;
    }

    return Matriz;
}

float* montaY(float *vetor, int tamanho){
    int linhas = tamanho-w;
    float *VetorY = malloc((linhas) * sizeof(float*)); // Aloca espaço para as linhas
    
    int indice = w;
    for (int i = 0; i < linhas; i++){
        VetorY[i] = vetor[indice];
        indice++;
    }

    return VetorY;
} 

float* calculaDistancia(float **xtrain,  float **xtest, int linhaAtual){
    int linhas = numLinhas - w;
    float *distancias = malloc(linhas * sizeof(float));

    double d = 0.0;
    for(int l = 0; l < linhas; l++){
        for(int c = 0; c < w; c++){
            d += pow(xtrain[l][c] - xtest[linhaAtual][c], 2);        //(x1train - x1test)^2 + ...
        }
        d = sqrt(d);        // tira a raiz
        distancias[l] = d;  //armazena no vetor de distancia
        d = 0.0;
    }

    return distancias;
}

void k_menores_indices(float *distancias, int *indices) {
    float menores[k];
    int indicesMenores[k];

    // Inicializa todos os menores valores com FLT_MAX e índices com -1
    for (int i = 0; i < k; i++) {
        menores[i] = FLT_MAX;
        indicesMenores[i] = -1;
    }

    for (int i = 0; i < (numLinhas - w); i++) {
        if (distancias[i] < menores[k - 1]) {
            // Insere o novo valor em sua posição correta nos menores valores
            int j = k - 1;
            while (j > 0 && distancias[i] < menores[j - 1]) {
                menores[j] = menores[j - 1];
                indicesMenores[j] = indicesMenores[j - 1];
                j--;
            }
            menores[j] = distancias[i];
            indicesMenores[j] = i;
        }
    }

    // Copia os índices dos k menores valores para o array de saída
    for (int i = 0; i < k; i++) {
        indices[i] = indicesMenores[i];
    }
}

float* knn (float **xtrain, float *ytrain, float **xtest){
    
    float *Ytest = malloc((numLinhas-w) * sizeof(float*)); // Aloca espaço para as linhas

    if(k == 1){
        for(int x = 0; x < (numLinhas - w); x++){
            float *vetorDistancias = calculaDistancia(xtrain, xtest, x);
            float menor = vetorDistancias[0];
            int indiceMenor = 0;

            for(int y = 0; y < (numLinhas - w); y++){
                if(vetorDistancias[y] < menor){
                    menor = vetorDistancias[y];
                    indiceMenor = y;
                }
            }
            Ytest[x] = ytrain[indiceMenor];
        }
        return Ytest;
    }

    if(k > 1){
        for(int x = 0; x < (numLinhas - w); x++){
            float *vetorDistancias = calculaDistancia(xtrain, xtest, x);

            int *indices = malloc(k * sizeof(int)); //array para guardar os indices
            k_menores_indices(vetorDistancias, indices);

            float media = 0.0;
            for(int t = 0; t < k; t++){
                media += ytrain[indices[t]];
            }
            media /= k;
            Ytest[x] = media;
        }
        return Ytest;
    } 
}

int main() {
    
    const char *Xtrain_txt = "c:\\Users\\55119\\Downloads\\EP OAC2 - C\\Xtrain.txt";
    const char *Xtest_txt = "c:\\Users\\55119\\Downloads\\EP OAC2 - C\\Xtest.txt";
    numLinhas = contarLinhas(Xtrain_txt); //conta as linhas do xtrain (que vale pelos dois)
    
    float *Xtrain_vetor = malloc(numLinhas * sizeof(float));      //vetor Xtrain
    float *Xtest_vetor = malloc(numLinhas * sizeof(float));       //vetor Xtest

    lerArquivo(Xtrain_txt, Xtrain_vetor, numLinhas);
    lerArquivo(Xtest_txt, Xtest_vetor, numLinhas);

    float **Xtrain = montaX(Xtrain_vetor, numLinhas);
    float **Xtest = montaX(Xtest_vetor, numLinhas);
    float *Ytrain = montaY(Xtrain_vetor, numLinhas);

    float *Ytest = knn(Xtrain, Ytrain, Xtest);
    
    salvarArrayEmArquivo(Ytest, numLinhas-w, "Ytest.txt");

    free(Xtrain);
    free(Ytrain);
    free(Xtest);
    free(Ytest);

    return 0;
}
