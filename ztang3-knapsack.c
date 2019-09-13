/* Knapsack calculation based on that of */
/* https://www.tutorialspoint.com/cplusplus-program-to-solve-knapsack-problem-using-dynamic-programming */

#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>

long int knapSack(long int C, long int w[], long int v[], int n);

uint64_t GetTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

int main() {
    long int C;    /* capacity of backpack */
    int n;    /* number of items */
    int i;    /* loop counter */

    scanf ("%ld", &C);
    scanf ("%d", &n);

    long int v[n], w[n];        /* value, weight */

    for (i = 0; i < n; i++) {
        scanf ("%ld %ld", &v[i], &w[i]);
    }

    uint64_t start = GetTimeStamp ();
    long int ks = knapSack(C, w, v, n); 
    printf ("knapsack occupancy %ld\n", ks);
    printf ("Time: %ld us\n", (uint64_t) (GetTimeStamp() - start));

    return 0;
}

/* PLACE YOUR CHANGES BELOW HERE */
#include <math.h>
#include <strings.h>

long int max(long int x, long int y) {
   return (x > y) ? x : y;
}

long int knapSack(long int C, long int w[], long int v[], int n) {
    int i;
    long int wt;
    long int K[n+1][C+1];
    
    
    unsigned char used [n], solution [n];
    int done = 0;
    bzero (used, sizeof (used));
    long int max_value = 0;
    long int weight_of_max = 0;
    long int weight = 0;
    long int value = 0;
    
    if (C*n<=pow(2,n)){
        
        for (i = 0; i <= n; i++) {
            for (wt = 0; wt <= C; wt++) {
                if (i == 0 || wt == 0)
                    K[i][wt] = 0;
                else if (w[i-1] <= wt)
                    K[i][wt] = max(v[i-1] + K[i-1][wt - w[i-1]], K[i-1][wt]);
                else
                    K[i][wt] = K[i-1][wt];
            }
            return K[n][C];
    }
    }else{
        while (!done) {
            int carry = 1;
            done = 1;
            for (i = 0; i < n; i++)
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
            if (weight <= C && value > max_value) {
                max_value = value;
                weight_of_max = weight;
                bcopy (used, solution, sizeof (used));
            }
        }
        return max_value;
    }
}
/* mpicc ztang3-knapsack -o ztang3-knapsack */
