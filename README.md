# Flagship

Header-only library for parsing command-line arguments.

Generate documentation using provided `docs.sh` for more info.

## Usage

```c
// only need to include header, all functions are inline
#include "flagship.h"

int main(int argc, char** argv) {
    // context holds all the data flagship will use
    struct FlagshipContext ctx;

    // begin initializes the context
    flagship_begin(&ctx); {

        // set the name of the program
        // this will be used when printing generated help message
        flagship_name(&ctx, argv[0]);

        // set the description of the program
        flagship_description(&ctx, "this is a program");

        // create a new mode
        // modes are an optional way to group flags together
        flagship_begin_mode(&ctx); {
            // set the name of this mode
            // this is the primary name used to refer to this mode
            // in subsequent calls
            flagship_name(&ctx, "some-mode");
            // subsequent calls will give this mode an alias
            flagship_name(&ctx, "some-mode-alias");

            // set the description of the mode
            flagship_description(&ctx, "this is a mode");

            // create a new flag
            flagship_begin_flag(&ctx, FLAGSHIP_TYPE_BOOL); {
                // set name of flag
                flagship_name(&ctx, "some-flag");

                // set description of flag
                flagship_description(&ctx, "this is a flag");

                // finish this flag
                flagship_end_flag(&ctx);
            }

            // you can also go back to an existing flag/mode to edit it.
            // always uses the first name assigned to the flag, or null if it's nameless
            flagship_begin_flag_existing(&ctx, FLAGSHIP_TYPE_BOOL, "some-flag"); {
                // remember to end
                flagship_end_flag(&ctx);
            }

            // finish this mode
            flagship_end_mode(&ctx);
        }

        // parse flags
        if(!flagship_parse(&ctx, argc, argv, true)) {
            return 1;
        }

        // iterate through all parsed modes
        struct FlagshipResult r;
        while(flagship_iter_next(&ctx, &r)) {
            printf(
                "mode: %s, flag: %s\n",
                flagship_result_mode(&ctx, &r), flagship_result_name(&ctx, &r));
        }
        // iterator can be reset
        flagship_iter_reset(&ctx);

        // you can also just read flags directly
        struct FlagshipResult result = {0};
        if(flagship_search(&ctx, "mode", "flag", &result)) {
            // do something with result
        } // false means that flag doesn't exist in schema

        // end frees all memory and resets the context
        // only call this when you're done working with flags!
        flagship_end(&ctx);
    }

    return 0;
}

```

