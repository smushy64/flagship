/**
 * @file   example.c
 * @brief  Example of Flagship.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   September 29, 2025
*/
// IWYU pragma: begin_keep
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
// IWYU pragma: end_keep

#include "flagship.h"

int main(int argc, char** argv) {
    // create context.
    struct FlagshipContext ctx = {0};

    // string token
    FlagshipString flag_help = 0;

    // define schema
    flagship_begin(&ctx); {
        // set program name
        flagship_name(&ctx, argv[0]);

        // define mode-less flag
        flagship_begin_flag(&ctx, FLAGSHIP_TYPE_BOOL); {
            // set name for this flag
            // first call to this function will set the name used to look up this flag
            //
            // all format string functions return a 'token'
            // this 'token' is an offset into the internal string buffer
            // which can be used to refer to this string later
            // or dereference into a const char * using flagship_deref()
            flag_help = flagship_name(&ctx, "help");
            // subsequent calls will attach aliases to this flag
            flagship_name(&ctx, "h");
            
            // set the description for this flag
            flagship_description(&ctx, "print this help message and exit.");

            // add a note to this flag's description
            // flagship_note(&ctx, "...");

            // add a warning to this flag's description
            // flagship_warning(&ctx, "...");

            // set the default value of this flag
            // flagship_default(&ctx, "false");

            // if this flag is encountered, stop parsing subsequent flags
            flagship_is_terminating(&ctx, true);

            // any duplicates found of this flag are
            // will overwrite previous instances
            //
            // by default, all flags are unique
            flagship_is_unique(&ctx, true);

            // finish defining this flag
            flagship_end_flag(&ctx);
        }

        // define a mode
        flagship_begin_mode(&ctx); {
            // name this mode
            flagship_name(&ctx, "build");
            // modes can also have aliases
            flagship_name(&ctx, "b");

            // defining a name-less string flag
            // each mode can only have one name-less string flag
            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_STRING); {
                // set flag description
                flagship_description(&ctx, "path to source file");

                // stop defining this flag
                flagship_end_flag(&ctx);
            }

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_BOOL); {
                // copy a flag from another mode
                // 0 for the 'mode' argument means this flag is mode-less
                flagship_copy(&ctx, 0, flag_help);

                flagship_end_flag(&ctx);
            }

            // end this mode
            flagship_end_mode(&ctx);
        }

#if 0
        // parse flags
        // last argument is if help should be printed upon failure
        if(!flagship_parse(&ctx, argc, argv, true)) {
            // cleanup before exiting
            flagship_end(&ctx);
            // return some error
            return 1;
        }

        // read flags by iterating through results
        int mode = 0;
        while(flagship_mode_iter_next(&ctx, &mode)) {
            struct FlagshipResult result = {0};
            while(flagship_flag_iter_next(&ctx, &result)) {
                // do something with flag . . .
            }
            // flag iterator can be reset like so:
            flagship_flag_iter_reset(&ctx);
        }
        // mode iterator can be reset like so:
        flagship_mode_iter_reset(&ctx);

        // you can also read flags directly via FlagshipStrings
        struct FlagshipResult result = {0};
        if(flagship_read(&ctx, &result, 0, 0)) {
            // do something with result . . .
        }

        // or search via strings
        result = (struct FlagshipResult){0};
        if(flagship_search(&ctx, &result, "mode", "flag")) {
            // do something with result . . .
        }

#endif
        flagship_help_print(&ctx, NULL);

        // cleanup everything
        // do not call any other flagship functions after this
        flagship_end(&ctx);
    }
}

#if 0
void make_flags( FShipContext* ctx, const char* program_name ) {
    fls_set_program_name( ctx, program_name );
    fls_set_description( ctx, "This is an example of Flagship." );

    fls_mode_begin( ctx, "help" ); {
        fls_mode_set_description( ctx, "Print this help message and exit." );

        fls_add_str( ctx, NULL, .description="Name of mode to print help for.", .is_terminating=true );

        fls_mode_end( ctx );
    }

    fls_mode_begin( ctx, "build" ); {
        fls_mode_set_description( ctx, "Build project." );

        fls_add_str(
            ctx, "output",
            .aliases=fls_strings("o"),
            .description="Set output path.",
            .default_value="./build"
        );
        fls_add_str(
            ctx, "target",
            .aliases=fls_strings("t"),
            .description="Set target platform.",
            .str={ .valid=fls_strings( "native", "windows", "gnu-linux", "wasm" ) },
            .default_value="native"
        );
        fls_add_str(
            ctx, "compiler",
            .aliases=fls_strings("c"),
            .description="Set compiler.",
            .str={ .valid=fls_strings( "gcc", "clang" ) },
            .default_value="gcc"
        );
        fls_add_str(
            ctx, "optimization",
            .aliases=fls_strings("opt"),
            .description="Set optimization level.",
            .str={ .valid=fls_strings( "none", "speed", "size" ) },
            .default_value="none"
        );
        fls_add_flag(
            ctx, "static",
            .description="Build a static library instead of dynamic."
        );
        fls_add_flag(
            ctx, "fPIC",
            .description="Build using -fPIC flag.",
            .note="If library is dynamic, -fPIC is used by default."
        );
        fls_add_flag(
            ctx, "stdlib",
            .description="Link against C standard library.",
            .note="Always links if compiling for windows."
        );
        fls_add_flag( ctx, "debug", .description="Build with debug symbols." );

        fls_mode_end( ctx );
    }

    fls_mode_begin( ctx, "run" ); {
        fls_mode_set_description( ctx, "Build and run project." );

        fls_add_str(
            ctx, "target",
            .aliases=fls_strings("t"),
            .description="Set target platform.",
            .str={ .valid=fls_strings( "native", "windows", "gnu-linux", "wasm" ) },
            .default_value="native"
        );
        fls_add_str(
            ctx, "compiler",
            .aliases=fls_strings("c"),
            .description="Set compiler.",
            .str={ .valid=fls_strings( "gcc", "clang" ) },
            .default_value="gcc"
        );
        fls_add_str(
            ctx, "optimization",
            .aliases=fls_strings("opt"),
            .description="Set optimization level.",
            .str={ .valid=fls_strings( "none", "speed", "size" ) },
            .default_value="none"
        );
        fls_add_flag(
            ctx, "static",
            .description="Build a static library instead of dynamic."
        );
        fls_add_flag(
            ctx, "fPIC",
            .description="Build using -fPIC flag.",
            .note="If library is dynamic, -fPIC is used by default."
        );
        fls_add_flag(
            ctx, "stdlib",
            .description="Link against C standard library.",
            .note="Always links if compiling for windows."
        );
        fls_add_flag( ctx, "debug", .description="Build with debug symbols." );

        fls_add_flag(
            ctx, "-",
            .description="Remaining flags are passed on to project.",
            .is_terminating=true
        );

        fls_mode_end( ctx );
    }

    fls_mode_begin( ctx, "term" ); {
        fls_mode_set_description( ctx, "This mode passes flags to sub-process." );
        fls_mode_set_terminating( ctx );
        fls_mode_end( ctx );
    }
}

int main( int argc, char** argv ) {

    FShipContext ctx = {};
    make_flags( &ctx, argv[0] );

    int last_arg = 0;
    if( !fls_parse( &ctx, argc, argv, &last_arg ) ) {
        fls_free( &ctx );
        return 1;
    }

    const char* mode = fls_query_mode( &ctx );

    if( mode ) {
        if( strcmp( mode, "help" ) == 0 ) {
            const char* mode_name = fls_read_str( &ctx, NULL );

            fls_help( &ctx, mode_name, true );
            fls_free( &ctx );
            return 0;
        } else if( strcmp( mode, "build" ) == 0 ) {
            const char* output    = fls_read_str( &ctx, "output" );
            const char* target    = fls_read_str( &ctx, "target" );
            const char* compiler  = fls_read_str( &ctx, "compiler" );
            const char* opt       = fls_read_str( &ctx, "optimization" );
            bool        is_static = fls_read_flag( &ctx, "static" );
            bool        is_fPIC   = fls_read_flag( &ctx, "fPIC" );
            bool        is_stdlib = fls_read_flag( &ctx, "stdlib" );
            bool        is_debug  = fls_read_flag( &ctx, "debug" );

            printf( "output:   '%s'\n", output );
            printf( "target:   %s\n", target );
            printf( "compiler: %s\n", compiler );
            printf( "opt:      %s\n", opt );
            printf( "static:   %s\n", is_static ? "true" : "false" );
            printf( "fPIC:     %s\n", is_fPIC ? "true" : "false" );
            printf( "stdlib:   %s\n", is_stdlib ? "true" : "false" );
            printf( "debug:    %s\n", is_debug ? "true" : "false" );
        } else if( strcmp( mode, "run" ) == 0 ) {
            const char* target    = fls_read_str( &ctx, "target" );
            const char* compiler  = fls_read_str( &ctx, "compiler" );
            const char* opt       = fls_read_str( &ctx, "optimization" );
            bool        is_static = fls_read_flag( &ctx, "static" );
            bool        is_fPIC   = fls_read_flag( &ctx, "fPIC" );
            bool        is_stdlib = fls_read_flag( &ctx, "stdlib" );
            bool        is_debug  = fls_read_flag( &ctx, "debug" );

            printf( "target:   %s\n", target );
            printf( "compiler: %s\n", compiler );
            printf( "opt:      %s\n", opt );
            printf( "static:   %s\n", is_static ? "true" : "false" );
            printf( "fPIC:     %s\n", is_fPIC ? "true" : "false" );
            printf( "stdlib:   %s\n", is_stdlib ? "true" : "false" );
            printf( "debug:    %s\n", is_debug ? "true" : "false" );

            if( fls_read_flag( &ctx, "-" ) ) {
                printf( "passthrough: '" );
                for( int i = last_arg + 1; i < argc; ++i ) {
                    printf( "%s", argv[i] );
                    if( (i + 1) < argc ) {
                        printf( " " );
                    }
                }
                printf( "'\n" );
            }
        } else if( strcmp( mode, "term" ) == 0 ) {
            printf( "passthrough: '" );
            for( int i = last_arg + 1; i < argc; ++i ) {
                printf( "%s", argv[i] );
                if( (i + 1) < argc ) {
                    printf( " " );
                }
            }
            printf( "'\n" );
        }
    } else {
        fls_help( &ctx, NULL, true );
    }

    fls_free( &ctx );
    return 0;
}
#endif

