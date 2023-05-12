#include "loser.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <tyrant/tyrant.h>

LSString ls_string_create(const LSByte *bytes, size_t len)
{
	if (!bytes) {
		goto err_exit;
	}

	LSByte *bytes_cpy = tyrant_alloc(len + 1);
	if (!bytes_cpy) {
		goto err_exit;
	}

	memcpy(bytes_cpy, bytes, len);
	bytes_cpy[len] = '\0';

	return (LSString){
		.bytes = bytes_cpy,
		.len = len
	};

err_exit:
	return (LSString){ 0 };
}

void ls_string_destroy(LSString *string)
{
	LSByte *bytes_mutable = (LSByte *)string->bytes;
	tyrant_free(bytes_mutable);
}

LSByteBuffer ls_bbuf_create_with_init_cap(size_t cap)
{
	if (cap == 0) {
		goto err_exit;
	}

	LSByte *bytes = tyrant_alloc(cap);
	if (!bytes) {
		goto err_exit;
	}

	return (LSByteBuffer){
		.cap = cap,
		.len = 0,
		.bytes = bytes
	};

err_exit:
	return LS_AN_INVALID_BBUF;
}

void ls_bbuf_destroy(LSByteBuffer *bbuf)
{
	tyrant_free(bbuf->bytes);
}
