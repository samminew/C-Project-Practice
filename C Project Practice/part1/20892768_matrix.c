#include "20892768_matrix.h"

void matrix_vector(int m, int n, double* matA, double* vecA, double* matC)
{
	double sum;
	int i, j;

	for(i =0; i<m;i++){
		sum = 0;
		for(j=0; j<n;j++){
			sum += matA[i*n + j]*vecA[j];
		}
		matC[i] = sum;
	}	

}

void matrix_matrix(int m, int n, double* matA, double* matB, double *matC){
	int i,j,k;

	for(i=0;i<m;i++)
	{
		for(j=0;j<n;j++)
		{
			double sum = 0;
			for(k=0;k<n;k++)
			{
				sum+=matA[i*n +k] * matB[k*n+j];
			}
			matC[i*n + j] = sum;
		}
	}

}
