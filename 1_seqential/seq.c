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
    int c;      // option character
    int i, j;   // matrix loop variables
    int m, n;   // matrix dimensions

    struct timespec tStart, tEnd;
    unsigned long int tDuration;

    char* in;
    char* out;
    
    FILE *fpIn = NULL;
    FILE *fpOut = NULL;
    
/* parameter processing */

    /* argument check */
    opterr = 0;
    
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
                    fprintf( stderr, "Option -%c requires an argument.\n", optopt );
                else if( isprint( optopt ) )
                    fprintf( stderr, "Unknown option `-%c´.\n", optopt );
                else
                    fprintf( stderr, "Unknown option characer `\\x%x´.\n", optopt );
                
                printUsage();
            default:
                printUsage();
        }
    
    /* value check - m,n = 2^x+2 */
    if( (m % 2) != 2 )
    {
        fprintf( stderr, "Input-error: m has to be of power of 2 plus 2 (e.g. 6, 10, 18) but is '%d' instead!\n", m );
    }
    
    if( (n % 2) != 2 )
    {
        fprintf( stderr, "Input-error: n has to be of power of 2 plus 2 (e.g. 6, 10, 18) but is '%d' instead!\n", n );
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
    
    clock_gettime( CLOCK_MONOTONIC, &tStart );
    
    /* open file */
    fpIn = fopen( in, "r" );
    
    if( fpIn == NULL )
    {
        fprintf( stderr, "Could not open input-file '%s'!\n", in );
        exit( 1 );
    }
    
    /* read values */
    for (i = 0; i < m; i++) {
        for (j = 0; i < n; j++) {
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
    
    for( i = 1; i < (m - 1); i++)
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
             *             |          |
             * |           | (i+1), j |
             *             |          |
             */
            A[ (i-1) * n + (j-1) ] = 0.25 * ( A[ (i-1)*n + j] + A[ i*n + (j-1) ] + A[ i*n + (j+1) ] + A[ (i+1)*n + j] );
        }
    }
    
    /* output */
    
    /* open file */
    fpOut = fopen( out, "w" );
    
    if( fpOut == NULL )
    {
        fprintf( stderr, "Could not write to output-file '%s'!\n", out );
        exit( 1 );
    }
    
    for (i = 0; i < (m-2); i++) {
        for (j = 0; i < (n-2); j++) {
            fprintf( fpOut, "%f, ", A[i*n + j] );
        }
        fprintf( fpOut, "\n" ); // newline at the end of row
    }
    
    clock_gettime( CLOCK_MONOTONIC, &tEnd );
    
    tDuration = ( tEnd.tv_sec - tStart.tv_sec ) * SECONDS_TO_NANOSECONDS + ( tEnd.tv_nsec - tStart.tv_nsec ) ;

    fprintf( stdout, "%d x %d - Matrix processed in %lu ns", m, n, tDuration );
    
    return( 0 );
}