#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "20892768_vector.h"
#include "20892768_matrix.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = atoi(argv[1]);   

  
    int base_rows = N / size;
    int rem_rows  = N % size;
    int my_rows   = (rank < rem_rows) ? base_rows + 1 : base_rows;

    int *row_counts  = NULL;  
    int *row_displs  = NULL;  
    int *elem_counts = NULL;  
    int *elem_displs = NULL;  

    double *matA = NULL, *matB = NULL, *vecA = NULL;
    double *matC_mv_full = NULL, *matC_mm_full = NULL;

    if (rank == 0) {
        matA = malloc((size_t)N * N * sizeof(double));
        matB = malloc((size_t)N * N * sizeof(double));
        vecA = malloc((size_t)N * sizeof(double));
        matC_mv_full = malloc((size_t)N * sizeof(double));
        matC_mm_full = malloc((size_t)N * N * sizeof(double));

        for (int i = 0; i < N * N; i++) matA[i] = i % 10;
        for (int i = 0; i < N * N; i++) matB[i] = i % 7;
        for (int i = 0; i < N; i++)     vecA[i] = i;

        row_counts  = malloc(size * sizeof(int));
        row_displs  = malloc(size * sizeof(int));
        elem_counts = malloc(size * sizeof(int));
        elem_displs = malloc(size * sizeof(int));

        int row_offset = 0;
        for (int r = 0; r < size; r++) {
            int rows_r = (r < rem_rows) ? base_rows + 1 : base_rows;
            row_counts[r]  = rows_r;
            row_displs[r]  = row_offset;
            elem_counts[r] = rows_r * N;
            elem_displs[r] = row_offset * N;
            row_offset += rows_r;
        }
    }

    
    if (rank != 0) {
        vecA = malloc((size_t)N * sizeof(double));
        matB = malloc((size_t)N * N * sizeof(double));
    }
    MPI_Bcast(vecA, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(matB, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    
    double *local_matA = malloc((size_t)my_rows * N * sizeof(double));
    double *local_matC_mv = malloc((size_t)my_rows * sizeof(double));
    double *local_matC_mm = malloc((size_t)my_rows * N * sizeof(double));

    
    double *vecB_local = NULL, *vecA_local = NULL, *vecB_full = NULL;
    int base_v = N / size, rem_v = N % size;
    int my_n = (rank < rem_v) ? base_v + 1 : base_v;
    int *v_counts = NULL, *v_displs = NULL;
    if (rank == 0) {
        vecB_full = malloc((size_t)N * sizeof(double));
        for (int i = 0; i < N; i++) vecB_full[i] = i + 1;
        v_counts = malloc(size * sizeof(int));
        v_displs = malloc(size * sizeof(int));
        int off = 0;
        for (int r = 0; r < size; r++) {
            int n_r = (r < rem_v) ? base_v + 1 : base_v;
            v_counts[r] = n_r;
            v_displs[r] = off;
            off += n_r;
        }
    }
    vecA_local = malloc((size_t)my_n * sizeof(double));
    vecB_local = malloc((size_t)my_n * sizeof(double));

    double t0 = MPI_Wtime();
    MPI_Scatterv(vecA, v_counts, v_displs, MPI_DOUBLE, vecA_local, my_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(vecB_full, v_counts, v_displs, MPI_DOUBLE, vecB_local, my_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double local_dot = vector_vector(my_n, my_n, vecA_local, vecB_local);
    double global_dot = 0.0;
    MPI_Reduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    double t1 = MPI_Wtime();
    double vv_time = t1 - t0;

   
    double t2 = MPI_Wtime();
    MPI_Scatterv(matA, elem_counts, elem_displs, MPI_DOUBLE,
                 local_matA, my_rows * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    matrix_vector(my_rows, N, local_matA, vecA, local_matC_mv);
    MPI_Gatherv(local_matC_mv, my_rows, MPI_DOUBLE,
                matC_mv_full, row_counts, row_displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double t3 = MPI_Wtime();
    double mv_time = t3 - t2;

   
    double t4 = MPI_Wtime();
    
    matrix_matrix(my_rows, N, local_matA, matB, local_matC_mm);
    MPI_Gatherv(local_matC_mm, my_rows * N, MPI_DOUBLE,
                matC_mm_full, elem_counts, elem_displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double t5 = MPI_Wtime();
    double mm_time = t5 - t4;

    if (rank == 0) {
    printf("N=%d size=%d vv=%f mv=%f mm=%f\n", N, size, vv_time, mv_time, mm_time);
        FILE *fp = fopen("20892768_bonus.txt", "a");
        if (fp != NULL) {
            fprintf(fp, "%d %d %f %f %f\n", N, size, vv_time, mv_time, mm_time);
            fclose(fp);
        }
    }

    
    free(local_matA); free(local_matC_mv); free(local_matC_mm);
    free(vecA_local); free(vecB_local);
    free(vecA); free(matB);
    if (rank == 0) {
        free(matA); free(matC_mv_full); free(matC_mm_full);
        free(row_counts); free(row_displs); free(elem_counts); free(elem_displs);
        free(vecB_full); free(v_counts); free(v_displs);
    }
	


    MPI_Finalize();




    return 0;
}
