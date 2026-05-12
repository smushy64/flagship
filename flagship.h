#if !defined(FLAGSHIP_H)
#define FLAGSHIP_H
/**
 * @file   flagship.h
 * @brief  Flagship.
 * @author Alicia D. Amarilla (smushyaa@gmail.com)
 * @date   May 09, 2026
*/
#include <stddef.h>  // size_t, NULL
#include <stdbool.h> // bool, true, false
#include <stdlib.h>  // calloc, realloc, free
#include <string.h>  // memset, memcpy, strstr, strcmp
#include <stdarg.h>  // va_list, va_start, va_end, va_copy
#include <stdio.h>   // vsnprintf, fprintf
#include <assert.h>  // assert()

#undef FLAGSHIP_FMTFUNC
#undef FLAGSHIP_INLINE
#if __cplusplus
    #define STRUCT(x, ...)  x{__VA_ARGS__}
    #define FLAGSHIP_INLINE inline
#else
    #define STRUCT(x, ...)  (struct x){__VA_ARGS__}
    #define FLAGSHIP_INLINE static inline
#endif

#if __GNUC__
    #define FLAGSHIP_FMTFUNC(string_index, first_to_check) \
        __attribute__((format (printf, string_index, first_to_check)))
#else
    #define FLAGSHIP_FMTFUNC(...)
#endif

/// @brief Flagship Data Types.
enum FlagshipType {
    /// @brief No data type - Triggers a panic when used, only useful for internal debugging.
    FLAGSHIP_TYPE_NULL,
    /// @brief Boolean flag.
    FLAGSHIP_TYPE_BOOL,
    /// @brief Integer.
    FLAGSHIP_TYPE_INTEGER,
    /// @brief Float.
    FLAGSHIP_TYPE_FLOAT,
    /// @brief String.
    FLAGSHIP_TYPE_STRING,
    /// @brief Enumerator.
    FLAGSHIP_TYPE_ENUM,

    /// @brief Last item in enum.
    FLAGSHIP_TYPE_LAST = FLAGSHIP_TYPE_ENUM
};

/// @brief Flagship String. Dereference into const char* using flagship_deref().
typedef unsigned int FlagshipString;

/// @brief Flagship streaming function.
/// @details
/// Useful for redirecting flagship_help_print() calls.
/// @param[in] target Pointer to target to receive bytes.
/// @param     count  Number of bytes being streamed.
/// @param[in] bytes  Bytes being streamed.
/// @return Number of bytes that could not be streamed to target.
typedef size_t FlagshipStreamFn(void *target, size_t count, const void *bytes);

/// @brief Flagship generic allocator.
/// @note Flagship expects that allocated memory is zero-initialized.
/// @param     size          Number of bytes to allocate.
/// @param[in] allocator_ctx Pointer to allocator context.
/// @return Pointer to allocated memory or NULL if failed to allocate.
typedef void *FlagshipAllocatorAllocFn(size_t size, void *allocator_ctx);
/// @brief Flagship generic allocator realloc.
/// @note Flagship expects that allocated memory is zero-initialized.
/// @note This function is optional. If it's missing, a combination of alloc and free are used.
/// @param[in] ptr           Pointer to memory to reallocate.
/// @param     old_size      Number of bytes memory currently holds.
/// @param     new_size      Number of bytes to reallocate to. Must be > old_size.
/// @param[in] allocator_ctx Pointer to allocator context.
/// @return Pointer to reallocated memory or NULL if failed to reallocate.
typedef void *FlagshipAllocatorReallocFn(
    void *ptr, size_t old_size, size_t new_size, void *allocator_ctx);
/// @brief Flagship generic allocator free.
/// @param[in] ptr           Pointer to memory to free.
/// @param     size          Number of bytes memory holds.
/// @param[in] allocator_ctx Pointer to allocator context.
typedef void  FlagshipAllocatorFreeFn(void *ptr, size_t size, void *allocator_ctx);

/// @brief Flagship Generic Allocator.
struct FlagshipAllocator {
    /// @brief Allocate function.
    FlagshipAllocatorAllocFn   *alloc;
    /// @brief Reallocate function. Optional.
    FlagshipAllocatorReallocFn *realloc;
    /// @brief Free function.
    FlagshipAllocatorFreeFn    *free;
    /// @brief Pointer to allocator context.
    void                       *ctx;
};

/// @brief Flagship Context. Do not edit directly.
struct FlagshipContext;

/// @brief Result of argument parsing.
struct FlagshipResult {
    /// @brief Union of values.
    union {
        /// @brief Boolean value.
        bool        t_bool;
        /// @brief Integer value.
        int         t_integer;
        /// @brief Float value.
        float       t_float;
        /// @brief String value.
        const char *t_string;
        /// @brief Enum value.
        int         t_enum;
        struct {
            int             t_enum;
            FlagshipString *variants;
        } __enum;
    };
    /// @brief Type of this flag.
    enum FlagshipType type;

    /// @brief If this flag was actually set.
    bool         was_set  : 1;
    /// @brief Position of this flag. Only valid if was_set is true.
    unsigned int position : (sizeof(int) * 8) - 1;
};

/// @brief Begin defining flagship flags.
/// @param[in] ctx Pointer to context.
FLAGSHIP_INLINE
void flagship_begin(struct FlagshipContext *ctx);

/// @brief Begin defining flagship flags.
/// @param[in] ctx       Pointer to context.
/// @param[in] allocator Pointer to allocator to use.
FLAGSHIP_INLINE
void flagship_begin_with_allocator(
    struct FlagshipContext *ctx, struct FlagshipAllocator *allocator);

/// @brief Begin a new mode.
/// @param[in] ctx Pointer to context.
FLAGSHIP_INLINE
void flagship_begin_mode(struct FlagshipContext *ctx);

/// @brief Search for existing mode and modify it.
/// @warning This will panic if mode doesn't already exist.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Name of mode to search for - format string.
/// @param[in] va  Variadic format string arguments.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
void flagship_begin_mode_existing_search_va(
    struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Search for existing mode and modify it.
/// @warning This will panic if mode doesn't already exist.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Name of mode to search for - format string.
/// @param     ... Format string arguments.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
void flagship_begin_mode_existing_search(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Modify existing mode.
/// @warning This will panic if mode doesn't actually exist.
/// @param[in] ctx  Pointer to context.
/// @param     name Name 'token' of mode to modify.
FLAGSHIP_INLINE
void flagship_begin_mode_existing(struct FlagshipContext *ctx, FlagshipString name);

/// @brief Stop modifying the current mode.
/// @param[in] ctx Pointer to context.
FLAGSHIP_INLINE
void flagship_end_mode(struct FlagshipContext *ctx);

/// @brief Begin a new flag.
/// @param[in] ctx  Pointer to context.
/// @param     type Type of flag to create.
FLAGSHIP_INLINE
void flagship_begin_flag(struct FlagshipContext *ctx, enum FlagshipType type);

/// @brief Search for existing flag and modify it.
/// @warning This will panic if flag doesn't already exist.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Name of flag to search for - format string.
/// @param[in] va  Variadic format string arguments.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
void flagship_begin_flag_existing_search_va(
    struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Search for existing flag and modify it.
/// @warning This will panic if flag doesn't already exist.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Name of flag to search for - format string.
/// @param     ... Format string arguments.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
void flagship_begin_flag_existing_search(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Modify existing flag.
/// @warning This will panic if flag doesn't actually exist.
/// @param[in] ctx  Pointer to context.
/// @param     name Name 'token' of flag to modify.
FLAGSHIP_INLINE
void flagship_begin_flag_existing(struct FlagshipContext *ctx, FlagshipString name);

/// @brief Stop modifying the current flag.
/// @param[in] ctx Pointer to context.
FLAGSHIP_INLINE
void flagship_end_flag(struct FlagshipContext *ctx);

/// @brief Set name or add an alias.
/// @details
/// If after flagship_begin, sets the name of program.
/// If after flagship_begin_flag, sets the name of the current flag or adds an alias.
/// If after flagship_begin_mode, sets the name of the current mode or adds an alias.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Name to add. This can be a format string.
/// @param[in] va  Variadic arguments to format string.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_name_va(struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Set name or add an alias.
/// @details
/// If after flagship_begin, sets the name of program.
/// If after flagship_begin_flag, sets the name of the current flag or adds an alias.
/// If after flagship_begin_mode, sets the name of the current mode or adds an alias.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Name to add. This can be a format string.
/// @param     ... Arguments to format string.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_name(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Set description.
/// @details
/// If after flagship_begin, sets the description of program.
/// If after flagship_begin_flag, sets the description of the current flag.
/// If after flagship_begin_mode, sets the description of the current mode.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Description. This can be a format string.
/// @param[in] va  Variadic arguments to format string.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_description_va(struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Set description.
/// @details
/// If after flagship_begin, sets the name of program.
/// If after flagship_begin_flag, sets the name of the current flag or adds an alias.
/// If after flagship_begin_mode, sets the name of the current mode or adds an alias.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Description. This can be a format string.
/// @param     ... Arguments to format string.
/// @brief String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_description(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Set note.
/// @details
/// If after flagship_begin, sets the note of program.
/// If after flagship_begin_flag, sets the note of the current flag.
/// If after flagship_begin_mode, sets the note of the current mode.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Description. This can be a format string.
/// @param[in] va  Variadic arguments to format string.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_note_va(struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Set note.
/// @details
/// If after flagship_begin, sets the name of program.
/// If after flagship_begin_flag, sets the name of the current flag or adds an alias.
/// If after flagship_begin_mode, sets the name of the current mode or adds an alias.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Description. This can be a format string.
/// @param     ... Arguments to format string.
/// @brief String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_note(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Set warning.
/// @details
/// If after flagship_begin, sets the warning of program.
/// If after flagship_begin_flag, sets the warning of the current flag.
/// If after flagship_begin_mode, sets the warning of the current mode.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Description. This can be a format string.
/// @param[in] va  Variadic arguments to format string.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_warning_va(struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Set warning.
/// @details
/// If after flagship_begin, sets the name of program.
/// If after flagship_begin_flag, sets the name of the current flag or adds an alias.
/// If after flagship_begin_mode, sets the name of the current mode or adds an alias.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Description. This can be a format string.
/// @param     ... Arguments to format string.
/// @brief String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_warning(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Set default value.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt String representation of default value. This can be a format string.
/// @param[in] va  Variadic format string arguments.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_default_va(struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Set default value.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt String representation of default value. This can be a format string.
/// @param     ... Format string arguments.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_default(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Set if flag or mode is unique.
/// @param[in] ctx   Pointer to context.
/// @param     value Whether flag is unique or not.
FLAGSHIP_INLINE
void flagship_is_unique(struct FlagshipContext *ctx, bool value);

/// @brief Set if flag or mode stops flag parsing.
/// @param[in] ctx   Pointer to context.
/// @param     value Whether flag stops flag parsing or not.
FLAGSHIP_INLINE
void flagship_is_terminating(struct FlagshipContext *ctx, bool value);

/// @brief Set if flag is required.
/// @note Must be between flagship_begin_flag and flagship_end_flag
/// @param[in] ctx   Pointer to context.
/// @param     value Whether flag is required or not.
FLAGSHIP_INLINE
void flagship_is_required(struct FlagshipContext *ctx, bool value);

/// @brief Set if boolean flag is flipped.
/// @param[in] ctx   Pointer to context.
/// @param     value Whether flag is flipped or not.
FLAGSHIP_INLINE
void flagship_bool_is_flipped(struct FlagshipContext *ctx, bool value);

/// @brief Set if boolean flag is toggle-able.
/// @param[in] ctx   Pointer to context.
/// @param     value Whether flag is toggle-able or not.
FLAGSHIP_INLINE
void flagship_bool_is_toggle(struct FlagshipContext *ctx, bool value);

/// @brief Set valid range for integer flag.
/// @param[in] ctx           Pointer to context.
/// @param     min_inclusive Minimum value (inclusive).
/// @param     max_inclusive Maximum value (exclusive).
FLAGSHIP_INLINE
void flagship_integer_range(struct FlagshipContext *ctx, int min_inclusive, int max_exclusive);

/// @brief Set valid range for float flag.
/// @param[in] ctx           Pointer to context.
/// @param     min_inclusive Minimum value (inclusive).
/// @param     max_inclusive Maximum value (exclusive).
FLAGSHIP_INLINE
void flagship_float_range(struct FlagshipContext *ctx, float min_inclusive, float max_exclusive);

/// @brief Add valid string for string flag.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Format string.
/// @param[in] va  Variadic arguments for format string.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_string_valid_va(struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Add valid string for string flag.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Format string.
/// @param     ... Arguments for format string.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_string_valid(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Set start to enum variant counter.
/// @param[in] ctx   Pointer to context.
/// @param     start Start integer.
FLAGSHIP_INLINE
void flagship_enum_variant_start(struct FlagshipContext *ctx, int start);

/// @brief Add enum variant to enum flag.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Format string.
/// @param[in] va  Variadic format string arguments.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_enum_variant_va(struct FlagshipContext *ctx, const char *fmt, va_list va);

/// @brief Add enum variant to enum flag.
/// @param[in] ctx Pointer to context.
/// @param[in] fmt Format string.
/// @param     ... Format string arguments.
/// @return String token.
FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_enum_variant(struct FlagshipContext *ctx, const char *fmt, ...);

/// @brief Copy flag to current flag.
/// @param[in] ctx  Pointer to context.
/// @param     mode String token of mode name.
/// @param     name String token of flag name.
FLAGSHIP_INLINE
void flagship_copy(struct FlagshipContext *ctx, FlagshipString mode, FlagshipString name);

/// @brief Clean up flagship context.
/// @warning Do not call any flagship functions after this!
/// @param[in] ctx Pointer to context.
FLAGSHIP_INLINE
void flagship_end(struct FlagshipContext *ctx);

/// @brief Stream generated help message.
/// @param[in] ctx    Pointer to context.
/// @param[in] mode   Mode to print help for.
/// @param[in] stream (nullable) Streaming function.
/// @param[in] target (nullable) Target to streaming function.
/// @return Number of bytes that could not be streamed.
FLAGSHIP_INLINE
size_t flagship_help_stream(
    struct FlagshipContext *ctx, const char *mode, FlagshipStreamFn *stream, void *target);

/// @brief Print help message to stdout.
/// @param[in] ctx  Pointer to context.
/// @param[in] mode Name of mode to print help for.
FLAGSHIP_INLINE
void flagship_help_print(struct FlagshipContext *ctx, const char *mode);

FLAGSHIP_INLINE
bool flagship_parse_streaming_errors(
    struct FlagshipContext *ctx, int argc, char** argv,
    FlagshipStreamFn *stream, void *error_target, void *opt_help_target);

FLAGSHIP_INLINE
bool flagship_parse(struct FlagshipContext *ctx, int argc, char** argv, bool print_help);

FLAGSHIP_INLINE
bool flagship_mode_iter_next(struct FlagshipContext *ctx, int *out_mode);

FLAGSHIP_INLINE
void flagship_mode_iter_reset(struct FlagshipContext *ctx);

FLAGSHIP_INLINE
bool flagship_flag_iter_next(struct FlagshipContext *ctx, struct FlagshipResult *out_result);

FLAGSHIP_INLINE
void flagship_flag_iter_reset(struct FlagshipContext *ctx);

FLAGSHIP_INLINE
bool flagship_read(
    struct FlagshipContext *ctx, struct FlagshipResult *out_result,
    FlagshipString mode, FlagshipString flag);

FLAGSHIP_INLINE
bool flagship_search(
    struct FlagshipContext *ctx, struct FlagshipResult *out_result,
    const char *mode, const char *flag);

FLAGSHIP_INLINE
int flagship_last_flag(struct FlagshipContext *ctx);

FLAGSHIP_INLINE
const char *flagship_deref(struct FlagshipContext *ctx, FlagshipString string);

/// @brief Convert Flagship type to string.
/// @param t Flagship type.
/// @return Read-only string or NULL if type is invalid.
FLAGSHIP_INLINE
const char* string_from_flagship_type(enum FlagshipType t);
/// @brief Convert string to Flagship type.
/// @param[in]  str    String to convert to flagship type.
/// @param[out] endptr (optional) Pointer to write pointer to end of valid string.
/// @return Flagship type or FLAGSHIP_TYPE_NULL if string is invalid.
FLAGSHIP_INLINE
enum FlagshipType flagship_type_from_string(const char* str, const char** endptr);

// NOTE(alicia): internal -

struct FlagshipEnumVariant {
    FlagshipString name;
    int            value;
};

struct FlagshipFlag {
    enum FlagshipType type;

    struct {
        unsigned int    cap;
        unsigned int    len;
        FlagshipString *ptr;
    } names;

    FlagshipString description, note, warning;

    union {
        bool           t_bool_default;
        int            t_integer_default;
        float          t_float_default;
        int            t_enum_default;
        FlagshipString t_string_default;
    };

    bool is_unique      : 1;
    bool is_required    : 1;
    bool is_terminating : 1;
    bool has_default    : 1;

    union {
        struct {
            bool is_flipped : 1;
            bool is_toggle  : 1;
        } s_bool;
        struct {
            int min, max;
        } s_integer;
        struct {
            float min, max;
        } s_float;
        struct {
            unsigned int    cap;
            unsigned int    len;
            FlagshipString *ptr;
        } s_string;
        struct {
            unsigned int                cap;
            unsigned int                len;
            struct FlagshipEnumVariant *ptr;
            int counter;
        } s_enum;
    };
};

struct FlagshipMode {
    struct {
        unsigned int         cap;
        unsigned int         len;
        struct FlagshipFlag *ptr;
    } flags;

    struct {
        unsigned int    cap;
        unsigned int    len;
        FlagshipString *ptr;
    } names;

    FlagshipString description, note, warning;

    bool is_unique      : 1;
    bool is_terminating : 1;
};

struct FlagshipContext {
    struct FlagshipAllocator allocator;

    struct FlagshipStringBuffer {
        unsigned int cap;
        unsigned int len;
        char        *ptr;
    } tmp, str;

    struct {
        unsigned int         cap;
        unsigned int         len;
        struct FlagshipMode *ptr;
    } modes;

    FlagshipString name, description;

    struct {
        int mode, flag;
    } current;
};

#define ALLOC(sz) \
    ctx->allocator.alloc(sz, ctx->allocator.ctx)
#define REALLOC(p, osz, nsz) \
    __flagship_realloc_(&ctx->allocator, p, osz, nsz)
#define FREE(p, sz) \
    ctx->allocator.free(p, sz, ctx->allocator.ctx)

#define RESERVE(b, min) do { \
    if(((b)->cap - (b)->len) < (min)) { \
        void *__new_ptr = \
            REALLOC((b)->ptr, sizeof((b)->ptr[0]) * (b)->cap, \
            sizeof((b)->ptr[0]) * (((b)->cap + ((min) - ((b)->cap - (b)->len))) + 16)); \
        (b)->ptr = __new_ptr; \
        (b)->cap = ((b)->cap + ((min) - ((b)->cap - (b)->len))) + 16; \
    } \
} while(0)

#define ASSERT(condition, fmt, ...) do { \
    if(!(condition)) { \
        fprintf(stderr, "FLAGSHIP: condtion '" #condition "' failed! " fmt "\n" __VA_OPT__(,) __VA_ARGS__ ); \
        assert(false); \
    } \
} while(0)

void *__flagship_realloc_(struct FlagshipAllocator *a, void *p, size_t osz, size_t nsz) {
    if(a->realloc) {
        return a->realloc(p, osz, nsz, a->ctx);
    } else {
        void *new_mem = a->alloc(nsz, a->ctx);
        if(!new_mem) {
            return NULL;
        }
        return memcpy(new_mem, p, osz);
    }
}

const char* __FLAGSHIP_TYPE_STRINGS[] = {
    "null",
    "bool",
    "int",
    "float",
    "string",
    "enum",
};

FLAGSHIP_INLINE
const char* string_from_flagship_type(enum FlagshipType t) {
    if((t < 0) || (t > FLAGSHIP_TYPE_LAST)) {
        return NULL;
    }
    return __FLAGSHIP_TYPE_STRINGS[t];
}
FLAGSHIP_INLINE
enum FlagshipType flagship_type_from_string(const char* str, const char** endptr) {
    for(enum FlagshipType t = 1; t < (FLAGSHIP_TYPE_LAST + 1); ++t) {
        const char* tstr   = __FLAGSHIP_TYPE_STRINGS[t];
        const char* needle = strstr(str, tstr);

        if(needle && (needle == str)) {
            if(endptr) {
                *endptr = str + strlen(tstr);
            }
            return t;
        }
    }
    return FLAGSHIP_TYPE_NULL;
}

FLAGSHIP_INLINE
void *__flagship_default_alloc(size_t size, void *params) {
    (void)params;
    return calloc(1, size);
}
FLAGSHIP_INLINE
void *__flagship_default_realloc(void *ptr, size_t old_size, size_t new_size, void *params) {
    (void)params;
    void *result = realloc(ptr, new_size);
    memset((char*)result + old_size, 0, new_size - old_size);
    return result;
}
FLAGSHIP_INLINE
void __flagship_default_free(void *ptr, size_t size, void *params) {
    (void)size, (void)params;
    free(ptr);
}

FLAGSHIP_INLINE
FlagshipString __flagship_fmt_tmp_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    va_list va2;
    va_copy(va2, va);

    int required = vsnprintf(NULL, 0, fmt, va2);

    va_end(va2);

    RESERVE(&ctx->tmp, required + 1);

    FlagshipString result = ctx->tmp.len;

    vsnprintf(ctx->tmp.ptr + ctx->tmp.len, required + 1, fmt, va);
    ctx->tmp.len += required + 1;

    return result;
}
FLAGSHIP_INLINE
FlagshipString __flagship_fmt_tmp(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = __flagship_fmt_tmp_va(ctx, fmt, va);
    va_end(va);

    return result;
}

FLAGSHIP_INLINE
FlagshipString __flagship_fmt_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    va_list va2;
    va_copy(va2, va);

    int required = vsnprintf(NULL, 0, fmt, va2);

    va_end(va2);

    RESERVE(&ctx->str, required + 1);

    FlagshipString result = ctx->str.len;

    vsnprintf(ctx->str.ptr + ctx->str.len, required + 1, fmt, va);
    ctx->str.len += required + 1;

    return result;
}
FLAGSHIP_INLINE
FlagshipString __flagship_fmt(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = __flagship_fmt_va(ctx, fmt, va);
    va_end(va);

    return result;
}
FLAGSHIP_INLINE
struct FlagshipMode *__flagship_get_current_mode(struct FlagshipContext *ctx) {
    if(ctx->current.mode < 0) {
        if(ctx->modes.len) {
            // search for 'null' mode
            for(int i = 0; i < ctx->modes.len; ++i) {
                struct FlagshipMode *mode = ctx->modes.ptr + i;
                if(!mode->names.len || !mode->names.ptr[0]) {
                    return mode;
                }
            }
            RESERVE(&ctx->modes, 1);

            return ctx->modes.ptr + ctx->modes.len++;
        } else {
            RESERVE(&ctx->modes, 1);

            return ctx->modes.ptr + ctx->modes.len++;
        }
    } else {
        return ctx->modes.ptr + ctx->current.mode;
    }
}

FLAGSHIP_INLINE
struct FlagshipFlag *__flagship_get_current_flag(struct FlagshipContext *ctx) {
    if(ctx->current.flag < 0) {
        return NULL;
    }

    struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
    return mode->flags.ptr + ctx->current.flag;
}



FLAGSHIP_INLINE
void flagship_begin(struct FlagshipContext *ctx) {
    flagship_begin_with_allocator(ctx, NULL);
}

FLAGSHIP_INLINE
void flagship_begin_with_allocator(
    struct FlagshipContext *ctx, struct FlagshipAllocator *allocator
) {
    memset(ctx, 0, sizeof(*ctx));

    if(allocator) {
        ctx->allocator = *allocator;
    } else {
        ctx->allocator.alloc   = __flagship_default_alloc;
        ctx->allocator.realloc = __flagship_default_realloc;
        ctx->allocator.free    = __flagship_default_free;
    }

    RESERVE(&ctx->tmp, 32);
    // first offset is null and should never be used.
    RESERVE(&ctx->str, 33);
    ctx->str.len = 1;

    ctx->current.mode = ctx->current.flag = -1;
}

FLAGSHIP_INLINE
void flagship_begin_mode(struct FlagshipContext *ctx) {
    ASSERT(ctx->current.mode < 0, "called begin mode again without matching end mode!");

    RESERVE(&ctx->modes, 1);
    ctx->current.mode = ctx->modes.len++;

    memset(ctx->modes.ptr + ctx->current.mode, 0, sizeof(struct FlagshipMode));
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
void flagship_begin_mode_existing_search_va(
    struct FlagshipContext *ctx, const char *fmt, va_list va
) {
    ASSERT(ctx->current.mode < 0, "called begin mode again without matching end mode!");

    ctx->tmp.len = 0;
    const char *search_string = NULL;
    if(fmt) {
        search_string = ctx->tmp.ptr + __flagship_fmt_tmp_va(ctx, fmt, va);
    }

    for(unsigned int i = 0; i < ctx->modes.len; ++i) {
        struct FlagshipMode *mode = ctx->modes.ptr + i;

        if(search_string) {
            if(mode->names.len && mode->names.ptr[0]) {
                const char *mode_name = flagship_deref(ctx, mode->names.ptr[0]);

                if(strcmp(mode_name, search_string) == 0) {
                    ctx->current.mode = i;
                    return;
                }
            }
        } else {
            if(!mode->names.len || !mode->names.ptr[0]) {
                ctx->current.mode = i;
                return;
            }
        }
    }

    ASSERT(false, "failed to find mode '%s'!", search_string);
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
void flagship_begin_mode_existing_search(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    flagship_begin_mode_existing_search_va(ctx, fmt, va);
    va_end(va);
}

FLAGSHIP_INLINE
void flagship_begin_mode_existing(struct FlagshipContext *ctx, FlagshipString name) {
    ASSERT(ctx->current.mode < 0, "called begin mode again without matching end mode!");

    for(unsigned int i = 0; i < ctx->modes.len; ++i) {
        struct FlagshipMode *mode = ctx->modes.ptr + i;

        if(name) {
            if(mode->names.len && name == mode->names.ptr[0]) {
                ctx->current.mode = i;
                return;
            }
        } else {
            if(!mode->names.len || !mode->names.ptr[0]) {
                ctx->current.mode = i;
                return;
            }
        }
    }

    ASSERT(false, "failed to find mode %d!\n", name);
}

FLAGSHIP_INLINE
void flagship_end_mode(struct FlagshipContext *ctx) {
    ASSERT(ctx->current.mode >= 0, "called end mode without matching begin mode!");
    ctx->current.mode = -1;
}

FLAGSHIP_INLINE
void flagship_begin_flag(struct FlagshipContext *ctx, enum FlagshipType type) {
    ASSERT(ctx->current.flag < 0, "called begin flag without matching end flag!");
    struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

    RESERVE(&mode->flags, 1);
    ctx->current.flag = mode->flags.len++;

    memset(mode->flags.ptr + ctx->current.flag, 0, sizeof(struct FlagshipFlag));

    mode->flags.ptr[ctx->current.flag].type = type;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
void flagship_begin_flag_existing_search_va(
    struct FlagshipContext *ctx, const char *fmt, va_list va
) {
    ASSERT(ctx->current.flag < 0, "called begin flag without matching end flag!");

    ctx->tmp.len = 0;
    const char *search_string = NULL;
    if(fmt) {
        search_string = ctx->tmp.ptr + __flagship_fmt_tmp_va(ctx, fmt, va);
    }

    struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

    for(unsigned int i = 0; i < mode->flags.len; ++i) {
        struct FlagshipFlag *flag = mode->flags.ptr + i;

        if(search_string) {
            if(flag->names.len && flag->names.ptr[0]) {
                const char *flag_name = flagship_deref(ctx, flag->names.ptr[0]);

                if(strcmp(flag_name, search_string) == 0) {
                    ctx->current.flag = i;
                    return;
                }
            }
        } else {
            if(!flag->names.len || !flag->names.ptr[0]) {
                ctx->current.flag = i;
                return;
            }
        }
    }

    ASSERT(false, "failed to find flag '%s'!\n", search_string);

}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
void flagship_begin_flag_existing_search(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    flagship_begin_flag_existing_search_va(ctx, fmt, va);
    va_end(va);
}

FLAGSHIP_INLINE
void flagship_begin_flag_existing(struct FlagshipContext *ctx, FlagshipString name) {
    ASSERT(ctx->current.flag < 0, "called begin flag without matching end flag!");

    struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

    for(unsigned int i = 0; i < mode->flags.len; ++i) {
        struct FlagshipFlag *flag = mode->flags.ptr + i;

        if(name) {
            if(flag->names.len && name == flag->names.ptr[0]) {
                ctx->current.flag = i;
                return;
            }
        } else {
            if(!flag->names.len || !flag->names.ptr[0]) {
                ctx->current.flag = i;
                return;
            }
        }
    }

    ASSERT(false, "failed to find flag %d!\n", name);

}

FLAGSHIP_INLINE
void flagship_end_flag(struct FlagshipContext *ctx) {
    ASSERT(ctx->current.flag >= 0, "called end flag without matching begin flag!");
    ctx->current.flag = -1;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_name_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    FlagshipString fstr = __flagship_fmt_va(ctx, fmt, va);

    if(ctx->current.mode >= 0) {
        struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

        if(ctx->current.flag >= 0) {
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            RESERVE(&flag->names, 1);
            flag->names.ptr[flag->names.len++] = fstr;
        } else {
            RESERVE(&mode->names, 1);
            mode->names.ptr[mode->names.len++] = fstr;
        }
    } else {
        if(ctx->current.flag >= 0) {
            struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            RESERVE(&flag->names, 1);
            flag->names.ptr[flag->names.len++] = fstr;
        } else {
            ctx->name = fstr;
        }
    }

    return fstr;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_name(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = flagship_name_va(ctx, fmt, va);
    va_end(va);
    return result;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_description_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    FlagshipString fstr = __flagship_fmt_va(ctx, fmt, va);

    if(ctx->current.mode >= 0) {
        struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

        if(ctx->current.flag >= 0) {
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            flag->description = fstr;
        } else {
            mode->description = fstr;
        }
    } else {
        if(ctx->current.flag >= 0) {
            struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            flag->description = fstr;
        } else {
            ctx->description = fstr;
        }
    }

    return fstr;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_description(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = flagship_description_va(ctx, fmt, va);
    va_end(va);
    return result;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_note_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    FlagshipString result = 0;
    if(ctx->current.mode >= 0) {
        struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

        if(ctx->current.flag >= 0) {
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            result = flag->note = __flagship_fmt_va(ctx, fmt, va);
        }
    } else {
        if(ctx->current.flag >= 0) {
            struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            result = flag->note = __flagship_fmt_va(ctx, fmt, va);
        } else {
            ASSERT(false, "notes cannot be attached to context!");
        }
    }

    return result;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_note(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = flagship_note_va(ctx, fmt, va);
    va_end(va);
    return result;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_warning_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    FlagshipString result = 0;
    if(ctx->current.mode >= 0) {
        struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

        if(ctx->current.flag >= 0) {
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            result = flag->warning = __flagship_fmt_va(ctx, fmt, va);
        }
    } else {
        if(ctx->current.flag >= 0) {
            struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            result = flag->warning = __flagship_fmt_va(ctx, fmt, va);
        } else {
            ASSERT(false, "warnings cannot be attached to context!");
        }
    }

    return result;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_warning(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = flagship_warning_va(ctx, fmt, va);
    va_end(va);
    return result;
}

FLAGSHIP_INLINE
bool __flagship_parse_bool(struct FlagshipFlag *flag, const char *str, bool *out_value) {
    bool success = false;
    bool value   = flag->s_bool.is_flipped ? true : false;

    size_t len = strlen(str);
    switch(len) {
        case sizeof("1") - 1: {
            if(str[0] == '1') {
                success = true;
                value   = true;
            } else if(str[0] == '0') {
                success = true;
                value   = false;
            }
        } break;
        case sizeof("true") - 1: {
            const char *comp = "true";
            success = true;

            while(*str && *comp) {
                char ch = *str;
                if(ch >= 'A' && ch <= 'Z') {
                    ch = 'a' + (ch - 'A');
                }
                if(ch != *comp) {
                    success = false;
                    break;
                }
                str++; comp++;
            }

            if(success) {
                value = true;
            }
        } break;
        case sizeof("false") - 1: {
            const char *comp = "false";
            success = true;

            while(*str && *comp) {
                char ch = *str;
                if(ch >= 'A' && ch <= 'Z') {
                    ch = 'a' + (ch - 'A');
                }
                if(ch != *comp) {
                    success = false;
                    break;
                }
                str++; comp++;
            }

            if(success) {
                value = true;
            }
        } break;
    }

    if(success) {
        *out_value = value;
    }

    return success;
}
FLAGSHIP_INLINE
bool __flagship_parse_integer(struct FlagshipFlag *flag, const char *str, int *out_value) {
    char* endptr = NULL;
    long  result = strtol(str, &endptr, 10);

    if(endptr && (endptr == (str + strlen(str)))) {
        bool success = true;
        if(flag->s_integer.min != flag->s_integer.max) {
            if(!((result >= flag->s_integer.min) && (result < flag->s_integer.max))) {
                success = false;
            }
        }

        if(success) {
            *out_value = result;
        }
        return success;
    }

    return false;
}
FLAGSHIP_INLINE
bool __flagship_parse_float(struct FlagshipFlag *flag, const char *str, float *out_value) {
    char*  endptr = NULL;
    double result = strtod(str, &endptr);

    if(endptr && (endptr == (str + strlen(str)))) {
        bool success = true;
        if(flag->s_float.min != flag->s_float.max) {
            if(!((result >= flag->s_float.min) && (result < flag->s_float.max))) {
                success = false;
            }
        }

        if(success) {
            *out_value = result;
        }
        return success;
    }

    return false;
}
FLAGSHIP_INLINE
bool __flagship_parse_string(
    struct FlagshipContext *ctx, struct FlagshipFlag *flag,
    const char *str, const char **out_value
) {
    if(flag->s_string.len) {
        bool success = false;
        for(unsigned int i = 0; i < flag->s_string.len; ++i) {
            const char *comp = flagship_deref(ctx, flag->s_string.ptr[i]);

            if(strcmp(str, comp) == 0) {
                success = true;
                break;
            }
        }

        if(success) {
            *out_value = str;
        }
        return success;
    } else {
        *out_value = str;
        return true;
    }
}
FLAGSHIP_INLINE
bool __flagship_parse_enum(
    struct FlagshipContext *ctx, struct FlagshipFlag *flag,
    const char *str, int *out_value
) {
    ASSERT(flag->s_enum.len, "enum must be well defined before it's used!");
    char *endptr = NULL;
    long value   = strtol(str, &endptr, 10);

    bool success = false;

    if(endptr && (endptr == (str + strlen(str)))) {
        for(unsigned int i = 0; i < flag->s_enum.len; ++i) {
            struct FlagshipEnumVariant *variant = flag->s_enum.ptr + i;

            if(variant->value == value) {
                success = true;
                break;
            }
        }
    } else {
        for(unsigned int i = 0; i < flag->s_enum.len; ++i) {
            struct FlagshipEnumVariant *variant = flag->s_enum.ptr + i;

            const char *variant_name = flagship_deref(ctx, variant->name);

            if(strcmp(str, variant_name) == 0) {
                success = true;
                value   = i;
                break;
            }
        }
    }


    if(success) {
        *out_value = value;
    }
    return success;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_default_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    ASSERT(ctx->current.flag >= 0, "cannot set default value for mode or context!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    ctx->tmp.len = 0;
    const char *default_value = ctx->tmp.ptr + __flagship_fmt_tmp_va(ctx, fmt, va);

    FlagshipString result = 0;

    // parse value
    switch(flag->type) {
        case FLAGSHIP_TYPE_NULL    :
            break;
        case FLAGSHIP_TYPE_BOOL    : {
            bool value = false;
            ASSERT(
                __flagship_parse_bool(flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default    = true;
            flag->t_bool_default = value;
        } break;
        case FLAGSHIP_TYPE_INTEGER : {
            int value = 0;
            ASSERT(
                __flagship_parse_integer(flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default       = true;
            flag->t_integer_default = value;
        } break; 
        case FLAGSHIP_TYPE_FLOAT   : {
            float value = 0.0f;
            ASSERT(
                __flagship_parse_float(flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default     = true;
            flag->t_float_default = value;
        } break;
        case FLAGSHIP_TYPE_STRING  : {
            const char *value = "";
            ASSERT(
                __flagship_parse_string(ctx, flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default      = true;
            flag->t_string_default = __flagship_fmt(ctx, "%s", value);
        } break;
        case FLAGSHIP_TYPE_ENUM    : {
            int value = 0;
            ASSERT(
                __flagship_parse_enum(ctx, flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default    = true;
            flag->t_enum_default = value;
        } break;
    }

    return __flagship_fmt(ctx, "%s", default_value);
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_default(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = flagship_default_va(ctx, fmt, va);
    va_end(va);
    return result;
}

FLAGSHIP_INLINE
void flagship_is_unique(struct FlagshipContext *ctx, bool value) {
    struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
    if(ctx->current.flag >= 0) {
        struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

        flag->is_unique = value;
    } else {
        mode->is_unique = value;
    }
}

FLAGSHIP_INLINE
void flagship_is_terminating(struct FlagshipContext *ctx, bool value) {
    struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
    if(ctx->current.flag >= 0) {
        struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

        flag->is_terminating = value;
    } else {
        mode->is_terminating = value;
    }
}

FLAGSHIP_INLINE
void flagship_is_required(struct FlagshipContext *ctx, bool value) {
    ASSERT(ctx->current.flag >= 0, "attempted to set is_required for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);
    flag->is_required = value;
}

FLAGSHIP_INLINE
void flagship_bool_is_flipped(struct FlagshipContext *ctx, bool value) {
    ASSERT(ctx->current.flag >= 0, "attempted to set bool_is_flipped for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    ASSERT(flag->type == FLAGSHIP_TYPE_BOOL, "attempted to set bool option on non-bool flag!");

    flag->s_bool.is_flipped = value;
}

FLAGSHIP_INLINE
void flagship_bool_is_toggle(struct FlagshipContext *ctx, bool value) {
    ASSERT(ctx->current.flag >= 0, "attempted to set bool_is_toggle for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    ASSERT(flag->type == FLAGSHIP_TYPE_BOOL, "attempted to set bool option on non-bool flag!");

    flag->s_bool.is_toggle = value;
}

FLAGSHIP_INLINE
void flagship_integer_range(struct FlagshipContext *ctx, int min_inclusive, int max_exclusive) {
    ASSERT(max_exclusive >= min_inclusive, "invalid range!");
    ASSERT(ctx->current.flag >= 0, "attempted to set integer_range for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    ASSERT(flag->type == FLAGSHIP_TYPE_INTEGER,
        "attempted to set integer option on non-integer flag!");

    flag->s_integer.min = min_inclusive;
    flag->s_integer.max = max_exclusive;
}

FLAGSHIP_INLINE
void flagship_float_range(struct FlagshipContext *ctx, float min_inclusive, float max_exclusive) {
    ASSERT(max_exclusive >= min_inclusive, "invalid range!");
    ASSERT(ctx->current.flag >= 0, "attempted to set float_range for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    ASSERT(flag->type == FLAGSHIP_TYPE_FLOAT,
        "attempted to set float option on non-float flag!");

    flag->s_float.min = min_inclusive;
    flag->s_float.max = max_exclusive;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_string_valid_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    ASSERT(ctx->current.flag >= 0, "attempted to set string_valid for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    ASSERT(flag->type == FLAGSHIP_TYPE_STRING,
        "attempted to set string option on non-string flag!");

    RESERVE(&flag->s_string, 1);

    FlagshipString result =
        flag->s_string.ptr[flag->s_string.len++] =
        __flagship_fmt_va(ctx, fmt, va);

    return result;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_string_valid(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = flagship_string_valid_va(ctx, fmt, va);
    va_end(va);
    return result;
}

FLAGSHIP_INLINE
void flagship_enum_variant_start(struct FlagshipContext *ctx, int start) {
    ASSERT(ctx->current.flag >= 0, "attempted to set enum_variant_start for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    ASSERT(flag->type == FLAGSHIP_TYPE_ENUM,
        "attempted to set enum option on non-enum flag!");

    flag->s_enum.counter = start;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_enum_variant_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    ASSERT(ctx->current.flag >= 0, "attempted to set enum_variant for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    ASSERT(flag->type == FLAGSHIP_TYPE_ENUM,
        "attempted to set enum option on non-enum flag!");

    RESERVE(&flag->s_enum, 1);

    struct FlagshipEnumVariant variant = {0};
    variant.name  = __flagship_fmt_va(ctx, fmt, va);
    variant.value = flag->s_enum.counter++;

    flag->s_enum.ptr[flag->s_enum.len++] = variant;

    return variant.name;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 3)
FlagshipString flagship_enum_variant(struct FlagshipContext *ctx, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result = flagship_enum_variant_va(ctx, fmt, va);
    va_end(va);
    return result;
}

FLAGSHIP_INLINE
void flagship_copy(struct FlagshipContext *ctx, FlagshipString mode_name, FlagshipString flag_name) {
    ASSERT(ctx->current.flag >= 0, "attempted to copy flag outside flag!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    bool did_copy = false;
    for(unsigned int i = 0; i < ctx->modes.len; ++i) {
        struct FlagshipMode *current_mode = ctx->modes.ptr + i;

        bool found_mode = false;
        if(mode_name) {
            const char *mode_name_str = ctx->str.ptr + mode_name;
            if(current_mode->names.len) {
                const char *current_mode_name_str = ctx->str.ptr + current_mode->names.ptr[0];

                if(strcmp(mode_name_str, current_mode_name_str) == 0) {
                    found_mode = true;
                }
            }
        } else {
            if(!current_mode->names.len || !current_mode->names.ptr[0]) {
                found_mode = true;
            }
        }

        if(!found_mode) {
            continue;
        }

        for(unsigned int j = 0; j < current_mode->flags.len; ++j) {
            struct FlagshipFlag *current_flag = current_mode    ->flags.ptr + j;

            bool found_flag = false;
            if(flag_name) {
                const char *flag_name_str = ctx->str.ptr + flag_name;
                if(current_flag->names.len) {
                    const char *current_flag_name_str = ctx->str.ptr + current_flag->names.ptr[0];

                    if(strcmp(flag_name_str, current_flag_name_str) == 0) {
                        found_flag = true;
                    }
                }
            } else {
                if(!current_flag->names.len || !current_flag->names.ptr[0]) {
                    found_flag = true;
                }
            }

            if(!found_flag) {
                continue;
            }

            did_copy = true;

            // NOTE(alicia): this has to be a deep copy to avoid double free later.
            // should probably avoid deep copy and only do a deep copy if this flag's
            // buffers need to be modified but... deep copy is easier
            *flag = *current_flag;

            memset(&flag->names, 0, (sizeof(int) * 2) + sizeof(void*));

            RESERVE(&flag->names, current_flag->names.len);
            for(unsigned int k = 0; k < current_flag->names.len; ++k) {
                flag->names.ptr[flag->names.len++] = current_flag->names.ptr[k];
            }

            switch(flag->type) {
                case FLAGSHIP_TYPE_STRING: {
                    if(current_flag->s_string.ptr) {
                        memset(&flag->s_string, 0, (sizeof(int) * 2) + sizeof(void*));

                        RESERVE(&flag->s_string, current_flag->s_string.len);
                        for(unsigned int k = 0; k < current_flag->s_string.len; ++k) {
                            flag->s_string.ptr[flag->s_string.len++] =
                                current_flag->s_string.ptr[k];
                        }
                    }
                } break;
                case FLAGSHIP_TYPE_ENUM: {
                    if(current_flag->s_enum.ptr) {
                        memset(&flag->s_enum, 0, (sizeof(int) * 2) + sizeof(void*));

                        RESERVE(&flag->s_enum, current_flag->s_enum.len);
                        for(unsigned int k = 0; k < current_flag->s_enum.len; ++k) {
                            flag->s_enum.ptr[flag->s_enum.len++] =
                                current_flag->s_enum.ptr[k];
                        }
                    }
                } break;
                case FLAGSHIP_TYPE_NULL:
                case FLAGSHIP_TYPE_BOOL:
                case FLAGSHIP_TYPE_INTEGER:
                case FLAGSHIP_TYPE_FLOAT:
                  break;
            }
        }
    }

    ASSERT(did_copy, "failed to find flag to copy!");
}

FLAGSHIP_INLINE
void flagship_end(struct FlagshipContext *ctx) {
    if(ctx->tmp.ptr) {
        ctx->allocator.free(ctx->tmp.ptr, ctx->tmp.cap, ctx->allocator.ctx);
    }
    if(ctx->str.ptr) {
        ctx->allocator.free(ctx->str.ptr, ctx->str.cap, ctx->allocator.ctx);
    }

    if(ctx->modes.ptr) {
        for(unsigned int i = 0; i < ctx->modes.len; ++i) {
            struct FlagshipMode *mode = ctx->modes.ptr + i;

            if(mode->names.ptr) {
                ctx->allocator.free(
                    mode->names.ptr,
                    sizeof(FlagshipString) * mode->names.cap,
                    ctx->allocator.ctx);
            }

            if(mode->flags.ptr) {
                for(unsigned int j = 0; j < mode->flags.len; ++j) {
                    struct FlagshipFlag *flag = mode->flags.ptr + j;

                    if(flag->names.ptr) {
                        ctx->allocator.free(
                            flag->names.ptr, sizeof(FlagshipString) * flag->names.cap,
                            ctx->allocator.ctx);
                    }

                    switch(flag->type) {
                        case FLAGSHIP_TYPE_STRING: {
                            if(flag->s_string.ptr) {
                                ctx->allocator.free(
                                    flag->s_string.ptr,
                                    sizeof(FlagshipString) * flag->s_string.cap,
                                    ctx->allocator.ctx);
                            }
                        } break;
                        case FLAGSHIP_TYPE_ENUM: {
                            if(flag->s_enum.ptr) {
                                ctx->allocator.free(
                                    flag->s_enum.ptr,
                                    sizeof(struct FlagshipEnumVariant) * flag->s_enum.cap,
                                    ctx->allocator.ctx);
                            }
                        } break;

                        case FLAGSHIP_TYPE_NULL:
                        case FLAGSHIP_TYPE_BOOL:
                        case FLAGSHIP_TYPE_INTEGER:
                        case FLAGSHIP_TYPE_FLOAT:
                            break;
                    }
                }

                ctx->allocator.free(
                    mode->flags.ptr,
                    sizeof(struct FlagshipFlag) * mode->flags.cap,
                    ctx->allocator.ctx);
            }
        }

        ctx->allocator.free(
            ctx->modes.ptr,
            sizeof(struct FlagshipMode) * ctx->modes.cap,
            ctx->allocator.ctx);
    }

    // reset context
    memset(ctx, 0, sizeof(*ctx));
}

FLAGSHIP_INLINE
const char *flagship_deref(struct FlagshipContext *ctx, FlagshipString string) {
    ASSERT((string) && (string < ctx->str.len), "attempted to dereference invalid string!");
    return ctx->str.ptr + string;
}

FLAGSHIP_INLINE
size_t __flagship_stream_file(void *target, size_t count, const void *bytes) {
    if(target) {
        size_t result = fwrite(bytes, count, 1, (FILE *)target);
        fflush((FILE *)target);
        return result;
    } else {
        return count;
    }
}

FLAGSHIP_INLINE
size_t flagship_help_stream(
    struct FlagshipContext *ctx, const char *mode_name,
    FlagshipStreamFn *stream, void *target
) {
    size_t result = 0;
#define p(fmt, ...) do { \
    ctx->tmp.len = 0; \
    const char *str = ctx->tmp.ptr + __flagship_fmt_tmp(ctx, fmt __VA_OPT__(,) __VA_ARGS__ ); \
    result += stream(target, strlen(str), str); \
} while(0)

    int padding = 0;

    if(ctx->description) {
        padding = padding < sizeof("OVERVIEW:") ? sizeof("OVERVIEW:") : padding;
    }
    if(ctx->name) {
        padding = padding < sizeof("USAGE:") ? sizeof("USAGE:") : padding;
    }

    padding = padding < sizeof("ARGUMENTS:") ? sizeof("ARGUMENTS:") : padding;

    padding++;

    struct FlagshipMode *mode = NULL;
    if(mode_name) {
        for(unsigned int i = 0; i < ctx->modes.len; ++i) {
            struct FlagshipMode *current_mode = ctx->modes.ptr + i;

            if(current_mode->names.ptr && current_mode->names.ptr[0]) {
                const char *current_name = flagship_deref(ctx, current_mode->names.ptr[0]);

                if(strcmp(current_name, mode_name) == 0) {
                    mode = current_mode;
                    break;
                }
            }
        }
    }

    bool should_print_modes = !mode;

    if(!mode) {
        mode = ctx->modes.ptr;
    }

    if(should_print_modes) {
        padding = padding < sizeof("MODES:") ? sizeof("MODES:") : padding;
    }

    if(ctx->description) {
        const char *desc = flagship_deref(ctx, ctx->description);

        p("OVERVIEW: %*s\n", padding, desc);
    }

    if(ctx->name) {
        const char *name = flagship_deref(ctx, ctx->name);

        p("USAGE: %*s ", padding, name);
        if(mode_name && !should_print_modes) {
            p("%s [args...]", mode_name);
        } else {
            p("<mode> [args...]");
        }
        p("\n");
    }

    if(should_print_modes) {
        p("MODES:\n");

        for(unsigned int i = 0; i < ctx->modes.len; ++i) {
            struct FlagshipMode *m = ctx->modes.ptr + i;

            if(m->names.ptr) {
            }
        }
    }

    p("ARGUMENTS:\n");

    return result;
#undef p
}

FLAGSHIP_INLINE
void flagship_help_print(struct FlagshipContext *ctx, const char *mode) {
    flagship_help_stream(ctx, mode, __flagship_stream_file, stdout);
}

FLAGSHIP_INLINE
bool flagship_parse_streaming_errors(
    struct FlagshipContext *ctx, int argc, char** argv,
    FlagshipStreamFn *stream, void *error_target, void *opt_help_target
);

FLAGSHIP_INLINE
bool flagship_parse(struct FlagshipContext *ctx, int argc, char** argv, bool print_help) {
    return flagship_parse_streaming_errors(
        ctx, argc, argv, __flagship_stream_file, stderr, print_help ? stdout : NULL);
}

#undef ALLOC
#undef REALLOC
#undef FREE
#undef RESERVE
#undef FLAGSHIP_INLINE
#undef FLAGSHIP_FMTFUNC
#undef STRUCT
#undef ASSERT

#endif /* header guard */
