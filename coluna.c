#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define M 1024 // Tamanho da matriz quadrada

#define MAX_RAND 5 // Valor maximo do numero randomico
#define SEED 1

void geraMatriz(int matriz[M][M]);
void geraVetor(int vetor[M]);

int main(int argc, char **argv)
{
    int size, rank;

    int matriz[M][M];
    int vetor[M];
    int vetor_resultado[M];
    int vetor_resultado_local[M];

    double t1, t2, menor_t1, maior_t2, t_final; // Variaveis para calcular o tempo

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(SEED);
    if (rank == 0)
    {
        geraMatriz(matriz);
        geraVetor(vetor);
    }

    int r = M / size;   // Parcela de colunas que cada processador vai ter
    int vetor_local[r]; // Vetor local do tamanho da parcela

    // Divide o vetor para cada processador e faz um Broadcast da matriz
    MPI_Scatter(vetor, r, MPI_INT, vetor_local, r, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(matriz, M * M, MPI_INT, 0, MPI_COMM_WORLD);

    t1 = MPI_Wtime(); // Coleta o tempo antes de iniciar o processamento
    int soma_local;
    // Para cada linha na matriz
    for (int i = 0; i < M; i++)
    {
        soma_local = 0;
        // E para cada coluna correspondente ao processador
        for (int j = rank * r; j < (rank * r) + r; j++)
        {
            // Faz a soma local
            soma_local += matriz[i][j] * vetor_local[j % r];
        }
        // Atribui a soma ao vetor de resultados local
        vetor_resultado_local[i] = soma_local;
    }

    // Soma os valores de cada vetor local no processador de rank 0
    MPI_Reduce(vetor_resultado_local, vetor_resultado, M, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Tempo após calcular e fazer comunicação com outros processadores
    t2 = MPI_Wtime();
    MPI_Reduce(&t1, &menor_t1, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&t2, &maior_t2, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        t_final = maior_t2 - menor_t1;
        printf("%lf\n", t_final);
    }

    MPI_Finalize();

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
