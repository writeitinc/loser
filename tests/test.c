#include <stdio.h>

#include <loser/loser.h>

int main(void)
{
	volatile LSShortString short_string = LS_SHORT_STRING_LITERAL("0123456789abcde");
	(void)short_string;

	return 0;
}
