#include <cblas.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
#include <string>
int main(int argc, char** argv) {
  float nul = 0;
  int n = strtol(argv[1], nullptr, 10);
  int m = strtol(argv[2], nullptr, 10);
  float* a = new float[n * n];
  float* i = new float[n * n];
  float* b = new float[n * n];
  float* r = new float[n * n];
  std::fill(r, r + n * n, nul);
  float* temp = new float[n * n];
  std::fill(i, i + n * n, nul);
  for (int j = 0; j < n; j++) {
    i[j * (n + 1)] = 1;
  }
  for (int j = 0; j < n; j++) {
    for (int k = 0; k < n; k++) {
      a[j * n + k] = (float)rand();
    }
  }
  clock_t t = clock();
  float a_1 = 0.0;
  float a_inf = 0.0;
  float cur_sum;
  for (int j = 0; j < n; j++) {
    cur_sum = 0.0;
    for (int k = 0; k < n; k++) {
      cur_sum += fabs(a[j * n + k]);
    }
    if (cur_sum > a_1) {
      a_1 = cur_sum;
    }
  }
  for (int j = 0; j < n; j++) {
    cur_sum = 0.0;
    for (int k = 0; k < n; k++) {
      cur_sum += fabs(a[k * n + j]);
    }
    if (cur_sum > a_inf) {
      a_inf = cur_sum;
    }
  }
  cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, n, n, n,
              -1.0 / (a_1 * a_inf), a, n, a, n, 0, r, n);
  // r = -ba
  cblas_saxpy(n * n, 1.0, i, 1, r, 1);  // r = i - ba
  std::copy(r, r + n * n, b);           // b = r; b(r^(k+1))
  for (int k = 0; k < m; k++) {
    cblas_saxpy(n * n, 1.0, b, 1, i, 1);  // i = (i + r + r^2 + ...)
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1.0, b, n,
                r, n, 0, temp, n);
    // temp = r^(k+1)
    std::copy(temp, temp + n * n, b);
  }
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, n, n, n,
              -1.0 / (a_1 * a_inf), i, n, a, n, 0, temp, n);
  // temp = a^(-1)
  std::cout << (double)(clock() - t) / CLOCKS_PER_SEC << " seconds"
            << std::endl;
  delete[] a;
  delete[] i;
  delete[] b;
  delete[] r;
  delete[] temp;
  return 0;
}
