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
void mul_matrix_vector(double* res, double* m, double* v, int N, int n) {
  for (int i = 0; i < n; i++) {
    res[i] = mul_vectors(m + i * N, v, N);
  }
}
int main(int argc, char* argv[]) {
  double starttime, endtime;
  MPI_Init(&argc, &argv);
  starttime = MPI_Wtime();
  int rank, size, n;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int N = 1920;
  n = N / size;
  int count = 0;
  if (argc > 1) {
    N = strtod(argv[1], NULL);
  }
  double* b;
  double* A;
  double* x = (double*)malloc(sizeof(double) * N);
  double* z = (double*)malloc(sizeof(double) * N);
  double* part_b = (double*)malloc(sizeof(double) * n);
  double* part_A = (double*)malloc(sizeof(double) * N * n);
  double* part_z = (double*)malloc(sizeof(double) * n);
  double* part_r = (double*)malloc(sizeof(double) * n);
  double* solution = (double*)malloc(sizeof(double) * N);
  if (rank == 0) {
    b = (double*)malloc(sizeof(double) * N);
    A = (double*)malloc(sizeof(double) * N * N);
  }
  double alpha = 0.0;
  double betta = 0.0;
  double e = 0.000000000001;
  if (rank == 0) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j <= i; j++) {
        A[i * N + j] = (((double)rand() / (double)__INT_MAX__) + (-0.5)) * 2.0;
        A[i * N + j] = A[j * N + i];
      }
    }
    for (int i = 0; i < N; i++) {
      b[i] =
          (((double)rand() / (double)__INT_MAX__) + (-0.5)) * 2.0 + (double)N;
      x[i] = (double)rand();
    }
  }
  MPI_Scatter(b, n, MPI_DOUBLE, part_b, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(A, n * N, MPI_DOUBLE, part_A, n * N, MPI_DOUBLE, 0,
              MPI_COMM_WORLD);
  MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  for (int i = 0; i < n; i++) {
    part_r[i] = part_b[i] - mul_vectors(part_A + (i * N), x, N);
    part_z[i] = part_r[i];
  }
  double* res = (double*)malloc(sizeof(double) * n);
  double r_n, part_temp, temp;
  bool stop_flag = true;
  int count_e = 0;
  // base r_n
  part_temp = mul_vectors(part_r, part_r, n);
  MPI_Allreduce(&part_temp, &r_n, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  double mod_b_temp = mul_vectors(part_b, part_b, n);
  double mod_b;
  MPI_Allreduce(&mod_b_temp, &mod_b, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  e *= e;
  e *= mod_b;
  while (stop_flag && (count < 10000)) {
    count++;
    MPI_Allgather(part_z, n, MPI_DOUBLE, z, n, MPI_DOUBLE, MPI_COMM_WORLD);
    mul_matrix_vector(res, part_A, z, N, n);
    part_temp = mul_vectors(res, part_z, n);
    MPI_Allreduce(&part_temp, &temp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    alpha = r_n / temp;
    for (int i = 0; i < n; i++) {
      x[i + rank * n] += alpha * part_z[i];
      part_r[i] -= alpha * res[i];
    }
    part_temp = mul_vectors(part_r, part_r, n);
    MPI_Allreduce(&part_temp, &temp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    betta = temp / r_n;
    r_n = temp;  // r_n1 is r_n for the next iteration
    for (int i = 0; i < n; i++) {
      part_z[i] = betta * part_z[i] + part_r[i];
    }
    if (r_n < e) {
      count_e++;
    }
    stop_flag = (count_e < 3);
  }
  MPI_Gather(x + rank * n, n, MPI_DOUBLE, solution, n, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);
  free(x);
  free(part_r);
  free(z);
  free(part_A);
  free(solution);
  free(part_b);
  free(part_z);
  free(res);
  if (rank == 0) {
    free(A);
    free(b);
  }
  endtime = MPI_Wtime();
  if (rank == 0) {
    printf("Time taken: %lf seconds\n", endtime - starttime);
    if (stop_flag) {
      printf("Too much iterations\n");
    }
    printf("%d\n", count);
  }
  MPI_Finalize();
  return 0;
}
