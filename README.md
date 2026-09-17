A C project implementing vector–vector, matrix–vector, and matrix–matrix multiplication
in three stages: a serial baseline, an MPI-parallel vector–vector version, and a bonus
MPI-parallel version of all three operations.

## Repository Structure
├── part1/ # Serial baseline (VV, MV, MM)
│ ├── 20892768_main.c
│ ├── 20892768_vector.c / .h
│ ├── 20892768_matrix.c / .h
│ ├── 20892768_Makefile
│ ├── 20892768_serial.txt
│ └── 20892768_report.tex
├── part2/ # MPI-parallel vector–vector multiplication
│ ├── 20892768_main.c
│ ├── 20892768_vector.c / .h
│ ├── 20892768_matrix.c / .h
│ ├── 20892768_Makefile
│ ├── 20892768_scale.txt
│ └── 20892768_report.tex
└── bonus/ # MPI-parallel MV and MM (plus VV)
├── 20892768_main.c
├── 20892768_vector.c / .h
├── 20892768_matrix.c / .h
├── 20892768_Makefile
├── 20892768_bonus.txt
└── 20892768_bonus_report.tex



## Part I — Serial Baseline

Implements the three required prototypes:

```c
double vector_vector(int m, int n, double *vecA, double *vecB);
void   matrix_vector(int m, int n, double *matA, double *vecA, double *matC);
void   matrix_matrix(int m, int n, double *matA, double *matB, double *matC);
```

Matrices are stored as flat, row-major `double*` buffers (`A[i][j]` → `A[i*N + j]`),
allocated on the heap. Data is generated internally (no file input). Each operation
is timed with `clock()` across N = 500, 1000, 2000, 4000 (scaled down by one order of
magnitude from the brief's 5000–40000, since matrix–matrix multiplication is O(N³) and
N = 40000 exceeds both practical runtime and the ~3.2 GB RAM available on the development
machine — see the Part I report for the full justification).

**Build & run:**
```bash
cd part1
make -f 20892768_Makefile run     # sweeps all four N values, writes 20892768_serial.txt
make -f 20892768_Makefile plot    # generates a gnuplot performance chart (requires gnuplot)
```

## Part II — MPI Vector–Vector Multiplication

Parallelises `vector_vector` using MPI:
- Root rank (0) generates the full vectors internally.
- Data is split with `MPI_Scatterv` (handles N not evenly divisible by process count).
- Each rank computes a partial dot product using the unmodified Part I kernel.
- `MPI_Reduce` (with `MPI_SUM`) combines partial results into the final dot product.
- Timed with `MPI_Wtime()`.

**Build & run:**
```bash
cd part2
mpicc 20892768_main.c 20892768_vector.c -o mpi_vv
mpirun -np <P> ./mpi_vv <N>        # P = number of processes, N = vector length
```

## Bonus — MPI Matrix–Vector and Matrix–Matrix Multiplication

Extends the MPI approach to `matrix_vector` and `matrix_matrix`:
- Root generates the full matrices/vector.
- Matrix A is distributed **by row** via `MPI_Scatterv`.
- Vector A / matrix B are broadcast in full to every rank (`MPI_Bcast`), since each
  output row needs the entire vector/matrix B.
- Each rank computes its row-block with the unmodified Part I kernels.
- Results are reassembled on root with `MPI_Gatherv`.

Fixed at N = 2000, swept across P = 1–8.

**Build & run:**
```bash
cd bonus
make -f 20892768_Makefile run
make -f 20892768_Makefile plot
```

## Notes

- All buffers are `malloc`-allocated on the heap with `NULL` checks, and freed before exit.
- Correctness at every stage was verified against hand-computed values at small N before
  trusting results at larger N (see individual reports for worked examples).
- Part II's full-scale run (large N) was executed on a collaborator's machine due to memory
  constraints on the primary development machine; see `part2/20892768_report.tex` for details.
