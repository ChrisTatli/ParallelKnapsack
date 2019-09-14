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
        MPI_Bcast(&C, 1, MPI_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);       
        MPI_Bcast(&w[0], n, MPI_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(&v[0], n, MPI_LONG, 0, MPI_COMM_WORLD);
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
#include <strings.h>
#include <math.h>
#include <stdlib.h>

long int max(long int x, long int y) {
   return (x > y) ? x : y;
}

void toBinary(unsigned long long permutation, unsigned char used[], int n)
{
    int i = 0;
    unsigned long long highBit = 1 << (n - 1);
    while(highBit)
    {
        unsigned long long k = (permutation & highBit ? 1 : 0);
        highBit >>= 1;
        used[i] = k;
        i++;
    }
}

/* (No longer from the URL given in line 2) */

long int knapSack(long int C, long int w[], long int v[], int n) {
    int rank, size;   
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    long int* wt = w;
    long int* val = v;
    
    if(rank != 0){
        MPI_Bcast(&C, 2, MPI_LONG, 0 , MPI_COMM_WORLD);
        MPI_Bcast(&n, 1, MPI_INT, 0 , MPI_COMM_WORLD);
        wt = malloc(sizeof(long int ) * n);
        val = malloc(sizeof(long int ) * n);
        MPI_Bcast(wt, n, MPI_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(val, n, MPI_LONG, 0 , MPI_COMM_WORLD);
    }
    #if 1
    if(rank ==1 ){
        for(int i =0; i<35; i++){
            printf("%d, %d\n", val[i], wt[i]);
        }
    }
    #endif

    unsigned long long permutations = pow(2.0, n);

    unsigned long long chunkSize = permutations / size;
    unsigned long long lowerBound = rank * chunkSize; 
    unsigned long long upperBound = lowerBound + chunkSize;
    
    unsigned long overallMaxValue = 0;
    unsigned long  maxValue = 0;
    for(unsigned long long p = lowerBound; p < upperBound; p++){
        
        if(rank ==1){
           // printf("%llu\n", p);
            printf("%d\n", maxValue);
        }
        
        unsigned char used[n];
        
        bzero(used, sizeof (used));
        toBinary(p, used, n);
        long int weight = 0;
        long int value = 0;
        
        int i = 0;
        while (i < (n - 1)) {
            if(used[i] == 1){
                if((wt[i] + weight ) < C){
                    weight += wt[i];
                    value += val[i];
                }
                else{
                    break;
                }
            }
            i++;
        }
        if (value > maxValue){
            maxValue = value;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&maxValue, &overallMaxValue, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);

    if(rank == 0){
        return overallMaxValue;
    } else {
        return 0;
    }
}