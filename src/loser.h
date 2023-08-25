#ifndef loser_h
#define loser_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/*
 * NOTE: Library guarantees may not hold true for objects created or modified
 * manually.
 *
 * NOTE: The contents of objects which have "invalid" values should not be
 * inspected as they may contain unspecified/trap values. This restriction is
 * reserved by the library for the sake of speed.
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
	LS_SSO_INVALID,
	LS_SSO_SHORT,
	LS_SSO_LONG
} LSSSOStringType;

// A non-owning range of bytes.
/*
 * Might not be null-terminated.
 */
typedef struct LSStringSpan {
	size_t len;
	const LSByte *bytes;
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

// The empty string constant (there can only be one).
extern const LSString LS_EMPTY_STRING;

// Other empty constants.
static const LSByte LS_EMPTY_BYTES[] = "";
static const LSShortString LS_EMPTY_SHORT_STRING = { 0 };
static const LSSSOString LS_EMPTY_SSO = { 0 };
static const LSStringSpan LS_EMPTY_SSPAN = {
	.len = 0,
	.bytes = LS_EMPTY_BYTES
};

/*
 * NOTE: As their names imply, the following constants are not the only invalid
 * values for their respective types but *an* invalid value of that type.
 * Thus, these should not be tested against. Instead, use the appropriate
 * validity-checking functions.
 */
#define LS_AN_INVALID_STRING (LSString){ .bytes = NULL }
#define LS_AN_INVALID_SSO \
	(LSSSOString){ ._long.len = SIZE_MAX, ._long.bytes = NULL }
#define LS_AN_INVALID_SHORT_STRING (LSShortString){ .len = SIZE_MAX }
#define LS_AN_INVALID_SSPAN (LSStringSpan){ .bytes = NULL }
#define LS_AN_INVALID_BBUF (LSByteBuffer){ .bytes = NULL }

#define LS_LINKAGE inline
#include "loser-inline-decls.h"
#undef LS_LINKAGE

/*
 * Constraints:
 * - `bytes` points to a array of at least `len` bytes
 *        OR is `NULL`
 *
 * Fails if:
 * - allocation fails
 * - `bytes` is `NULL`
 */
LSString ls_string_create(const LSByte *bytes, size_t len);

/*
 * Constraints:
 * - `string` is not `NULL`
 * - `string` was not previously destroyed
 */
void ls_string_destroy(LSString *string);

/*
 * Constraints:
 * - `bytes` points to an array of at least `len` bytes
 *        OR is `NULL`
 *
 * Fails if:
 * - `len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `bytes` is `NULL`
 */
LSShortString ls_short_string_create(const LSByte *bytes, size_t len);

/*
 * Constraints:
 * - `bytes` points to an array of at least `len` bytes
 *        OR is `NULL`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `bytes` is `NULL`
 */
LSSSOString ls_sso_create(const LSByte *bytes, size_t len);

/*
 * Constraints:
 * - `sso` is not `NULL`
 * - `sso` was not previously destroyed
 */
void ls_sso_destroy(LSSSOString *sso);

/*
 * Fails if:
 *  - allocation fails
 */
LSByteBuffer ls_bbuf_create(void);

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
 * - allocation fails
 * - `string` is invalid
 */
LSString ls_string_clone(LSString string);

/*
 * Fails if:
 * - allocation fails
 * - `short_string` is invalid
 */
LSString ls_string_from_short_string(LSShortString short_string);

/*
 * Fails if:
 * - allocation fails
 * - `sspan` is invalid
 */
LSString ls_string_from_sspan(LSStringSpan sspan);

/*
 * Fails if:
 * - allocation fails
 * - `bbuf` is invalid
 */
LSString ls_string_from_bbuf(LSByteBuffer bbuf);

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
 *        OR is `NULL`
 *
 * Fails if:
 * - allocation fails
 * - `chars` is `NULL`
 */
LSString ls_string_from_chars(const char *chars, size_t len);

/*
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * Fails if:
 * - allocation fails
 * - `cstr` is `NULL`
 */
LSString ls_string_from_cstr(const char *cstr);

/*
 * Fails if:
 * - `string.len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `string` is invalid
 */
LSShortString ls_short_string_from_string(LSString string);

/*
 * Fails if:
 * - `sspan.len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `sspan` is invalid
 */
LSShortString ls_short_string_from_sspan(LSStringSpan sspan);

/*
 * Fails if:
 * - `bbuf.len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `bbuf` is invalid
 */
LSShortString ls_short_string_from_bbuf(LSByteBuffer bbuf);

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
 *        OR is `NULL`
 *
 * Fails if:
 * - `len` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `chars` is `NULL`
 */
LSShortString ls_short_string_from_chars(const char *chars, size_t len);

/*
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * Fails if:
 * - length of `cstr` is greater than `LS_SHORT_STRING_MAX_LEN`
 * - `cstr` is `NULL`
 */
LSShortString ls_short_string_from_cstr(const char *cstr);

/*
 * Fails if:
 * - allocation is attempted and fails
 * - `string` is invalid
 */
LSSSOString ls_sso_from_string(LSString string);

/*
 * Fails if:
 * - allocation is attempted and fails
 * - `sspan` is invalid
 */
LSSSOString ls_sso_from_sspan(LSStringSpan sspan);

/*
 * Fails if:
 * - allocation is attempted and fails
 * - `bbuf` is invalid
 */
LSSSOString ls_sso_from_bbuf(LSByteBuffer bbuf);

/*
 * Constraints:
 * - `chars` points to an array of at least `len` `char`s
 *        OR is `NULL`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `chars` is `NULL`
 */
LSSSOString ls_sso_from_chars(const char *chars, size_t len);

/*
 * Constraints:
 * - `cstr` points to a null-terminated array of `char`s
 *       OR is `NULL`
 *
 * Fails if:
 * - allocation is attempted and fails
 * - `cstr` is invalid
 */
LSSSOString ls_sso_from_cstr(const char *cstr);

/*
 * Fails if:
 * - allocation is attempted and fails
 * - `sso` is invalid
 */
LSSSOString ls_sso_clone(LSSSOString sso);

/*
 * Fails if:
 * - `sspan` is invalid
 * - allocation fails
 */
LSByteBuffer ls_bbuf_from_sspan(LSStringSpan sspan);

/*
 * Fails if:
 * - `bbuf` is invalid
 * - allocation fails
 */
LSByteBuffer ls_bbuf_clone(LSByteBuffer bbuf);

/*
 * Constraints:
 * `string` is not `NULL`
 */
LSSSOString ls_string_move_to_sso(LSString *string);

/*
 * Constraints:
 * `sso` is not `NULL`
 */
LSString ls_sso_move_to_string(LSSSOString *sso);

/*
 * Constraints:
 * `bbuf` is not `NULL`
 */
LSSSOString ls_bbuf_finalize_as_sso(LSByteBuffer *bbuf);

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
 * - `string` is invalid
 * - reallocation is attempted and fails
 */
LSStatus ls_bbuf_append_string(LSByteBuffer *bbuf, LSString string);

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `sspan` is invalid
 * - reallocation is attempted and fails
 */
LSStatus ls_bbuf_append_sspan(LSByteBuffer *bbuf, LSStringSpan sspan);

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
 * - `string` is invalid
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
LSStatus ls_bbuf_insert_string(LSByteBuffer *bbuf, size_t idx,
		LSString string);

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `sspan` is invalid
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
LSStatus ls_bbuf_insert_sspan(LSByteBuffer *bbuf, size_t idx,
		LSStringSpan sspan);

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

/*
 * NOTE: An invalid value never compares equal to another value--not even
 * another invalid value.
 */
bool ls_string_equals(LSString a, LSString b);
bool ls_short_string_equals(LSShortString a, LSShortString b);
bool ls_sso_equals(LSSSOString a, LSSSOString b);
bool ls_sspan_equals(LSStringSpan a, LSStringSpan b);

/*
 * Constraints:
 * - `a` and `b` each point to an array of at least `len` bytes
 *
 * TODO: Should this function really be exposed? Feels like a bit of a footgun.
 */
bool ls_bytes_equals(const LSByte a[static 1], const LSByte b[static 1],
		size_t len);

/*
 * Constraints:
 * - `a` and `b` each point to an array of at least `len` bytes
 *            OR `a` or `b` is `NULL`
 *
 * NOTE: `NULL` will never compare equal--not even to itself.
 */
bool ls_bytes_equals_nullsafe(const LSByte *a, const LSByte *b, size_t len);

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
	if (!ls_short_string_is_valid(*short_string)) {
		return NULL;
	}

	return short_string->_mut_bytes;
}

inline bool ls_sspan_is_valid(LSStringSpan sspan)
{
	return sspan.bytes != NULL;
}

inline bool ls_bbuf_is_valid(LSByteBuffer bbuf)
{
	return bbuf.bytes != NULL;
}

inline LSSSOStringType ls_sso_get_type(LSSSOString sso)
{
	if (ls_short_string_is_valid(sso._short)) {
		return LS_SSO_SHORT;
	}

	if (ls_string_is_valid(sso._long)) {
		return LS_SSO_LONG;
	}

	return LS_SSO_INVALID;
}

inline bool ls_sso_is_valid(LSSSOString sso)
{
	return ls_sso_get_type(sso) != LS_SSO_INVALID;
}

/*
 * Constraints:
 * `sso` is not `NULL`
 */
inline const LSByte *ls_sso_get_bytes(const LSSSOString *sso)
{
	switch (ls_sso_get_type(*sso)) {
	case LS_SSO_SHORT:
		return sso->_short._mut_bytes;
	case LS_SSO_LONG:
		return sso->_long.bytes;
	default:
		return NULL;
	}
}

/*
 * Constraints:
 * - `bytes` points to an array of at least `len` bytes
 *        OR is `NULL`
 *
 * Fails if:
 * - `bytes` is `NULL`
 */
inline LSStringSpan ls_sspan_create(const LSByte *bytes, size_t len)
{
	return (LSStringSpan){
		.len = len,
		.bytes = bytes
	};
}

/*
 * Constraints:
 * - `string` is not `NULL`
 */
inline void ls_string_invalidate(LSString *string)
{
	string->bytes = NULL;
}

/*
 * Constraints:
 * - `short_string` is not `NULL`
 */
inline void ls_short_string_invalidate(LSShortString *short_string)
{
	short_string->len = SIZE_MAX;
}

/*
 * Constraints:
 * - `sso` is not `NULL`
 */
inline void ls_sso_invalidate(LSSSOString *sso)
{
	sso->_long = (LSString){
		.len = SIZE_MAX,
		.bytes = NULL
	};
}

/*
 * Constraints:
 * - `sspan` is not `NULL`
 */
inline void ls_sspan_invalidate(LSStringSpan *sspan)
{
	sspan->bytes = NULL;
}

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 */
inline void ls_bbuf_invalidate(LSByteBuffer *bbuf)
{
	bbuf->bytes = NULL;
}

/*
 * Constraints:
 * `string` is not `NULL`
 */
inline LSString ls_string_move(LSString *string)
{
	LSString mv = *string;
	ls_string_invalidate(string);

	return mv;
}

/*
 * Constraints:
 * `sso` is not `NULL`
 */
inline LSSSOString ls_sso_move(LSSSOString *sso)
{
	LSSSOString mv = *sso;
	ls_sso_invalidate(sso);

	return mv;
}

/*
 * Constraints:
 * `bbuf` is not `NULL`
 */
inline LSByteBuffer ls_bbuf_move(LSByteBuffer *bbuf)
{
	LSByteBuffer mv = *bbuf;
	ls_bbuf_invalidate(bbuf);

	return mv;
}

/*
 * Constraints:
 * `bbuf` is not `NULL`
 */
inline LSString ls_bbuf_finalize(LSByteBuffer *bbuf)
{
	LSString mv = {
		.len = bbuf->len,
		.bytes = bbuf->bytes
	};

	ls_bbuf_invalidate(bbuf);

	return mv;
}

/*
 * Fails if:
 * - allocation fails
 * - `sso` is invalid
 */
inline LSString ls_string_from_sso(LSSSOString sso)
{
	const LSByte *bytes = ls_sso_get_bytes(&sso);
	return ls_string_create(bytes, sso.len);
}

/*
 * Fails if:
 * - `sso` doesn't contain a short string
 */
inline LSShortString ls_short_string_from_sso(LSSSOString sso)
{
	return sso._short; // potential (well-defined) type-punning
}

/*
 * Fails if:
 * - `short_string` is invalid
 */
inline LSSSOString ls_sso_from_short_string(LSShortString short_string)
{
	if (!ls_short_string_is_valid(short_string)) {
		return LS_AN_INVALID_SSO;
	}

	return (LSSSOString){ ._short = short_string };
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
		.bytes = string.bytes
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
inline LSStringSpan ls_sspan_from_short_string(
		const LSShortString *short_string)
{
	if (!ls_short_string_is_valid(*short_string)) {
		return LS_AN_INVALID_SSPAN;
	}

	return (LSStringSpan){
		.len = short_string->len,
		.bytes = short_string->_mut_bytes
	};
}

/*
 * Constraints:
 *  - `sso` is not `NULL`
 *
 * Resulting `LSStringSpan` does not include the final null-terminator.
 *
 * Fails if:
 * - `sso` is invalid
 */
inline LSStringSpan ls_sspan_from_sso(const LSSSOString *sso)
{
	const LSByte *bytes = ls_sso_get_bytes(sso);

	return (LSStringSpan){
		.len = sso->len,
		.bytes = bytes
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
		.bytes = (const LSByte *)chars
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
 * - `string` is invalid
 */
inline LSString ls_string_substr(LSString string, size_t bytes, size_t len)
{
	return ls_string_from_sspan(ls_string_subspan(string, bytes, len));
}

/*
 * Fails if:
 * - `string` is invalid
 */
inline LSStringSpan ls_string_subspan(LSString string, size_t bytes, size_t len)
{
	return ls_sspan_subspan(ls_sspan_from_string(string), bytes, len);
}

/*
 * Fails if:
 * - `sspan` is invalid
 */
inline LSString ls_sspan_substr(LSStringSpan sspan, size_t bytes, size_t len)
{
	return ls_string_from_sspan(ls_sspan_subspan(sspan, bytes, len));
}

/*
 * Fails if:
 * - `sspan` is invalid
 * - the given range doesn't make sense
 */
inline LSStringSpan ls_sspan_subspan(LSStringSpan sspan, size_t bytes,
		size_t len)
{
	if (!ls_sspan_is_valid(sspan)
			|| bytes > sspan.len
			|| len > sspan.len
			|| bytes > sspan.len - len) {
		return LS_AN_INVALID_SSPAN;
	}

	return ls_sspan_create(&sspan.bytes[bytes], len);
}

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `short_string` is invalid
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_append_short_string(LSByteBuffer *bbuf,
		LSShortString short_string)
{
	const LSByte *bytes = ls_short_string_get_bytes(&short_string);

	return ls_bbuf_append(bbuf, bytes, short_string.len);
}

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `sso` is invalid
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_append_sso(LSByteBuffer *bbuf, LSSSOString sso)
{
	const LSByte *bytes = ls_sso_get_bytes(&sso);

	return ls_bbuf_append(bbuf, bytes, sso.len);
}

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `short_string` is invalid
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_insert_short_string(LSByteBuffer *bbuf, size_t idx,
		LSShortString short_string)
{
	const LSByte *bytes = ls_short_string_get_bytes(&short_string);

	return ls_bbuf_insert(bbuf, idx, bytes, short_string.len);
}

/*
 * Constraints:
 * - `bbuf` is not `NULL`
 *
 * Fails if:
 * - `bbuf` is invalid
 * - `sso` is invalid
 * - `idx` is greater than `bbuf->len`
 * - reallocation is attempted and fails
 */
inline LSStatus ls_bbuf_insert_sso(LSByteBuffer *bbuf, size_t idx,
		LSSSOString sso)
{
	const LSByte *bytes = ls_sso_get_bytes(&sso);

	return ls_bbuf_insert(bbuf, idx, bytes, sso.len);
}

#endif // loser_h
