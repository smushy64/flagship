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

enum Mode {
    M_HELP,
    M_BUILD,
    M_RUN,
    M_PKG,
    M_INIT,

    M_COUNT
};
const char *MODE_NAMES[] = {
    "help", "build", "run", "package", "init"
};

enum BuildStep {
    B_ALL,
    B_PROJECT,
    B_VENDOR,
    B_RESOURCES,

    B_COUNT
};
const char *BUILD_STEP_NAMES[] = {
    "all", "project", "vendor", "resources"
};

enum BuildStepField {
    BF_ALL       = 0xFFFFFFFF,
    BF_PROJECT   = (1 << B_PROJECT),
    BF_VENDOR    = (1 << B_VENDOR),
    BF_RESOURCES = (1 << B_RESOURCES)
};

enum Target {
    T_NATIVE,
    T_WINDOWS,
    T_LINUX,
    T_MACOS,
    T_ANDROID,
    T_IOS,
    T_WASM,

    T_COUNT
};
const char *TARGET_NAMES[] = {
    "native", "windows", "linux", "macos", "android", "ios", "wasm"
};
enum Target target_native(void);

void flagship_define(struct FlagshipContext *ctx, const char *proc_name);

void mode_build(struct FlagshipContext *ctx);
void mode_run(struct FlagshipContext *ctx);
void mode_package(struct FlagshipContext *ctx);
void mode_init(struct FlagshipContext *ctx);

int    ARGC = 0;
char **ARGV = 0;

int main(int argc, char **argv) {
    ARGC = argc;
    ARGV = argv;

    // create context.
    struct FlagshipContext ctx = {0};

    // begin
    flagship_begin(&ctx);

    // define flags
    flagship_define(&ctx, argv[0]);

    // parse
    if(!flagship_parse(&ctx, argc, argv, true)) {
        // failed to parse flags!
        // error printed and help displayed
        return 1;
    }

    // use flags
    enum Mode mode = 0;
    const char *mode_name = "";
    if(!flagship_get_last_mode(&ctx, (int *)&mode, &mode_name)) {
        mode_name = "(none)";
    }

    if(mode == M_HELP) {
        struct FlagshipResult result = {0};
        if(flagship_search(&ctx, "help", NULL, &result)) {
            flagship_help_print(&ctx, MODE_NAMES[result.t_enum], result.t_enum == (int)M_HELP);
        }
        return 0;
    }

    if(flagship_search(&ctx, mode_name, "help", NULL)) {
        flagship_help_print(&ctx, mode_name, mode == M_HELP);
        return 0;
    }

    printf("mode: %s\n", mode_name);

    switch(mode) {
        case M_HELP:  break;
        case M_BUILD:
            mode_build(&ctx);
            break;
        case M_RUN:
            mode_run(&ctx);
            break;
        case M_PKG:
            mode_package(&ctx);
            break;
        case M_INIT:
            mode_init(&ctx);
            break;

        case M_COUNT:
            break;
    }

    // end
    flagship_end(&ctx);
}

void read_rebuild(struct FlagshipContext *ctx, const char *mode) {
    enum BuildStepField bf = 0;
    unsigned int n = 0, c = 0, len = 0;
    struct FlagshipResult r, *rs, *rss;

    if(flagship_search_repeatable(ctx, mode, "rebuild", &n, &rs)) {
        c += n;
    }

    if(flagship_search_repeatable(ctx, mode, "no-rebuild", &n, &rs)) {
        c += n;
    }

    rss = malloc(sizeof(struct FlagshipResult) * c);

    if(flagship_search_repeatable(ctx, mode, "rebuild", &n, &rs)) {
        memcpy(rss, rs, sizeof(*rss) * n);
        len += n;
    }
    if(flagship_search_repeatable(ctx, mode, "no-rebuild", &n, &rs)) {
        memcpy(rss + len, rs, sizeof(*rss) * n);
        len += n;
    }

    flagship_sort(len, rss);

    for(unsigned int i = 0; i < len; ++i) {
        r = *(rss + i);
        if(strcmp(flagship_result_name(ctx, &r), "rebuild") == 0) {
            if(r.t_enum) {
                bf |= (1 << r.t_enum);
            } else {
                bf = BF_ALL;
            }
        } else {
            if(r.t_enum) {
                bf &= ~(1 << r.t_enum);
            } else {
                bf = ~BF_ALL;
            }
        }
    }

    free(rss);

    printf("  rebuild:\n");
    if((bf & BF_PROJECT) == BF_PROJECT) {
        printf("    project\n");
    }
    if((bf & BF_VENDOR) == BF_VENDOR) {
        printf("    vendor\n");
    }
    if((bf & BF_RESOURCES) == BF_RESOURCES) {
        printf("    resources\n");
    }
}

void mode_build(struct FlagshipContext *ctx) {
    struct FlagshipResult r;

    if(flagship_search(ctx, "build", "target", &r)) {
        printf("  target:        %s\n", TARGET_NAMES[(int)r.t_enum]);
    }

    bool optimized = flagship_search(ctx, "build", "optimized", NULL);
    printf("  optimized:     %s\n", optimized ? "true" : "false");

    bool strip_symbols = flagship_search(ctx, "build", "strip-symbols", NULL);
    printf("  strip-symbols: %s\n", strip_symbols ? "true" : "false");

    read_rebuild(ctx, "build");
}
void mode_run(struct FlagshipContext *ctx) {
    struct FlagshipResult r;

    bool optimized = flagship_search(ctx, "run", "optimized", NULL);
    printf("  optimized:     %s\n", optimized ? "true" : "false");

    bool strip_symbols = flagship_search(ctx, "run", "strip-symbols", NULL);
    printf("  strip-symbols: %s\n", strip_symbols ? "true" : "false");

    read_rebuild(ctx, "run");

    if(flagship_search(ctx, "run", "resources", &r)) {
        printf("  resources: %s\n", r.t_string ? r.t_string : "");
    }

    if(flagship_search(ctx, "run", "-", &r)) {
        int i = flagship_end_position(ctx);
        if(i < ARGC) {
            printf("  args:\n    ");
        }
        for(; i < ARGC; ++i) {
            printf("%s", ARGV[i]);
            if((i + 1) < ARGC) {
                printf(" ");
            } else {
                printf("\n");
            }
        }
    }
}
void mode_package(struct FlagshipContext *ctx) {
    struct FlagshipResult r;

    if(flagship_search(ctx, "package", "target", &r)) {
        printf("  target: %s\n", TARGET_NAMES[r.t_enum]);
    }
}
void mode_init(struct FlagshipContext *ctx) {
    struct FlagshipResult r;

    if(flagship_search(ctx, "init", "target", &r)) {
        printf("  target: %s\n", TARGET_NAMES[r.t_enum]);
    }
}


void flagship_define(struct FlagshipContext *ctx, const char *proc_name) {
    flagship_name(ctx, "%s", proc_name);
    flagship_description(ctx, "example program styled after a build system");

    flagship_begin_mode(ctx); {
        flagship_name(ctx, "%s", MODE_NAMES[(int)M_HELP]);
        flagship_description(ctx, "print help message and exit");

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_ENUM); {
            for(enum Mode m = 0; m < M_COUNT; ++m) {
                flagship_enum_variant(ctx, "%s", MODE_NAMES[(int)m]);
            }

            flagship_description(ctx, "print help for given mode");
            flagship_default(ctx, "%s", MODE_NAMES[(int)M_HELP]);
            flagship_is_terminating(ctx, true);
            flagship_end_flag(ctx);
        }

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_BOOL); {
            flagship_name(ctx, "help");
            flagship_name(ctx, "h");

            flagship_description(ctx, "print help and exit");

            flagship_is_terminating(ctx, true);
            flagship_end_flag(ctx);
        }

        flagship_end_mode(ctx);
    }

    flagship_begin_mode(ctx); {
        flagship_name(ctx, "%s", MODE_NAMES[(int)M_BUILD]);
        flagship_description(ctx, "build project");

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_ENUM); {
            flagship_name(ctx, "target");
            flagship_name(ctx, "t");

            flagship_description(ctx, "set target of build");

            for(enum Target t = 0; t < T_COUNT; ++t) {
                flagship_enum_variant(ctx, "%s", TARGET_NAMES[(int)t]);
            }

            flagship_note(ctx, "native is: %s", TARGET_NAMES[(int)target_native()]);

            flagship_default(ctx, "%s", TARGET_NAMES[(int)T_NATIVE]);
            flagship_end_flag(ctx);
        }

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_BOOL); {
            flagship_name(ctx, "optimized");
            flagship_name(ctx, "o");

            flagship_description(ctx, "if project should be compiled with optimizations on");

            flagship_end_flag(ctx);
        }

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_BOOL); {
            flagship_name(ctx, "strip-symbols");
            flagship_name(ctx, "s");

            flagship_description(ctx, "if project should be compiled without debug symbols");

            flagship_end_flag(ctx);
        }

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_ENUM); {
            flagship_name(ctx, "rebuild");
            flagship_name(ctx, "r");

            flagship_description(ctx, "always rebuild given build step");

            for(enum BuildStep b = 0; b < B_COUNT; ++b) {
                flagship_enum_variant(ctx, "%s", BUILD_STEP_NAMES[(int)b]);
            }

            flagship_default(ctx, "%s", BUILD_STEP_NAMES[(int)B_PROJECT]);
            flagship_note(ctx, "last instance of -rebuild or -no-rebuild takes precedence");
            flagship_is_repeatable(ctx, true);

            flagship_end_flag(ctx);
        }

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_ENUM); {
            flagship_name(ctx, "no-rebuild");
            flagship_name(ctx, "nr");

            flagship_description(ctx, "skip rebuilding given build step");

            for(enum BuildStep b = 0; b < B_COUNT; ++b) {
                flagship_enum_variant(ctx, "%s", BUILD_STEP_NAMES[(int)b]);
            }

            flagship_note(ctx, "last instance of -rebuild or -no-rebuild takes precedence");
            flagship_is_repeatable(ctx, true);

            flagship_end_flag(ctx);
        }

        flagship_copy(ctx, "help", "help");

        flagship_end_mode(ctx);
    }
    flagship_begin_mode(ctx); {
        flagship_name(ctx, "%s", MODE_NAMES[(int)M_RUN]);
        flagship_description(ctx, "build and run project");

        flagship_copy(ctx, "build", "optimized");
        flagship_copy(ctx, "build", "strip-symbols");
        flagship_copy(ctx, "build", "rebuild");
        flagship_copy(ctx, "build", "no-rebuild");

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_STRING); {
            flagship_name(ctx, "resources");
            flagship_description(ctx, "replace default resources location with custom one");
            flagship_default(ctx, "./resources");

            flagship_end_flag(ctx);
        }

        flagship_copy(ctx, "build", "help");

        flagship_begin_flag(ctx, FLAGSHIP_TYPE_BOOL); {
            flagship_name(ctx, "-");
            flagship_description(ctx, "pass remaining flags to project");
            flagship_is_terminating(ctx, true);
            flagship_end_flag(ctx);
        }

        flagship_end_mode(ctx);
    }
    flagship_begin_mode(ctx); {
        flagship_name(ctx, "%s", MODE_NAMES[(int)M_PKG]);
        flagship_description(ctx, "build in release mode and package for distribution");

        flagship_copy(ctx, "build", "target");
        flagship_copy(ctx, "build", "help");

        flagship_end_mode(ctx);
    }
    flagship_begin_mode(ctx); {
        flagship_name(ctx, "%s", MODE_NAMES[(int)M_INIT]);
        flagship_description(ctx, "generate required files for working on project");

        flagship_copy(ctx, "build", "target");
        flagship_copy(ctx, "build", "help");

        flagship_end_mode(ctx);
    }
}


enum Target target_native(void) {
    // in a real program, this would change based on the native platform that
    // program is being compiled on
    return T_LINUX;
}


