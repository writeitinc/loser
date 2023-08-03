#include <assert.h>
#include <stdio.h>

#include <loser/loser.h>

static void test_constructors(void);

static const LSByte NONEMPTY_BYTES[] = "deadbeef";
static size_t NONEMPTY_LEN = sizeof(NONEMPTY_BYTES) - 1;

int main(void)
{
	test_constructors();

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
	}
	{
		LSSSOString from_empty = ls_sso_string_create(LS_EMPTY_BYTES, 0);
		LSSSOString from_nonempty = ls_sso_string_create(NONEMPTY_BYTES, NONEMPTY_LEN);
		LSSSOString from_null = ls_sso_string_create(NULL, 0);

		assert(ls_sso_string_is_valid(from_empty));
		assert(ls_sso_string_is_valid(from_nonempty));
		assert(!ls_sso_string_is_valid(from_null));

		ls_sso_string_destroy(&from_empty);
		ls_sso_string_destroy(&from_nonempty);
	}
	{
		LSStringSpan from_empty = ls_sspan_create(LS_EMPTY_BYTES, 0);
		LSStringSpan from_nonempty = ls_sspan_create(NONEMPTY_BYTES, NONEMPTY_LEN);
		LSStringSpan from_null = ls_sspan_create(NULL, 0);

		assert(ls_sspan_is_valid(from_empty));
		assert(ls_sspan_is_valid(from_nonempty));
		assert(!ls_sspan_is_valid(from_null));
	}
}
