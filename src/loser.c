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

	LSByte *bytes_cpy = TYRANT_ALLOC_ARR(bytes, len + 1);
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
