#include <stdio.h>
#include <stdlib.h>

#include <limits>

#include "mpi.h"
double mul_vectors(double* a, double* b, int N) {
  double sum = 0.0;
  for (int i = 0; i < N; i++) {
    sum += a[i] * b[i];
  }
  return sum;
}
void mul_matrix_vector(double* res, double* m, double* v, int N) {
  for (int i = 0; i < N; i++) {
    res[i] = mul_vectors(m + i * N, v, N);
  }
}
int main(int argc, char* argv[]) {
  double starttime, endtime;
  MPI_Init(&argc, &argv);
  starttime = MPI_Wtime();
  int N = 1920;
  int count = 0;
  if (argc > 1) {
    N = strtod(argv[1], NULL);
  }
  double* b;
  double* x;
  double* A;
  b = (double*)malloc(sizeof(double) * N);
  x = (double*)malloc(sizeof(double) * N);
  A = (double*)malloc(sizeof(double) * N * N);
  double* r = (double*)malloc(sizeof(double) * N);
  double* z = (double*)malloc(sizeof(double) * N);
  double alpha = 0.0;
  double betta = 0.0;
  double e = 0.000000000001;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j <= i; j++) {
      A[i * N + j] = (((double)rand() / (double)__INT_MAX__) + (-0.5)) * 2.0;
      A[i * N + j] = A[j * N + i];
    }
  }
  for (int i = 0; i < N; i++) {
    b[i] = (((double)rand() / (double)__INT_MAX__) + (-0.5)) * 2.0 + (double)N;
    x[i] = (double)rand();
  }
  for (int i = 0; i < N; i++) {
    r[i] = b[i] - mul_vectors(A + (i * N), x, N);
    z[i] = r[i];
  }
  double* res = (double*)malloc(sizeof(double) * N);
  double r_n, mod_b, temp;
  bool stop_flag = true;
  int count_e = 0;
  r_n = mul_vectors(r, r, N);
  mod_b = mul_vectors(b, b, N);
  e *= e;
  e *= mod_b;
  while (stop_flag && (count < 10000)) {
    count++;
    mul_matrix_vector(res, A, z, N);
    alpha = r_n / mul_vectors(res, z, N);
    for (int i = 0; i < N; i++) {
      x[i] += alpha * z[i];
      r[i] -= alpha * res[i];
    }
    temp = mul_vectors(r, r, N);
    betta = temp / r_n;
    r_n = temp;
    for (int i = 0; i < N; i++) {
      z[i] = betta * z[i] + r[i];
    }
    if (r_n < e) {
      count_e++;
    }
    stop_flag = (count_e < 3);
  }
  free(b);
  free(x);
  free(A);
  free(r);
  free(z);
  free(res);
  endtime = MPI_Wtime();
  printf("Time taken: %lf seconds\n", endtime - starttime);
  if (stop_flag) {
    printf("Too much iterations\n");
  }
  printf("%d\n", count);
  MPI_Finalize();
  return 0;
}
