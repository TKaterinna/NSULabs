#include <math.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
#include <string>
void transpose(float* dst, float* src, int size) {
  __m128* mm_matrix = (__m128*)src;
  __m128* block;
  int i, j;
  for (i = 0, block = mm_matrix; i < size; i++) {
    for (j = 0; j < size / 4; j++, block++) {
      __m128 buf = _mm_load_ps((float*)block);
      float temp[4];
      _mm_store_ps(temp, buf);
      dst[(j * 4) * size + i] = temp[0];
      dst[(j * 4 + 1) * size + i] = temp[1];
      dst[(j * 4 + 2) * size + i] = temp[2];
      dst[(j * 4 + 3) * size + i] = temp[3];
    }
  }
}
void multiple_scal(float* dst, float* src, float a, int size) {
  int i, j;
  int num_block = 0;
  __m128* mm_matrix = (__m128*)src;
  __m128* mm_result = (__m128*)dst;
  __m128 mm_scalar = _mm_load1_ps(&a);
  __m128* block;
  for (i = 0, num_block = 0, block = mm_matrix; i < size; i++) {
    for (j = 0; j < size / 4; j++, block++, num_block++) {
      __m128 buf = _mm_load_ps((float*)block);
      mm_result[num_block] = _mm_mul_ps(buf, mm_scalar);
    }
  }
}
void multiple(float* dst, float* src1, float* src2, int size) {
  int i, j, k;
  int num_block = 0;
  __m128* mm_matrix2 = (__m128*)src2;
  __m128* mm_result = (__m128*)dst;
  __m128 mm_scalar;
  __m128* block;
  int c = size / 4;
  for (i = 0; i < size; i++) {
    for (k = 0, block = mm_matrix2; k < size; k++) {
      num_block = c * i;
      mm_scalar = _mm_load1_ps(&src1[i * size + k]);
      for (j = 0; j < size / 4; j++, block++, num_block++) {
        __m128 rowsrc2 = _mm_load_ps((float*)block);
        __m128 mul = _mm_mul_ps(rowsrc2, mm_scalar);
        mm_result[num_block] = _mm_add_ps(mm_result[num_block], mul);
      }
    }
  }
}
void sub(float* dst, float* src1, float* src2, int size) {
  int i;
  __m128* mm_matrix1 = (__m128*)src1;
  __m128* mm_matrix2 = (__m128*)src2;
  __m128* mm_result = (__m128*)dst;
  for (i = 0; i < size * size / 4; i++) {
    mm_result[i] = _mm_sub_ps(mm_matrix1[i], mm_matrix2[i]);
  }
}
void add(float* dst, float* src1, float* src2, int size) {
  int i;
  __m128* mm_matrix1 = (__m128*)src1;
  __m128* mm_matrix2 = (__m128*)src2;
  __m128* mm_result = (__m128*)dst;
  for (i = 0; i < size * size / 4; i++) {
    mm_result[i] = _mm_add_ps(mm_matrix1[i], mm_matrix2[i]);
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
