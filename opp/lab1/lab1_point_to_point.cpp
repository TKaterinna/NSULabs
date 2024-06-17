#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
  double starttime, endtime;
  MPI_Init(&argc, &argv);
  starttime = MPI_Wtime();
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int N = 99984;
  int a[N];
  int b[N];
  int s = 0;
  if (size > 1) {
    if (rank == 0) {
      for (int i = 0; i < N; i++) {
        a[i] = i % 50;
        b[i] = i % 50;
      }
      for (int i = 1; i < size; i++) {
        MPI_Send(a + (N * i / size), N / size, MPI_INT, i, 123, MPI_COMM_WORLD);
        MPI_Send(b, N, MPI_INT, i, 1234, MPI_COMM_WORLD);
      }
    } else {
      MPI_Recv(a, N / size, MPI_INT, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(b, N, MPI_INT, 0, 1234, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }
  for (int i = 0; i < N / size; i++) {
    for (int j = 0; j < N; j++) {
      s += a[i] * b[j];
    }
  }
  if (size > 1) {
    if (rank == 0) {
      int s_temp = 0;
      for (int i = 1; i < size; i++) {
        MPI_Recv(&s_temp, 1, MPI_INT, i, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        s += s_temp;
      }
    } else {
      MPI_Send(&s, 1, MPI_INT, 0, 12, MPI_COMM_WORLD);
    }
  }
  if (rank == 0) {
    printf("s = %d\n", s);
    endtime = MPI_Wtime();
    printf("Time taken: %lf seconds\n", endtime - starttime);
  }
  MPI_Finalize();
  return (0);
}
