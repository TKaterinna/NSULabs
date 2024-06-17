#include <math.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
#include <string>
void transpose(float* dst, float* src, int size) {
  int i, j;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      dst[j * size + i] = src[i * size + j];
    }
  }
}
void multiple_scal(float* dst, float* src, float a, int size) {
  int i, j;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      dst[i * size + j] = src[i * size + j] * a;
    }
  }
}
void multiple(float* dst, float* src1, float* src2, int size) {
  int i, j, k;
  for (i = 0; i < size; i++) {
    for (k = 0; k < size; k++) {
      for (j = 0; j < size; j++) {
        dst[i * size + j] += src1[i * size + k] * src2[k * size + j];
      }
    }
  }
}
void sub(float* dst, float* src1, float* src2, int size) {
  int i, j;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      dst[i * size + j] = src1[i * size + j] - src2[i * size + j];
    }
  }
}
void add(float* dst, float* src1, float* src2, int size) {
  int i, j;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      dst[i * size + j] = src1[i * size + j] + src2[i * size + j];
    }
  }
}
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
  transpose(b, a, n);
  for (int j = 0; j < n; j++) {
    cur_sum = 0.0;
    for (int k = 0; k < n; k++) {
      cur_sum += fabs(b[j * n + k]);
    }
    if (cur_sum > a_inf) {
      a_inf = cur_sum;
    }
  }
  multiple_scal(b, b, 1 / (a_1 * a_inf), n);
  multiple(r, b, a, n);
  sub(r, i, r, n);
  std::copy(r, r + n * n, a);
  for (int k = 0; k < m; k++) {
    add(i, i, a, n);
    std::fill(temp, temp + n * n, nul);
    multiple(temp, a, r, n);
    std::copy(temp, temp + n * n, a);
  }
  multiple(a, i, b, n);
  std::cout << (double)(clock() - t) / CLOCKS_PER_SEC << " seconds"
            << std::endl;
  delete[] a;
  delete[] i;
  delete[] b;
  delete[] r;
  delete[] temp;
  return 0;
}
