#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <loser/loser.h>

static void test_constructors(void);
static void test_conversions(void);
static void test_append_funcs(void);
static void test_insert_funcs(void);

static const LSByte NONEMPTY_BYTES[] = "deadbeef";
static size_t NONEMPTY_LEN = sizeof(NONEMPTY_BYTES) - 1;

int main(void)
{
	test_constructors();
	test_conversions();
	test_append_funcs();
	test_insert_funcs();

	return 0;
}

void test_constructors(void)
{
	{
		LSString from_empty = ls_string_create(LS_EMPTY_BYTES, 0);
		LSString from_nonempty = ls_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
		LSString from_null = ls_string_create(NULL, 0);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSShortString from_empty = ls_short_string_create(LS_EMPTY_BYTES, 0);
		LSShortString from_nonempty = ls_short_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
		LSShortString from_null = ls_short_string_create(NULL, 0);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty._mut_bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSSSOString from_empty = ls_sso_string_create(LS_EMPTY_BYTES, 0);
		LSSSOString from_nonempty = ls_sso_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
		LSSSOString from_null = ls_sso_string_create(NULL, 0);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSStringSpan from_empty = ls_sspan_create(LS_EMPTY_BYTES, 0);
		LSStringSpan from_nonempty = ls_sspan_create(NONEMPTY_BYTES, NONEMPTY_LEN);
		LSStringSpan from_null = ls_sspan_create(NULL, 0);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.start, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_null));
	}
}

void test_conversions(void)
{
	LSString nonempty_string = ls_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSShortString nonempty_short_string = ls_short_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSSSOString nonempty_sso_string = ls_sso_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSStringSpan nonempty_sspan = ls_sspan_create(NONEMPTY_BYTES, NONEMPTY_LEN);

	LSByteBuffer empty_bbuf = ls_bbuf_create();

	LSByteBuffer nonempty_bbuf = ls_bbuf_create_with_init_cap(NONEMPTY_LEN);
	memcpy(nonempty_bbuf.bytes, NONEMPTY_BYTES, NONEMPTY_LEN);
	nonempty_bbuf.len = NONEMPTY_LEN;

	{
		LSString from_empty = ls_string_clone(LS_EMPTY_STRING);
		LSString from_nonempty = ls_string_clone(nonempty_string);
		LSString from_invalid = ls_string_clone(LS_AN_INVALID_STRING);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_short_string(LS_EMPTY_SHORT_STRING);
		LSString from_nonempty = ls_string_from_short_string(nonempty_short_string);
		LSString from_invalid = ls_string_from_short_string(LS_AN_INVALID_SHORT_STRING);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_sso_string(LS_EMPTY_SSO_STRING);
		LSString from_nonempty = ls_string_from_sso_string(nonempty_sso_string);
		LSString from_invalid = ls_string_from_sso_string(LS_AN_INVALID_SSO_STRING);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_sspan(LS_EMPTY_SSPAN);
		LSString from_nonempty = ls_string_from_sspan(nonempty_sspan);
		LSString from_invalid = ls_string_from_sspan(LS_AN_INVALID_SSPAN);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_bbuf(empty_bbuf);
		LSString from_nonempty = ls_string_from_bbuf(nonempty_bbuf);
		LSString from_invalid = ls_string_from_bbuf(LS_AN_INVALID_BBUF);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_chars((const char *)LS_EMPTY_BYTES, 0);
		LSString from_nonempty = ls_string_from_chars((const char *)NONEMPTY_BYTES, NONEMPTY_LEN);
		LSString from_null = ls_string_from_chars(NULL, 0);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_cstr((const char *)LS_EMPTY_BYTES);
		LSString from_nonempty = ls_string_from_cstr((const char *)NONEMPTY_BYTES);
		LSString from_null = ls_string_from_cstr(NULL);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}

	{
		LSShortString from_empty = ls_short_string_from_string(LS_EMPTY_STRING);
		LSShortString from_nonempty = ls_short_string_from_string(nonempty_string);
		LSShortString from_invalid = ls_short_string_from_string(LS_AN_INVALID_STRING);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty._mut_bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_sso_string(LS_EMPTY_SSO_STRING);
		LSShortString from_nonempty = ls_short_string_from_sso_string(nonempty_sso_string);
		LSShortString from_invalid = ls_short_string_from_sso_string(LS_AN_INVALID_SSO_STRING);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty._mut_bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_sspan(LS_EMPTY_SSPAN);
		LSShortString from_nonempty = ls_short_string_from_sspan(nonempty_sspan);
		LSShortString from_invalid = ls_short_string_from_sspan(LS_AN_INVALID_SSPAN);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty._mut_bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_bbuf(empty_bbuf);
		LSShortString from_nonempty = ls_short_string_from_bbuf(nonempty_bbuf);
		LSShortString from_invalid = ls_short_string_from_bbuf(LS_AN_INVALID_BBUF);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty._mut_bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_chars((const char *)LS_EMPTY_BYTES, 0);
		LSShortString from_nonempty = ls_short_string_from_chars((const char *)NONEMPTY_BYTES, NONEMPTY_LEN);
		LSShortString from_null = ls_short_string_from_chars(NULL, 0);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty._mut_bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_cstr((const char *)LS_EMPTY_BYTES);
		LSShortString from_nonempty = ls_short_string_from_cstr((const char *)NONEMPTY_BYTES);
		LSShortString from_null = ls_short_string_from_cstr(NULL);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty._mut_bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}

	{
		LSSSOString from_empty = ls_sso_string_from_string(LS_EMPTY_STRING);
		LSSSOString from_nonempty = ls_sso_string_from_string(nonempty_string);
		LSSSOString from_invalid = ls_sso_string_from_string(LS_AN_INVALID_STRING);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_from_short_string(LS_EMPTY_SHORT_STRING);
		LSSSOString from_nonempty = ls_sso_string_from_short_string(nonempty_short_string);
		LSSSOString from_invalid = ls_sso_string_from_short_string(LS_AN_INVALID_SHORT_STRING);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_clone(LS_EMPTY_SSO_STRING);
		LSSSOString from_nonempty = ls_sso_string_clone(nonempty_sso_string);
		LSSSOString from_invalid = ls_sso_string_clone(LS_AN_INVALID_SSO_STRING);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_from_sspan(LS_EMPTY_SSPAN);
		LSSSOString from_nonempty = ls_sso_string_from_sspan(nonempty_sspan);
		LSSSOString from_invalid = ls_sso_string_from_sspan(LS_AN_INVALID_SSPAN);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_from_bbuf(empty_bbuf);
		LSSSOString from_nonempty = ls_sso_string_from_bbuf(nonempty_bbuf);
		LSSSOString from_invalid = ls_sso_string_from_bbuf(LS_AN_INVALID_BBUF);
		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_from_chars((const char *)LS_EMPTY_BYTES, 0);
		LSSSOString from_nonempty = ls_sso_string_from_chars((const char *)NONEMPTY_BYTES, NONEMPTY_LEN);
		LSSSOString from_null = ls_sso_string_from_chars(NULL, 0);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_from_cstr((const char *)LS_EMPTY_BYTES);
		LSSSOString from_nonempty = ls_sso_string_from_cstr((const char *)NONEMPTY_BYTES);
		LSSSOString from_null = ls_sso_string_from_cstr(NULL);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_null));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}

	{
		LSStringSpan from_empty = ls_sspan_from_string(LS_EMPTY_STRING);
		LSStringSpan from_nonempty = ls_sspan_from_string(nonempty_string);
		LSStringSpan from_invalid = ls_sspan_from_string(LS_AN_INVALID_STRING);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.start, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSStringSpan from_empty = ls_sspan_from_short_string(&LS_EMPTY_SHORT_STRING);
		LSStringSpan from_nonempty = ls_sspan_from_short_string(&nonempty_short_string);
		LSStringSpan from_invalid = ls_sspan_from_short_string(&LS_AN_INVALID_SHORT_STRING);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.start, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSStringSpan from_empty = ls_sspan_from_sso_string(&LS_EMPTY_SSO_STRING);
		LSStringSpan from_nonempty = ls_sspan_from_sso_string(&nonempty_sso_string);
		LSStringSpan from_invalid = ls_sspan_from_sso_string(&LS_AN_INVALID_SSO_STRING);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.start, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}
	{
		LSStringSpan from_empty = ls_sspan_from_bbuf(empty_bbuf);
		LSStringSpan from_nonempty = ls_sspan_from_bbuf(nonempty_bbuf);
		LSStringSpan from_invalid = ls_sspan_from_bbuf(LS_AN_INVALID_BBUF);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.start, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
	}

	{
		LSByteBuffer from_empty = ls_bbuf_from_sspan(LS_EMPTY_SSPAN);
		LSByteBuffer from_nonempty = ls_bbuf_from_sspan(nonempty_sspan);
		LSByteBuffer from_invalid = ls_bbuf_from_sspan(LS_AN_INVALID_SSPAN);

		assert(ls_bbuf_is_valid(from_empty));
		assert(ls_bbuf_is_valid(from_nonempty));
		assert(!ls_bbuf_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);
		
		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_clone(empty_bbuf);
		LSByteBuffer from_nonempty = ls_bbuf_clone(nonempty_bbuf);
		LSByteBuffer from_invalid = ls_bbuf_clone(LS_AN_INVALID_BBUF);

		assert(ls_bbuf_is_valid(from_empty));
		assert(ls_bbuf_is_valid(from_nonempty));
		assert(!ls_bbuf_is_valid(from_invalid));

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
	}

	ls_string_destroy(&nonempty_string);
	ls_sso_string_destroy(&nonempty_sso_string);

	ls_bbuf_destroy(&empty_bbuf);
	ls_bbuf_destroy(&nonempty_bbuf);
}

void test_append_funcs(void)
{
	LSString nonempty_string = ls_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSShortString nonempty_short_string = ls_short_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSSSOString nonempty_sso_string = ls_sso_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSStringSpan nonempty_sspan = ls_sspan_create(NONEMPTY_BYTES, NONEMPTY_LEN);

	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_null = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_append(&from_empty, LS_EMPTY_BYTES, 0);
		LSStatus nonempty_status = ls_bbuf_append(&from_nonempty, NONEMPTY_BYTES, NONEMPTY_LEN);
		LSStatus null_status = ls_bbuf_append(&from_null, NULL, 0);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(null_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_null);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_append_string(&from_empty, LS_EMPTY_STRING);
		LSStatus nonempty_status = ls_bbuf_append_string(&from_nonempty, nonempty_string);
		LSStatus invalid_status = ls_bbuf_append_string(&from_invalid, LS_AN_INVALID_STRING);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_append_short_string(&from_empty, LS_EMPTY_SHORT_STRING);
		LSStatus nonempty_status = ls_bbuf_append_short_string(&from_nonempty, nonempty_short_string);
		LSStatus invalid_status = ls_bbuf_append_short_string(&from_invalid, LS_AN_INVALID_SHORT_STRING);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_append_sso_string(&from_empty, LS_EMPTY_SSO_STRING);
		LSStatus nonempty_status = ls_bbuf_append_sso_string(&from_nonempty, nonempty_sso_string);
		LSStatus invalid_status = ls_bbuf_append_sso_string(&from_invalid, LS_AN_INVALID_SSO_STRING);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_append_sspan(&from_empty, LS_EMPTY_SSPAN);
		LSStatus nonempty_status = ls_bbuf_append_sspan(&from_nonempty, nonempty_sspan);
		LSStatus invalid_status = ls_bbuf_append_sspan(&from_invalid, LS_AN_INVALID_SSPAN);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}

	ls_string_destroy(&nonempty_string);
	ls_sso_string_destroy(&nonempty_sso_string);
}

void test_insert_funcs(void)
{
	LSString nonempty_string = ls_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSShortString nonempty_short_string = ls_short_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSSSOString nonempty_sso_string = ls_sso_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
	LSStringSpan nonempty_sspan = ls_sspan_create(NONEMPTY_BYTES, NONEMPTY_LEN);

	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_null = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_insert(&from_empty, 0, LS_EMPTY_BYTES, 0);
		LSStatus nonempty_status = ls_bbuf_insert(&from_nonempty, 0, NONEMPTY_BYTES, NONEMPTY_LEN);
		LSStatus null_status = ls_bbuf_insert(&from_null, 0, NULL, 0);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(null_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_null);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_insert_string(&from_empty, 0, LS_EMPTY_STRING);
		LSStatus nonempty_status = ls_bbuf_insert_string(&from_nonempty, 0, nonempty_string);
		LSStatus invalid_status = ls_bbuf_insert_string(&from_invalid, 0, LS_AN_INVALID_STRING);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_insert_short_string(&from_empty, 0, LS_EMPTY_SHORT_STRING);
		LSStatus nonempty_status = ls_bbuf_insert_short_string(&from_nonempty, 0, nonempty_short_string);
		LSStatus invalid_status = ls_bbuf_insert_short_string(&from_invalid, 0, LS_AN_INVALID_SHORT_STRING);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_insert_sso_string(&from_empty, 0, LS_EMPTY_SSO_STRING);
		LSStatus nonempty_status = ls_bbuf_insert_sso_string(&from_nonempty, 0, nonempty_sso_string);
		LSStatus invalid_status = ls_bbuf_insert_sso_string(&from_invalid, 0, LS_AN_INVALID_SSO_STRING);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_insert_sspan(&from_empty, 0, LS_EMPTY_SSPAN);
		LSStatus nonempty_status = ls_bbuf_insert_sspan(&from_nonempty, 0, nonempty_sspan);
		LSStatus invalid_status = ls_bbuf_insert_sspan(&from_invalid, 0, LS_AN_INVALID_SSPAN);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_nonempty.len == NONEMPTY_LEN);
		assert(memcmp(from_nonempty.bytes, NONEMPTY_BYTES, NONEMPTY_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}

	ls_string_destroy(&nonempty_string);
	ls_sso_string_destroy(&nonempty_sso_string);
}
