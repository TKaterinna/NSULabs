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
  if (rank == 0) {
    for (int i = 0; i < N; i++) {
      a[i] = i % 50;
      b[i] = i % 50;
    }
  }
  MPI_Bcast(b, N, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(a, N / size, MPI_INT, a, N, MPI_INT, 0, MPI_COMM_WORLD);
  int s_temp = 0;
  for (int i = 0; i < N / size; i++) {
    for (int j = 0; j < N; j++) {
      s_temp += a[i] * b[j];
    }
  }
  MPI_Reduce(&s_temp, &s, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    printf("s = %d\n", s);
    endtime = MPI_Wtime();
    printf("Time taken: %lf seconds\n", endtime - starttime);
  }
  MPI_Finalize();
  return (0);
}
