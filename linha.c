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
    int size, rank;

    int matriz[M][M];
    int vetor[M];
    int vetor_resultado[M];

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

    int r = M / size;             // Parcela de linhas que cada processador vai ter
    int matriz_local[r][M];       // Submatriz com r linhas para operação
    int vetor_resultado_local[r]; // Vetor de resultados do tamanho da parcela;

    MPI_Bcast(vetor, M, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(matriz, M * r, MPI_INT, matriz_local, M * r, MPI_INT, 0, MPI_COMM_WORLD);

    t1 = MPI_Wtime(); // Coleta o tempo antes de iniciar o processamento
    int soma_local;
    // Para cada parcela de linha
    for (int i = 0; i < r; i++)
    {
        soma_local = 0;
        // E para cada coluna da matriz
        for (int j = 0; j < M; j++)
        {
            // Faz a soma_local local
            soma_local += matriz_local[i][j] * vetor[j];
        }
        // Atribui a soma ao vetor de resultados local
        vetor_resultado_local[i] = soma_local;
    }

    // Combina os resultados no processador de rank 0
    MPI_Gather(vetor_resultado_local, r, MPI_INT, vetor_resultado, r, MPI_INT, 0, MPI_COMM_WORLD);

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
