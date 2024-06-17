#include <stdlib.h>
#include <string.h>

#include "mpi.h"
void multiple(double* dst, double* src1, double* src2, int n1, int n2, int n3) {
  int i, j, k;
  for (i = 0; i < n1; i++) {
    for (j = 0; j < n2; j++) {
      for (k = 0; k < n3; k++) {
        dst[i * n3 + k] += src1[i * n2 + j] * src2[j * n3 + k];
      }
    }
  }
}
int main(int argc, char* argv[]) {
  double starttime, endtime;
  MPI_Init(&argc, &argv);
  starttime = MPI_Wtime();
  int n1 = 24 * 300;
  int n2 = 24 * 260;
  int n3 = 24 * 200;
  int periods[2] = {0, 0};
  int coords[2];
  int reorder = 0;
  int sizey = 2;
  int sizex = 4;
  int rank, ranky, rankx;
  if (argc > 2) {
    sizey = strtod(argv[1], NULL);
    sizex = strtod(argv[2], NULL);
  }
  int size = sizex * sizey;
  if (argc > 5) {
    n1 = strtod(argv[3], NULL);
    n2 = strtod(argv[4], NULL);
    n3 = strtod(argv[5], NULL);
  }
  int part_A_n1 = n1 / sizey;
  int part_B_n3 = n3 / sizex;
  MPI_Comm comm2d;
  int dims[2] = {sizey, sizex};
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm2d);
  MPI_Cart_get(comm2d, 2, dims, periods, coords);
  ranky = coords[0];
  rankx = coords[1];
  MPI_Cart_rank(comm2d, coords, &rank);
  double* A;
  double* B;
  double* C;
  double *part_A, *part_B, *part_C;
  if (coords[0] == 0 && coords[1] == 0) {
    A = (double*)malloc(n1 * n2 * sizeof(double));
    B = (double*)malloc(n2 * n3 * sizeof(double));
    C = (double*)malloc(n1 * n3 * sizeof(double));
    for (int i = 0; i < n1; i++) {
      for (int j = 0; j < n2; j++) {
        if (i == j) {
          A[i * n2 + j] = 2.0;
        } else {
          A[i * n2 + j] = 0.0;
        }
      }
    }
    for (int i = 0; i < n2; i++) {
      for (int j = 0; j < n3; j++) {
        if (i == j) {
          B[i * n3 + j] = 1.0;
        } else {
          B[i * n3 + j] = 0.0;
        }
      }
    }
  }
  part_A = (double*)malloc(part_A_n1 * n2 * sizeof(double));
  part_B = (double*)malloc(n2 * part_B_n3 * sizeof(double));
  part_C = (double*)malloc(part_A_n1 * part_B_n3 * sizeof(double));
  for (int i = 0; i < part_A_n1 * part_B_n3; i++) {
    part_C[i] = 0;
  }
  MPI_Datatype cols_B, cols_B_type, part_C_type;
  MPI_Type_vector(n2, part_B_n3, n3, MPI_DOUBLE, &cols_B);
  MPI_Type_commit(&cols_B);
  MPI_Type_create_resized(cols_B, 0, part_B_n3 * sizeof(double), &cols_B_type);
  MPI_Type_commit(&cols_B_type);
  MPI_Type_vector(part_A_n1, part_B_n3, n3, MPI_DOUBLE, &part_C_type);
  MPI_Type_commit(&part_C_type);
  MPI_Comm comm_row, comm_col;
  int subdims[2];
  subdims[0] = 0;
  subdims[1] = 1;
  MPI_Cart_sub(comm2d, subdims, &comm_row);
  subdims[0] = 1;
  subdims[1] = 0;
  MPI_Cart_sub(comm2d, subdims, &comm_col);
  if (rankx == 0) {
    MPI_Scatter(A, n2 * part_A_n1, MPI_DOUBLE, part_A, n2 * part_A_n1,
                MPI_DOUBLE, 0, comm_col);
  }
  if (ranky == 0) {
    MPI_Scatter(B, 1, cols_B_type, part_B, n2 * part_B_n3, MPI_DOUBLE, 0,
                comm_row);
  }
  MPI_Bcast(part_A, n2 * part_A_n1, MPI_DOUBLE, 0, comm_row);
  MPI_Bcast(part_B, n2 * part_B_n3, MPI_DOUBLE, 0, comm_col);
  multiple(part_C, part_A, part_B, part_A_n1, n2, part_B_n3);
  if (rank != 0) {
    MPI_Send(part_C, part_A_n1 * part_B_n3, MPI_DOUBLE, 0, 123, comm2d);
  }
  if (rank == 0) {
    for (int i = 1; i < size; i++) {
      MPI_Recv(C + part_B_n3 * (i % sizex) + ((i / sizex) * n3 * part_A_n1), 1,
               part_C_type, i, 123, comm2d, MPI_STATUS_IGNORE);
    }
    for (int i = 0; i < part_A_n1; i++) {
      for (int j = 0; j < part_B_n3; j++) {
        C[i * n3 + j] = part_C[i * part_B_n3 + j];
      }
    }
  }
  endtime = MPI_Wtime();
  if (rank == 0) {
    printf("Time taken: %lf seconds\n", endtime - starttime);
    free(A);
    free(B);
    free(C);
  }
  free(part_A);
  free(part_B);
  free(part_C);
  MPI_Type_free(&cols_B);
  MPI_Type_free(&cols_B_type);
  MPI_Type_free(&part_C_type);
  MPI_Comm_free(&comm2d);
  MPI_Comm_free(&comm_row);
  MPI_Comm_free(&comm_col);
  MPI_Finalize();
  return 0;
}
