#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

void printUsage(void)
{
    fprintf( stdout, "Usage: seq -m <rows> -n <cols> -o <output-file>\n");
    exit( 1 );
}

int main (int argc, char* argv[])
{
    int c;      // option character
    int i, j;   // matrix loop variables
    int m, n;   // matrix dimensions
    
    char* out;
    
    FILE *fpOut = NULL;
    
    /* parameter processing */
    
    fprintf( stdout, "checking paramters ...\n" );
    
    /* argument check */
    opterr = 0;
    
    while( ( c = getopt( argc, argv, "m:n:o:") ) != -1 )
        switch( c )
    {
        case 'm':
            m = atoi( optarg );
            break;
        case 'n':
            n = atoi( optarg );
            break;
        case 'o':
            out = optarg;
            break;
        case '?':
            if( optopt == 'm' || optopt == 'n' || optopt == 'o' )
                fprintf( stderr, "Option -%c requires an argument.\n", optopt );
            else if( isprint( optopt ) )
                fprintf( stderr, "Unknown option `-%c´.\n", optopt );
            else
                fprintf( stderr, "Unknown option characer `\\x%x´.\n", optopt );
            printUsage();
        default:
            printUsage();
    }
    
    fprintf( stdout, "Values: m=%d, n=%d, out=%s\n", m, n, out );
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
    
    /* output */
    
    fprintf( stdout, "writing file ...\n" );
    
    /* open file */
    fpOut = fopen( out, "w" );
    
    if( fpOut == NULL )
    {
        fprintf( stderr, "Could not write to output-file '%s'!\n", out );
        exit( 1 );
    }
    
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            fprintf( fpOut, "%.2f;", (float) (i+j) );
        }
        fprintf( fpOut, "\n" ); // newline at the end of row
    }
    
    fclose( fpOut );
    
    return( 0 );
}
