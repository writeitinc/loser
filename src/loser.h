#ifndef loser_h
#define loser_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/*
 * NOTE: Library guarantees may not hold true for objects created or modified
 * manually.
 */

// ###########################
// ######## Constants ########
// ###########################

enum { LS_SHORT_STRING_MAX_LEN = 15 };

// #######################
// ######## Types ########
// #######################

// Just call `unsigned char` what it is--a byte.
typedef unsigned char LSByte;

// An immutable array of bytes.
/*
 * Check validity using `LS_STRING_VALID()`.
 *
 * If valid:
 * - `bytes` is null-terminated
 * - `bytes[len]` can be read from and is set to '\0'
 */
typedef struct LSString {
	size_t len;
	const LSByte *bytes;
} LSString;

// A short immutable array of bytes.
/*
 * Check validity using `LS_SHORT_STRING_VALID()`.
 *
 * If valid:
 * - `bytes` is null-terminated
 * - `bytes[len]` can be read from and is set to '\0'
 *
 * Has a maximum length of `LS_SHORT_STRING_MAX_LEN`.
 */
typedef struct LSShortString {
	size_t len;
	LSByte bytes[LS_SHORT_STRING_MAX_LEN + 1];
} LSShortString;

// An small string-optimized immutable array of bytes.
/*
 * Check validity/type using `LS_SSO_STRING_TYPE()`.
 * Get bytes using `LS_SSO_STRING_BYTES()`.
 *
 * If valid:
 * - `LS_SSO_STRING_BYTES()` is null-terminated
 * - `LS_SSO_STRING_BYTES()[len]` can be read from and is set to '\0'
 *
 * Intended to perform better when storing strings short enough to fit in a
 * `LSShortString`.
 */
typedef union LSSSOString {
	size_t len;
	LSShortString _short; // <= Please use LS_SSO_STRING_BYTES()
	LSString _long;       // <= ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
} LSSSOString;

// The type of string stored in a `LSSSOString`.
typedef enum LSSSOStringType {
	LS_SSO_STRING_INVALID,
	LS_SSO_STRING_SHORT,
	LS_SSO_STRING_LONG
} LSSSOStringType;

// A non-owning range of bytes.
/*
 * Check validity using `LS_SSPAN_VALID()`.
 *
 * Might not be null-terminated.
 */
typedef struct LSStringSpan {
	size_t len;
	const LSByte *start;
} LSStringSpan;

// A mutable array of bytes.
/*
 * Check validity using `LS_BBUF_VALID()`.
 *
 * Might not be null-terminated.
 */
typedef struct LSByteBuffer {
	size_t len;
	size_t cap;
	LSByte *bytes;
} LSByteBuffer;

// ###################################
// ######## Invalid Constants ########
// ###################################

/*
 * NOTE: As their names imply, the following constants are not the only invalid
 * values for their respective types but *an* invalid value of that type.
 * These should not be tested against. Instead, use the provided validity-
 * checking functions (see section [Macro-like Functions] or type definitions).
 */

static const LSString LS_AN_INVALID_STRING = { .bytes = NULL };
static const LSSSOString LS_AN_INVALID_SSO_STRING = {
	._long.len = SIZE_MAX,
	._long.bytes = NULL
};
static const LSShortString LS_AN_INVALID_SHORT_STRING = { .len = SIZE_MAX };
static const LSStringSpan LS_AN_INVALID_SSPAN = { .start = NULL };

// ######################################
// ######## Macro-like Functions ########
// ######################################

static inline bool LS_STRING_VALID(LSString string);
static inline bool LS_SHORT_STRING_VALID(LSShortString short_string);
static inline bool LS_SSPAN_VALID(LSStringSpan sspan);
static inline bool LS_BBUF_VALID(LSByteBuffer bbuf);

static inline LSSSOStringType LS_SSO_STRING_TYPE(LSSSOString sso_string);
static inline const LSByte *LS_SSO_STRING_BYTES(const LSSSOString *sso_string);

// ###############################################
// ######## Base Constructors/Destructors ########
// ###############################################

/*
 * Constraints:
 * - `bytes` points to a array of at least `len` bytes
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
 * - `bytes` points to an array of at least `len` bytes
 *        OR is `NULL`
 *
 * On success:
 * - returns a valid `LSShortString`
 * On failure:
 * - returns an invalid `LSShortString`
 *
 * Fails if:
 * - `len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `bytes` is `NULL`
 */
static inline LSShortString ls_short_string_create(const LSByte *bytes,
		size_t len);

/*
 * On success:
 * - returns a valid `LSShortString`
 * On failure:
 * - returns an invalid `LSShortString`
 *
 * Fails if:
 * - `string.len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `string` is invalid
 */
static inline LSShortString ls_short_string_from_string(LSString string);

/*
 * On success:
 * - returns a valid `LSShortString`
 * On failure:
 * - returns an invalid `LSShortString`
 *
 * Fails if:
 * - `LS_SSO_STRING_TYPE(sso_string)` is not LS_SSO_STRING_SHORT
 */
static inline LSShortString ls_short_string_from_sso_string(
		LSSSOString sso_string);

/*
 * On success:
 * - returns a valid `LSShortString`
 * On failure:
 * - returns an invalid `LSShortString`
 *
 * Fails if:
 * - `sspan.len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `sspan` is invalid
 */
static inline LSShortString ls_short_string_from_sspan(LSStringSpan sspan);

/*
 * Constraints:
 * - `chars` points to an array of at least `len` chars
 *        OR is `NULL`
 *
 * On success:
 * - returns a valid `LSShortString`
 * On failure:
 * - returns an invalid `LSShortString`
 *
 * Fails if:
 * - `len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `chars` is `NULL`
 */
static inline LSShortString ls_short_string_from_chars(const char *chars,
		size_t len);

/*
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * On success:
 * - returns a valid `LSShortString`
 * On failure:
 * - returns an invalid `LSShortString`
 *
 * Fails if:
 * - length of `cstr` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `cstr` is `NULL`
 */
static inline LSShortString ls_short_string_from_cstr(const char *cstr);

/*
 * Constraints:
 * - `bytes` points to an array of at least `len` bytes
 *        OR is `NULL`
 *
 * On success:
 * - returns a valid `LSSSOString`
 * On failure:
 * - returns an invalid `LSSSOString`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `bytes` is `NULL`
 */
static inline LSSSOString ls_sso_string_create(const LSByte *bytes, size_t len);

/*
 * Passing an invalid `LSSSOString` is safe.
 *
 * Constraints:
 * - `sso_string` was not previously destroyed
 */
static inline void ls_sso_string_destroy(LSSSOString *sso_string);

/*
 * Constraints:
 * - `start` points to an array of at least `len` bytes
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
 * - `short_string` is invalid
 */
static inline LSString ls_string_from_short_string(LSShortString short_string);

/*
 * On success:
 * - returns a valid `LSString`
 * On failure:
 * - returns an invalid `LSString`
 *
 * Fails if:
 * - allocation fails
 * - `sso_string` is invalid
 */
static inline LSString ls_string_from_sso_string(LSSSOString sso_string);

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
 * - `chars` points to an array of at least `len` `char`s
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
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
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
 * On success:
 * - returns a valid `LSSSOString`
 * On failure:
 * - returns an invalid `LSSSOString`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `sso_string` is invalid
 */
static inline LSSSOString ls_sso_string_clone(LSSSOString sso_string);

/*
 * On success:
 * - returns a valid `LSSSOString`
 * On failure:
 * - returns an invalid `LSSSOString`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `string` is invalid
 */
static inline LSSSOString ls_sso_string_from_string(LSString string);

/*
 * On success:
 * - returns a valid `LSSSOString`
 * On failure:
 * - returns an invalid `LSSSOString`
 *
 * Fails if:
 * - `short_string` is invalid
 */
static inline LSSSOString ls_sso_string_from_short_string(
		LSShortString short_string);

/*
 * On success:
 * - returns a valid `LSSSOString`
 * On failure:
 * - returns an invalid `LSSSOString`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `sspan` is invalid
 */
static inline LSSSOString ls_sso_string_from_sspan(LSStringSpan sspan);

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
 *        OR is `NULL`
 *
 * On success:
 * - returns a valid `LSSSOString`
 * On failure:
 * - returns an invalid `LSSSOString`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `chars` is `NULL`
 */
static inline LSSSOString ls_sso_string_from_chars(const char *chars,
		size_t len);

/*
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * On success:
 * - returns a valid `LSSSOString`
 * On failure:
 * - returns an invalid `LSSSOString`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `cstr` is invalid
 */
static inline LSSSOString ls_sso_string_from_cstr(const char *cstr);

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
 * Resulting `LSStringSpan` does not include the final null-terminator.
 *
 * On success:
 * - returns a valid `LSStringSpan`
 * On failure:
 * - returns an invalid `LSStringSpan`
 *
 * Fails if:
 * - `short_string` is invalid
 */
static inline LSStringSpan ls_sspan_from_short_string(
		LSShortString short_string);

/*
 * Resulting `LSStringSpan` does not include the final null-terminator.
 *
 * On success:
 * - returns a valid `LSStringSpan`
 * On failure:
 * - returns an invalid `LSStringSpan`
 *
 * Fails if:
 * - `sso_string` is invalid
 */
static inline LSStringSpan ls_sspan_from_sso_string(LSSSOString sso_string);

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
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
 * - `cstr` points to a null-terminated array of `char`s
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

static inline bool LS_SHORT_STRING_VALID(LSShortString short_string)
{
	return short_string.len <= LS_SHORT_STRING_MAX_LEN;
}

static inline bool LS_SSPAN_VALID(LSStringSpan sspan)
{
	return sspan.start != NULL;
}

static inline bool LS_BBUF_VALID(LSByteBuffer bbuf)
{
	return bbuf.bytes != NULL;
}

static inline LSSSOStringType LS_SSO_STRING_TYPE(LSSSOString sso_string)
{
	if (LS_SHORT_STRING_VALID(sso_string._short)) {
		return LS_SSO_STRING_SHORT;
	}

	if (LS_STRING_VALID(sso_string._long)) {
		return LS_SSO_STRING_LONG;
	}

	return LS_SSO_STRING_INVALID;
}

static inline const LSByte *LS_SSO_STRING_BYTES(const LSSSOString *sso_string)
{
	switch (LS_SSO_STRING_TYPE(*sso_string)) {
	case LS_SSO_STRING_SHORT:
		return sso_string->_short.bytes;
	case LS_SSO_STRING_LONG:
		return sso_string->_long.bytes;
	default:
		return NULL;
	}
}

static inline LSShortString ls_short_string_create(const LSByte *bytes,
		size_t len)
{
	if (len > LS_SHORT_STRING_MAX_LEN
			|| bytes == NULL) {
		return (LSShortString){ .len = SIZE_MAX };
	}

	LSShortString short_string = { .len = len };
	memcpy(short_string.bytes, bytes, len);
	short_string.bytes[len] = '\0';

	return short_string;
}

static inline LSShortString ls_short_string_from_string(LSString string)
{
	return ls_short_string_create(string.bytes, string.len);
}

static inline LSShortString ls_short_string_from_sso_string(
		LSSSOString sso_string)
{
	if (LS_SSO_STRING_TYPE(sso_string) != LS_SSO_STRING_SHORT) {
		return LS_AN_INVALID_SHORT_STRING;
	}

	return sso_string._short;
}

static inline LSShortString ls_short_string_from_sspan(LSStringSpan sspan)
{
	return ls_short_string_create(sspan.start, sspan.len);
}

static inline LSShortString ls_short_string_from_chars(const char *chars,
		size_t len)
{
	return ls_short_string_create((const LSByte *)chars, len);
}

static inline LSShortString ls_short_string_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return LS_AN_INVALID_SHORT_STRING;
	}

	return ls_short_string_from_chars(cstr, strlen(cstr));
}

static inline LSSSOString ls_sso_string_create(const LSByte *bytes, size_t len)
{
	if (len <= LS_SHORT_STRING_MAX_LEN) {
		return (LSSSOString){
			._short = ls_short_string_create(bytes, len)
		};
	}

	LSString string = ls_string_create(bytes, len);
	if (!LS_STRING_VALID(string)) {
		goto err_exit;
	}

	return (LSSSOString){ ._long = string };

err_exit:
	return LS_AN_INVALID_SSO_STRING;
}

static inline void ls_sso_string_destroy(LSSSOString *sso_string)
{
	if (LS_SSO_STRING_TYPE(*sso_string) == LS_SSO_STRING_LONG) {
		ls_string_destroy(&sso_string->_long);
	}
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

static inline LSString ls_string_from_short_string(LSShortString short_string)
{
	if (!LS_SHORT_STRING_VALID(short_string)) {
		return LS_AN_INVALID_STRING;
	}

	return ls_string_create(short_string.bytes, short_string.len);
}

static inline LSString ls_string_from_sso_string(LSSSOString sso_string)
{
	const LSByte *bytes = LS_SSO_STRING_BYTES(&sso_string);
	return ls_string_create(bytes, sso_string.len);
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
		return LS_AN_INVALID_STRING;
	}

	return ls_string_from_chars(cstr, strlen(cstr));
}

static inline LSSSOString ls_sso_string_clone(LSSSOString sso_string)
{
	LSSSOStringType type = LS_SSO_STRING_TYPE(sso_string);

	if (type == LS_SSO_STRING_SHORT || type == LS_SSO_STRING_INVALID) {
		return sso_string;
	}

	LSString string = ls_string_clone(sso_string._long);
	if (!LS_STRING_VALID(string)) {
		goto err_exit;
	}

	return (LSSSOString){ ._long = string };

err_exit:
	return LS_AN_INVALID_SSO_STRING;
}

static inline LSSSOString ls_sso_string_from_string(LSString string)
{
	return ls_sso_string_create(string.bytes, string.len);
}

static inline LSSSOString ls_sso_string_from_short_string(
		LSShortString short_string)
{
	if (!LS_SHORT_STRING_VALID(short_string)) {
		return LS_AN_INVALID_SSO_STRING;
	}

	return (LSSSOString){ ._short = short_string };
}

static inline LSSSOString ls_sso_string_from_sspan(LSStringSpan sspan)
{
	return ls_sso_string_create(sspan.start, sspan.len);
}

static inline LSSSOString ls_sso_string_from_chars(const char *chars,
		size_t len)
{
	return ls_sso_string_create((const LSByte *)chars, len);
}

static inline LSSSOString ls_sso_string_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return LS_AN_INVALID_SSO_STRING;
	}

	return ls_sso_string_from_chars(cstr, strlen(cstr));
}

static inline LSStringSpan ls_sspan_from_string(LSString string)
{
	return (LSStringSpan){
		.start = string.bytes,
		.len = string.len
	};
}

static inline LSStringSpan ls_sspan_from_short_string(
		LSShortString short_string)
{
	if (!LS_SHORT_STRING_VALID(short_string)) {
		return LS_AN_INVALID_SSPAN;
	}

	return (LSStringSpan){
		.start = short_string.bytes,
		.len = short_string.len
	};
}

static inline LSStringSpan ls_sspan_from_sso_string(LSSSOString sso_string)
{
	const LSByte *bytes = LS_SSO_STRING_BYTES(&sso_string);

	return (LSStringSpan){
		.start = bytes,
		.len = sso_string.len
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
		return LS_AN_INVALID_SSPAN;
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
		return LS_AN_INVALID_SSPAN;
	}

	return ls_sspan_create(&sspan.start[start], len);
}

#endif // loser_h
