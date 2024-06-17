#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "mpi.h"
void print_map(bool* A, int n1, int n2) {
  for (int i = 0; i < n1; i++) {
    for (int j = 0; j < n2; j++) {
      if (A[(i + 1) * n2 + j]) {
        printf("*");
      } else {
        printf(".");
      }
    }
    printf("\n");
    fflush(stdout);
  }
}
void new_state(bool* A, bool* A_new, int n1, int n2, int i) {
  int res = 0;
  res += A[(i + 1) * n2 - 1];
  res += A[i * n2];
  res += A[i * n2 + 1];
  res += A[(i + 2) * n2 - 1];
  res += A[(i + 1) * n2 + 1];
  res += A[(i + 3) * n2 - 1];
  res += A[(i + 2) * n2];
  res += A[(i + 2) * n2 + 1];
  if (A[(i + 1) * n2]) {
    A_new[(i + 1) * n2] = (res == 2 || res == 3) ? 1 : 0;
  } else {
    A_new[(i + 1) * n2] = (res == 3) ? 1 : 0;
  }
  for (int j = 1; j < n2 - 1; j++) {
    res = 0;
    res += A[i * n2 + j - 1];
    res += A[i * n2 + j];
    res += A[i * n2 + j + 1];
    res += A[(i + 1) * n2 + j - 1];
    res += A[(i + 1) * n2 + j + 1];
    res += A[(i + 2) * n2 + j - 1];
    res += A[(i + 2) * n2 + j];
    res += A[(i + 2) * n2 + j + 1];
    if (A[(i + 1) * n2 + j]) {
      A_new[(i + 1) * n2 + j] = (res == 2 || res == 3) ? 1 : 0;
    } else {
      A_new[(i + 1) * n2 + j] = (res == 3) ? 1 : 0;
    }
  }
  res = 0;
  res += A[(i + 1) * n2 - 2];
  res += A[(i + 1) * n2 - 1];
  res += A[i * n2];
  res += A[(i + 2) * n2 - 2];
  res += A[(i + 1) * n2];
  res += A[(i + 3) * n2 - 2];
  res += A[(i + 3) * n2 - 1];
  res += A[(i + 2) * n2];
  if (A[(i + 2) * n2 - 1]) {
    A_new[(i + 2) * n2 - 1] = (res == 2 || res == 3) ? 1 : 0;
  } else {
    A_new[(i + 2) * n2 - 1] = (res == 3) ? 1 : 0;
  }
}
int main(int argc, char* argv[]) {
  double starttime, endtime;
  MPI_Init(&argc, &argv);
  starttime = MPI_Wtime();
  int n1 = 400;
  int n2 = 407;
  int max_iter_count = 3000;
  if (argc == 4) {
    n1 = strtod(argv[1], NULL);
    n2 = strtod(argv[2], NULL);
    max_iter_count = strtod(argv[3], NULL);
  }
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int next_rank = (rank == size - 1) ? 0 : (rank + 1);
  int priv_rank = (rank == 0) ? (size - 1) : (rank - 1);
  int last_str = (int)(ceil((double)n1 / (double)size * (double)rank));
  n1 = (int)(ceil((double)n1 / (double)size * (double)(rank + 1))) - last_str;
  bool* A = (bool*)malloc(sizeof(bool) * (n1 + 2) * n2);
  memset(A, 0, sizeof(bool) * (n1 + 2) * n2);
  int first_str = (last_str == 0) ? 0 : (last_str + 1);
  if (first_str == 0) {
    A[n2 + 1] = 1;
    if (first_str + 1 <= n1) {
      A[2 * n2 + 2] = 1;
      if (first_str + 2 <= n1) {
        A[3 * n2] = 1;
        A[3 * n2 + 1] = 1;
        A[3 * n2 + 2] = 1;
      }
    }
  } else if (first_str == 1) {
    A[1] = 1;
    A[n2 + 2] = 1;
    if (first_str + 1 <= n1) {
      A[2 * n2] = 1;
      A[2 * n2 + 1] = 1;
      A[2 * n2 + 2] = 1;
    }
  } else if (first_str == 2) {
    A[2] = 1;
    A[n2] = 1;
    A[n2 + 1] = 1;
    A[n2 + 2] = 1;
  }
  if (rank == size - 1) {
    A[n2 * (n1 + 1) + 1] = 1;
  }
  bool running = 1;
  MPI_Request req_sn, req_sp, req_rn, req_rp, req_alltoall;
  bool* priv_A[max_iter_count];
  bool flags[max_iter_count];
  memset(flags, 0, sizeof(bool) * max_iter_count);
  bool flags_comm[max_iter_count];
  int curr_iter = 0;
  int res;
  bool tmp_running;
  MPI_Barrier(MPI_COMM_WORLD);
  while (running && (curr_iter < max_iter_count)) {
    MPI_Isend(A + n2, n2, MPI_C_BOOL, priv_rank, 123, MPI_COMM_WORLD, &req_sp);
    MPI_Isend(A + n2 * n1, n2, MPI_C_BOOL, next_rank, 1234, MPI_COMM_WORLD,
              &req_sn);
    MPI_Irecv(A, n2, MPI_C_BOOL, priv_rank, 1234, MPI_COMM_WORLD, &req_rp);
    MPI_Irecv(A + n2 * (n1 + 1), n2, MPI_C_BOOL, next_rank, 123, MPI_COMM_WORLD,
              &req_rn);
    for (int i = 0; i < curr_iter; i++) {
      res = memcmp(priv_A[i] + n2, A + n2, n1 * n2);
      if (res == 0) {
        flags[i] = 1;
      } else {
        flags[i] = 0;
      }
    }
    MPI_Ialltoall(flags, (curr_iter + size - (curr_iter % size)) / size,
                  MPI_C_BOOL, flags_comm,
                  (curr_iter + size - (curr_iter % size)) / size, MPI_C_BOOL,
                  MPI_COMM_WORLD, &req_alltoall);
    bool* A_new = (bool*)malloc(sizeof(bool) * (n1 + 2) * n2);
    for (int i = 1; i < n1 - 1; i++) {
      new_state(A, A_new, n1, n2, i);
    }
    MPI_Wait(&req_sp, MPI_STATUS_IGNORE);
    MPI_Wait(&req_rp, MPI_STATUS_IGNORE);
    new_state(A, A_new, n1, n2, 0);
    MPI_Wait(&req_sn, MPI_STATUS_IGNORE);
    MPI_Wait(&req_rn, MPI_STATUS_IGNORE);
    new_state(A, A_new, n1, n2, n1 - 1);
    MPI_Wait(&req_alltoall, MPI_STATUS_IGNORE);
    tmp_running = 1;
    for (int i = 0; i < (curr_iter + size - (curr_iter % size)) / size; i++) {
      tmp_running = 0;
      for (int j = 0; j < size; j++) {
        if (!flags_comm[j * (curr_iter + size - (curr_iter % size)) / size +
                        i]) {
          tmp_running = 1;
          break;
        }
      }
      if (tmp_running == 0) {
        break;
      }
    }
    MPI_Allreduce(&tmp_running, &running, 1, MPI_C_BOOL, MPI_LAND,
                  MPI_COMM_WORLD);
    priv_A[curr_iter] = A;
    A = A_new;
    curr_iter++;
  }
  endtime = MPI_Wtime();
  if (rank == 0) {
    printf("Time taken: %lf seconds\n", endtime - starttime);
    printf("Iter = %d\n", curr_iter);
  }
  for (int i = 0; i < curr_iter; i++) {
    free(priv_A[i]);
  }
  free(A);
  MPI_Finalize();
  return 0;
}
