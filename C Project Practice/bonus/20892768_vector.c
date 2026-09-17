#include "20892768_vector.h"

double vector_vector(int m, int n, double* vecA, double* vecB){
	double sum=0.0;
	int i;

	for(i = 0; i<n; i++){
		sum += vecA[i]*vecB[i];
	}

	return sum;

}
