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
    (void)argc;
    (void)argv;

    // create context.
    struct FlagshipContext ctx = {0};

    flagship_begin(&ctx); {
        flagship_name(&ctx, "%s", argv[0]);
        flagship_description(&ctx, "example of flags: build system");

        flagship_begin_mode(&ctx); {
            flagship_name(&ctx, "help");
            flagship_description(&ctx, "print help for given mode and exit");

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_ENUM); {
                flagship_description(&ctx, "mode to print help for");
                flagship_is_terminating(&ctx, true);

                flagship_enum_variant(&ctx, "help");
                flagship_enum_variant(&ctx, "build");
                flagship_enum_variant(&ctx, "run");
                flagship_enum_variant(&ctx, "pkg");
                flagship_end_flag(&ctx);
            }

            flagship_end_mode(&ctx);
        }

        flagship_begin_mode(&ctx); {
            flagship_name(&ctx, "build");
            flagship_description(&ctx, "build program");

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_STRING); {
                flagship_name(&ctx, "output");
                flagship_name(&ctx, "o");

                flagship_description(&ctx, "set output directory");
                flagship_note(&ctx, "program will create directory if it doesn't already exist");

                flagship_end_flag(&ctx);
            }

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_ENUM); {
                flagship_name(&ctx, "rebuild");
                flagship_name(&ctx, "r");

                flagship_description(&ctx, "always rebuild given module");

                flagship_is_repeatable(&ctx, true);

                flagship_enum_variant(&ctx, "all");
                flagship_enum_variant(&ctx, "project");
                flagship_enum_variant(&ctx, "vendor");
                flagship_enum_variant(&ctx, "resources");

                flagship_end_flag(&ctx);
            }

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_ENUM); {
                flagship_name(&ctx, "no-rebuild");
                flagship_name(&ctx, "nr");

                flagship_description(&ctx, "never rebuild given module");

                flagship_is_repeatable(&ctx, true);

                flagship_enum_variant(&ctx, "all");
                flagship_enum_variant(&ctx, "project");
                flagship_enum_variant(&ctx, "vendor");
                flagship_enum_variant(&ctx, "resources");

                flagship_end_flag(&ctx);
            }

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_ENUM); {
                flagship_name(&ctx, "target");
                flagship_name(&ctx, "t");

                flagship_description(&ctx, "set target platform");

                flagship_enum_variant(&ctx, "native");
                flagship_enum_variant(&ctx, "linux");
                flagship_enum_variant(&ctx, "windows");
                flagship_enum_variant(&ctx, "macos");

                flagship_default(&ctx, "native");

                flagship_end_flag(&ctx);
            }

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_BOOL); {
                flagship_name(&ctx, "strip-symbols");
                flagship_name(&ctx, "s");
                flagship_description(&ctx, "strip debug symbols");

                flagship_end_flag(&ctx);
            }

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_BOOL); {
                flagship_name(&ctx, "optimize");
                flagship_name(&ctx, "O");
                flagship_description(&ctx, "compile with highest optimizations");

                flagship_end_flag(&ctx);
            }

            flagship_end_mode(&ctx);
        }

        flagship_begin_mode(&ctx); {
            flagship_name(&ctx, "run");
            flagship_description(&ctx, "build and run program");

            flagship_copy(&ctx, "build", "output");
            flagship_copy(&ctx, "build", "rebuild");
            flagship_copy(&ctx, "build", "no-rebuild");
            flagship_copy(&ctx, "build", "target");
            flagship_copy(&ctx, "build", "strip-symbols");
            flagship_copy(&ctx, "build", "optimize");

            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_BOOL); {
                flagship_name(&ctx, "-");
                flagship_description(&ctx, "pass remaining flags to sub-process");
                flagship_is_terminating(&ctx, true);
                flagship_end_flag(&ctx);
            }

            flagship_end_mode(&ctx);
        }

        flagship_begin_mode(&ctx); {
            flagship_name(&ctx, "pkg");
            flagship_description(&ctx, "build in release mode and package for distribution");

            flagship_copy(&ctx, "build", "output");
            flagship_copy(&ctx, "build", "target");

            flagship_end_mode(&ctx);
        }

        flagship_help_print(&ctx, "pkg", true);

        printf("memory allocated: %zu\n", ctx.allocator.sz);

        flagship_end(&ctx);
    }
}

