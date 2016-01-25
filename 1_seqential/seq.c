#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SECONDS_TO_NANOSECONDS 1000000000L

void printUsage(void)
{
    fprintf( stdout, "Usage: seq -m<rows> -n<cols> -i<input-file> -o<output-file>\n");
	exit(1);
}

int main (int argc, char* argv[])
{
    int c;
    int m, n;
    struct timespec tStart, tEnd;
    unsigned long int tDuration;
    int i, j;
	int index;
    
    char* in;
    char* out;
    
    FILE *fpIn = NULL;
    FILE *fpOut = NULL;

	fprintf( stdout, "start\n" );

	fflush( stdout );
    
    /* Input matrix is m x n, with first and last columns and rows containing boundary-values.
     * To operate only on a single matrix the output will be stored from 0,0 to (m-2),(n-2) leaving
     * the last 2 rows and columns unchanged.
     * m and n are both expected to be of power of 2 plus 2 to make the output matrix dimensions and
     * therfore the rows and columns which have to be computed are of power of two.
     */
    
    opterr = 0;
    
/* input processing */

    /* argument check */
    while( ( c = getopt( argc, argv, "m:n:i:o:") ) != -1 )
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
                if( optopt == 'm' || optopt == 'n' || optopt == 'i' || optopt == 'o')
                    fprintf( stderr, "Option -%c requires an argument.\n", optopt);
                else if( isprint( optopt ) )
                    fprintf( stderr, "Unknown option `-%c´.\n", optopt );
                else
                    fprintf( stderr, "Unknown option characer `\\x%x´.\n", optopt );

                printUsage();
            default:
                printUsage();
        }
/*
	int abort = 0;
	for( index = optind; index < argc; index++)
    	{
		printf("Non-option argument %s\n", argv[index] );
		abort = 1;
	}
	if( abort == 0 )
		printUsage();
*/

	fprintf( stdout, "computing ...\n");

	fflush( stdout );

    float *A = malloc( sizeof( float ) * m * n );

    
/* computation */
    
    clock_gettime( CLOCK_REALTIME, &tStart );

	fprintf( stdout, "Input:%s\n", in );
    
    /* get input values */
    
    fpIn = fopen( in, "r" );

	fprintf( stdout, "2\n" );

	fflush( stdout );
    
    if( fpIn == NULL )
    {
        fprintf( stderr, "Could not open input-file!\n");
        exit( 1 );
    }
    
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            fscanf( fpIn, "%f;", &A[i*n + j] );
            fprintf( stdout, "%f,", A[i*n + j] );
        }
        fprintf( stdout, "\n" );
    }

	fprintf( stdout, "3\n");
    
    fclose( fpIn );
    
    /* process values */
    
    /* output */
    
    
    clock_gettime( CLOCK_REALTIME, &tEnd );
    
    tDuration = ( tEnd.tv_sec - tStart.tv_sec ) * SECONDS_TO_NANOSECONDS + ( tEnd.tv_nsec - tStart.tv_nsec ) ;

    fprintf( stdout, "%d x %d - Matrix processed in %lu ns (%lu - %lu)\n", m, n, tDuration, tStart.tv_nsec, tEnd.tv_nsec );
}
