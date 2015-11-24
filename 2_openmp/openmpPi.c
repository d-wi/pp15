#include <omp.h>
#include <stdio.h>


#define SERIAL TRUE

static long num_steps = 1000000000;
double step;

void main ()
{
    int i;
    double x;
    double pi, sum = 0.0;
    double tBefore, time;
    
    step = 1.0 / (double) num_steps;
    
    tBefore = omp_get_wtime();
    
#if( SERIAL )
    
    for ( i = 0; i < num_steps; i++)
    {
        x = ( i + 0.5 ) * step;
        sum = sum + 4.0 / ( 1.0 + x * x );
    }
    
#else
    
    omp_set_num_threads(2);
    
    #pragma omp parallel private(i,x)
    {
        
    #pragma omp for reduction(+:sum) schedule(static)
        for ( i = 0; i < num_steps; i++)
        {
            x = ( i + 0.5 ) * step;
            sum += 4.0 / ( 1.0 + x * x );
        }
    }
    
#endif
    
    pi = step * sum;
    
    time = omp_get_wtime() - tBefore;

    printf("%f - %f\n",pi,time);
}