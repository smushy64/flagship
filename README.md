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

    // add flag with settings
    fls_add_flag( &ctx, "flag-bool-with-settings", .description="This is a description." );

    // add flag with aliases
    fls_add_flag( &ctx, "flag-with-aliases", .aliases=fls_strings( "fwa", "another-alias" ) );

    // define a mode
    // mode names cannot start with - or +
    fls_mode_begin( &ctx, "mode-name" ); {
        fls_mode_set_description( &ctx, "Some description for the current mode." );
        // to stop parsing once this mode is encountered:
        // fls_mode_set_terminating( &ctx );

        // to set this flag when invoking program:
        // ./program mode-name -some-flag
        fls_add_flag( &ctx, "some-flag" );

        // always call this when done creating mode.
        fls_mode_end( &ctx );
    }

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

