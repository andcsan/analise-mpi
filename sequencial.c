#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define M 1024 // Tamanho da matriz quadrada

#define MAX_RAND 5 // Valor máximo do numero randomico
#define SEED 1

void geraMatriz(int matriz[M][M]);
void geraVetor(int vetor[M]);

int main(int argc, char **argv)
{
    int i, j, soma;

    int matriz[M][M];
    int vetor[M];
    int vetor_resultado[M];

    double t1, t2, t_final; // Variaveis para calcular o tempo

    srand(SEED);

    geraMatriz(matriz);
    geraVetor(vetor);

    t1 = MPI_Wtime();
    for (i = 0; i < M; i++)
    {
        soma = 0;
        for (j = 0; j < M; j++)
        {
            soma += matriz[i][j] * vetor[j];
        }
        vetor_resultado[i] = soma;
    }
    t2 = MPI_Wtime();
    t_final = t2 - t1;

    printf("%lf\n", t_final);

    return 0;
}

void geraMatriz(int matriz[M][M])
{
    int i, j;

    for (i = 0; i < M; i++)
    {
        for (j = 0; j < M; j++)
        {
            matriz[i][j] = rand() % MAX_RAND + 1;
        }
    }
}

void geraVetor(int vetor[M])
{
    int i;

    for (i = 0; i < M; i++)
    {
        vetor[i] = rand() % MAX_RAND + 1;
    }
}
