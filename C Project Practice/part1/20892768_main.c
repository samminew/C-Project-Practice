#include <stdio.h>
#include <stdlib.h>
#include "20892768_vector.h"
#include "20892768_matrix.h"
#include <time.h>

int main(int argc, char* argv[]){
	printf("Checkpoint 1: starting\n"); fflush(stdout);
	int N = atoi(argv[1]);
	int i;
	printf("Checkpoint 2: N = %d\n", N); fflush(stdout); 


	double *vecA = malloc(N * sizeof(double));
 	if(vecA == NULL)
	{
		printf("Memory allocation failed for vecA\n");
		return 1;
	}

	double *vecB = malloc(N * sizeof(double));
	if(vecB == NULL)
	{
         	printf("Memory allocation failed for vecB\n");
		return 1;	
	}

//	printf("vecA[2] =%f\n", vecA[2]);
//      printf("vecB[2] = %f\n",vecB[2]);




	double *matA = malloc(N*N*sizeof(double));
	if(matA==NULL){
		printf("Memory allocation failed for matA\n");
		return 1;
	}

	double *matC = malloc(N*sizeof(double));
	if(matC == NULL){
		printf("Memory allocation failed fot matC\n");
		return 1;
	}
	
	
	double *matB = malloc(N*N*sizeof(double));
	if(matB == NULL){
                printf("Memory allocation failed fot matB\n");
		return 1;
        }


//      printf("matA[2] =%f\n", matA[2]);       
//      printf("matB[2] =%f\n", matB[2]); 
//      printf("matC[2] = %f\n",matC[2]);

	
	double *matC2 = malloc(N * N * sizeof(double));
	if (matC2 == NULL) {
    		printf("Memory allocation failed for matC2\n");
    		return 1;
}

	 printf("Checkpoint 3: mallocs done\n"); fflush(stdout);


	for(i=0; i<N; i++){
		vecA[i] = i;
		vecB[i] = i+1;
	
	}

	for(i=0;i<N*N;i++)
	{
		matA[i] = i%10;

	}


	for(i=0;i<N*N;i++){
		matB[i] = i%7;
	}

	printf("Checkpoint 4: fill loops done\n");


	clock_t start = clock();
	double result = vector_vector(N,N,vecA,vecB);
	clock_t end = clock();
	double time_taken = (double)(end-start)/CLOCKS_PER_SEC;

//	printf("Dot product result, vecA*vecB = %f\n", result);
//	printf("Time taken = %f\n",time_taken);
	printf("Checkpoint 5: vector_vector done\n"); fflush(stdout);
	
	clock_t start2 = clock();
	matrix_vector(N,N,matA,vecA,matC);
	clock_t end2 = clock();
	double time_taken2 = (double)(end2-start2)/CLOCKS_PER_SEC;

//	printf("matrix_vector time = %f\n", time_taken2);
//	printf("matC[0] = %f\n",matC[0]);
	printf("Checkpoint 6: matrix_vector done\n"); fflush(stdout);

	
	clock_t start3 = clock();
	matrix_matrix(N,N,matA,matB,matC2);
	clock_t end3 = clock();
	double time_taken3 = (double)(end3-start3)/CLOCKS_PER_SEC;
	printf("Checkpoint 7: matrix_matrix done\n"); fflush(stdout);
//	printf("matrix_matrix time = %f\n", time_taken3);
//	printf("matC2[0] = %f\n",matC2[0]);


	FILE *fp = fopen("20892768_serial.txt","a");
	fprintf(fp, "%d %f %f %f\n", N, time_taken, time_taken2, time_taken3);
	if (fp == NULL) 
	{
    		printf("Failed to open file\n");
    		return 1;
	}
	fclose(fp);


	 printf("Checkpoint 8: file written\n"); fflush(stdout);

	free(vecA);
	free(vecB);
	free(matA);
	free(matB);
	free(matC);
	free(matC2);

	return 0;
}
