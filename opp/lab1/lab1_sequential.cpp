#include <stdio.h>

#include "mpi.h"
int main(int argc, char *argv[]) {
  double starttime, endtime;
  MPI_Init(&argc, &argv);
  starttime = MPI_Wtime();
  int N = 99984;
  int a[N];
  int b[N];
  for (int i = 0; i < N; i++) {
    a[i] = i % 50;
    b[i] = i % 50;
  }
  int s = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      s += a[i] * b[j];
    }
  }
  printf("s = %d\n", s);
  endtime = MPI_Wtime();
  printf("Time taken: %lf sec.\n", endtime - starttime);
  MPI_Finalize();
  return (0);
}
