#include "loser.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <tyrant/tyrant.h>

static size_t geom_grow_size(size_t cap);

LSString ls_string_create(const LSByte *bytes, size_t len)
{
	if (!bytes) {
		return LS_AN_INVALID_STRING;
	}

	LSByte *bytes_cpy = tyrant_alloc(len + 1);
	if (!bytes_cpy) {
		return LS_AN_INVALID_STRING;
	}

	memcpy(bytes_cpy, bytes, len);
	bytes_cpy[len] = '\0';

	return (LSString){
		.bytes = bytes_cpy,
		.len = len
	};
}

void ls_string_destroy(LSString *string)
{
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
		.cap = cap,
		.len = 0,
		.bytes = bytes
	};
}

void ls_bbuf_destroy(LSByteBuffer *bbuf)
{
	tyrant_free(bbuf->bytes);
}

LSStatus ls_bbuf_append(LSByteBuffer *bbuf, const LSByte *bytes, size_t len)
{
	size_t new_len = bbuf->len + len;
	if (new_len > bbuf->cap) {
		size_t new_cap = geom_grow_size(bbuf->cap);
		LSStatus status = ls_bbuf_expand(bbuf, new_cap);
		if (status != LS_SUCCESS) {
			return LS_FAILURE;
		}
	}

	LSByte *bytes_dest = &bbuf->bytes[bbuf->len];
	memcpy(bytes_dest, bytes, len);

	bbuf->len = new_len;

	return LS_SUCCESS;
}

LSStatus ls_bbuf_insert(LSByteBuffer *bbuf, size_t idx, const LSByte *bytes,
		size_t len)
{
	if (idx > bbuf->len) {
		return LS_FAILURE;
	}

	// TODO check for append case?

	size_t new_len = bbuf->len + len;
	if (new_len > bbuf->cap) {
		size_t new_cap = geom_grow_size(bbuf->cap);
		LSStatus status = ls_bbuf_expand(bbuf, new_cap);
		if (status != LS_SUCCESS) {
			return LS_FAILURE;
		}
	}

	LSByte *moving_bytes = &bbuf->bytes[idx];
	size_t nmoving_bytes = bbuf->len - idx;
	LSByte *moving_bytes_dest = &bbuf->bytes[idx + len];
	memmove(moving_bytes_dest, moving_bytes, nmoving_bytes);

	LSByte *bytes_dest = &bbuf->bytes[idx];
	memcpy(bytes_dest, bytes, len);

	bbuf->len = new_len;

	return LS_SUCCESS;
}

LSStatus ls_bbuf_expand(LSByteBuffer *bbuf, size_t new_cap)
{
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

size_t geom_grow_size(size_t cap)
{
	size_t half_cap = cap / 2;
	size_t half_cap_rounded = half_cap + (cap % 2 == 1);
	return tyrant_add_size_capped(
			cap, half_cap_rounded, // 3/2 growth rate
			SIZE_MAX,              // capped at SIZE_MAX
			&(bool){ 0 });
}
