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
        }
    } else {
        fls_help( &ctx, NULL, true );
    }

    fls_free( &ctx );
    return 0;
}

