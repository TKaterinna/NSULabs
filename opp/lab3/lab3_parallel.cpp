#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include <limits>
void mul_vectors(double* a, double* b, int N, double* res) {
  double d_res = 0.0;
#pragma omp parallel for schedule(runtime) reduction(+ : d_res)
  for (int i = 0; i < N; i++) {
    d_res += a[i] * b[i];
  }
#pragma omp single
  *res = d_res;
}
void mul_matrix_vector(double* res, double* m, double* v, int N) {
#pragma omp for schedule(runtime)
  for (int i = 0; i < N; i++) {
    double* a = m + i * N;
    double sum = 0.0;
    for (int j = 0; j < N; j++) {
      sum += a[j] * v[j];
    }
    res[i] = sum;
  }
}
int main(int argc, char* argv[]) {
  int N = 1680;
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
      A[j * N + i] = A[i * N + j];
    }
  }
  for (int i = 0; i < N; i++) {
    b[i] = (((double)rand() / (double)__INT_MAX__) + (-0.5)) * 2.0 + (double)N;
    x[i] = (double)rand();
  }
  double* res = (double*)malloc(sizeof(double) * N);
  double r_n, mod_b, temp;
  bool stop_flag = true;
  int count_e = 0;
  double sum = 0.0;
  double start = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp for schedule(runtime)
    for (int i = 0; i < N; i++) {
      double d_res = 0.0;
      for (int j = 0; j < N; j++) {
        d_res += (A + (i * N))[j] * x[j];
      }
      r[i] = b[i] - d_res;
      z[i] = r[i];
    }
    mul_vectors(b, b, N, &mod_b);
#pragma omp single
    {
      e *= e;
      e *= mod_b;
    }
    mul_vectors(r, r, N, &r_n);
    while (stop_flag && (count < 10000)) {
#pragma omp single nowait
      { count++; }
      mul_matrix_vector(res, A, z, N);
      mul_vectors(res, z, N, &sum);
#pragma omp single
      { alpha = r_n / sum; }
#pragma omp for schedule(runtime)
      for (int i = 0; i < N; i++) {
        x[i] += alpha * z[i];
        r[i] -= alpha * res[i];
      }
#pragma omp barrier
      mul_vectors(r, r, N, &temp);
#pragma omp single
      {
        betta = temp / r_n;
        r_n = temp;
      }
#pragma omp for schedule(runtime)
      for (int i = 0; i < N; i++) {
        z[i] = betta * z[i] + r[i];
      }
#pragma omp single
      {
        if (r_n < e) {
          count_e++;
        }
        stop_flag = (count_e < 3);
      }
    }
  }
  printf("%lf\n", omp_get_wtime() - start);
  free(b);
  free(x);
  free(A);
  free(r);
  free(z);
  free(res);
  if (stop_flag) {
    printf("Too much iterations\n");
  }
  printf("%d\n", count);
  return 0;
}
