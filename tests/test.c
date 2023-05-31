#include <stdio.h>

#include <loser/loser.h>

int main(void)
{
	LSByteBuffer bbuf = ls_bbuf_create();

	ls_bbuf_append_sspan(&bbuf, ls_sspan_from_cstr("Hello, my cool friend!"));

	for (size_t i = 0; i < 32; ++i) {
		ls_bbuf_insert_sspan(&bbuf, 10, ls_sspan_from_cstr("very "));
	}

	printf("%.*s\n", (int)bbuf.len, (const char *)bbuf.bytes);

	ls_bbuf_destroy(&bbuf);

	return 0;
}
