#include <loser/loser.h>

#include <stddef.h>
#include <stdio.h>
#include <time.h>

#ifndef NITERATIONS
#define NITERATIONS 10000000
#endif

static void benchmark_text(const char *cstr);

static union {
	LSString strings[NITERATIONS];
	LSShortString short_strings[NITERATIONS];
	LSSSOString sso_strings[NITERATIONS];
	LSStringSpan sspans[NITERATIONS];
} arrays;

#define BENCHMARK(name, expr) \
	do { \
		clock_t start = clock(); \
		for (size_t i = 0; i < NITERATIONS; ++i) { \
			(expr); \
		} \
		clock_t end = clock(); \
		clock_t diff = end - start; \
		printf("%-32s: %8zu\n", name, (size_t)diff); \
	} while (0)

#define DONT_BENCHMARK(expr) \
	do { \
		for (size_t i = 0; i < NITERATIONS; ++i) { \
			(expr); \
		} \
	} while (0)

int main(void)
{
	benchmark_text("");                                 // empty
	benchmark_text("1234567");                          // len 7
	benchmark_text("12345678");                         // len 8
	benchmark_text("123456789012345");                  // len 15
	benchmark_text("1234567890123456");                 // len 16
	benchmark_text("12345678901234567890123");          // len 23
	benchmark_text("123456789012345678901234");         // len 24
	benchmark_text("1234567890123456789012345678901");  // len 31
	benchmark_text("12345678901234567890123456789012"); // len 32

	return 0;
}

void benchmark_text(const char *cstr)
{
	size_t len = strlen(cstr);
	const LSByte *bytes = (const LSByte *)cstr;

	LSShortString short_string = ls_short_string_create(bytes, len);
	LSSSOString sso_string = ls_sso_string_create(bytes, len);
	LSString string = ls_string_create(bytes, len);
	LSStringSpan sspan = ls_sspan_create(bytes, len);

	printf("\"%s\"\n", cstr);

	// ### LSString ###

	// warm up memory
	DONT_BENCHMARK(arrays.strings[i] = ls_string_create(bytes, len));
	DONT_BENCHMARK(ls_string_destroy(&arrays.strings[i]));

	DONT_BENCHMARK(arrays.strings[i] = ls_string_create(bytes, len));
	BENCHMARK("ls_string_is_valid",
			ls_string_is_valid(arrays.strings[i]));
	DONT_BENCHMARK(ls_string_destroy(&arrays.strings[i]));

	BENCHMARK("ls_string_create",
			arrays.strings[i] = ls_string_create(bytes, len));
	DONT_BENCHMARK(ls_string_destroy(&arrays.strings[i]));

	BENCHMARK("ls_string_clone",
			arrays.strings[i] = ls_string_clone(string));
	DONT_BENCHMARK(ls_string_destroy(&arrays.strings[i]));

	BENCHMARK("ls_string_from_short_string",
			arrays.strings[i] = ls_string_from_short_string(short_string));
	DONT_BENCHMARK(ls_string_destroy(&arrays.strings[i]));

	BENCHMARK("ls_string_from_sso_string",
			arrays.strings[i] = ls_string_from_sso_string(sso_string));
	DONT_BENCHMARK(ls_string_destroy(&arrays.strings[i]));

	BENCHMARK("ls_string_from_sspan",
			arrays.strings[i] = ls_string_from_sspan(sspan));
	DONT_BENCHMARK(ls_string_destroy(&arrays.strings[i]));

	BENCHMARK("ls_string_from_chars",
			arrays.strings[i] = ls_string_from_chars(cstr, len));
	DONT_BENCHMARK(ls_string_destroy(&arrays.strings[i]));

	BENCHMARK("ls_string_from_cstr",
			arrays.strings[i] = ls_string_from_cstr(cstr));
	BENCHMARK("ls_string_destroy",
			ls_string_destroy(&arrays.strings[i]));

	// ### LSSSOString ###

	// warm up memory
	DONT_BENCHMARK(arrays.short_strings[i] = ls_short_string_create(bytes, len));

	DONT_BENCHMARK(arrays.short_strings[i] = ls_short_string_create(bytes, len));
	BENCHMARK("ls_short_string_is_valid",
			ls_short_string_is_valid(arrays.short_strings[i]));

	BENCHMARK("ls_short_string_create",
			arrays.short_strings[i] = ls_short_string_create(bytes, len));

	BENCHMARK("ls_short_string_from_string",
			arrays.short_strings[i] = ls_short_string_from_string(string));

	BENCHMARK("ls_short_string_from_sso_string",
			arrays.short_strings[i] = ls_short_string_from_sso_string(sso_string));

	BENCHMARK("ls_short_string_from_sspan",
			arrays.short_strings[i] = ls_short_string_from_sspan(sspan));

	BENCHMARK("ls_short_string_from_chars",
			arrays.short_strings[i] = ls_short_string_from_chars(cstr, len));

	BENCHMARK("ls_short_string_from_cstr",
			arrays.short_strings[i] = ls_short_string_from_cstr(cstr));


	// ### LSSSOString ###

	// warm up memory
	DONT_BENCHMARK(arrays.sso_strings[i] = ls_sso_string_create(bytes, len));
	DONT_BENCHMARK(ls_sso_string_destroy(&arrays.sso_strings[i]));

	DONT_BENCHMARK(arrays.sso_strings[i] = ls_sso_string_create(bytes, len));
	BENCHMARK("ls_sso_string_get_type",
			ls_sso_string_get_type(arrays.sso_strings[i]));
	BENCHMARK("ls_sso_string_get_bytes",
			ls_sso_string_get_bytes(&arrays.sso_strings[i]));
	DONT_BENCHMARK(ls_sso_string_destroy(&arrays.sso_strings[i]));

	BENCHMARK("ls_sso_string_create",
			arrays.sso_strings[i] = ls_sso_string_create(bytes, len));
	DONT_BENCHMARK(ls_sso_string_destroy(&arrays.sso_strings[i]));

	BENCHMARK("ls_sso_string_from_string",
			arrays.sso_strings[i] = ls_sso_string_from_string(string));
	DONT_BENCHMARK(ls_sso_string_destroy(&arrays.sso_strings[i]));

	BENCHMARK("ls_sso_string_from_short_string",
			arrays.sso_strings[i] = ls_sso_string_from_short_string(short_string));
	DONT_BENCHMARK(ls_sso_string_destroy(&arrays.sso_strings[i]));

	BENCHMARK("ls_sso_string_clone",
			arrays.sso_strings[i] = ls_sso_string_clone(sso_string));
	DONT_BENCHMARK(ls_sso_string_destroy(&arrays.sso_strings[i]));

	BENCHMARK("ls_sso_string_from_sspan",
			arrays.sso_strings[i] = ls_sso_string_from_sspan(sspan));
	DONT_BENCHMARK(ls_sso_string_destroy(&arrays.sso_strings[i]));

	BENCHMARK("ls_sso_string_from_chars",
			arrays.sso_strings[i] = ls_sso_string_from_chars(cstr, len));
	DONT_BENCHMARK(ls_sso_string_destroy(&arrays.sso_strings[i]));

	BENCHMARK("ls_sso_string_from_cstr",
			arrays.sso_strings[i] = ls_sso_string_from_cstr(cstr));

	BENCHMARK("ls_sso_string_destroy",
			ls_sso_string_destroy(&arrays.sso_strings[i]));

	// ### LSStringSpan

	// warm up memory
	DONT_BENCHMARK(arrays.sspans[i] = ls_sspan_create(bytes, len));

	BENCHMARK("ls_sspan_is_valid",
			ls_sspan_is_valid(arrays.sspans[i]));

	BENCHMARK("ls_sspan_create",
			arrays.sspans[i] = ls_sspan_create(bytes, len));

	BENCHMARK("ls_sspan_from_string",
			arrays.sspans[i] = ls_sspan_from_string(string));

	BENCHMARK("ls_sspan_from_short_string",
			arrays.sspans[i] = ls_sspan_from_short_string(&short_string));

	BENCHMARK("ls_sspan_from_sso_string",
			arrays.sspans[i] = ls_sspan_from_sso_string(&sso_string));

	BENCHMARK("ls_sspan_from_chars",
			arrays.sspans[i] = ls_sspan_from_chars(cstr, len));

	BENCHMARK("ls_sspan_from_cstr",
			arrays.sspans[i] = ls_sspan_from_cstr(cstr));

	putchar('\n');

	fflush(stdout);

	ls_string_destroy(&string);
	ls_sso_string_destroy(&sso_string);
}
