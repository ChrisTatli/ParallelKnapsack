/* Knapsack calculation based on that of */
/* https://www.tutorialspoint.com/cplusplus-program-to-solve-knapsack-problem-using-dynamic-programming */

#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <mpi.h>
#include <sys/resource.h>

long int knapSack(long int C, long int w[], long int v[], int n);

uint64_t GetTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

int main(int argc, char *argv[]) {
    //struct rlimit limit;
    //getrlimit(RLIMIT_STACK, &limit);
    //printf ("\nStack Limit = %ld and %ld max\n", limit.rlim_cur, limit.rlim_max);
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

void toBinary(unsigned long long permutation, unsigned char used[], int n, int rank)
{   
    for (int i = 0; i < n; i++) {
        unsigned char bit = (((unsigned long long)1 << i) & permutation) >> i;
        used[i] = bit;
    }
}


void print_arr(unsigned char arr[], int n, int rank)
{
    printf("=====================================\n");
    for (int i = 0; i < n; i++)
    {
        printf("%u ", arr[i]);
    }

    printf("\n");
    printf("=====================================\n");
}

/* (No longer from the URL given in line 2) */

long int knapSack(long int C, long int w[], long int v[], int n) {
    int rank, size;   
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    long int* wt;
    long int* val;
    if(rank == 0){
        wt = w;
        val = v;
    }
    
    if(rank != 0){
        MPI_Bcast(&C, 2, MPI_LONG, 0 , MPI_COMM_WORLD);
        MPI_Bcast(&n, 1, MPI_INT, 0 , MPI_COMM_WORLD);
        wt = malloc(sizeof(long int ) * n);
        val = malloc(sizeof(long int ) * n);
        MPI_Bcast(wt, n, MPI_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(val, n, MPI_LONG, 0 , MPI_COMM_WORLD);
    }

    unsigned long long permutations = (((unsigned long long)1) << (unsigned long long)n) - 1 ;

    unsigned long long chunkSize = permutations / size;
    unsigned long long lowerBound = rank * chunkSize; 
    unsigned long long upperBound = (rank + 1) * chunkSize;             
    unsigned char used[n];
    unsigned char finalUsedState[n];

        
    bzero(used, sizeof (used));
    bzero(finalUsedState, sizeof(finalUsedState));

    
    toBinary(lowerBound, used, n,rank) ;
    if(rank == 1){
        //print_arr(used, n, rank);
    }
    //toBinary(upperBound, finalUsedState, N,rank);
    if(rank == 1){
      //  print_arr(finalUsedState, n, rank);
    }
    
    
    long int weight = 0;
    long int value = 0;
    unsigned long maxValue = 0;
    unsigned long overallMaxValue = 0;
    int done = 0;
    printf("rank:%d, size: %d, lowerbound:%llu, upperbound: %llu\n",rank, size, lowerBound, upperBound );
    if (rank ==1 )print_arr(used,n,rank);
    while(!done){   
        
        done = 1;
        for (int i = 0 ; i < n ; i++){
            if(!used[i]){
                used[i] = 1;
                weight += wt[i];
                value += val[i];
                done = 0;
                break;
            } else {
                used[i] = 0;
                weight -= wt[i];
                value -= val[i];         
            }
        }
        if(weight <= C && value > maxValue && value > 0){
            if(rank == 1){
                printf("Bound:%llu,    Value=%d \n",lowerBound, value);
                print_arr(used,n,rank) ;
            }
            maxValue = value;
        }
        lowerBound++;
        if(lowerBound > upperBound){
            break;
        }
        
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&maxValue, &overallMaxValue, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);

    return overallMaxValue;

}
