#include "loser.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <seifu/seifu.h>
#include <tyrant/tyrant.h>

static const LSByte EMPTY_STRING_BYTES[] = "";
const LSString LS_EMPTY_STRING = {
	.len = 0,
	.bytes = EMPTY_STRING_BYTES
};

#define LS_LINKAGE extern
#include "loser-inline-decls.h"
#undef LS_LINKAGE

static LSStatus bbuf_reserve_space(LSByteBuffer *bbuf, size_t len);
static size_t three_halves_geom_growth(size_t cap);
static size_t size_max(size_t a, size_t b);

LSString ls__intern_string_create_unchecked(const LSByte *bytes, size_t len)
{
	LSByte *bytes_cpy = tyrant_alloc(len + 1);
	if (!bytes_cpy) {
		return LS_AN_INVALID_STRING;
	}

	memcpy(bytes_cpy, bytes, len);
	bytes_cpy[len] = '\0';

	return (LSString){
		.len = len,
		.bytes = bytes_cpy
	};
}

void ls_string_destroy(LSString *string)
{
	if (string->bytes == EMPTY_STRING_BYTES) {
		return;
	}

	LSByte *bytes_mutable = (LSByte *)string->bytes;
	tyrant_free(bytes_mutable);
}

LSByteBuffer ls_bbuf_create_with_init_cap(size_t cap)
{
	if (cap == 0) {
		return LS_AN_INVALID_BBUF;
	}

	LSByte *bytes = tyrant_alloc(cap);
	if (!bytes) {
		return LS_AN_INVALID_BBUF;
	}

	return (LSByteBuffer){
		.len = 0,
		.cap = cap,
		.bytes = bytes
	};
}

void ls_bbuf_destroy(LSByteBuffer *bbuf)
{
	tyrant_free(bbuf->bytes);
}

LSShortString ls_short_string_from_sso(LSSSOString sso)
{
	if (ls_sso_get_type(sso) != LS_SSO_SHORT) {
		return LS_AN_INVALID_SHORT_STRING;
	}

	return sso._short;
}

LSSSOString ls_sso_from_short_string(LSShortString short_string)
{
	if (!ls_short_string_is_valid(short_string)) {
		return LS_AN_INVALID_SSO;
	}

	return (LSSSOString){ ._short = short_string };
}

LSByteBuffer ls_bbuf_from_sspan(LSStringSpan sspan)
{
	if (!ls_sspan_is_valid(sspan)) {
		return LS_AN_INVALID_BBUF;
	}

	size_t cap = size_max(8, sspan.len);
	LSByteBuffer bbuf = ls_bbuf_create_with_init_cap(cap);
	if (!ls_bbuf_is_valid(bbuf)) {
		return LS_AN_INVALID_BBUF;
	}

	memcpy(bbuf.bytes, sspan.start, sspan.len);
	bbuf.len = sspan.len;

	return bbuf;
}

LSStatus ls_bbuf_append(LSByteBuffer *bbuf, const LSByte *bytes, size_t len)
{
	if (!ls_bbuf_is_valid(*bbuf)
			|| !bytes) {
		return LS_FAILURE;
	}

	if (bbuf_reserve_space(bbuf, len) != LS_SUCCESS) {
		return LS_FAILURE;
	}

	LSByte *bytes_dest = &bbuf->bytes[bbuf->len];
	memcpy(bytes_dest, bytes, len);

	bbuf->len += len;

	return LS_SUCCESS;
}

LSStatus ls_bbuf_insert(LSByteBuffer *bbuf, size_t idx, const LSByte *bytes,
		size_t len)
{
	if (!ls_bbuf_is_valid(*bbuf)
			|| !bytes
			|| idx > bbuf->len) {
		return LS_FAILURE;
	}

	// TODO check for append case?

	if (bbuf_reserve_space(bbuf, len) != LS_SUCCESS) {
		return LS_FAILURE;
	}

	LSByte *moving_bytes = &bbuf->bytes[idx];
	size_t nmoving_bytes = bbuf->len - idx;
	LSByte *moving_bytes_dest = &bbuf->bytes[idx + len];
	memmove(moving_bytes_dest, moving_bytes, nmoving_bytes);

	LSByte *bytes_dest = &bbuf->bytes[idx];
	memcpy(bytes_dest, bytes, len);

	bbuf->len += len;

	return LS_SUCCESS;
}

LSStatus ls_bbuf_expand_to(LSByteBuffer *bbuf, size_t new_cap)
{
	if (!ls_bbuf_is_valid(*bbuf)) {
		return LS_FAILURE;
	}

	if (new_cap <= bbuf->cap) {
		return LS_FAILURE;
	}

	bool success;
	bbuf->bytes = tyrant_realloc(bbuf->bytes, new_cap, &success);
	if (!success) {
		return LS_FAILURE;
	}

	bbuf->cap = new_cap;

	return LS_SUCCESS;
}

LSStatus ls_bbuf_expand_by(LSByteBuffer *bbuf, size_t add_cap)
{
	if (!ls_bbuf_is_valid(*bbuf)) {
		return LS_FAILURE;
	}

	if (add_cap == 0) {
		return 0;
	}

	size_t new_cap;
	SeifuStatus status = seifu_add(bbuf->cap, add_cap, &new_cap);
	if (status != SEIFU_OK) {
		return LS_FAILURE;
	}

	return ls_bbuf_expand_to(bbuf, new_cap);
}

bool ls_string_equals(LSString a, LSString b)
{
	return a.len == b.len
			&& ls_string_is_valid(a) && ls_string_is_valid(b)
			&& ls_bytes_equals(a.bytes, b.bytes, a.len);
}

bool ls_short_string_equals(LSShortString a, LSShortString b)
{
	return a.len == b.len
			&& ls_short_string_is_valid(a) // implies b is valid
			&& ls_bytes_equals(a._mut_bytes, b._mut_bytes, a.len);
}

bool ls_sso_equals(LSSSOString a, LSSSOString b)
{
	const LSByte *a_bytes = ls_sso_get_bytes(&a);
	const LSByte *b_bytes = ls_sso_get_bytes(&b);

	return a.len == b.len
			&& a_bytes != NULL && b_bytes != NULL
			&& ls_bytes_equals(a_bytes, b_bytes, a.len);
}

bool ls_sspan_equals(LSStringSpan a, LSStringSpan b)
{
	return a.len == b.len
			&& ls_sspan_is_valid(a) && ls_sspan_is_valid(b)
			&& ls_bytes_equals(a.start, b.start, a.len);
}

bool ls_bytes_equals(const LSByte a[static 1], const LSByte b[static 1],
		size_t len)
{
	return memcmp(a, b, len) == 0;
}

LSStatus bbuf_reserve_space(LSByteBuffer *bbuf, size_t len)
{
	size_t new_len = bbuf->len + len;
	if (new_len > bbuf->cap) {
		size_t geom_growth = three_halves_geom_growth(bbuf->cap);
		size_t new_cap = size_max(geom_growth, new_len);

		LSStatus status = ls_bbuf_expand_to(bbuf, new_cap);
		if (status != LS_SUCCESS) {
			return LS_FAILURE;
		}
	}

	return LS_SUCCESS;
}

size_t three_halves_geom_growth(size_t cap)
{
	return seifu_add_bounded(cap, seifu_div_round_bounded(cap, 2));
}

size_t size_max(size_t a, size_t b)
{
	return a > b ? a : b;
}
