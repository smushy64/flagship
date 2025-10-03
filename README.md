# Flagship

Header-only library for parsing command-line arguments.

## Usage

```c
// only need to include header, all functions are inline
#include <flagship.h>

int main( int argc, char** argv ) {
    // create context
    // must always be zero-init
    FShipContext ctx = {};

    // create schema
    fls_add_flag( &ctx, "flag-bool" );
    fls_add_int( &ctx, "flag-integer" );
    fls_add_flt( &ctx, "flag-float" );
    fls_add_str( &ctx, "flag-string" );

    // parse arguments
    int last_arg = 0;
    if( fls_parse( &ctx, argc, argv, &last_arg ) ) {
        // read arguments
        const char* string = fls_read_str( &ctx, "flag-string" );
    }

    // free context
    fls_free( &ctx );

    return 0;
}

```

