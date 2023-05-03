#include <loser/loser.h>

#include <stddef.h>
#include <stdio.h>
#include <time.h>

enum { NITERATIONS = 10000000 };

static void benchmark_text(const char *cstr);

static LSString strings[NITERATIONS];
static LSStringSpan sspans[NITERATIONS];

#define BENCHMARK(name, expr) \
	do { \
		clock_t start = clock(); \
		for (size_t i = 0; i < NITERATIONS; ++i) { \
			(expr); \
		} \
		clock_t end = clock(); \
		clock_t diff = end - start; \
		printf("%-32s: %8zu\n", name, diff); \
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
	LSStringSpan sspan = ls_sspan_create(bytes, len);
	LSString string = ls_string_create(bytes, len);

	printf("\"%s\"\n", cstr);

	// ### LSString ###

	// warm up memory
	DONT_BENCHMARK(strings[i] = ls_string_create(bytes, len));
	DONT_BENCHMARK(ls_string_destroy(&strings[i]));

	DONT_BENCHMARK(strings[i] = ls_string_create(bytes, len));
	BENCHMARK("LS_STRING_VALID",
			LS_STRING_VALID(strings[i]));
	DONT_BENCHMARK(ls_string_destroy(&strings[i]));

	BENCHMARK("ls_string_create",
			strings[i] = ls_string_create(bytes, len));
	DONT_BENCHMARK(ls_string_destroy(&strings[i]));

	BENCHMARK("ls_string_clone",
			strings[i] = ls_string_clone(string));
	DONT_BENCHMARK(ls_string_destroy(&strings[i]));

	BENCHMARK("ls_string_from_sspan",
			strings[i] = ls_string_from_sspan(sspan));
	DONT_BENCHMARK(ls_string_destroy(&strings[i]));

	BENCHMARK("ls_string_from_chars",
			strings[i] = ls_string_from_chars(cstr, len));
	DONT_BENCHMARK(ls_string_destroy(&strings[i]));

	BENCHMARK("ls_string_from_cstr",
			strings[i] = ls_string_from_cstr(cstr));
	BENCHMARK("ls_string_destroy",
			ls_string_destroy(&strings[i]));

	// ### LSStringSpan

	// warm up memory
	DONT_BENCHMARK(sspans[i] = ls_sspan_create(bytes, len));

	BENCHMARK("LS_SSPAN_VALID",
			LS_SSPAN_VALID(sspans[i]));

	BENCHMARK("ls_sspan_create",
			sspans[i] = ls_sspan_create(bytes, len));

	BENCHMARK("ls_sspan_from_string",
			sspans[i] = ls_sspan_from_string(string));

	BENCHMARK("ls_sspan_from_chars",
			sspans[i] = ls_sspan_from_chars(cstr, len));

	BENCHMARK("ls_sspan_from_cstr",
			sspans[i] = ls_sspan_from_cstr(cstr));

	putchar('\n');

	fflush(stdout);

	ls_string_destroy(&string);
}
