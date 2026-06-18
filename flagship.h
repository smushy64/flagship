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

struct FlagshipEnumVariant;

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
    /// @brief Amount of memory allocated.
    size_t                      sz;
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
    };
    /// @brief Type of this flag.
    enum FlagshipType type;

    /// @brief Name of mode and flag.
    /// @note Pointer to string buffers so that
    /// results can be looked up with any mode/flag name alias.
    struct __FlagshipBufferString *mode, *name;

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

/// @brief Set if flag or mode is repeatable.
/// @param[in] ctx   Pointer to context.
/// @param     value Whether flag is repeatable or not.
FLAGSHIP_INLINE
void flagship_is_repeatable(struct FlagshipContext *ctx, bool value);

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

/// @brief Copy flag to current mode.
/// @param[in] ctx  Pointer to context.
/// @param     mode String token of mode name.
/// @param     name String token of flag name.
FLAGSHIP_INLINE
void flagship_copy_by_token(struct FlagshipContext *ctx, FlagshipString mode, FlagshipString name);

/// @brief Copy flag to current mode.
/// @param[in] ctx  Pointer to context.
/// @param     mode Name of mode.
/// @param     name Name of flag.
FLAGSHIP_INLINE
void flagship_copy(struct FlagshipContext *ctx, const char *mode, const char *name);

/// @brief Clean up flagship context.
/// @warning Do not call any flagship functions after this!
/// @param[in] ctx Pointer to context.
FLAGSHIP_INLINE
void flagship_end(struct FlagshipContext *ctx);

/// @brief Check if this schema is modal.
/// @param[in] ctx Pointer to context.
/// @return True if modal, false if not.
FLAGSHIP_INLINE
bool flagship_is_modal(struct FlagshipContext *ctx);

/// @brief Stream generated help message.
/// @param[in] ctx         Pointer to context.
/// @param     print_modes If true, modes will always be printed. Ignored if schema is not modal.
/// @param[in] mode        Mode to print help for.
/// @param[in] stream      (nullable) Streaming function.
/// @param[in] target      (nullable) Target to streaming function.
/// @return Number of bytes that could not be streamed.
FLAGSHIP_INLINE
size_t flagship_help_stream(
    struct FlagshipContext *ctx, const char *mode, bool print_modes,
    FlagshipStreamFn *stream, void *target);

/// @brief Print help message to stdout.
/// @param[in] ctx         Pointer to context.
/// @param[in] mode        Name of mode to print help for.
/// @param     print_modes If true, modes will always be printed. Ignored if schema is not modal.
FLAGSHIP_INLINE
void flagship_help_print(struct FlagshipContext *ctx, const char *mode, bool print_modes);

/// @brief Parse flags.
/// @param[in] ctx          Pointer to context.
/// @param     argc         Argument count.
/// @param[in] argv         Arguments.
/// @param[in] stream       Message streaming function.
/// @param[in] error_target Error streaming target.
/// @param[in] help_target  Help print streaming target.
/// @param     print_help   If help should be printed when an error is encountered.
/// @return True if flags are parsed successfully.
FLAGSHIP_INLINE
bool flagship_parse_streaming_errors(
    struct FlagshipContext *ctx, int argc, char** argv,
    FlagshipStreamFn *stream, void *error_target, void *help_target, bool print_help);

/// @brief Parse flags.
/// @param[in] ctx        Pointer to context.
/// @param     argc       Argument count.
/// @param[in] argv       Arguments.
/// @param     print_help If help should be printed when an error is encountered.
/// @return True if flags are parsed successfully.
FLAGSHIP_INLINE
bool flagship_parse(struct FlagshipContext *ctx, int argc, char** argv, bool print_help);

/// @brief Iterate through parse results.
/// @param[in]  ctx        Pointer to context.
/// @param[out] out_result Pointer to write result.
/// @return True if @c out_result was written to.
FLAGSHIP_INLINE
bool flagship_iter_next(struct FlagshipContext *ctx, struct FlagshipResult *out_result);

/// @brief Reset iterator.
/// @param[in] ctx Pointer to context.
FLAGSHIP_INLINE
void flagship_iter_reset(struct FlagshipContext *ctx);

/// @brief Search for enum by string name.
/// @param[in]  ctx               Pointer to context.
/// @param[in]  mode              Name of mode this enum belongs to.
/// @param[in]  name              Name of enum.
/// @param[out] out_variant_count Pointer to write number of variants.
/// @param[out] out_variants      Pointer to write pointer to variants to.
/// @return
///     - @c true  : Enum was found. Pointer to variants is written to @c out_variants.
///     - @c false : Enum was not found. @c out_variants is NULL.
FLAGSHIP_INLINE
bool flagship_enum_search(
    struct FlagshipContext *ctx, const char *mode, const char *name,
    unsigned int *out_variant_count, struct FlagshipEnumVariant **out_variants);

/// @brief Search for enum by string name.
/// @param[in]  ctx               Pointer to context.
/// @param      mode              Name of mode this enum belongs to.
/// @param      name              Name of enum.
/// @param[out] out_variant_count Pointer to write number of variants.
/// @param[out] out_variants      Pointer to write pointer to variants to.
/// @return
///     - @c true  : Enum was found. Pointer to variants is written to @c out_variants.
///     - @c false : Enum was not found. @c out_variants is NULL.
FLAGSHIP_INLINE
bool flagship_enum_get(
    struct FlagshipContext *ctx, FlagshipString mode, FlagshipString name,
    unsigned int *out_variant_count, struct FlagshipEnumVariant **out_variants);

/// @brief Search for flag by string name.
/// @param[in]  ctx         Pointer to context.
/// @param[in]  mode        Name of mode this flag belongs to.
/// @param[in]  name        Name of flag.
/// @param[out] out_count   Pointer to write number of results found.
/// @param[out] out_results Pointer to write pointer to results to.
/// @return
///     - @c true  : Flag was found in results. Pointer to instances is written to @c out_results.
///     - @c false : Flag was not found in results. @c out_results is NULL.
FLAGSHIP_INLINE
bool flagship_search(
    struct FlagshipContext *ctx, const char *mode, const char *name,
    unsigned int *out_count, struct FlagshipResult **out_results);

/// @brief Search for flag by string token.
/// @param[in]  ctx         Pointer to context.
/// @param      mode        Name of mode this flag belongs to.
/// @param      name        Name of flag.
/// @param[out] out_count   Pointer to write number of results found.
/// @param[out] out_results Pointer to write pointer to results to.
/// @return
///     - @c true  : Flag was found in results. Pointer to instances is written to @c out_results.
///     - @c false : Flag was not found in results. @c out_results is NULL.
FLAGSHIP_INLINE
bool flagship_get(
    struct FlagshipContext *ctx, FlagshipString mode, FlagshipString name,
    unsigned int *out_count, struct FlagshipResult **out_results);

/// @brief Dereference a string token.
/// @param[in] ctx    Pointer to context.
/// @param     string String token.
/// @return String.
FLAGSHIP_INLINE
const char *flagship_deref(struct FlagshipContext *ctx, FlagshipString string);

/// @brief Convert Flagship type to string.
/// @param t Flagship type.
/// @return Read-only string or NULL if type is invalid.
FLAGSHIP_INLINE
const char* flagship_string_from_type(enum FlagshipType t);
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

struct __FlagshipBufferEnum {
    unsigned int                cap;
    unsigned int                len;
    struct FlagshipEnumVariant *ptr;
};

struct __FlagshipBufferChar {
    unsigned int cap;
    unsigned int len;
    char        *ptr;
};

struct __FlagshipBufferString {
    unsigned int    cap;
    unsigned int    len;
    FlagshipString *ptr;
};

struct __FlagshipBufferFlag {
    unsigned int         cap;
    unsigned int         len;
    struct FlagshipFlag *ptr;
};

struct __FlagshipBufferMode {
    unsigned int         cap;
    unsigned int         len;
    struct FlagshipMode *ptr;
};

struct __FlagshipBufferResult {
    unsigned int           cap;
    unsigned int           len;
    struct FlagshipResult *ptr;
};

struct FlagshipFlag {
    enum FlagshipType type;

    struct __FlagshipBufferString names;

    FlagshipString description, note, warning;

    union {
        bool           t_bool_default;
        int            t_integer_default;
        float          t_float_default;
        int            t_enum_default;
        FlagshipString t_string_default;
    };

    bool is_repeatable  : 1;
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
            struct __FlagshipBufferString valid;
        } s_string;
        struct {
            struct __FlagshipBufferEnum variants;
            int counter;
        } s_enum;
    };
};

struct FlagshipMode {
    struct __FlagshipBufferFlag   flags;
    struct __FlagshipBufferString names;

    FlagshipString description, note, warning;

    bool is_repeatable  : 1;
    bool is_terminating : 1;
};

struct FlagshipContext {
    struct FlagshipAllocator allocator;

    struct __FlagshipBufferChar   tmp, str;
    struct __FlagshipBufferMode   modes;
    struct __FlagshipBufferResult results;
    struct __FlagshipBufferResult search;

    FlagshipString name, description;

    struct {
        int mode, flag;
    } current;

    unsigned int iter;
};

#define flagship_reset_cbuf(cbuf) \
    (cbuf)->len = 1

void *__flagship_alloc(struct FlagshipAllocator *a, size_t sz) {
    void *result = a->alloc(sz, a->ctx);

    if(result) {
        a->sz += sz;
    }

    return result;
}

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

void *__flagship_realloc(
    struct FlagshipAllocator *a, void *p, size_t oldsz, size_t newsz
) {
    void *result = NULL;
    if(a->realloc) {
        result = a->realloc(p, oldsz, newsz, a->ctx);
    } else {
        void *new_mem = a->alloc(newsz, a->ctx);
        if(!new_mem) {
            result = NULL;
        }
        result = memcpy(new_mem, p, oldsz);
    }

    if(result) {
        a->sz += newsz - oldsz;
    }

    return result;
}

void __flagship_free(struct FlagshipAllocator *a, void *p, size_t sz) {
    if(p && sz) {
        a->free(p, sz, a->ctx);
        a->sz -= sz;
    }
}

#define flagship_alloc(a, sz) \
    __flagship_alloc((a), (sz))

#define flagship_realloc(a, p, oldsz, newsz) \
    __flagship_realloc((a), (p), (oldsz), (newsz))

#define flagship_free(a, p, sz) \
    __flagship_free((a), (p), (sz))

#define flagship_reserve(a, b, min) \
    __flagship_reserve((a), sizeof((b)->ptr[0]), (b)->len, &(b)->cap, (void **)&(b)->ptr, (min))

#define flagship_assert(condition, fmt, ...) do { \
    if(!(condition)) { \
        fprintf(stderr, "FLAGSHIP: condtion '" #condition "' failed! " fmt "\n" __VA_OPT__(,) __VA_ARGS__ ); \
        assert(false); \
    } \
} while(0)

void __flagship_reserve(
    struct FlagshipAllocator *a,
    size_t stride,
    unsigned int len, unsigned int *out_cap, void **out_ptr,
    unsigned int minimum
) {
    if(*out_ptr) {
        if((*out_cap - len) < minimum) {
            size_t oldsz = stride * *out_cap;

            size_t delta = minimum - (*out_cap - len);
            delta += 16 - (delta % 16);
            delta += 16;

            size_t newsz = stride * (*out_cap + delta);

            *out_ptr = flagship_realloc(a, *out_ptr, oldsz, newsz);
            *out_cap += delta;
        }
    } else {
        size_t sz = minimum;
        sz = sz < 16 ? 16 : sz;
        sz = stride * sz;

        *out_ptr = flagship_alloc(a, sz);
        *out_cap = sz;
    }
}

const char* __FLAGSHIP_TYPE_STRINGS[] = {
    "null",
    "bool",
    "integer",
    "float",
    "string",
    "enum",
};

FLAGSHIP_INLINE
const char* flagship_string_from_type(enum FlagshipType t) {
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
FlagshipString __flagship_internal_format_string_va(
    struct FlagshipAllocator *alloc, struct __FlagshipBufferChar *buffer,
    unsigned int *opt_out_len, const char *fmt, va_list va
) {
    va_list va2;
    va_copy(va2, va);
    unsigned int required_no_null = vsnprintf(NULL, 0, fmt, va2);
    va_end(va2);

    flagship_reserve(alloc, buffer, required_no_null + 1);

    FlagshipString offset = buffer->len;

    vsnprintf(buffer->ptr + buffer->len, required_no_null + 1, fmt, va);
    buffer->len += required_no_null + 1;

    if(opt_out_len) {
        *opt_out_len = required_no_null;
    }

    return offset;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(4, 5)
FlagshipString __flagship_internal_format_string(
    struct FlagshipAllocator *alloc, struct __FlagshipBufferChar *buffer,
    unsigned int *opt_out_len, const char *fmt, ...
) {
    va_list va;
    va_start(va, fmt);
    FlagshipString result =
        __flagship_internal_format_string_va(alloc, buffer, opt_out_len, fmt, va);
    va_end(va);

    return result;
}

FLAGSHIP_INLINE
const char *__flagship_internal_format_string_deref_va(
    struct FlagshipAllocator *alloc, struct __FlagshipBufferChar *buffer,
    unsigned int *opt_out_len, const char *fmt, va_list va
) {
    FlagshipString offset =
        __flagship_internal_format_string_va(alloc, buffer, opt_out_len, fmt, va);
    return buffer->ptr + offset;
}

FLAGSHIP_INLINE
const char *__flagship_internal_format_string_deref(
    struct FlagshipAllocator *alloc, struct __FlagshipBufferChar *buffer,
    unsigned int *opt_out_len, const char *fmt, ...
) {
    va_list va;
    va_start(va, fmt);
    const char *result =
        __flagship_internal_format_string_deref_va(alloc, buffer, opt_out_len, fmt, va);
    va_end(va);
    return result;
}

#define flagship_fmt_va(a, b, l, f, va) \
    __flagship_internal_format_string_va((a), (b), (l), f, va)

#define flagship_fmt(a, b, l, f, ...) \
    __flagship_internal_format_string((a), (b), (l), f __VA_OPT__(,) __VA_ARGS__ )

#define flagship_fmt_deref_va(a, b, l, f, va) \
    __flagship_internal_format_string_deref_va((a), (b), (l), f, va)

#define flagship_fmt_deref(a, b, l, f, ...) \
    __flagship_internal_format_string_deref((a), (b), (l), f __VA_OPT__(,) __VA_ARGS__ )

FLAGSHIP_INLINE
struct FlagshipMode *__flagship_get_current_mode(struct FlagshipContext *ctx) {
    if(ctx->current.mode < 0) {
        if(ctx->modes.len) {
            // search for 'null' mode
            for(unsigned int i = 0; i < ctx->modes.len; ++i) {
                struct FlagshipMode *mode = ctx->modes.ptr + i;
                if(!mode->names.len || !mode->names.ptr[0]) {
                    return mode;
                }
            }
            flagship_reserve(&ctx->allocator, &ctx->modes, 1);

            return ctx->modes.ptr + ctx->modes.len++;
        } else {
            flagship_reserve(&ctx->allocator, &ctx->modes, 1);

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

    // first offset is null and should never be used.
    flagship_reserve(&ctx->allocator, &ctx->tmp, 32);
    flagship_reserve(&ctx->allocator, &ctx->str, 32);
    flagship_reset_cbuf(&ctx->str);
    flagship_reset_cbuf(&ctx->tmp);

    ctx->current.mode = ctx->current.flag = -1;
}

FLAGSHIP_INLINE
void flagship_begin_mode(struct FlagshipContext *ctx) {
    flagship_assert(ctx->current.mode < 0, "called begin mode again without matching end mode!");

    flagship_reserve(&ctx->allocator, &ctx->modes, 1);
    ctx->current.mode = ctx->modes.len++;

    memset(ctx->modes.ptr + ctx->current.mode, 0, sizeof(struct FlagshipMode));
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
void flagship_begin_mode_existing_search_va(
    struct FlagshipContext *ctx, const char *fmt, va_list va
) {
    flagship_assert(ctx->current.mode < 0, "called begin mode again without matching end mode!");

    flagship_reset_cbuf(&ctx->tmp);
    const char *search_string = NULL;
    if(fmt) {
        search_string = flagship_fmt_deref_va(&ctx->allocator, &ctx->tmp, NULL, fmt, va);
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

    flagship_assert(false, "failed to find mode '%s'!", search_string);
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
    flagship_assert(ctx->current.mode < 0, "called begin mode again without matching end mode!");

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

    flagship_assert(false, "failed to find mode %d!\n", name);
}

FLAGSHIP_INLINE
void flagship_end_mode(struct FlagshipContext *ctx) {
    flagship_assert(ctx->current.mode >= 0, "called end mode without matching begin mode!");
    ctx->current.mode = -1;
}

FLAGSHIP_INLINE
void flagship_begin_flag(struct FlagshipContext *ctx, enum FlagshipType type) {
    flagship_assert(ctx->current.flag < 0, "called begin flag without matching end flag!");
    struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

    flagship_reserve(&ctx->allocator, &mode->flags, 1);
    ctx->current.flag = mode->flags.len++;

    memset(mode->flags.ptr + ctx->current.flag, 0, sizeof(struct FlagshipFlag));

    mode->flags.ptr[ctx->current.flag].type = type;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
void flagship_begin_flag_existing_search_va(
    struct FlagshipContext *ctx, const char *fmt, va_list va
) {
    flagship_assert(ctx->current.flag < 0, "called begin flag without matching end flag!");

    flagship_reset_cbuf(&ctx->tmp);
    const char *search_string = NULL;
    if(fmt) {
        search_string = flagship_fmt_deref_va(&ctx->allocator, &ctx->tmp, NULL, fmt, va);
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

    flagship_assert(false, "failed to find flag '%s'!\n", search_string);

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
    flagship_assert(ctx->current.flag < 0, "called begin flag without matching end flag!");

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

    flagship_assert(false, "failed to find flag %d!\n", name);

}

FLAGSHIP_INLINE
void flagship_end_flag(struct FlagshipContext *ctx) {
    flagship_assert(ctx->current.flag >= 0, "called end flag without matching begin flag!");
    ctx->current.flag = -1;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_name_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    FlagshipString fstr = fmt ? flagship_fmt_va(&ctx->allocator, &ctx->str, NULL, fmt, va) : 0;

    if(ctx->current.mode >= 0) {
        struct FlagshipMode *mode = __flagship_get_current_mode(ctx);

        if(ctx->current.flag >= 0) {
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            flagship_reserve(&ctx->allocator, &flag->names, 1);
            flag->names.ptr[flag->names.len++] = fstr;
        } else {
            flagship_reserve(&ctx->allocator, &mode->names, 1);
            mode->names.ptr[mode->names.len++] = fstr;
        }
    } else {
        if(ctx->current.flag >= 0) {
            struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            flagship_reserve(&ctx->allocator, &flag->names, 1);
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
    FlagshipString fstr = flagship_fmt_va(&ctx->allocator, &ctx->str, NULL, fmt, va);

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

        result = flagship_fmt_va(&ctx->allocator, &ctx->str, NULL, fmt, va);
        if(ctx->current.flag >= 0) {
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            flag->note = result;
        } else {
            mode->note = result;
        }
    } else {
        if(ctx->current.flag >= 0) {
            struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            result = flag->note = flagship_fmt_va(&ctx->allocator, &ctx->str, NULL, fmt, va);
        } else {
            flagship_assert(false, "notes cannot be attached to context!");
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

        result = flagship_fmt_va(&ctx->allocator, &ctx->str, NULL, fmt, va);
        if(ctx->current.flag >= 0) {
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            flag->warning = result;
        } else {
            mode->warning = result;
        }
    } else {
        if(ctx->current.flag >= 0) {
            struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
            struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

            result = flag->warning = flagship_fmt_va(&ctx->allocator, &ctx->str, NULL, fmt, va);
        } else {
            flagship_assert(false, "warnings cannot be attached to context!");
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
    if(flag->s_string.valid.len) {
        bool success = false;
        for(unsigned int i = 0; i < flag->s_string.valid.len; ++i) {
            const char *comp = flagship_deref(ctx, flag->s_string.valid.ptr[i]);

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
    flagship_assert(flag->s_enum.variants.len, "enum must be well defined before it's used!");
    char *endptr = NULL;
    long value   = strtol(str, &endptr, 10);

    bool success = false;

    if(endptr && (endptr == (str + strlen(str)))) {
        for(unsigned int i = 0; i < flag->s_enum.variants.len; ++i) {
            struct FlagshipEnumVariant *variant = flag->s_enum.variants.ptr + i;

            if(variant->value == value) {
                success = true;
                break;
            }
        }
    } else {
        for(unsigned int i = 0; i < flag->s_enum.variants.len; ++i) {
            struct FlagshipEnumVariant *variant = flag->s_enum.variants.ptr + i;

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
    flagship_assert(ctx->current.flag >= 0, "cannot set default value for mode or context!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    flagship_reset_cbuf(&ctx->tmp);
    const char *default_value = flagship_fmt_deref_va(&ctx->allocator, &ctx->tmp, NULL, fmt, va);

    // parse value
    switch(flag->type) {
        case FLAGSHIP_TYPE_NULL    :
            break;
        case FLAGSHIP_TYPE_BOOL    : {
            bool value = false;
            flagship_assert(
                __flagship_parse_bool(flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default    = true;
            flag->t_bool_default = value;
        } break;
        case FLAGSHIP_TYPE_INTEGER : {
            int value = 0;
            flagship_assert(
                __flagship_parse_integer(flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default       = true;
            flag->t_integer_default = value;
        } break; 
        case FLAGSHIP_TYPE_FLOAT   : {
            float value = 0.0f;
            flagship_assert(
                __flagship_parse_float(flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default     = true;
            flag->t_float_default = value;
        } break;
        case FLAGSHIP_TYPE_STRING  : {
            const char *value = "";
            flagship_assert(
                __flagship_parse_string(ctx, flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default      = true;
            flag->t_string_default = flagship_fmt(&ctx->allocator, &ctx->str, NULL, "%s", value);
        } break;
        case FLAGSHIP_TYPE_ENUM    : {
            int value = 0;
            flagship_assert(
                __flagship_parse_enum(ctx, flag, default_value, &value),
                "failed to parse default value!");

            flag->has_default    = true;
            flag->t_enum_default = value;
        } break;
    }

    return flagship_fmt(&ctx->allocator, &ctx->str, NULL, "%s", default_value);
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
void flagship_is_repeatable(struct FlagshipContext *ctx, bool value) {
    struct FlagshipMode *mode = __flagship_get_current_mode(ctx);
    if(ctx->current.flag >= 0) {
        struct FlagshipFlag *flag = mode->flags.ptr + ctx->current.flag;

        flag->is_repeatable = value;
    } else {
        mode->is_repeatable = value;
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
    flagship_assert(ctx->current.flag >= 0, "attempted to set is_required for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);
    flag->is_required = value;
}

FLAGSHIP_INLINE
void flagship_bool_is_flipped(struct FlagshipContext *ctx, bool value) {
    flagship_assert(ctx->current.flag >= 0, "attempted to set bool_is_flipped for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    flagship_assert(
        flag->type == FLAGSHIP_TYPE_BOOL, "attempted to set bool option on non-bool flag!");

    flag->s_bool.is_flipped = value;
}

FLAGSHIP_INLINE
void flagship_bool_is_toggle(struct FlagshipContext *ctx, bool value) {
    flagship_assert(ctx->current.flag >= 0, "attempted to set bool_is_toggle for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    flagship_assert(
        flag->type == FLAGSHIP_TYPE_BOOL, "attempted to set bool option on non-bool flag!");

    flag->s_bool.is_toggle = value;
}

FLAGSHIP_INLINE
void flagship_integer_range(struct FlagshipContext *ctx, int min_inclusive, int max_exclusive) {
    flagship_assert(max_exclusive >= min_inclusive, "invalid range!");
    flagship_assert(ctx->current.flag >= 0, "attempted to set integer_range for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    flagship_assert(flag->type == FLAGSHIP_TYPE_INTEGER,
        "attempted to set integer option on non-integer flag!");

    flag->s_integer.min = min_inclusive;
    flag->s_integer.max = max_exclusive;
}

FLAGSHIP_INLINE
void flagship_float_range(struct FlagshipContext *ctx, float min_inclusive, float max_exclusive) {
    flagship_assert(max_exclusive >= min_inclusive, "invalid range!");
    flagship_assert(ctx->current.flag >= 0, "attempted to set float_range for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    flagship_assert(flag->type == FLAGSHIP_TYPE_FLOAT,
        "attempted to set float option on non-float flag!");

    flag->s_float.min = min_inclusive;
    flag->s_float.max = max_exclusive;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_string_valid_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    flagship_assert(ctx->current.flag >= 0, "attempted to set string_valid for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    flagship_assert(flag->type == FLAGSHIP_TYPE_STRING,
        "attempted to set string option on non-string flag!");

    flagship_reserve(&ctx->allocator, &flag->s_string.valid, 1);

    FlagshipString result =
        flag->s_string.valid.ptr[flag->s_string.valid.len++] =
        flagship_fmt_va(&ctx->allocator, &ctx->str, NULL, fmt, va);

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
    flagship_assert(ctx->current.flag >= 0, "attempted to set enum_variant_start for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    flagship_assert(flag->type == FLAGSHIP_TYPE_ENUM,
        "attempted to set enum option on non-enum flag!");

    flag->s_enum.counter = start;
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(2, 0)
FlagshipString flagship_enum_variant_va(struct FlagshipContext *ctx, const char *fmt, va_list va) {
    flagship_assert(ctx->current.flag >= 0, "attempted to set enum_variant for mode!");

    struct FlagshipFlag *flag = __flagship_get_current_flag(ctx);

    flagship_assert(flag->type == FLAGSHIP_TYPE_ENUM,
        "attempted to set enum option on non-enum flag!");

    flagship_reserve(&ctx->allocator, &flag->s_enum.variants, 1);

    struct FlagshipEnumVariant variant = {0};
    variant.name  = flagship_fmt_va(&ctx->allocator, &ctx->str, NULL, fmt, va);
    variant.value = flag->s_enum.counter++;

    flag->s_enum.variants.ptr[flag->s_enum.variants.len++] = variant;

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
void __flagship_copy(struct FlagshipContext *ctx, struct FlagshipFlag *src) {
    flagship_begin_flag(ctx, src->type);

    struct FlagshipFlag *dst = __flagship_get_current_flag(ctx);

    flagship_reserve(&ctx->allocator, &dst->names, src->names.len);
    memcpy(dst->names.ptr, src->names.ptr, sizeof(src->names.ptr[0]) * src->names.len);
    dst->names.len = src->names.len;

    dst->description = src->description;
    dst->note        = src->note;
    dst->warning     = src->warning;

    dst->is_repeatable  = src->is_repeatable;
    dst->is_required    = src->is_required;
    dst->is_terminating = src->is_terminating;
    dst->has_default    = src->has_default;

    switch(src->type) {
        case FLAGSHIP_TYPE_NULL: break;
        case FLAGSHIP_TYPE_BOOL: {
            dst->s_bool = src->s_bool;
            if(src->has_default) {
                dst->t_bool_default = src->t_bool_default;
            }
        } break;
        case FLAGSHIP_TYPE_INTEGER: {
            dst->s_integer = src->s_integer;
            if(src->has_default) {
                dst->t_integer_default = src->t_integer_default;
            }
        } break;
        case FLAGSHIP_TYPE_FLOAT: {
            dst->s_float = src->s_float;
            if(src->has_default) {
                dst->t_float_default = src->t_float_default;
            }
        } break;
        case FLAGSHIP_TYPE_STRING:{
            if(src->s_string.valid.len) {
                flagship_reserve(&ctx->allocator, &dst->s_string.valid, src->s_string.valid.len);
                memcpy(
                    dst->s_string.valid.ptr,
                    src->s_string.valid.ptr,
                    sizeof(src->s_string.valid.ptr[0]) * src->s_string.valid.len);
                dst->s_string.valid.len = src->s_string.valid.len;
            }
            dst->t_string_default = src->t_string_default;
        } break;
        case FLAGSHIP_TYPE_ENUM: {
            dst->s_enum.counter = src->s_enum.counter;
            if(src->s_enum.variants.len) {
                flagship_reserve(&ctx->allocator, &dst->s_enum.variants, src->s_enum.variants.len);
                memcpy(
                    dst->s_enum.variants.ptr,
                    src->s_enum.variants.ptr,
                    sizeof(src->s_enum.variants.ptr[0]) * src->s_enum.variants.len);
                dst->s_enum.variants.len = src->s_enum.variants.len;
            }
            dst->t_enum_default = src->t_enum_default;
        } break;
    }

    flagship_end_flag(ctx);
}

FLAGSHIP_INLINE
void flagship_copy_by_token(
    struct FlagshipContext *ctx, FlagshipString fstr_mode, FlagshipString fstr_name
) {
    for(unsigned int i = 0; i < ctx->modes.len; ++i) {
        struct FlagshipMode *mode = ctx->modes.ptr + i;

        bool mode_found = false;
        if(fstr_mode) {
            if(!mode->names.ptr) {
                continue;
            }

            if(mode->names.ptr[0] == fstr_mode) {
                mode_found = true;
            }
        } else {
            if(!mode->names.ptr || !mode->names.ptr[0]) {
                mode_found = true;
            }
        }

        if(!mode_found) {
            continue;
        }

        for(unsigned int j = 0; j < mode->flags.len; ++j) {
            struct FlagshipFlag *flag = mode->flags.ptr + j;

            bool flag_found = false;
            if(fstr_name) {
                if(!flag->names.ptr) {
                    continue;
                }

                if(flag->names.ptr[0] == fstr_name) {
                    flag_found = true;
                }
            } else {
                if(!flag->names.ptr || !flag->names.ptr[0]) {
                    flag_found = true;
                }
            }

            if(!flag_found) {
                continue;
            }

            __flagship_copy(ctx, flag);
            return;
        }

        break;
    }

    flagship_assert(false, "failed to find flag by tokens %d and %d!", fstr_mode, fstr_name);
}

FLAGSHIP_INLINE
void flagship_copy(
    struct FlagshipContext *ctx, const char *str_mode, const char *str_name
) {
    for(unsigned int i = 0; i < ctx->modes.len; ++i) {
        struct FlagshipMode *mode = ctx->modes.ptr + i;

        bool mode_found = false;
        if(str_mode) {
            if(!mode->names.ptr) {
                continue;
            }

            const char *this_name = flagship_deref(ctx, mode->names.ptr[0]);
            if(strcmp(this_name, str_mode) == 0) {
                mode_found = true;
            }
        } else {
            if(!mode->names.ptr || !mode->names.ptr[0]) {
                mode_found = true;
            }
        }

        if(!mode_found) {
            continue;
        }

        for(unsigned int j = 0; j < mode->flags.len; ++j) {
            struct FlagshipFlag *flag = mode->flags.ptr + j;

            bool flag_found = false;
            if(str_name) {
                if(!flag->names.ptr) {
                    continue;
                }

                const char *this_name = flagship_deref(ctx, flag->names.ptr[0]);
                if(strcmp(this_name, str_name) == 0) {
                    flag_found = true;
                }
            } else {
                if(!flag->names.ptr || !flag->names.ptr[0]) {
                    flag_found = true;
                }
            }

            if(!flag_found) {
                continue;
            }

            __flagship_copy(ctx, flag);
            return;
        }

        break;
    }

    flagship_assert(false, "failed to find flag by names %s and %s!", str_mode, str_name);
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
                            if(flag->s_string.valid.ptr) {
                                ctx->allocator.free(
                                    flag->s_string.valid.ptr,
                                    sizeof(FlagshipString) * flag->s_string.valid.cap,
                                    ctx->allocator.ctx);
                            }
                        } break;
                        case FLAGSHIP_TYPE_ENUM: {
                            if(flag->s_enum.variants.ptr) {
                                ctx->allocator.free(
                                    flag->s_enum.variants.ptr,
                                    sizeof(struct FlagshipEnumVariant) * flag->s_enum.variants.cap,
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

    if(ctx->results.ptr) {
        ctx->allocator.free(
            ctx->results.ptr,
            sizeof(ctx->results.ptr[0]) * ctx->results.cap,
            ctx->allocator.ctx);
    }

    // reset context
    memset(ctx, 0, sizeof(*ctx));
}

FLAGSHIP_INLINE
bool flagship_is_modal(struct FlagshipContext *ctx) {
    if(!ctx->modes.ptr) {
        return false;
    }

    if(ctx->modes.len > 1) {
        return true;
    } else {
        return ctx->modes.ptr[0].names.ptr != NULL;
    }
}

FLAGSHIP_INLINE
const char *flagship_deref(struct FlagshipContext *ctx, FlagshipString string) {
    flagship_assert(
        (string) && (string < ctx->str.len), "attempted to dereference invalid string!");
    return ctx->str.ptr + string;
}

FLAGSHIP_INLINE
size_t __flagship_stream_file(void *target, size_t count, const void *bytes) {
    if(target) {
        FILE *file = (FILE *)target;

        size_t result = fwrite(bytes, count, 1, file);
        fflush((FILE *)target);

        return result;
    } else {
        return count;
    }
}

FLAGSHIP_INLINE
size_t __flagship_stream_columns(
    FlagshipStreamFn *stream, void *target,
    struct FlagshipAllocator *alloc,
    struct __FlagshipBufferChar *_left,
    struct __FlagshipBufferChar *_right
) {
    flagship_reserve(alloc, _left, 1);
    flagship_reserve(alloc, _right, 1);
    _left->ptr[_left->len++]   = 0;
    _right->ptr[_right->len++] = 0;

    size_t result = 0;

    const char *left  = _left->ptr;
    const char *right = _right->ptr;

    size_t longest_line = 0;
    while(*left) {
        const char *nl = strchr(left, '\n');

        size_t line_len = 0;

        if(nl) {
            line_len = nl - left;
        } else {
            line_len = strlen(left);
        }

        if(line_len > longest_line) {
            longest_line = line_len;
        }

        if(nl) {
            left += line_len + 1;
        } else {
            break;
        }
    }

    size_t padding = longest_line + 1;

    left = _left->ptr;

    while(*left || *right) {
        const char *nl = NULL;

        size_t this_padding = padding;
        if(*left) {
            nl = strchr(left, '\n');

            size_t len = 0;
            if(nl) {
                len = nl - left;
            } else {
                len = strlen(left);
            }

            this_padding -= len;

            result += stream(target, len, left);

            left += len;
            if(nl) {
                left++;
            }
        }

        if(*right) {
            for(size_t i = 0; i < this_padding; ++i) {
                char ch = ' ';
                result += stream(target, 1, &ch);
            }

            nl = strchr(right, '\n');

            size_t len = 0;
            if(nl) {
                len = nl - right;
            } else {
                len = strlen(right);
            }

            result += stream(target, len, right);

            right += len;
            if(nl) {
                right++;
            }
        }

        char ch = '\n';
        result += stream(target, 1, &ch);
    }

    _left->len = _right->len = 0;

    return result;
}

FLAGSHIP_INLINE
size_t flagship_help_stream(
    struct FlagshipContext *ctx, const char *mode_name, bool print_modes,
    FlagshipStreamFn *stream, void *target
) {
    size_t result = 0;

    struct __FlagshipBufferChar l, r;
    memset(&l, 0, sizeof(l));
    memset(&r, 0, sizeof(r));

    #define fmt_(b, f, ...) flagship_fmt(&ctx->allocator, (b), NULL, f __VA_OPT__(,) __VA_ARGS__ ); (b)->len--
    #define fmt(b, f, ...)  fmt_(b, f "\n" __VA_OPT__(,) __VA_ARGS__ )
    #define flush()         result += __flagship_stream_columns(stream, target, &ctx->allocator, &l, &r)

    bool is_modal;
    if(!ctx->modes.ptr) {
        return result;
    }

    is_modal = flagship_is_modal(ctx);

    struct FlagshipMode *mode = ctx->modes.ptr;
    if(mode_name) {
        for(unsigned int i = 0; i < ctx->modes.len; ++i) {
            struct FlagshipMode *current_mode = ctx->modes.ptr + i;

            if(!current_mode->names.ptr || !current_mode->names.ptr[0]) {
                continue;
            }

            const char *current_name = flagship_deref(ctx, current_mode->names.ptr[0]);

            if(strcmp(mode_name, current_name) == 0) {
                mode = current_mode;
                break;
            }
        }
    } else {
        for(unsigned int i = 0; i < ctx->modes.len; ++i) {
            struct FlagshipMode *current_mode = ctx->modes.ptr + i;

            if(!current_mode->names.ptr || !current_mode->names.ptr[0]) {
                mode = current_mode;
                break;
            }
        }
    }

    if(ctx->description) {
        fmt(&l, "OVERVIEW:");
        fmt(&r, "%s", flagship_deref(ctx, ctx->description));
    }

    if(ctx->name) {
        fmt(&l, "USAGE:");

        const char *program_name = flagship_deref(ctx, ctx->name);

        if(is_modal) {
            if(mode_name) {
                fmt(&r, "%s %s [args...]", program_name, mode_name);
            } else {
                fmt(&r, "%s <mode> [args...]", program_name);
            }
        } else {
            fmt(&r, "%s [args...]", program_name);
        }
    }

    if(mode->description) {
        fmt(&l, "DESCRIPTION:");
        fmt(&r, "%s", flagship_deref(ctx, mode->description));
    }

    if(is_modal && (!mode_name || print_modes)) {
        fmt(&l, "MODES:");
        flush();

        for(unsigned int i = 0; i < ctx->modes.len; ++i) {
            struct FlagshipMode *mode = ctx->modes.ptr + i;

            if(mode->names.len) {
                for(unsigned int j = 0; j < mode->names.len; ++j) {
                    const char *name  = "";
                    if(mode->names.ptr[j]) {
                        name = flagship_deref(ctx, mode->names.ptr[j]);
                    } else {
                        name = "<no-name>";
                    }

                    const char *pad   = j ? "  " : "";
                    const char *comma = (j + 1) < mode->names.len ? "," : "";
                    fmt(&l, "  %s%s%s ", pad, name, comma);
                }
            } else {
                fmt(&l, "  <no-name> ");
            }

            int attachment_padding = 0;
            if(mode->note || mode->is_terminating) {
                attachment_padding =
                    attachment_padding < (int)sizeof("note") ?
                        (int)sizeof("note") : attachment_padding;
            }
            if(mode->warning) {
                attachment_padding =
                    attachment_padding < (int)sizeof("warning") ?
                        (int)sizeof("warning") : attachment_padding;
            }
            attachment_padding++;

            if(mode->description) {
                const char *d = flagship_deref(ctx, mode->description);
                fmt(&r, "%s", d);
            }

            if(mode->note) {
                const char *n = flagship_deref(ctx, mode->note);
                fmt(&r, "  %*s%s", -attachment_padding, "note:", n);
            }

            if(mode->warning) {
                const char *w = flagship_deref(ctx, mode->warning);
                fmt(&r, "  %*s%s", -attachment_padding, "warning:", w);
            }

            if(mode->is_terminating) {
                fmt(&r, "  %*swhen this mode is encountered, no other modes are parsed", -attachment_padding, "note:");
            }
        }

        flush();
    }

    fmt(&l, "ARGUMENTS:");
    flush();

    fmt(&l, "  to ignore a flag without deleting it from command line, change - to +");
    fmt(&l, "    ex: -help -> +help");
    fmt(&l, "  when a flag takes an argument, argument can be provided as a separate argument");
    fmt(&l, "  or combined with the flag using ':'");
    fmt(&l, "    ex: -output dir/file.txt -> -output:dir/file.txt");
    fmt(&l, "");

    flush();

    for(unsigned int i = 0; i < mode->flags.len; ++i) {
        struct FlagshipFlag *flag = mode->flags.ptr + i;

        const char *type_name = flagship_string_from_type(flag->type);

        int lc, rc;
        lc = rc = 0;

        if(flag->names.len) {
            for(unsigned int j = 0; j < flag->names.len; ++j) {
                const char *pad   = j ? "  " : "";
                const char *comma = (j + 1) < flag->names.len ? "," : "";

                if(flag->names.ptr[j]) {
                    const char *name = flagship_deref(ctx, flag->names.ptr[j]);

                    switch(flag->type) {
                        case FLAGSHIP_TYPE_NULL: break;
                        case FLAGSHIP_TYPE_BOOL: {
                            fmt(&l, "  %s-%s%s ", pad, name, comma);
                            lc++;
                        } break;
                        case FLAGSHIP_TYPE_FLOAT:
                        case FLAGSHIP_TYPE_STRING:
                        case FLAGSHIP_TYPE_ENUM:
                        case FLAGSHIP_TYPE_INTEGER: {
                            fmt(&l, "  %s-%s <%s>%s", pad, name, type_name, comma);
                            lc++;
                        } break;
                      break;
                    }
                } else {
                    fmt(&l, "  %s<%s>%s ", pad, type_name, comma);
                    lc++;
                }
            }
        } else {
            fmt(&l, "  <%s> ", type_name);
            lc++;
        }

        if(flag->description) {
            const char *str = flagship_deref(ctx, flag->description);

            fmt(&r, " %s", str);
            rc++;
        }

        int attachment_padding = 0;
        if(flag->note || flag->is_terminating || flag->is_repeatable) {
            attachment_padding =
                attachment_padding < (int)sizeof("note") ?
                    (int)sizeof("note") : attachment_padding;
        }
        if(flag->warning || flag->is_required) {
            attachment_padding =
                attachment_padding < (int)sizeof("warning") ?
                    (int)sizeof("warning") : attachment_padding;
        }
        if(flag->has_default) {
            attachment_padding =
                attachment_padding < (int)sizeof("default") ?
                    (int)sizeof("default") : attachment_padding;
        }
        switch(flag->type) {
            case FLAGSHIP_TYPE_NULL: break;
            case FLAGSHIP_TYPE_BOOL: {
                if(flag->s_bool.is_toggle) {
                    attachment_padding =
                        attachment_padding < (int)sizeof("note") ?
                            (int)sizeof("note") : attachment_padding;
                }
            } break;
            case FLAGSHIP_TYPE_INTEGER: {
                if(flag->s_integer.min != flag->s_integer.max) {
                    attachment_padding =
                        attachment_padding < (int)sizeof("range") ?
                            (int)sizeof("range") : attachment_padding;
                }
            } break;
            case FLAGSHIP_TYPE_FLOAT: {
                if(flag->s_float.min != flag->s_float.max) {
                    attachment_padding =
                        attachment_padding < (int)sizeof("range") ?
                            (int)sizeof("range") : attachment_padding;
                }
            } break;
            case FLAGSHIP_TYPE_STRING: {
                if(flag->s_string.valid.len) {
                    attachment_padding =
                        attachment_padding < (int)sizeof("valid") ?
                            (int)sizeof("valid") : attachment_padding;
                }
            } break;
            case FLAGSHIP_TYPE_ENUM: {
                attachment_padding =
                    attachment_padding < (int)sizeof("variants") ?
                        (int)sizeof("variants") : attachment_padding;
            } break;
        }
        attachment_padding++;

        bool has_note    = false;
        bool has_warning = false;

        if(flag->note) {
            const char *str = flagship_deref(ctx, flag->note);
            fmt(&r, "   %*s%s",
                -(attachment_padding + (has_note ? 2 : 0)), has_note ? "" : "note:", str);
            rc++;
            has_note = true;
        }

        if(flag->is_terminating) {
            fmt(&r, "   %*sif this flag is encountered, stops parsing all remaining flags",
                -(attachment_padding + (has_note ? 2 : 0)), has_note ? "" : "note:");
            rc++;
            has_note = true;
        }

        if(flag->is_repeatable) {
            fmt(&r, "   %*sthis flag can be used multiple times",
                -(attachment_padding + (has_note ? 2 : 0)), has_note ? "" : "note:");
            rc++;
            has_note = true;
        }

        if(flag->type == FLAGSHIP_TYPE_BOOL) {
            if(flag->s_bool.is_toggle) {
                fmt(&r, "   %*seach occurrence of this flag toggles it",
                    -(attachment_padding + (has_note ? 2 : 0)), has_note ? "" : "note:");
                rc++;
                has_note = true;
            }
        }

        if(flag->warning) {
            const char *str = flagship_deref(ctx, flag->warning);
            fmt(&r, "   %*s%s",
                -(attachment_padding + (has_warning ? 2 : 0)),
                has_warning ? "  " : "warning:", str);
            rc++;
            has_warning = true;
        }

        if(flag->is_required) {
            fmt(&r, "   %*sthis flag is required",
                -(attachment_padding + (has_warning ? 2 : 0)), has_warning ? "" : "warning:");
            rc++;
            has_warning = true;
        }

        switch(flag->type) {
            case FLAGSHIP_TYPE_NULL:
            case FLAGSHIP_TYPE_BOOL: break;
            case FLAGSHIP_TYPE_INTEGER: {
                if(flag->s_integer.min != flag->s_integer.max) {
                    fmt(&r, "   %*s[%d, %d)",
                        -attachment_padding, "range:", flag->s_integer.min, flag->s_integer.max);
                    rc++;
                }
            } break;
            case FLAGSHIP_TYPE_FLOAT: {
                if(flag->s_float.min != flag->s_float.max) {
                    fmt(&r, "   %*s[%f, %f)",
                        -attachment_padding, "range:", flag->s_float.min, flag->s_float.max);
                    rc++;
                }
            } break;
            case FLAGSHIP_TYPE_STRING: {
                if(flag->s_string.valid.len) {
                    fmt_(&r, "   %*s ", -attachment_padding, "valid:");
                    for(unsigned j = 0; j < flag->s_string.valid.len; ++j) {
                        const char *v = "";
                        if(flag->s_string.valid.ptr[j]) {
                            v = flagship_deref(ctx, flag->s_string.valid.ptr[j]);
                        }

                        const char *comma = (j + 1) < flag->s_string.valid.len ? ", " : "";

                        fmt_(&r,  "%s%s", v, comma);
                    }
                    fmt_(&r, "\n");
                    rc++;
                }
            } break;
            case FLAGSHIP_TYPE_ENUM: {
                if(flag->s_enum.variants.len) {
                    fmt_(&r, "   %*s", -attachment_padding, "variants:");
                    int previous_value = 0;
                    for(unsigned j = 0; j < flag->s_enum.variants.len; ++j) {
                        struct FlagshipEnumVariant *variant = flag->s_enum.variants.ptr + j;
                        const char *v = "";
                        if(variant->name) {
                            v = flagship_deref(ctx, variant->name);
                        }

                        const char *comma = (j + 1) < flag->s_string.valid.len ? ", " : "";

                        if((variant->value - previous_value) > 1) {
                            fmt_(&r, "%d: %s%s", variant->value, v, comma);
                        } else {
                            fmt_(&r, "%s%s", v, comma);
                        }
                        previous_value = variant->value;
                    }
                    fmt_(&r, "\n");
                    rc++;
                }
            } break;
        }

        switch(flag->type) {
            case FLAGSHIP_TYPE_NULL: break;
            case FLAGSHIP_TYPE_BOOL: {
                if(flag->has_default) {
                    fmt(&r, "   %*s%s",
                        -attachment_padding, "default:", flag->t_bool_default ? "true" : "false");
                    rc++;
                }
            } break;
            case FLAGSHIP_TYPE_INTEGER: {
                if(flag->has_default) {
                    fmt(&r, "   %*s%d",
                        -attachment_padding, "default:", flag->t_integer_default);
                    rc++;
                }
            } break;
            case FLAGSHIP_TYPE_FLOAT: {
                if(flag->has_default) {
                    fmt(&r, "   %*s%f",
                        -attachment_padding, "default:", flag->t_float_default);
                    rc++;
                }
            } break;
            case FLAGSHIP_TYPE_STRING: {
                if(flag->has_default) {
                    const char *str = "";
                    if(flag->t_string_default) {
                        str = flagship_deref(ctx, flag->t_string_default);
                    }
                    fmt(&r, "   %*s%s",
                        -attachment_padding, "default:", str);
                    rc++;
                }
            } break;
            case FLAGSHIP_TYPE_ENUM: {
                if(flag->has_default) {
                    struct FlagshipEnumVariant *variant = flag->s_enum.variants.ptr + flag->t_enum_default;
                    const char *v = "";
                    if(variant->name) {
                        v = flagship_deref(ctx, variant->name);
                    }
                    fmt(&r, "   %*s%s",
                        -attachment_padding, "default:", v);
                    rc++;
                }
            } break;
        }

        if(lc > rc) {
            unsigned int pc = lc - rc;
            for(unsigned int j = 0; j < pc; ++j) {
                fmt(&r, "");
            }
        } else if(rc > lc) {
            unsigned int pc = rc - lc;
            for(unsigned int j = 0; j < pc; ++j) {
                fmt(&l, "");
            }
        }

        if((i + 1) < mode->flags.len) {
            // separate arguments with an empty line
            fmt(&l, "");
            fmt(&r, "");
        }
    }

    flush();

    if(l.ptr) {
        flagship_free(&ctx->allocator, l.ptr, l.cap);
    }
    if(r.ptr) {
        flagship_free(&ctx->allocator, r.ptr, r.cap);
    }

    return result;
    #undef fmt_
    #undef fmt
    #undef flush
}

FLAGSHIP_INLINE
void flagship_help_print(struct FlagshipContext *ctx, const char *mode, bool print_modes) {
    flagship_help_stream(ctx, mode, print_modes, __flagship_stream_file, stdout);
}

FLAGSHIP_INLINE FLAGSHIP_FMTFUNC(4, 5)
void __flagship_stream_message(
    struct FlagshipContext *ctx, FlagshipStreamFn *stream, void *target, const char *message, ...
) {
    ctx->tmp.len = 0;
    va_list va;
    va_start(va, message);
    unsigned int len = 0;
    flagship_fmt_va(&ctx->allocator, &ctx->tmp, &len, message, va);
    va_end(va);

    stream(target, len, ctx->tmp.ptr);
}

FLAGSHIP_INLINE
int __flagship_result_sort_comp(const void *_lhs, const void *_rhs) {
    const struct FlagshipResult *lhs, *rhs;
    lhs = (const struct FlagshipResult *)_lhs;
    rhs = (const struct FlagshipResult *)_rhs;

    if(lhs->position < rhs->position) {
        return -1;
    } else if(lhs->position > rhs->position) {
        return 1;
    }

    return 0;
}

FLAGSHIP_INLINE
bool flagship_parse_streaming_errors(
    struct FlagshipContext *ctx, int argc, char** argv,
    FlagshipStreamFn *stream, void *error_target, void *help_target,
    bool print_help
) {
    flagship_assert(stream, "NULL streaming function provided!");

    bool is_modal = flagship_is_modal(ctx);
    int  mode     = -1;

    if(!is_modal) {
        mode = 0;
    }

    #define adv() argc--; argv++
    #define err(fmt, ...) \
        __flagship_stream_message( \
            ctx, stream, error_target, "error: " fmt __VA_OPT__(,) __VA_ARGS__ )

    bool success = true;

    int total_argc = argc;

    struct FlagshipResult f;
    memset(&f, 0, sizeof(f));

    // TODO(alicia): add default flags!

    while(argc) {
        if(argc == total_argc) {
            adv();
        }

        const char *arg = NULL;
        if(argc) {
            arg = *argv;
        }

        if(is_modal && (mode < 0)) {
            /* get mode */ {
                if(arg && arg[0] != '-') {
                    for(unsigned int i = 0; i < ctx->modes.len; ++i) {
                        struct FlagshipMode *m = ctx->modes.ptr + i;

                        for(unsigned int j = 0; j < m->names.len; ++j) {
                            if(!m->names.ptr[j]) {
                                continue;
                            }

                            const char *mode_name = flagship_deref(ctx, m->names.ptr[j]);

                            if(strcmp(arg, mode_name) == 0) {
                                mode = i;
                                break;
                            }
                        }

                        if(mode >= 0) {
                            break;
                        }
                    }

                    if(mode >= 0) {
                        adv();
                        continue;
                    }
                }

                if(mode < 0) {
                    bool has_null_mode = false;
                    for(unsigned int i = 0; i < ctx->modes.len; ++i) {
                        struct FlagshipMode *mode = ctx->modes.ptr + i;

                        if(!mode->names.ptr || !mode->names.ptr[0]) {
                            has_null_mode = true;
                            break;
                        }
                    }

                    if(has_null_mode) {
                        mode = 0;
                    } else {
                        success = false;
                        err("no mode was provided!\n");

                        break;
                    }
                }
            }
        }

        struct FlagshipMode *m = ctx->modes.ptr + mode;

        /* parse flag */ {
            // search for flag name
            const char *name = arg;

            // argument is literal
            bool is_literal = true;

            switch(name[0]) {
                case '\\':
                    // check if next char is +
                    // if so, treat argument as literal
                    // otherwise, do nothing
                    if(name[1] == '+') {
                        name++;
                    }
                    break;
                case '+':
                    // skip arguments that start with +
                    break;
                // skip leading -
                case '-':
                    name++;
                    // argument is not literal
                    is_literal = false;
                    break;
                // otherwise, do nothing
                default:
                    break;
            }

            if(is_literal) {
                enum FlagshipType type = FLAGSHIP_TYPE_STRING;
                union {
                    double f;
                    long   i;
                } numerical = {0};

                char *endptr = NULL;
                numerical.i = strtol(name, &endptr, 10);

                if(endptr && name != endptr && !*endptr) {
                    type = FLAGSHIP_TYPE_INTEGER;
                } else {
                    numerical.f = strtod(name, &endptr);

                    if(endptr && name != endptr && !*endptr) {
                        type = FLAGSHIP_TYPE_FLOAT;
                    }
                }

                // now that we know what type of nameless flag
                // to search for, search for nameless + valid type

                struct FlagshipFlag *f = NULL;

                for(unsigned int i = 0; i < m->flags.len; ++i) {
                    struct FlagshipFlag *current = m->flags.ptr + i;

                    if(!current->names.ptr || !current->names.ptr[0]) {
                        if(current->type == type) {
                            f = current;
                            break;
                        }
                    }
                }

                if(!f) {
                    success = false;
                    err("invalid nameless flag: %s\n", name);

                    break;
                }

                struct FlagshipResult result = {0};

                result.was_set  = true;
                result.position = total_argc - argc;

                result.mode = &m->names;
                result.name = &f->names;

                result.type = f->type;

                switch(f->type) {
                    case FLAGSHIP_TYPE_NULL:
                    case FLAGSHIP_TYPE_BOOL:
                        break;
                    case FLAGSHIP_TYPE_INTEGER:
                        if(f->s_integer.min != f->s_integer.max) {
                            if(
                                (numerical.i >= f->s_integer.max) ||
                                (numerical.i < f->s_integer.min)
                            ) {
                                success = false;
                                err("integer out of range: %d [%d, %d)\n",
                                    (int)numerical.i, f->s_integer.min, f->s_integer.max);
                            }
                        }

                        if(success) {
                            result.t_integer = numerical.i;
                        }
                        break;
                    case FLAGSHIP_TYPE_FLOAT:
                        if(f->s_float.min != f->s_float.max) {
                            if(
                                (numerical.i >= f->s_float.max) ||
                                (numerical.i < f->s_float.min)
                            ) {
                                success = false;
                                err("float out of range: %f [%f, %f)\n",
                                    numerical.f, f->s_float.min, f->s_float.max);
                            }
                        }

                        if(success) {
                            result.t_float = numerical.f;
                        }
                        break;
                    case FLAGSHIP_TYPE_STRING: {
                        bool is_valid = false;
                        if(f->s_string.valid.len) {
                            for(unsigned int i = 0; i < f->s_string.valid.len; ++i) {
                                const char *v = flagship_deref(ctx, f->s_string.valid.ptr[i]);

                                if(strcmp(name, v) == 0) {
                                    is_valid = true;
                                    break;
                                }
                            }
                        } else {
                            is_valid = true;
                        }

                        if(is_valid) {
                            result.t_string = name;
                        } else {
                            success = false;
                            err("string is not valid: %s\n", name);

                            break;
                        }
                    } break;
                    case FLAGSHIP_TYPE_ENUM: {
                        int variant = -1;
                        for(unsigned int i = 0; i < f->s_enum.variants.len; ++i) {
                            struct FlagshipEnumVariant *v = f->s_enum.variants.ptr + i;

                            const char *v_name = flagship_deref(ctx, v->name);

                            if(strcmp(name, v_name) == 0) {
                                variant = i;
                                break;
                            }
                        }

                        if(variant >= 0) {
                            result.t_enum = variant;
                        } else {
                            success = false;
                            err("enum variant is not valid: %s\n", name);

                            break;
                        }
                    } break;
                }

                if(!success) {
                    break;
                }

                bool push_result = false;

                if(f->is_repeatable) {
                    push_result = true;
                } else {
                    struct FlagshipResult *dst = NULL;

                    for(unsigned int i = 0; i < ctx->results.len; ++i) {
                        struct FlagshipResult *this_result = ctx->results.ptr + i;
                        if(
                            (this_result->mode == result.mode) &&
                            (this_result->name == result.name)
                        ) {
                            dst = this_result;
                            break;
                        }
                    }

                    if(dst) {
                        memcpy(dst, &result, sizeof(result));
                    } else {
                        push_result = true;
                    }
                }

                if(push_result) {
                    flagship_reserve(&ctx->allocator, &ctx->results, 1);
                    ctx->results.ptr[ctx->results.len++] = result;
                }
            } else {
                // search for flag name

                struct FlagshipFlag *f = NULL;

                size_t arg_name_len = 0;
                const char *colon   = strchr(name, ':');
                if(colon) {
                    arg_name_len = colon - name;
                } else {
                    arg_name_len = strlen(name);
                }

                for(unsigned int i = 0; i < m->flags.len; ++i) {
                    struct FlagshipFlag *current = m->flags.ptr + i;

                    if(!current->names.ptr) {
                        continue;
                    }

                    for(unsigned int j = 0; j < current->names.len; ++j) {
                        const char *n =
                            current->names.ptr[j] ?
                                flagship_deref(ctx, current->names.ptr[j]) : NULL;

                        if(!n) {
                            continue;
                        }

                        size_t n_len = strlen(n);
                        if((n_len == arg_name_len) && (memcmp(n, name, arg_name_len) == 0)) {
                            f = current;
                            break;
                        }
                    }

                    if(f) {
                        break;
                    }
                }

                // flag not found
                if(!f) {
                    success = false;
                    err("unknown flag: %s\n", name);

                    break;
                }

                struct FlagshipResult result = {0};

                result.was_set  = true;
                result.position = total_argc - argc;

                result.mode = &m->names;
                result.name = &f->names;

                result.type = f->type;

                switch(f->type) {
                    case FLAGSHIP_TYPE_NULL:
                        break;
                    case FLAGSHIP_TYPE_BOOL: {
                        if(colon) {
                            success = false;
                            err("boolean flags do not take a payload!\n");

                            break;
                        }

                        if(f->s_bool.is_toggle) {
                            unsigned int counter = 0;
                            for(unsigned int i = 0; i < ctx->results.len; ++i) {
                                struct FlagshipResult *r = ctx->results.ptr + i;

                                if((r->mode == result.mode) && (r->name == result.name)) {
                                    counter++;
                                }
                            }

                            if(f->s_bool.is_flipped) {
                                result.t_bool = !((counter % 2) != 0);
                            } else {
                                result.t_bool = !((counter % 2) == 0);
                            }
                        } else {
                            result.t_bool = f->s_bool.is_flipped ? false : true;
                        }
                    } break;
                    case FLAGSHIP_TYPE_INTEGER: {
                        const char *literal = colon;
                        if(literal) {
                            literal++;
                        } else {
                            adv();
                            literal = name = *argv;
                        }

                        char *endptr = NULL;
                        long v = strtol(literal, &endptr, 10);

                        if(!(endptr && literal != endptr && !*endptr)) {
                            success = false;
                            err("invalid integer: %s\n", literal);

                            break;
                        }

                        if(f->s_integer.min != f->s_integer.max) {
                            if(
                                (v >= f->s_integer.max) ||
                                (v < f->s_integer.min)
                            ) {
                                success = false;
                                err("integer out of range: %d [%d, %d)\n",
                                    (int)v, f->s_integer.min, f->s_integer.max);

                                break;
                            }
                        }

                        result.t_integer = v;
                    } break;
                    case FLAGSHIP_TYPE_FLOAT: {
                        const char *literal = colon;
                        if(literal) {
                            literal++;
                        } else {
                            adv();
                            literal = name = *argv;
                        }

                        char *endptr = NULL;
                        double v = strtod(literal, &endptr);

                        if(!(endptr && literal != endptr && !*endptr)) {
                            success = false;
                            err("invalid float: %s\n", literal);

                            break;
                        }

                        if(f->s_float.min != f->s_float.max) {
                            if(
                                (v >= f->s_float.max) ||
                                (v < f->s_float.min)
                            ) {
                                success = false;
                                err("float out of range: %f [%f, %f)\n",
                                    v, f->s_float.min, f->s_float.max);

                                break;
                            }
                        }

                        result.t_float = v;
                    } break;
                    case FLAGSHIP_TYPE_STRING: {
                        const char *literal = colon;
                        if(literal) {
                            literal++;
                        } else {
                            adv();
                            literal = name = *argv;
                        }

                        if(f->s_string.valid.ptr) {
                            bool is_valid = false;
                            for(unsigned int i = 0; i < f->s_string.valid.len; ++i) {
                                const char *cmp = NULL;
                                if(f->s_string.valid.ptr[i]) {
                                    cmp = flagship_deref(ctx, f->s_string.valid.ptr[i]);
                                }

                                if(!cmp) {
                                    continue;
                                }

                                if(strcmp(literal, cmp) == 0) {
                                    is_valid = true;
                                    break;
                                }
                            }

                            if(!is_valid) {
                                success = false;
                                err("string is not valid: %s\n", literal);
                                break;
                            }
                        }

                        result.t_string = literal;
                    } break;
                    case FLAGSHIP_TYPE_ENUM: {
                        const char *literal = colon;
                        if(literal) {
                            literal++;
                        } else {
                            adv();
                            literal = name = *argv;
                        }

                        int variant = -1;

                        for(unsigned int i = 0; i < f->s_enum.variants.len; ++i) {
                            struct FlagshipEnumVariant *v = f->s_enum.variants.ptr + i;

                            const char *v_name = flagship_deref(ctx, v->name);

                            if(strcmp(literal, v_name) == 0) {
                                variant = i;
                                break;
                            }

                            char *endptr = NULL;
                            long integer = strtol(literal, &endptr, 10);

                            if(!(endptr && literal != endptr && !*endptr)) {
                                continue;
                            }

                            variant = integer;
                            break;
                        }

                        if(variant < 0) {
                            success = false;
                            err("enum variant is not valid: %s\n", literal);
                            break;
                        }

                        result.t_integer = variant;
                    } break;
                }

                if(!success) {
                    break;
                }

                // TODO(alicia): remove default flag if it exists!
                bool push_result = false;

                if(f->is_repeatable) {
                    push_result = true;
                } else {
                    struct FlagshipResult *dst = NULL;

                    for(unsigned int i = 0; i < ctx->results.len; ++i) {
                        struct FlagshipResult *this_result = ctx->results.ptr + i;
                        if(
                            (this_result->mode == result.mode) &&
                            (this_result->name == result.name)
                        ) {
                            dst = this_result;
                            break;
                        }
                    }

                    if(dst) {
                        memcpy(dst, &result, sizeof(result));
                    } else {
                        push_result = true;
                    }
                }

                if(push_result) {
                    flagship_reserve(&ctx->allocator, &ctx->results, 1);
                    ctx->results.ptr[ctx->results.len++] = result;
                }
            }
        }

        adv();
    }

    // sort results
    if(success) {
        if(ctx->results.len) {
            qsort(ctx->results.ptr, ctx->results.len,
                sizeof(ctx->results.ptr[0]), __flagship_result_sort_comp);
        }
    } else {
        if(print_help) {
            flagship_help_stream(ctx, NULL, true, stream, help_target);
        }
    }

    if(success) {
        // allocate search buffer
        if(ctx->search.cap != ctx->results.len) {
            flagship_reserve(&ctx->allocator, &ctx->search, ctx->results.len);
        }
    }

    #undef adv
    #undef err
    return success;
}

FLAGSHIP_INLINE
bool flagship_parse(struct FlagshipContext *ctx, int argc, char** argv, bool print_help) {
    return flagship_parse_streaming_errors(
        ctx, argc, argv, __flagship_stream_file, stderr, print_help ? stdout : NULL, print_help);
}

FLAGSHIP_INLINE
bool flagship_iter_next(struct FlagshipContext *ctx, struct FlagshipResult *out_result) {
    unsigned int idx = ctx->iter++;
    if(idx >= ctx->results.len) {
        return false;
    }

    memcpy(out_result, ctx->results.ptr + idx, sizeof(*out_result));
    return true;
}

FLAGSHIP_INLINE
void flagship_iter_reset(struct FlagshipContext *ctx) {
    ctx->iter = 0;
}

// TODO(alicia): replace string searches with hash searches eventually?
FLAGSHIP_INLINE
bool flagship_enum_search(
    struct FlagshipContext *ctx, const char *mode, const char *name,
    unsigned int *out_variant_count, struct FlagshipEnumVariant **out_variants
) {
    bool found = false;

    for(unsigned int i = 0; i < ctx->modes.len; ++i) {
        struct FlagshipMode *m = ctx->modes.ptr + i;
        if(mode) {
            const char *rmode =
                (m->names.ptr && m->names.ptr[0]) ? flagship_deref(ctx, m->names.ptr[0]) : NULL;

            if(!rmode || !(strcmp(rmode, mode) == 0)) {
                continue;
            }
        } else {
            if(m->names.ptr) {
                if(m->names.ptr[0]) {
                    continue;
                }
            }
        }

        for(unsigned int j = 0; j < m->flags.len; ++j) {
            struct FlagshipFlag *f = m->flags.ptr + j;

            if(name) {
                const char *rname =
                    (f->names.ptr && f->names.ptr[0]) ? flagship_deref(ctx, f->names.ptr[0]) : NULL;

                if(!rname || !(strcmp(rname, name) == 0)) {
                    continue;
                }
            } else {
                if(f->names.ptr) {
                    if(f->names.ptr[0]) {
                        continue;
                    }
                }
            }

            if(f->type == FLAGSHIP_TYPE_ENUM) {
                found = true;

                *out_variant_count = f->s_enum.variants.len;
                *out_variants      = f->s_enum.variants.ptr;
                break;
            }
        }

        if(found) {
            break;
        }
    }

    return found;
}

FLAGSHIP_INLINE
bool flagship_enum_get(
    struct FlagshipContext *ctx, FlagshipString mode, FlagshipString name,
    unsigned int *out_variant_count, struct FlagshipEnumVariant **out_variants
) {
    const char *mode_str, *name_str;
    mode_str = name_str = NULL;

    if(mode) {
        mode_str = flagship_deref(ctx, mode);
    }

    if(name) {
        name_str = flagship_deref(ctx, name);
    }

    return flagship_enum_search(ctx, mode_str, name_str, out_variant_count, out_variants);
}

FLAGSHIP_INLINE
bool flagship_search(
    struct FlagshipContext *ctx, const char *mode, const char *name,
    unsigned int *out_count, struct FlagshipResult **out_results
) {
    struct __FlagshipBufferResult *s = &ctx->search;

    s->len = 0;
    flagship_reserve(&ctx->allocator, s, ctx->results.len);

    bool found = false;

    for(unsigned int i = 0; i < ctx->results.len; ++i) {
        struct FlagshipResult *r = ctx->results.ptr + i;

        if(mode) {
            const char *rmode =
                (r->mode->ptr && r->mode->ptr[0]) ?
                    flagship_deref(ctx, r->mode->ptr[0]) : NULL;

            if(!rmode || !(strcmp(rmode, mode) == 0)) {
                continue;
            }
        } else {
            if(r->mode->ptr) {
                if(r->mode->ptr[0]) {
                    continue;
                }
            }
        }

        if(name) {
            const char *rname =
                (r->name->ptr && r->name->ptr[0]) ?
                    flagship_deref(ctx, r->name->ptr[0]) : NULL;

            if(!rname || !(strcmp(rname, name) == 0)) {
                continue;
            }
        } else {
            if(r->name->ptr) {
                if(r->name->ptr[0]) {
                    continue;
                }
            }
        }

        found = true;
        s->ptr[s->len++] = *r;
    }

    if(found) {
        *out_count   = s->len;
        *out_results = s->ptr;
    } else {
        *out_count   = 0;
        *out_results = NULL;
    }
    return found;
}

FLAGSHIP_INLINE
bool flagship_get(
    struct FlagshipContext *ctx, FlagshipString mode, FlagshipString name,
    unsigned int *out_count, struct FlagshipResult **out_results
) {
    const char *mode_str, *name_str;
    mode_str = name_str = NULL;

    if(mode) {
        mode_str = flagship_deref(ctx, mode);
    }

    if(name) {
        name_str = flagship_deref(ctx, name);
    }

    return flagship_search(ctx, mode_str, name_str, out_count, out_results);
}

#undef FLAGSHIP_INLINE
#undef FLAGSHIP_FMTFUNC
#undef STRUCT
#undef flagship_reset_cbuf
#undef flagship_alloc
#undef flagship_realloc
#undef flagship_free
#undef flagship_reserve
#undef flagship_assert
#undef flagship_fmt
#undef flagship_fmt_va
#undef flagship_fmt_deref
#undef flagship_fmt_deref_va

#endif /* header guard */
