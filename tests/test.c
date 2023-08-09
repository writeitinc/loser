#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <loser/loser.h>

static void test_constructors(void);
static void test_conversions(void);
static void test_invalidate_funcs(void);
static void test_append_funcs(void);
static void test_insert_funcs(void);

static void test_append_very_big(void);
static void test_insert_very_big(void);
static void test_append_many(void);
static void test_insert_many(void);

static void test_move_funcs(void);
static void test_move_to_funcs(void);

static const LSByte SMALL_BYTES[] = "deadbeef";
static size_t SMALL_LEN = sizeof(SMALL_BYTES) - 1;

static const LSByte BIG_BYTES[] = "do re mi fa so la ti do!";
static size_t BIG_LEN = sizeof(BIG_BYTES) - 1;

static const LSByte VERY_BIG_BYTES[] = "do re mi fa so la ti do!";
static size_t VERY_BIG_LEN = sizeof(VERY_BIG_BYTES) - 1;

int main(void)
{
	test_constructors();
	test_conversions();
	test_invalidate_funcs();
	test_append_funcs();
	test_insert_funcs();

	test_append_very_big();
	test_insert_very_big();
	test_append_many();
	test_insert_many();

	test_move_funcs();
	test_move_to_funcs();

	return 0;
}

void test_constructors(void)
{
	{
		LSString from_empty = ls_string_create(LS_EMPTY_BYTES, 0);
		LSString from_nonempty = ls_string_create(SMALL_BYTES, SMALL_LEN);
		LSString from_null = ls_string_create(NULL, 0);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_null));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSShortString from_empty = ls_short_string_create(LS_EMPTY_BYTES, 0);
		LSShortString from_nonempty = ls_short_string_create(SMALL_BYTES, SMALL_LEN);
		LSShortString from_null = ls_short_string_create(NULL, 0);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_null));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty._mut_bytes, SMALL_BYTES, SMALL_LEN) == 0);
	}
	{
		LSSSOString from_empty = ls_sso_string_create(LS_EMPTY_BYTES, 0);
		LSSSOString from_small = ls_sso_string_create(SMALL_BYTES, SMALL_LEN);
		LSSSOString from_big = ls_sso_string_create(BIG_BYTES, BIG_LEN);
		LSSSOString from_null = ls_sso_string_create(NULL, 0);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_small));
		assert(ls_sso_string_is_valid(from_big));
		assert(!ls_sso_string_is_valid(from_null));

		assert(from_small.len == SMALL_LEN);
		assert(from_big.len == BIG_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_small), SMALL_BYTES, SMALL_LEN) == 0);
		assert(memcmp(ls_sso_string_get_bytes(&from_big), BIG_BYTES, BIG_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_small);
		ls_sso_string_destroy(&from_big);
	}
	{
		LSStringSpan from_empty = ls_sspan_create(LS_EMPTY_BYTES, 0);
		LSStringSpan from_nonempty = ls_sspan_create(SMALL_BYTES, SMALL_LEN);
		LSStringSpan from_null = ls_sspan_create(NULL, 0);

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.start, SMALL_BYTES, SMALL_LEN) == 0);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_null));
	}
}

void test_conversions(void)
{
	LSString nonempty_string = ls_string_create(SMALL_BYTES, SMALL_LEN);
	LSShortString nonempty_short_string = ls_short_string_create(SMALL_BYTES, SMALL_LEN);
	LSSSOString small_sso_string = ls_sso_string_create(SMALL_BYTES, SMALL_LEN);
	LSSSOString big_sso_string = ls_sso_string_create(BIG_BYTES, BIG_LEN);
	LSStringSpan nonempty_sspan = ls_sspan_create(SMALL_BYTES, SMALL_LEN);

	LSByteBuffer empty_bbuf = ls_bbuf_create();

	LSByteBuffer nonempty_bbuf = ls_bbuf_create_with_init_cap(SMALL_LEN);
	memcpy(nonempty_bbuf.bytes, SMALL_BYTES, SMALL_LEN);
	nonempty_bbuf.len = SMALL_LEN;

	{
		LSString from_empty = ls_string_clone(LS_EMPTY_STRING);
		LSString from_nonempty = ls_string_clone(nonempty_string);
		LSString from_invalid = ls_string_clone(LS_AN_INVALID_STRING);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_sso_string(LS_EMPTY_SSO_STRING);
		LSString from_small = ls_string_from_sso_string(small_sso_string);
		LSString from_big = ls_string_from_sso_string(big_sso_string);
		LSString from_invalid = ls_string_from_sso_string(LS_AN_INVALID_SSO_STRING);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_small));
		assert(ls_string_is_valid(from_big));
		assert(!ls_string_is_valid(from_invalid));

		assert(from_small.len == SMALL_LEN);
		assert(from_big.len == BIG_LEN);
		assert(memcmp(from_small.bytes, SMALL_BYTES, SMALL_LEN) == 0);
		assert(memcmp(from_big.bytes, BIG_BYTES, BIG_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_small);
		ls_string_destroy(&from_big);
	}
	{
		LSString from_empty = ls_string_from_sspan(LS_EMPTY_SSPAN);
		LSString from_nonempty = ls_string_from_sspan(nonempty_sspan);
		LSString from_invalid = ls_string_from_sspan(LS_AN_INVALID_SSPAN);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_chars((const char *)LS_EMPTY_BYTES, 0);
		LSString from_nonempty = ls_string_from_chars((const char *)SMALL_BYTES, SMALL_LEN);
		LSString from_null = ls_string_from_chars(NULL, 0);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_null));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_string_destroy(&from_empty);
		ls_string_destroy(&from_nonempty);
	}
	{
		LSString from_empty = ls_string_from_cstr((const char *)LS_EMPTY_BYTES);
		LSString from_nonempty = ls_string_from_cstr((const char *)SMALL_BYTES);
		LSString from_null = ls_string_from_cstr(NULL);

		assert(ls_string_is_valid(from_empty));
		assert(ls_string_is_valid(from_nonempty));
		assert(!ls_string_is_valid(from_null));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty._mut_bytes, SMALL_BYTES, SMALL_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_sso_string(LS_EMPTY_SSO_STRING);
		LSShortString from_small = ls_short_string_from_sso_string(small_sso_string);
		LSShortString from_big = ls_short_string_from_sso_string(big_sso_string);
		LSShortString from_invalid = ls_short_string_from_sso_string(LS_AN_INVALID_SSO_STRING);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_small));
		assert(!ls_short_string_is_valid(from_big));
		assert(!ls_short_string_is_valid(from_invalid));

		assert(from_small.len == SMALL_LEN);
		assert(memcmp(from_small._mut_bytes, SMALL_BYTES, SMALL_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_sspan(LS_EMPTY_SSPAN);
		LSShortString from_nonempty = ls_short_string_from_sspan(nonempty_sspan);
		LSShortString from_invalid = ls_short_string_from_sspan(LS_AN_INVALID_SSPAN);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty._mut_bytes, SMALL_BYTES, SMALL_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_bbuf(empty_bbuf);
		LSShortString from_nonempty = ls_short_string_from_bbuf(nonempty_bbuf);
		LSShortString from_invalid = ls_short_string_from_bbuf(LS_AN_INVALID_BBUF);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty._mut_bytes, SMALL_BYTES, SMALL_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_chars((const char *)LS_EMPTY_BYTES, 0);
		LSShortString from_nonempty = ls_short_string_from_chars((const char *)SMALL_BYTES, SMALL_LEN);
		LSShortString from_null = ls_short_string_from_chars(NULL, 0);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_null));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty._mut_bytes, SMALL_BYTES, SMALL_LEN) == 0);
	}
	{
		LSShortString from_empty = ls_short_string_from_cstr((const char *)LS_EMPTY_BYTES);
		LSShortString from_nonempty = ls_short_string_from_cstr((const char *)SMALL_BYTES);
		LSShortString from_null = ls_short_string_from_cstr(NULL);

		assert(ls_short_string_is_valid(from_empty));
		assert(ls_short_string_is_valid(from_nonempty));
		assert(!ls_short_string_is_valid(from_null));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty._mut_bytes, SMALL_BYTES, SMALL_LEN) == 0);
	}

	{
		LSSSOString from_empty = ls_sso_string_from_string(LS_EMPTY_STRING);
		LSSSOString from_nonempty = ls_sso_string_from_string(nonempty_string);
		LSSSOString from_invalid = ls_sso_string_from_string(LS_AN_INVALID_STRING);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), SMALL_BYTES, SMALL_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_clone(LS_EMPTY_SSO_STRING);
		LSSSOString from_small = ls_sso_string_clone(small_sso_string);
		LSSSOString from_big = ls_sso_string_clone(big_sso_string);
		LSSSOString from_invalid = ls_sso_string_clone(LS_AN_INVALID_SSO_STRING);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_small));
		assert(ls_sso_string_is_valid(from_big));
		assert(!ls_sso_string_is_valid(from_invalid));

		assert(from_small.len == SMALL_LEN);
		assert(from_big.len == BIG_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_small), SMALL_BYTES, SMALL_LEN) == 0);
		assert(memcmp(ls_sso_string_get_bytes(&from_big), BIG_BYTES, BIG_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_small);
		ls_sso_string_destroy(&from_big);
	}
	{
		LSSSOString from_empty = ls_sso_string_from_sspan(LS_EMPTY_SSPAN);
		LSSSOString from_nonempty = ls_sso_string_from_sspan(nonempty_sspan);
		LSSSOString from_invalid = ls_sso_string_from_sspan(LS_AN_INVALID_SSPAN);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), SMALL_BYTES, SMALL_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_from_chars((const char *)LS_EMPTY_BYTES, 0);
		LSSSOString from_nonempty = ls_sso_string_from_chars((const char *)SMALL_BYTES, SMALL_LEN);
		LSSSOString from_null = ls_sso_string_from_chars(NULL, 0);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_null));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), SMALL_BYTES, SMALL_LEN) == 0);

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSSSOString from_empty = ls_sso_string_from_cstr((const char *)LS_EMPTY_BYTES);
		LSSSOString from_nonempty = ls_sso_string_from_cstr((const char *)SMALL_BYTES);
		LSSSOString from_null = ls_sso_string_from_cstr(NULL);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_null));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(ls_sso_string_get_bytes(&from_nonempty), SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.start, SMALL_BYTES, SMALL_LEN) == 0);
	}
	{
		LSStringSpan from_empty = ls_sspan_from_short_string(&LS_EMPTY_SHORT_STRING);
		LSStringSpan from_nonempty = ls_sspan_from_short_string(&nonempty_short_string);
		LSStringSpan from_invalid = ls_sspan_from_short_string(&LS_AN_INVALID_SHORT_STRING);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.start, SMALL_BYTES, SMALL_LEN) == 0);
	}
	{
		LSStringSpan from_empty = ls_sspan_from_sso_string(&LS_EMPTY_SSO_STRING);
		LSStringSpan from_small = ls_sspan_from_sso_string(&small_sso_string);
		LSStringSpan from_big = ls_sspan_from_sso_string(&big_sso_string);
		LSStringSpan from_invalid = ls_sspan_from_sso_string(&LS_AN_INVALID_SSO_STRING);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_small));
		assert(ls_sspan_is_valid(from_big));
		assert(!ls_sspan_is_valid(from_invalid));

		assert(from_small.len == SMALL_LEN);
		assert(from_big.len == BIG_LEN);
		assert(memcmp(from_small.start, SMALL_BYTES, SMALL_LEN) == 0);
		assert(memcmp(from_big.start, BIG_BYTES, BIG_LEN) == 0);
	}
	{
		LSStringSpan from_empty = ls_sspan_from_bbuf(empty_bbuf);
		LSStringSpan from_nonempty = ls_sspan_from_bbuf(nonempty_bbuf);
		LSStringSpan from_invalid = ls_sspan_from_bbuf(LS_AN_INVALID_BBUF);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.start, SMALL_BYTES, SMALL_LEN) == 0);
	}

	{
		LSByteBuffer from_empty = ls_bbuf_from_sspan(LS_EMPTY_SSPAN);
		LSByteBuffer from_nonempty = ls_bbuf_from_sspan(nonempty_sspan);
		LSByteBuffer from_invalid = ls_bbuf_from_sspan(LS_AN_INVALID_SSPAN);

		assert(ls_bbuf_is_valid(from_empty));
		assert(ls_bbuf_is_valid(from_nonempty));
		assert(!ls_bbuf_is_valid(from_invalid));

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);
		
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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
	}

	ls_string_destroy(&nonempty_string);
	ls_sso_string_destroy(&small_sso_string);
	ls_sso_string_destroy(&big_sso_string);

	ls_bbuf_destroy(&empty_bbuf);
	ls_bbuf_destroy(&nonempty_bbuf);
}

void test_invalidate_funcs(void)
{
	LSString string = ls_string_create(SMALL_BYTES, SMALL_LEN);
	LSShortString short_string = ls_short_string_create(SMALL_BYTES, SMALL_LEN);
	LSSSOString small_sso_string = ls_sso_string_create(SMALL_BYTES, SMALL_LEN);
	LSSSOString big_sso_string = ls_sso_string_create(BIG_BYTES, BIG_LEN);
	LSStringSpan sspan = ls_sspan_create(SMALL_BYTES, SMALL_LEN);
	LSByteBuffer bbuf = ls_bbuf_create();

	assert(ls_string_is_valid(string));
	assert(ls_short_string_is_valid(short_string));
	assert(ls_sso_string_is_valid(small_sso_string));
	assert(ls_sso_string_is_valid(big_sso_string));
	assert(ls_sspan_is_valid(sspan));
	assert(ls_bbuf_is_valid(bbuf));

	ls_string_destroy(&string);
	ls_sso_string_destroy(&small_sso_string);
	ls_sso_string_destroy(&big_sso_string);
	ls_bbuf_destroy(&bbuf);

	ls_string_invalidate(&string);
	ls_short_string_invalidate(&short_string);
	ls_sso_string_invalidate(&small_sso_string);
	ls_sso_string_invalidate(&big_sso_string);
	ls_sspan_invalidate(&sspan);
	ls_bbuf_invalidate(&bbuf);

	assert(!ls_string_is_valid(string));
	assert(!ls_short_string_is_valid(short_string));
	assert(!ls_sso_string_is_valid(small_sso_string));
	assert(!ls_sso_string_is_valid(big_sso_string));
	assert(!ls_sspan_is_valid(sspan));
	assert(!ls_bbuf_is_valid(bbuf));
}

void test_append_funcs(void)
{
	LSString nonempty_string = ls_string_create(SMALL_BYTES, SMALL_LEN);
	LSShortString nonempty_short_string = ls_short_string_create(SMALL_BYTES, SMALL_LEN);
	LSSSOString small_sso_string = ls_sso_string_create(SMALL_BYTES, SMALL_LEN);
	LSSSOString big_sso_string = ls_sso_string_create(BIG_BYTES, BIG_LEN);
	LSStringSpan nonempty_sspan = ls_sspan_create(SMALL_BYTES, SMALL_LEN);
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_null = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_append(&from_empty, LS_EMPTY_BYTES, 0);
		LSStatus nonempty_status = ls_bbuf_append(&from_nonempty, SMALL_BYTES, SMALL_LEN);
		LSStatus null_status = ls_bbuf_append(&from_null, NULL, 0);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(null_status == LS_FAILURE);

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_small = ls_bbuf_create();
		LSByteBuffer from_big = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_append_sso_string(&from_empty, LS_EMPTY_SSO_STRING);
		LSStatus small_status = ls_bbuf_append_sso_string(&from_small, small_sso_string);
		LSStatus big_status = ls_bbuf_append_sso_string(&from_big, big_sso_string);
		LSStatus invalid_status = ls_bbuf_append_sso_string(&from_invalid, LS_AN_INVALID_SSO_STRING);

		assert(empty_status == LS_SUCCESS);
		assert(small_status == LS_SUCCESS);
		assert(big_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_small.len == SMALL_LEN);
		assert(from_big.len == BIG_LEN);
		assert(memcmp(from_small.bytes, SMALL_BYTES, SMALL_LEN) == 0);
		assert(memcmp(from_big.bytes, BIG_BYTES, BIG_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_small);
		ls_bbuf_destroy(&from_big);
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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}

	ls_string_destroy(&nonempty_string);
	ls_sso_string_destroy(&small_sso_string);
	ls_sso_string_destroy(&big_sso_string);
}

void test_insert_funcs(void)
{
	LSString nonempty_string = ls_string_create(SMALL_BYTES, SMALL_LEN);
	LSShortString nonempty_short_string = ls_short_string_create(SMALL_BYTES, SMALL_LEN);
	LSSSOString small_sso_string = ls_sso_string_create(SMALL_BYTES, SMALL_LEN);
	LSSSOString big_sso_string = ls_sso_string_create(BIG_BYTES, BIG_LEN);
	LSStringSpan nonempty_sspan = ls_sspan_create(SMALL_BYTES, SMALL_LEN);

	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_nonempty = ls_bbuf_create();
		LSByteBuffer from_null = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_insert(&from_empty, 0, LS_EMPTY_BYTES, 0);
		LSStatus nonempty_status = ls_bbuf_insert(&from_nonempty, 0, SMALL_BYTES, SMALL_LEN);
		LSStatus null_status = ls_bbuf_insert(&from_null, 0, NULL, 0);

		assert(empty_status == LS_SUCCESS);
		assert(nonempty_status == LS_SUCCESS);
		assert(null_status == LS_FAILURE);

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}
	{
		LSByteBuffer from_empty = ls_bbuf_create();
		LSByteBuffer from_small = ls_bbuf_create();
		LSByteBuffer from_big = ls_bbuf_create();
		LSByteBuffer from_invalid = ls_bbuf_create();

		LSStatus empty_status = ls_bbuf_insert_sso_string(&from_empty, 0, LS_EMPTY_SSO_STRING);
		LSStatus small_status = ls_bbuf_insert_sso_string(&from_small, 0, small_sso_string);
		LSStatus big_status = ls_bbuf_insert_sso_string(&from_big, 0, big_sso_string);
		LSStatus invalid_status = ls_bbuf_insert_sso_string(&from_invalid, 0, LS_AN_INVALID_SSO_STRING);

		assert(empty_status == LS_SUCCESS);
		assert(small_status == LS_SUCCESS);
		assert(big_status == LS_SUCCESS);
		assert(invalid_status == LS_FAILURE);

		assert(from_small.len == SMALL_LEN);
		assert(from_big.len == BIG_LEN);
		assert(memcmp(from_small.bytes, SMALL_BYTES, SMALL_LEN) == 0);
		assert(memcmp(from_big.bytes, BIG_BYTES, BIG_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_small);
		ls_bbuf_destroy(&from_big);
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

		assert(from_nonempty.len == SMALL_LEN);
		assert(memcmp(from_nonempty.bytes, SMALL_BYTES, SMALL_LEN) == 0);

		ls_bbuf_destroy(&from_empty);
		ls_bbuf_destroy(&from_nonempty);
		ls_bbuf_destroy(&from_invalid);
	}

	ls_string_destroy(&nonempty_string);
	ls_sso_string_destroy(&small_sso_string);
	ls_sso_string_destroy(&big_sso_string);
}

void test_append_very_big(void)
{
	LSByteBuffer bbuf = ls_bbuf_create();

	LSStringSpan very_big_sspan = ls_sspan_create(VERY_BIG_BYTES, VERY_BIG_LEN);

	LSStatus status = ls_bbuf_append_sspan(&bbuf, very_big_sspan);
	assert(status == LS_SUCCESS);

	assert(bbuf.len == very_big_sspan.len);
	assert(memcmp(bbuf.bytes, very_big_sspan.start, very_big_sspan.len) == 0);

	ls_bbuf_destroy(&bbuf);
}

void test_insert_very_big(void)
{
	LSByteBuffer bbuf = ls_bbuf_create();

	LSStringSpan very_big_sspan = ls_sspan_create(VERY_BIG_BYTES, VERY_BIG_LEN);

	LSStatus status = ls_bbuf_insert_sspan(&bbuf, 0, very_big_sspan);
	assert(status == LS_SUCCESS);

	assert(bbuf.len == very_big_sspan.len);
	assert(memcmp(bbuf.bytes, very_big_sspan.start, very_big_sspan.len) == 0);

	ls_bbuf_destroy(&bbuf);
}

void test_append_many(void)
{
	enum { NAPPENDS = 8 };

	LSByteBuffer bbuf = ls_bbuf_create();

	LSStringSpan sspan = ls_sspan_create(SMALL_BYTES, SMALL_LEN);
	for (size_t i = 0; i < NAPPENDS; ++i) {
		LSStatus status = ls_bbuf_append_sspan(&bbuf, sspan);
		assert(status == LS_SUCCESS);
	}

	assert(bbuf.len == sspan.len * NAPPENDS);

	for (size_t i = 0; i < NAPPENDS; ++i) {
		assert(memcmp(&bbuf.bytes[i * sspan.len], sspan.start, sspan.len) == 0);
	}

	ls_bbuf_destroy(&bbuf);
}

void test_insert_many(void)
{
	enum { NINSERTIONS = 8 };

	LSByteBuffer bbuf = ls_bbuf_create();

	LSStringSpan sspan = ls_sspan_create(SMALL_BYTES, SMALL_LEN);
	for (size_t i = 0; i < NINSERTIONS; ++i) {
		LSStatus status = ls_bbuf_insert_sspan(&bbuf, 0, sspan);
		assert(status == LS_SUCCESS);
	}

	assert(bbuf.len == sspan.len * NINSERTIONS);

	for (size_t i = 0; i < NINSERTIONS; ++i) {
		assert(memcmp(&bbuf.bytes[i * sspan.len], sspan.start, sspan.len) == 0);
	}

	ls_bbuf_destroy(&bbuf);
}

void test_move_funcs(void)
{
	{
		LSString string = ls_string_create(SMALL_BYTES, SMALL_LEN);
		LSString invalid_string = LS_AN_INVALID_STRING;

		LSString mv = ls_string_move(&string);
		LSString invalid_mv = ls_string_move(&invalid_string);

		assert(!ls_string_is_valid(string));
		assert(ls_string_is_valid(mv));

		assert(!ls_string_is_valid(invalid_string));
		assert(!ls_string_is_valid(invalid_mv));

		ls_string_destroy(&mv);
	}
	{
		LSSSOString small_sso_string = ls_sso_string_create(SMALL_BYTES, SMALL_LEN);
		LSSSOString big_sso_string = ls_sso_string_create(BIG_BYTES, BIG_LEN);
		LSSSOString invalid_sso_string = LS_AN_INVALID_SSO_STRING;

		LSSSOString small_mv = ls_sso_string_move(&small_sso_string);
		LSSSOString big_mv = ls_sso_string_move(&big_sso_string);
		LSSSOString invalid_mv = ls_sso_string_move(&invalid_sso_string);

		assert(!ls_sso_string_is_valid(small_sso_string));
		assert(!ls_sso_string_is_valid(big_sso_string));
		assert(ls_sso_string_is_valid(small_mv));
		assert(ls_sso_string_is_valid(big_mv));

		assert(!ls_sso_string_is_valid(invalid_sso_string));
		assert(!ls_sso_string_is_valid(invalid_mv));

		ls_sso_string_destroy(&small_mv);
		ls_sso_string_destroy(&big_mv);
	}
	{
		LSByteBuffer bbuf = ls_bbuf_create();
		LSByteBuffer invalid_bbuf = LS_AN_INVALID_BBUF;

		LSByteBuffer mv = ls_bbuf_move(&bbuf);
		LSByteBuffer invalid_mv = ls_bbuf_move(&invalid_bbuf);

		assert(!ls_bbuf_is_valid(bbuf));
		assert(ls_bbuf_is_valid(mv));

		assert(!ls_bbuf_is_valid(invalid_bbuf));
		assert(!ls_bbuf_is_valid(invalid_mv));

		ls_bbuf_destroy(&mv);
	}
}

void test_move_to_funcs(void)
{
	{
		LSString small_string = ls_string_create(SMALL_BYTES, SMALL_LEN);
		LSString big_string = ls_string_create(BIG_BYTES, BIG_LEN);
		LSString invalid_string = LS_AN_INVALID_STRING;

		LSSSOString small_mv = ls_string_move_to_sso_string(&small_string);
		LSSSOString big_mv = ls_string_move_to_sso_string(&big_string);
		LSSSOString invalid_mv = ls_string_move_to_sso_string(&invalid_string);

		assert(!ls_string_is_valid(small_string));
		assert(!ls_string_is_valid(big_string));
		assert(ls_sso_string_is_valid(small_mv));
		assert(ls_sso_string_is_valid(big_mv));

		assert(!ls_string_is_valid(invalid_string));
		assert(!ls_sso_string_is_valid(invalid_mv));

		ls_sso_string_destroy(&small_mv);
		ls_sso_string_destroy(&big_mv);
	}
	{
		LSSSOString small_sso_string = ls_sso_string_create(SMALL_BYTES, SMALL_LEN);
		LSSSOString big_sso_string = ls_sso_string_create(BIG_BYTES, BIG_LEN);
		LSSSOString invalid_sso_string = LS_AN_INVALID_SSO_STRING;

		LSString small_mv = ls_sso_string_move_to_string(&small_sso_string);
		LSString big_mv = ls_sso_string_move_to_string(&big_sso_string);
		LSString invalid_mv = ls_sso_string_move_to_string(&invalid_sso_string);

		assert(!ls_sso_string_is_valid(small_sso_string));
		assert(!ls_sso_string_is_valid(big_sso_string));
		assert(ls_string_is_valid(small_mv));
		assert(ls_string_is_valid(big_mv));

		assert(!ls_sso_string_is_valid(invalid_sso_string));
		assert(!ls_string_is_valid(invalid_mv));

		ls_string_destroy(&small_mv);
		ls_string_destroy(&big_mv);
	}

	{
		LSByteBuffer small_bbuf = ls_bbuf_create();
		LSByteBuffer big_bbuf = ls_bbuf_create();
		LSByteBuffer invalid_bbuf = LS_AN_INVALID_BBUF;

		LSString small_mv = ls_bbuf_finalize(&small_bbuf);
		LSString big_mv = ls_bbuf_finalize(&big_bbuf);
		LSString invalid_mv = ls_bbuf_finalize(&invalid_bbuf);

		assert(!ls_bbuf_is_valid(small_bbuf));
		assert(!ls_bbuf_is_valid(big_bbuf));
		assert(ls_string_is_valid(small_mv));
		assert(ls_string_is_valid(big_mv));

		assert(!ls_bbuf_is_valid(invalid_bbuf));
		assert(!ls_string_is_valid(invalid_mv));

		ls_string_destroy(&small_mv);
		ls_string_destroy(&big_mv);
	}
	{
		LSByteBuffer small_bbuf = ls_bbuf_create();
		LSByteBuffer big_bbuf = ls_bbuf_create();
		LSByteBuffer invalid_bbuf = LS_AN_INVALID_BBUF;

		LSSSOString small_mv = ls_bbuf_finalize_sso(&small_bbuf);
		LSSSOString big_mv = ls_bbuf_finalize_sso(&big_bbuf);
		LSSSOString invalid_mv = ls_bbuf_finalize_sso(&invalid_bbuf);

		assert(!ls_bbuf_is_valid(small_bbuf));
		assert(!ls_bbuf_is_valid(big_bbuf));
		assert(ls_sso_string_is_valid(small_mv));
		assert(ls_sso_string_is_valid(big_mv));

		assert(!ls_bbuf_is_valid(invalid_bbuf));
		assert(!ls_sso_string_is_valid(invalid_mv));

		ls_sso_string_destroy(&small_mv);
		ls_sso_string_destroy(&big_mv);
	}
}
