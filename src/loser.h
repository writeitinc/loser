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

#define LS_SHORT_STRING_LITERAL(cstr_lit) \
	(LSShortString){ \
		.len = strlen(cstr_lit), \
		._mut_bytes = cstr_lit "\0" \
	}

enum { LS_SHORT_STRING_MAX_LEN = 23 };

typedef enum LSStatus {
	LS_SUCCESS = 0,
	LS_FAILURE = -1
} LSStatus;

// Just call `unsigned char` what it is--a byte.
typedef unsigned char LSByte;

// A (null-terminated) immutable array of bytes.
typedef struct LSString {
	size_t len;
	const LSByte *bytes;
} LSString;

// A (null-terminated) short array of bytes.
/*
 * Has a maximum length of `LS_SHORT_STRING_MAX_LEN`.
 */
typedef struct LSShortString {
	size_t len;
	LSByte _mut_bytes[LS_SHORT_STRING_MAX_LEN + 1];
} LSShortString;

// A (null-terminated) small string-optimized immutable array of bytes.
typedef union LSSSOString {
	size_t len;
	LSShortString _short;
	LSString _long;
} LSSSOString;

// Indicates the contents of an `LSSSOString`.
typedef enum LSSSOStringType {
	LS_SSO_STRING_INVALID,
	LS_SSO_STRING_SHORT,
	LS_SSO_STRING_LONG
} LSSSOStringType;

// A non-owning range of bytes.
/*
 * Might not be null-terminated.
 */
typedef struct LSStringSpan {
	size_t len;
	const LSByte *start;
} LSStringSpan;

// A mutable array of bytes.
/*
 * Might not be null-terminated.
 */
typedef struct LSByteBuffer {
	size_t len;
	size_t cap;
	LSByte *bytes;
} LSByteBuffer;

// The empty string constant.
extern const LSString LS_EMPTY_STRING;

/*
 * NOTE: As their names imply, the following constants are not the only invalid
 * values for their respective types but *an* invalid value of that type.
 * Thus, these should not be tested against. Instead, use the appropriate
 * validity-checking functions.
 */
#define LS_AN_INVALID_STRING (LSString){ .bytes = NULL }
#define LS_AN_INVALID_SSO_STRING \
	(LSSSOString){ ._long.len = SIZE_MAX, ._long.bytes = NULL }
#define LS_AN_INVALID_SHORT_STRING (LSShortString){ .len = SIZE_MAX }
#define LS_AN_INVALID_SSPAN (LSStringSpan){ .start = NULL }
#define LS_AN_INVALID_BBUF (LSByteBuffer){ .bytes = NULL }

#define LS_LINKAGE inline
#include "loser-inline-decls.h"
#undef LS_LINKAGE

/*
 * Don't call this function directly.
 */
LSString ls__intern_string_create_unchecked(const LSByte *bytes, size_t len);

/*
 * Constraints:
 * - `string` is not `NULL`
 * - `string` was not previously destroyed
 */
void ls_string_destroy(LSString *string);

/*
 * Fails if:
 *  - allocation fails
 *  - `cap` is `0`
 */
LSByteBuffer ls_bbuf_create_with_init_cap(size_t cap);

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 * - `bbuf` was not previously destroyed
 */
void ls_bbuf_destroy(LSByteBuffer *bbuf);

/*
 * Fails if:
 * - `sso_string` doesn't contain a short string
 */
LSShortString ls_short_string_from_sso_string(LSSSOString sso_string);

/*
 * Fails if:
 * - `short_string` is invalid
 */
LSSSOString ls_sso_string_from_short_string(LSShortString short_string);

/*
 * Fails if:
 * - `sspan` is invalid
 * - allocation fails
 */
LSByteBuffer ls_bbuf_from_sspan(LSStringSpan sspan);

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `bytes` is `NULL`
 * - reallocation is attempted and fails
 */
LSStatus ls_bbuf_append(LSByteBuffer *bbuf, const LSByte *bytes, size_t len);

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `bytes` is `NULL`
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
LSStatus ls_bbuf_insert(LSByteBuffer *bbuf, size_t idx, const LSByte *bytes,
		size_t len);

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `new_cap` is not greater than `bbuf->cap`
 * - reallocation fails
 */
LSStatus ls_bbuf_expand_to(LSByteBuffer *bbuf, size_t new_cap);

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - resulting capacity would exceed `SIZE_MAX`
 * - reallocation fails
 */
LSStatus ls_bbuf_expand_by(LSByteBuffer *bbuf, size_t add_cap);

inline bool ls_string_is_valid(LSString string)
{
	return string.bytes != NULL;
}

inline bool ls_short_string_is_valid(LSShortString short_string)
{
	return short_string.len <= LS_SHORT_STRING_MAX_LEN;
}

inline const LSByte *ls_short_string_get_bytes(
		const LSShortString *short_string)
{
	return short_string->_mut_bytes;
}

inline bool ls_sspan_is_valid(LSStringSpan sspan)
{
	return sspan.start != NULL;
}

inline bool ls_bbuf_is_valid(LSByteBuffer bbuf)
{
	return bbuf.bytes != NULL;
}

inline LSSSOStringType ls_sso_string_get_type(LSSSOString sso_string)
{
	if (ls_short_string_is_valid(sso_string._short)) {
		return LS_SSO_STRING_SHORT;
	}

	if (ls_string_is_valid(sso_string._long)) {
		return LS_SSO_STRING_LONG;
	}

	return LS_SSO_STRING_INVALID;
}

inline bool ls_sso_string_is_valid(LSSSOString sso_string)
{
	return ls_sso_string_get_type(sso_string) != LS_SSO_STRING_INVALID;
}

/*
 * Constraints:
 * `sso_string` is not `NULL`
 */
inline const LSByte *ls_sso_string_get_bytes(const LSSSOString *sso_string)
{
	switch (ls_sso_string_get_type(*sso_string)) {
	case LS_SSO_STRING_SHORT:
		return sso_string->_short._mut_bytes;
	case LS_SSO_STRING_LONG:
		return sso_string->_long.bytes;
	default:
		return NULL;
	}
}

/*
 * Constraints:
 * - `bytes` points to a array of at least `len` bytes
 *        OR is `NULL`
 *
 * Fails if:
 * - allocation fails
 * - `bytes` is `NULL`
 */
inline LSString ls_string_create(const LSByte *bytes, size_t len)
{
	if (!bytes) {
		return LS_AN_INVALID_STRING;
	}

	if (len == 0) {
		return LS_EMPTY_STRING;
	}

	return ls__intern_string_create_unchecked(bytes, len);
}

/*
 * Constraints:
 * - `bytes` points to an array of at least `len` bytes
 *        OR is `NULL`
 *
 * Fails if:
 * - `len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `bytes` is `NULL`
 */
inline LSShortString ls_short_string_create(const LSByte *bytes, size_t len)
{
	// null terminator comes free
	LSShortString short_string = LS_AN_INVALID_SHORT_STRING;

	if (len > LS_SHORT_STRING_MAX_LEN
			|| bytes == NULL) {
		return short_string;
	}

	short_string.len = len;
	memcpy(short_string._mut_bytes, bytes, len);

	return short_string;
}

/*
 * Constraints:
 * - `bytes` points to an array of at least `len` bytes
 *        OR is `NULL`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `bytes` is `NULL`
 */
inline LSSSOString ls_sso_string_create(const LSByte *bytes, size_t len)
{
	if (len <= LS_SHORT_STRING_MAX_LEN) {
		return (LSSSOString){
			._short = ls_short_string_create(bytes, len)
		};
	}

	LSString string = bytes == NULL
			? LS_AN_INVALID_STRING
			: ls__intern_string_create_unchecked(bytes, len);
	if (!ls_string_is_valid(string)) {
		return LS_AN_INVALID_SSO_STRING;
	}

	return (LSSSOString){ ._long = string };
}

/*
 * Constraints:
 * - `sso_string` is not `NULL`
 * - `sso_string` was not previously destroyed
 */
inline void ls_sso_string_destroy(LSSSOString *sso_string)
{
	if (ls_sso_string_get_type(*sso_string) == LS_SSO_STRING_LONG) {
		ls_string_destroy(&sso_string->_long);
	}
}

/*
 * Constraints:
 * - `start` points to an array of at least `len` bytes
 *        OR is `NULL`
 *
 * Fails if:
 * - `start` is `NULL`
 */
inline LSStringSpan ls_sspan_create(const LSByte *start, size_t len)
{
	return (LSStringSpan){
		.len = len,
		.start = start
	};
}

/*
 * Fails if:
 *  - allocation fails
 */
inline LSByteBuffer ls_bbuf_create(void)
{
	return ls_bbuf_create_with_init_cap(16);
}

/*
 * Fails if:
 * - allocation fails
 * - `string` is invalid
 */
inline LSString ls_string_clone(LSString string)
{
	return ls_string_create(string.bytes, string.len);
}

/*
 * Fails if:
 * - allocation fails
 * - `short_string` is invalid
 */
inline LSString ls_string_from_short_string(LSShortString short_string)
{
	if (!ls_short_string_is_valid(short_string)) {
		return LS_AN_INVALID_STRING;
	}

	return ls_string_create(short_string._mut_bytes, short_string.len);
}

/*
 * Fails if:
 * - allocation fails
 * - `sso_string` is invalid
 */
inline LSString ls_string_from_sso_string(LSSSOString sso_string)
{
	const LSByte *bytes = ls_sso_string_get_bytes(&sso_string);
	return ls_string_create(bytes, sso_string.len);
}

/*
 * Fails if:
 * - allocation fails
 * - `sspan` is invalid
 */
inline LSString ls_string_from_sspan(LSStringSpan sspan)
{
	return ls_string_create(sspan.start, sspan.len);
}

/*
 * Fails if:
 * - allocation fails
 * - `bbuf` is invalid
 */
inline LSString ls_string_from_bbuf(LSByteBuffer bbuf)
{
	return ls_string_create(bbuf.bytes, bbuf.len);
}

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
 *        OR is `NULL`
 *
 * Fails if:
 * - allocation fails
 * - `chars` is `NULL`
 */
inline LSString ls_string_from_chars(const char *chars, size_t len)
{
	return ls_string_create((const LSByte *)chars, len);
}

/*
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * Fails if:
 * - allocation fails
 * - `cstr` is `NULL`
 */
inline LSString ls_string_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return LS_AN_INVALID_STRING;
	}

	return ls_string_from_chars(cstr, strlen(cstr));
}

/*
 * Fails if:
 * - `string.len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `string` is invalid
 */
inline LSShortString ls_short_string_from_string(LSString string)
{
	return ls_short_string_create(string.bytes, string.len);
}

/*
 * Fails if:
 * - `sspan.len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `sspan` is invalid
 */
inline LSShortString ls_short_string_from_sspan(LSStringSpan sspan)
{
	return ls_short_string_create(sspan.start, sspan.len);
}

/*
 * Fails if:
 * - `bbuf.len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `bbuf` is invalid
 */
inline LSShortString ls_short_string_from_bbuf(LSByteBuffer bbuf)
{
	return ls_short_string_create(bbuf.bytes, bbuf.len);
}

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
 *        OR is `NULL`
 *
 * Fails if:
 * - `len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `chars` is `NULL`
 */
inline LSShortString ls_short_string_from_chars(const char *chars, size_t len)
{
	return ls_short_string_create((const LSByte *)chars, len);
}

/*
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * Fails if:
 * - length of `cstr` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `cstr` is `NULL`
 */
inline LSShortString ls_short_string_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return LS_AN_INVALID_SHORT_STRING;
	}

	return ls_short_string_from_chars(cstr, strlen(cstr));
}

/*
 * Fails if:
 * - allocation is attempted and fails
 * - `sso_string` is invalid
 */
inline LSSSOString ls_sso_string_clone(LSSSOString sso_string)
{
	LSSSOStringType type = ls_sso_string_get_type(sso_string);

	if (type == LS_SSO_STRING_SHORT || type == LS_SSO_STRING_INVALID) {
		return sso_string;
	}

	LSString string = ls_string_clone(sso_string._long);
	if (!ls_string_is_valid(string)) {
		return LS_AN_INVALID_SSO_STRING;
	}

	return (LSSSOString){ ._long = string };
}

/*
 * Fails if:
 * - allocation is attempted and fails
 * - `string` is invalid
 */
inline LSSSOString ls_sso_string_from_string(LSString string)
{
	return ls_sso_string_create(string.bytes, string.len);
}

/*
 * Fails if:
 * - allocation is attempted and fails
 * - `sspan` is invalid
 */
inline LSSSOString ls_sso_string_from_sspan(LSStringSpan sspan)
{
	return ls_sso_string_create(sspan.start, sspan.len);
}

/*
 * Fails if:
 * - allocation is attempted and fails
 * - `bbuf` is invalid
 */
inline LSSSOString ls_sso_string_from_bbuf(LSByteBuffer bbuf)
{
	return ls_sso_string_create(bbuf.bytes, bbuf.len);
}

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
 *        OR is `NULL`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `chars` is `NULL`
 */
inline LSSSOString ls_sso_string_from_chars(const char *chars, size_t len)
{
	return ls_sso_string_create((const LSByte *)chars, len);
}

/*
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `cstr` is invalid
 */
inline LSSSOString ls_sso_string_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return LS_AN_INVALID_SSO_STRING;
	}

	return ls_sso_string_from_chars(cstr, strlen(cstr));
}

/*
 * Resulting `LSStringSpan` does not include the final null-terminator.
 *
 * Fails if:
 * - `string` is invalid
 */
inline LSStringSpan ls_sspan_from_string(LSString string)
{
	return (LSStringSpan){
		.len = string.len,
		.start = string.bytes
	};
}

/*
 * Constraints:
 * - `short_string` is not `NULL`
 *
 * Resulting `LSStringSpan` does not include the final null-terminator.
 *
 * Fails if:
 * - `short_string` is invalid
 */
inline LSStringSpan ls_sspan_from_short_string(LSShortString *short_string)
{
	if (!ls_short_string_is_valid(*short_string)) {
		return LS_AN_INVALID_SSPAN;
	}

	return (LSStringSpan){
		.len = short_string->len,
		.start = short_string->_mut_bytes
	};
}

/*
 * Constraints:
 *  - `sso_string` is not `NULL`
 *
 * Resulting `LSStringSpan` does not include the final null-terminator.
 *
 * Fails if:
 * - `sso_string` is invalid
 */
inline LSStringSpan ls_sspan_from_sso_string(LSSSOString *sso_string)
{
	const LSByte *bytes = ls_sso_string_get_bytes(sso_string);

	return (LSStringSpan){
		.len = sso_string->len,
		.start = bytes
	};
}

/*
 * Fails if:
 * - `bbuf` is invalid
 */
inline LSStringSpan ls_sspan_from_bbuf(LSByteBuffer bbuf)
{
	return ls_sspan_create(bbuf.bytes, bbuf.len);
}

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
 *        OR is `NULL`
 *
 * Fails if:
 * - `chars` is `NULL`
 */
inline LSStringSpan ls_sspan_from_chars(const char *chars, size_t len)
{
	return (LSStringSpan){
		.len = len,
		.start = (const LSByte *)chars
	};
}

/*
 * Resulting `LSStringSpan` does not include the null-terminator.
 *
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * Fails if:
 * - `chars` is `NULL`
 */
inline LSStringSpan ls_sspan_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return LS_AN_INVALID_SSPAN;
	}

	return ls_sspan_from_chars(cstr, strlen(cstr));
}

/*
 * Fails if:
 * - `bbuf` is invalid
 * - allocation fails
 */
inline LSByteBuffer ls_bbuf_clone(LSByteBuffer bbuf)
{
	LSStringSpan contents = ls_sspan_from_bbuf(bbuf);

	return ls_bbuf_from_sspan(contents);
}

/*
 * Fails if:
 * - `string` is invalid
 */
inline LSString ls_string_substr(LSString string, size_t start, size_t len)
{
	return ls_string_from_sspan(ls_string_subspan(string, start, len));
}

/*
 * Fails if:
 * - `string` is invalid
 */
inline LSStringSpan ls_string_subspan(LSString string, size_t start, size_t len)
{
	return ls_sspan_subspan(ls_sspan_from_string(string), start, len);
}

/*
 * Fails if:
 * - `sspan` is invalid
 */
inline LSString ls_sspan_substr(LSStringSpan sspan, size_t start, size_t len)
{
	return ls_string_from_sspan(ls_sspan_subspan(sspan, start, len));
}

/*
 * Fails if:
 * - `sspan` is invalid
 * - the given range doesn't make sense
 */
inline LSStringSpan ls_sspan_subspan(LSStringSpan sspan, size_t start,
		size_t len)
{
	if (!ls_sspan_is_valid(sspan)
			|| start > sspan.len
			|| len > sspan.len
			|| start > sspan.len - len) {
		return LS_AN_INVALID_SSPAN;
	}

	return ls_sspan_create(&sspan.start[start], len);
}

/*
 * Constraints:
 *  - `bbuf` is not `NULL`
 *
 * Fails if:
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_append_string(LSByteBuffer *bbuf, LSString string)
{
	return ls_bbuf_append(bbuf, string.bytes, string.len);
}

/*
 * Constraints:
 *  - `bbuf` is not `NULL`
 *
 * Fails if:
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_append_short_string(LSByteBuffer *bbuf,
		LSShortString short_string)
{
	return ls_bbuf_append(bbuf, short_string._mut_bytes, short_string.len);
}

/*
 * Constraints:
 *  - `bbuf` is not `NULL`
 *
 * Fails if:
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_append_sso_string(LSByteBuffer *bbuf,
		LSSSOString sso_string)
{
	const LSByte *bytes = ls_sso_string_get_bytes(&sso_string);

	return ls_bbuf_append(bbuf, bytes, sso_string.len);
}

/*
 * Constraints:
 *  - `bbuf` is not `NULL`
 *
 * Fails if:
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_append_sspan(LSByteBuffer *bbuf, LSStringSpan sspan)
{
	return ls_bbuf_append(bbuf, sspan.start, sspan.len);
}

/*
 * Constraints:
 *  - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_insert_string(LSByteBuffer *bbuf, size_t idx,
		LSString string)
{
	return ls_bbuf_insert(bbuf, idx, string.bytes, string.len);
}

/*
 * Constraints:
 *  - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_insert_short_string(LSByteBuffer *bbuf, size_t idx,
		LSShortString short_string)
{
	return ls_bbuf_insert(bbuf, idx, short_string._mut_bytes,
			short_string.len);
}

/*
 * Constraints:
 *  - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_insert_sso_string(LSByteBuffer *bbuf, size_t idx,
		LSSSOString sso_string)
{
	const LSByte *bytes = ls_sso_string_get_bytes(&sso_string);

	return ls_bbuf_insert(bbuf, idx, bytes, sso_string.len);
}

/*
 * Constraints:
 *  - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_insert_sspan(LSByteBuffer *bbuf, size_t idx,
		LSStringSpan sspan)
{
	return ls_bbuf_insert(bbuf, idx, sspan.start, sspan.len);
}

#endif // loser_h
