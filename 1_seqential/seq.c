#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SECONDS_TO_NANOSECONDS 1000000000L

void printUsage(void)
{
    fprintf( stdout, "Usage: seq -m <rows> -n <cols> -i <input-file> -o <output-file>\n");
    exit(1);
}

int main (int argc, char* argv[])
{
    int c;
    int m, n;
    struct timespec tStart, tEnd;
    unsigned long int tDuration;
    int i, j;
    
    char* in;
    char* out;
    
    FILE *fpIn = NULL;
    FILE *fpOut = NULL;
    
    /* Input matrix is m x n, with first and last columns and rows containing boundary-values.
     * To operate only on a single matrix the output will be stored from 0,0 to (m-2),(n-2) leaving
     * the last 2 rows and columns unchanged.
     * m and n are both expected to be of power of 2 plus 2 to make the output matrix dimensions and
     * therfore the rows and columns which have to be computed are of power of two.
     */
    float *A = malloc( sizeof( float ) * m * n );
    
    opterr = 0;
    
/* parameter processing */

    /* argument check */
    while( ( c = getopt( argc, argv, "mnio") ) != -1 )
        switch( c )
        {
            case 'm':
                m = atoi( optarg );
                break;
            case 'n':
                n = atoi( optarg );
                break;
            case 'i':
                in = optarg;
                break;
            case 'o':
                out = optarg;
                break;
            case '?':
                if( optopt == 'm' || optopt == 'n' || optopt == 'i' || optopt == 'o' )
                    fprintf( stderr, "Option -%c requires an argument.\n", optopt);
                else if( isprint( optopt ) )
                    fprintf( stderr, "Unknown option `-%c´.\n", optopt );
                else
                    fprintf( stderr, "Unknown option characer `\\x%x´.\n", optopt );
                
                printUsage();
            default:
                printUsage();
        }
    
/* computation */
    
    clock_gettime( CLOCK_MONOTONIC, &tStart );
    
    /* get input values */
    
    fpIn = fopen( in, "r" );
    
    if( fpIn == NULL )
    {
        fprintf( stderr, "Could not open input-file '%s'!\n", in);
        exit( 1 );
    }
    
    for (i = 0; i < m; ++i) {
        for (j = 0; i < n; ++j) {
            if( fscanf( fpIn, "%f;", &A[i*n + j] ) == 0 )
            {
                fprintf( stderr, "Error: input does not match expected format!\n" );
                exit ( 1 );
            }
            fprintf( stdout, "%f, ", A[i*n + j] ); // print items of row
        }
        fprintf( stdout, "\n" ); // newline at the end of row
    }
    
    fclose( fpIn );
    
    /* process values */
    
    /* output */
    
    for (i = 0; i < m; ++i) {
        for (j = 0; i < n; ++j) {
            fprintf( stdout, "%f, ", A[i*n + j] );
        }
        fprintf( stdout, "\n" ); // newline at the end of row
    }
    
    clock_gettime( CLOCK_MONOTONIC, &tEnd );
    
    tDuration = ( tEnd.tv_sec - tStart.tv_sec ) * SECONDS_TO_NANOSECONDS + ( tEnd.tv_nsec - tStart.tv_nsec ) ;

    fprintf( stdout, "%d x %d - Matrix processed in %lu ns", m, n, tDuration );
}