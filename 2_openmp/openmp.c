#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

#define SECONDS_TO_NANOSECONDS 1000000000L

void printUsage(void)
{
    fprintf( stdout, "Usage: seq -m <rows> -n <cols> -p <processor-count> -i <input-file> -o <output-file>\n");
    exit( 1 );
}

int main (int argc, char* argv[])
{
    int c;      // option character
    int i, j;   // matrix loop variables
    int m, n;   // matrix dimensions
    int p;      // core count
    
    double tStart, tEnd;
    double tDuration;
    
    char* in;
    char* out;
    
    FILE *fpIn = NULL;
    FILE *fpOut = NULL;
    FILE *fpMeas = NULL;
    
    /* parameter processing */
    
    fprintf( stdout, "checking paramters ...\n" );
    
    /* argument check */
    opterr = 0;
    
    while( ( c = getopt( argc, argv, "m:n:p:i:o:") ) != -1 )
        switch( c )
    {
        case 'm':
            m = atoi( optarg );
            break;
        case 'n':
            n = atoi( optarg );
            break;
        case 'p':
            p = atoi( optarg );
            break;
        case 'i':
            in = optarg;
            break;
        case 'o':
            out = optarg;
            break;
        case '?':
            if( optopt == 'm' || optopt == 'n' || optopt == 'i' || optopt == 'o' )
                fprintf( stderr, "Option -%c requires an argument.\n", optopt );
            else if( isprint( optopt ) )
                fprintf( stderr, "Unknown option `-%c´.\n", optopt );
            else
                fprintf( stderr, "Unknown option characer `\\x%x´.\n", optopt );
            printUsage();
        default:
            printUsage();
    }
    
    fprintf( stdout, "Values: m=%d, n=%d, p=%d, in=%s, out=%s\n", m, n, p, in, out );
    fflush( stdout );
    
    
    int tmp = (m-2);
    
    /* value check - m,n = 2^x+2 */
    if( !((tmp > 0) && ((tmp & (tmp - 1)) == 0)))
    {
        fprintf( stderr, "Input-error: m has to be of power of 2 plus 2 (e.g. 6, 10, 18) but is '%d' instead! (tmp=%d)\n", m, tmp );
        exit( 1 );
    }
    
    tmp = (n-2);
    
    if( !((tmp > 0) && ((tmp & (tmp - 1)) == 0)))
    {
        fprintf( stderr, "Input-error: n has to be of power of 2 plus 2 (e.g. 6, 10, 18) but is '%d' instead!\n", n );
        exit( 1 );
    }
    
    /* p has to be at least 1 */
    if(p < 1)
    {
        fprintf( stderr, "Input-error: p has to be at least 1");
        exit( 1 );
    }
    /* limit p to maximum */
    if(p > omp_get_max_threads())
    {
        fprintf( stdout, "Warning: p exceeds maximum thread count, proceeding with p=%d instead!", omp_get_max_threads() );
        p = omp_get_max_threads();
    }
    
    /* Input matrix is m x n, with first and last columns and rows containing boundary-values.
     * To operate only on a single matrix the output will be stored from 0,0 to (m-2),(n-2) leaving
     * the last 2 rows and columns unchanged.
     * m and n are both expected to be of power of 2 plus 2 to make the output matrix dimensions and
     * therfore the rows and columns which have to be computed are of power of two.
     */
    
    /*       0   1   2   3  ...  n
     *     -------------------------
     *   0 | x | b | b | b |...| x |
     *     +---+---+---+---+---+---+
     *   1 | b |   |   |   |...| b |
     *     +---+---+---+---+---+---+
     *   2 | b |   |   |   |...| b |
     *     +---+---+---+---+---+---+
     *   3 | b |   |   |   |...| b |
     *     +---+---+---+---+---+---+
     * ... |...|...|...|...|...|...|    x ... ignored values
     *     +---+---+---+---+---+---+    b ... boundary values
     *   m | x | b | b | b |...| x |
     *     +---+---+---+---+---+---+
     */
    float *A = malloc( sizeof( float ) * m * n );
    
    /* computation */
    
    fprintf( stdout, "reading input\n" );
    
    omp_set_num_threads( p );
    
    tStart = omp_get_wtime();
    
    /* open file */
    fpIn = fopen( in, "r" );
    
    if( fpIn == NULL )
    {
        fprintf( stderr, "Could not open input-file '%s'!\n", in );
        exit( 1 );
    }
    
    /* read values */
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            if( fscanf( fpIn, "%f;", &A[i*n + j] ) == 0 )
            {
                fprintf( stderr, "Error: input does not match expected format!\n" );
                exit ( 1 );
            }
            // fprintf( stdout, "%f, ", A[i*n + j] ); // print items of row
        }
        // fprintf( stdout, "\n" ); // newline at the end of row
    }
    
    fclose( fpIn );
    
    fprintf( stdout, "processing values ...\n" );

    float *prevBound = malloc( sizeof( float ) * n);
    float *lastRowOfBlock = malloc( sizeof(float) * n);
    float *nextBound = malloc( sizeof( float ) * n);

    int id;
    int tmax;
    int size;
    long offset;
    
    /* process values */
    #pragma omp parallel shared(A) private( id, tmax, size, offset, i, j, prevBound, lastRowOfBlock, nextBound) schedule(static, (m-2)/p ) 
    {
	id = omp_get_thread_num();
	tmax = omp_get_num_threads();

        size = (m-2) / tmax;

	offset = id * size * n;

        /* copy boundary values */
	memcpy( prevBound, A + ( sizeof(float) * (offset - n) ), sizeof( float ) * n );
	memcpy( lastRowRowOfBlock, A + ( sizeof(float) * ( offset + (size - 1) * n ) ), sizeof( float ) * n );
	memcpy( nextBound, A + ( sizeof(float) * (offset + size * n ) ), sizeof( float ) * n );

        /* synchronize - all threads must have their local copy of bondaries ready for computation */
	#pragma omp barrier

	if( size == 1)
        {
	    for( j = 1; j < (n - 1); j++)
            {
                A[ offset + j-1 ] = 0.25 * ( prevBound[ j ] + lastRowOfBlock[ j-1 ] + lastRowOfBlock[ j+1 ] + nextBound[ j ] );
	    }
        }
	else
        {
            /* bondary-values to previous block */
            for( j = 1; j < (n - 1); j++)
	    {
                A[ offset + j-1 ] = 0.25 * ( prevBound[ j ] + A[ offset + n + (j-1)] + A[ offset + n + (j+1) ] + A[ offset + 2*n + j] );
	    }

            for( i = 1; i < (size - 2); i++)
            {
                for( j = 1; j < (n - 1); j++)
                {
	    // 4-point stencil
            
        	    /*          <--------------------
             *             |          |
             *    <NEWVAL> | (i-1), j |
             * A           |          |
             * | --------------------------------
             * |           |          |
             * |  i, (j-1) |    i,  j | i, (j+1)
             * |           |          |
             * | --------------------------------
             * |           |          |
             *             | (i+1), j |
             *             |          |
             */
                     A[ offset + n*i + (j-1) ] = 0.25 * ( A[ offset + n*i + j] + A[ offset + n*(i+1) + (j-1) ] + A[ offset + n*(n+1) + (j+1) ] + A[ offset + n*(n+2) + j] );
                }
            }

            for( j = 1; j < (n - 1); j++)
            {
                A[ offset + (size-1) * n + (j-1) ] = 0.25 * ( A[ offset + (size-1) * n + j ] + A[ offset + size*n + (j-1) ] + A[ offset + size*n + (j+1) ] + nextBound[ j ] );
            }
        }
    }
    
    /* output */
    
    fprintf( stdout, "writing results back ...\n" );
    
    /* open file */
    fpOut = fopen( out, "w" );
    
    if( fpOut == NULL )
    {
        fprintf( stderr, "Could not write to output-file '%s'!\n", out );
        exit( 1 );
    }
    
    for (i = 0; i < (m-2); i++) {
        for (j = 0; j < (n-2); j++) {
            fprintf( fpOut, "%f, ", A[i*n + j] );
        }
        fprintf( fpOut, "\n" ); // newline at the end of row
    }
    
    fclose( fpOut );
    
    tEnd = omp_get_wtime();
    
    tDuration = ( tEnd - tStart );
    
    fprintf( stdout, "%d x %d - Matrix processed in %f s\n", m, n, tDuration );
    
    /* reporting measurement to file */
    
    char measFile[255];
    snprintf( measFile, 255, "../output/%s_%d_%d.txt", argv[0], m, n );
    
    fpMeas = fopen( measFile, "a" );
    
    fprintf( fpMeas, "%f\n", tDuration );
    
    fclose( fpMeas );
    
    return( 0 );
}
