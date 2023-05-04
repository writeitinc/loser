#ifndef loser_h
#define loser_h

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// #######################
// ######## Types ########
// #######################

// Just call `unsigned char` what it is--a byte.
typedef unsigned char LSByte;

// An immutable array of bytes.
/*
 * Will always be null-terminated:
 * - `bytes[len]` can be read from and is set to '\0'
 *
 * A `bytes` value of `NULL` indicates an invalid value.
 */
typedef struct LSString {
	size_t len;
	const LSByte *bytes;
} LSString;

// A non-owning range of bytes.
/*
 * Might not be null-terminated.
 *
 * A `start` value of `NULL` indicates an invalid value.
 */
typedef struct LSStringSpan {
	size_t len;
	const LSByte *start;
} LSStringSpan;

// A mutable array of bytes.
/*
 * Might not be null-terminated.
 *
 * A `bytes` value of `NULL` indicates an invalid value.
 */
typedef struct LSByteBuffer {
	size_t len;
	size_t cap;
	LSByte *bytes;
} LSByteBuffer;

// #################################
// ######## Validity Checks ########
// #################################

// Convenience functions for checking validity of objects.
static inline bool LS_STRING_VALID(LSString string);
static inline bool LS_SSPAN_VALID(LSStringSpan sspan);
static inline bool LS_BBUF_VALID(LSByteBuffer bbuf);

// ###############################################
// ######## Base Constructors/Destructors ########
// ###############################################

/*
 * Constraints:
 * - `bytes` points to a valid array of at least `len` bytes
 *        OR is `NULL`
 *
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - allocation fails
 * - `bytes` is `NULL`
 */
LSString ls_string_create(const LSByte *bytes, size_t len);

/*
 * Passing an invalid `LSString` is safe.
 *
 * Constraints:
 * - `string` was not previously destroyed
 */
void ls_string_destroy(LSString *string);

/*
 * Constraints:
 * - `start` points to a valid array of at least `len` bytes
 *        OR is `NULL`
 *
 * On success:
 * - returns a valid `LSStringSpan`
 * On failure:
 * - returns an invalid `LSStringSpan`
 *
 * Fails if:
 * - `start` is `NULL`
 */
static inline LSStringSpan ls_sspan_create(const LSByte *start, size_t len);

// ##########################################
// ######## Convenience Constructors ########
// ##########################################

/*
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - allocation fails
 * - `string` is invalid
 */
static inline LSString ls_string_clone(LSString string);

/*
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - allocation fails
 * - `sspan` is invalid
 */
static inline LSString ls_string_from_sspan(LSStringSpan sspan);

/*
 * Constraints:
 * - `chars` points to a valid array of at least `len` `char`s
 *        OR is `NULL`
 *
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - allocation fails
 * - `chars` is `NULL`
 */
static inline LSString ls_string_from_chars(const char *chars, size_t len);

/*
 * Constraints:
 * - `cstr` points to a valid null-terminated array of `char`s
 *          OR is `NULL`
 *
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - allocation fails
 * - `cstr` is `NULL`
 */
static inline LSString ls_string_from_cstr(const char *cstr);

/*
 * Resulting `LSStringSpan` does not include the final null-terminator.
 *
 * On success:
 * - returns a valid `LSStringSpan`
 * On failure:
 * - returns an invalid `LSStringSpan`
 *
 * Fails if:
 * - `string` is invalid
 */
static inline LSStringSpan ls_sspan_from_string(LSString string);

/*
 * Constraints:
 * - `chars` points to a valid array of at least `len` `char`s
          OR is `NULL`
 *
 * On success:
 * - returns a valid `LSStringSpan`
 * On failure:
 * - returns an invalid `LSStringSpan`
 *
 * Fails if:
 * - `chars` is `NULL`
 */
static inline LSStringSpan ls_sspan_from_chars(const char *chars, size_t len);

/*
 * Resulting `LSStringSpan` does not include the null-terminator.
 *
 * Constraints:
 * - `cstr` points to a valid null-terminated array of `char`s
 *       OR is `NULL`
 *
 * On success:
 * - returns a valid `LSStringSpan`
 * On failure:
 * - returns an invalid `LSStringSpan`
 *
 * Fails if:
 * - `chars` is `NULL`
 */
static inline LSStringSpan ls_sspan_from_cstr(const char *cstr);

// ################################################
// ######## Substring/Subspan Constructors ########
// ################################################

/*
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - `string` is invalid
 */
static inline LSString ls_string_substr(LSString string, size_t start,
		size_t len);

/*
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - `string` is invalid
 */
static inline LSStringSpan ls_string_subspan(LSString string, size_t start,
		size_t len);

/*
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - `sspan` is invalid
 */
static inline LSString ls_sspan_substr(LSStringSpan sspan, size_t start,
		size_t len);

/*
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - `sspan` is invalid
 */
static inline LSStringSpan ls_sspan_subspan(LSStringSpan sspan, size_t start,
		size_t len);

// ####################################
// ######## Inline Definitions ########
// ####################################

static inline bool LS_STRING_VALID(LSString string)
{
	return string.bytes != NULL;
}

static inline bool LS_SSPAN_VALID(LSStringSpan sspan)
{
	return sspan.start != NULL;
}

static inline bool LS_BBUF_VALID(LSByteBuffer bbuf)
{
	return bbuf.bytes != NULL;
}

static inline LSStringSpan ls_sspan_create(const LSByte *start, size_t len)
{
	return (LSStringSpan){
		.start = start,
		.len = len
	};
}

static inline LSString ls_string_clone(LSString string)
{
	return ls_string_create(string.bytes, string.len);
}

static inline LSString ls_string_from_sspan(LSStringSpan sspan)
{
	return ls_string_create(sspan.start, sspan.len);
}

static inline LSString ls_string_from_chars(const char *chars, size_t len)
{
	return ls_string_create((const LSByte *)chars, len);
}

static inline LSString ls_string_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return (LSString){ 0 };
	}

	return ls_string_from_chars(cstr, strlen(cstr));
}

static inline LSStringSpan ls_sspan_from_string(LSString string)
{
	return (LSStringSpan){
		.start = string.bytes,
		.len = string.len
	};
}

static inline LSStringSpan ls_sspan_from_chars(const char *chars, size_t len)
{
	return (LSStringSpan){
		.start = (const LSByte *)chars,
		.len = len
	};
}

static inline LSStringSpan ls_sspan_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return (LSStringSpan){ 0 };
	}

	return ls_sspan_from_chars(cstr, strlen(cstr));
}

static inline LSString ls_string_substr(LSString string, size_t start,
		size_t len)
{
	return ls_string_from_sspan(ls_string_subspan(string, start, len));
}

static inline LSStringSpan ls_string_subspan(LSString string, size_t start,
		size_t len)
{
	return ls_sspan_subspan(ls_sspan_from_string(string), start, len);
}

static inline LSString ls_sspan_substr(LSStringSpan sspan, size_t start,
		size_t len)
{
	return ls_string_from_sspan(ls_sspan_subspan(sspan, start, len));
}

static inline LSStringSpan ls_sspan_subspan(LSStringSpan sspan, size_t start,
		size_t len)
{
	if (!LS_SSPAN_VALID(sspan)
			|| start > sspan.len
			|| len > sspan.len
			|| start > sspan.len - len) {
		return (LSStringSpan){ 0 };
	}

	return ls_sspan_create(&sspan.start[start], len);
}

#endif // loser_h
