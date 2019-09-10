/* Knapsack calculation based on that of */
/* https://www.tutorialspoint.com/cplusplus-program-to-solve-knapsack-problem-using-dynamic-programming */

#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <mpi.h>

long int knapSack(long int C, long int w[], long int v[], int n);

uint64_t GetTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

int main(int argc, char *argv[]) {
    long int C;    /* capacity of backpack */
    int n;    /* number of items */
    int i;    /* loop counter */

    MPI_Init (&argc, &argv);
    int rank;
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        scanf ("%ld", &C);
        scanf ("%d", &n);
    }

    long int v[n], w[n];        /* value, weight */

    if (rank == 0) {
        for (i = 0; i < n; i++) {
            scanf ("%ld %ld", &v[i], &w[i]);
        }

    }

    uint64_t start = GetTimeStamp ();
    long int ks = knapSack(C, w, v, n); 

    if (rank == 0) {
        printf ("knapsack occupancy %ld\n", ks);
        printf ("Time: %ld us\n", (uint64_t) (GetTimeStamp() - start));
    }

    MPI_Finalize ();

    return 0;
}

/* PLACE YOUR CHANGES BELOW HERE */
#include <string.h>
// #include <omp.h>

unsigned long max(unsigned long x, unsigned long y) {
   return (x > y) ? x : y;
}

unsigned long long byte_to_int(unsigned char used[], int n) {
    unsigned long long rv;

    for(int i = 0; i < n; i++) {
        rv += used[n-i-1] << (unsigned long long)i;
    }
    return rv;
}

/* (No longer from the URL given in line 2) */
long int knapSack(long int C, long int w[], long int v[], int n) {

    MPI_Bcast(&C, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&w[0], n, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&v[0], n, MPI_LONG, 0, MPI_COMM_WORLD);

    int rank, size;
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    unsigned long long total = (((unsigned long long)1 << (unsigned long long)n) -1);
    unsigned long long step = total / size;

    unsigned long long lo = rank * step;
    unsigned long long hi = (rank + 1) * step;

    if(rank == size -1 ) hi = total;

    // printf("rank: %d, step: %d, lo: %d, hi: %d\n", rank, step, lo, hi);

    int i;
    unsigned char used [n], solution [n];
    int done = 0;
    long int wt;

    bzero (used, sizeof (used));
    long int max_value = 0;
    long int weight_of_max = 0;
    long int weight = 0;
    long int value = 0;

    while (!done) {
        int carry = 1;
        done = 1;

        for (i = 0; i < n; i++) {
            unsigned long long config = byte_to_int(used, n);
            if(config < lo) continue;
            if(config >= hi) break;
            if (!used[i]) {
                used[i] = 1;
                weight += w[i];
                value += v[i];
                done= 0;
                break;
            } else {
                used[i] = 0;
                weight -= w[i];
                value -= v[i];
            }
        }
        if (weight <= C && value > max_value) {
            max_value = value;
            weight_of_max = weight;
            bcopy (used, solution, sizeof (used));
        }
    }

    long int rv = 0;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&max_value, &rv, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
    return rv;
}

/* mpicc -fopenmp knap_MPI_v2.c -o knap_MPI_v2 */