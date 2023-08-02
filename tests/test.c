#include <assert.h>
#include <stdio.h>

#include <loser/loser.h>

int main(void)
{
	LSString string = LS_EMPTY_STRING;
	LSShortString short_string = LS_EMPTY_SHORT_STRING;
	LSSSOString sso_string = LS_EMPTY_SSO_STRING;
	LSStringSpan sspan = LS_EMPTY_SSPAN;

	assert(ls_string_is_valid(string));
	assert(ls_short_string_is_valid(short_string));
	assert(ls_sso_string_is_valid(sso_string));
	assert(ls_sspan_is_valid(sspan));

	return 0;
}
