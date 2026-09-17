#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "20892768_vector.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = atoi(argv[1]);
    int base = N / size;
    int remainder = N % size;
    int local_n = (rank < remainder) ? base + 1 : base;

    double *vecA = NULL;
    double *vecB = NULL;
    int *sendcounts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        vecA = malloc(N * sizeof(double));
        vecB = malloc(N * sizeof(double));
        for (int i = 0; i < N; i++) {
            vecA[i] = i;
            vecB[i] = i + 1;
        }

        sendcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
        int offset = 0;
        for (int i = 0; i < size; i++) {
            sendcounts[i] = (i < remainder) ? base + 1 : base;
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    double *local_vecA = malloc(local_n * sizeof(double));
    double *local_vecB = malloc(local_n * sizeof(double));

    double start_time = MPI_Wtime();

    MPI_Scatterv(vecA, sendcounts, displs, MPI_DOUBLE, local_vecA, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(vecB, sendcounts, displs, MPI_DOUBLE, local_vecB, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double local_result = vector_vector(local_n, local_n, local_vecA, local_vecB);

    double global_result = 0.0;
    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();
    double elapsed = end_time - start_time;

    if (rank == 0) {
        printf("N=%d, size=%d, result=%f, time=%f\n", N, size, global_result, elapsed);
        FILE *fp = fopen("20892768_scale.txt", "a");
        if (fp != NULL) {
            fprintf(fp, "%d %d %f\n", N, size, elapsed);
            fclose(fp);
        }
        free(vecA);
        free(vecB);
        free(sendcounts);
        free(displs);
    }

    free(local_vecA);
    free(local_vecB);

    MPI_Finalize();
    return 0;
}
