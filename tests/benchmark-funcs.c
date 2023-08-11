#include <loser/loser.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stopwatch.h"

#ifndef NITERATIONS
#define NITERATIONS 10000000
#endif

#ifndef PRECISION
#define PRECISION 2
#endif

#define NELEMS(arr) (sizeof(arr) / sizeof((arr)[0]))

#define FOREACH(T, ident, arr) \
	for (T *ident = &(arr)[0]; ident != &(arr)[NELEMS(arr)]; ident++)

#define BENCHMARK(func, len_tag_idx, expr) \
	do { \
		Stopwatch stopwatch = stopwatch_create(); \
		stopwatch_start(&stopwatch); \
		{ \
			expr \
		} \
		stopwatch_stop(&stopwatch); \
		benchmarks[func][len_tag_idx] = stopwatch_get_elapsed_time(stopwatch); \
	} while (0)

static void print_benchmarks(size_t norm_factor);
static void benchmark_text(const char *cstr, size_t len_tag_idx);
static size_t size_max(size_t a, size_t b);

static union {
	LSString strings[NITERATIONS];
	LSShortString short_strings[NITERATIONS];
	LSSSOString ssos[NITERATIONS];
	LSStringSpan sspans[NITERATIONS];
	LSByteBuffer bbufs[NITERATIONS];
} arrays;

enum Function {
	LS_STRING_IS_VALID = 0,
	LS_STRING_CREATE,
	LS_STRING_CLONE,
	LS_STRING_FROM_SHORT_STRING,
	LS_STRING_FROM_SSO,
	LS_STRING_FROM_SSPAN,
	LS_STRING_FROM_CHARS,
	LS_STRING_FROM_CSTR,
	LS_STRING_DESTROY,
	LS_STRING_INVALIDATE,
	LS_STRING_MOVE,
	LS_SHORT_STRING_IS_VALID,
	LS_SHORT_STRING_GET_BYTES,
	LS_SHORT_STRING_CREATE,
	LS_SHORT_STRING_FROM_STRING,
	LS_SHORT_STRING_FROM_SSO,
	LS_SHORT_STRING_FROM_SSPAN,
	LS_SHORT_STRING_FROM_CHARS,
	LS_SHORT_STRING_FROM_CSTR,
	LS_SHORT_STRING_INVALIDATE,
	LS_SSO_GET_TYPE,
	LS_SSO_IS_VALID,
	LS_SSO_GET_BYTES,
	LS_SSO_CREATE,
	LS_SSO_FROM_STRING,
	LS_SSO_FROM_SHORT_STRING,
	LS_SSO_CLONE,
	LS_SSO_FROM_SSPAN,
	LS_SSO_FROM_CHARS,
	LS_SSO_FROM_CSTR,
	LS_SSO_DESTROY,
	LS_SSO_INVALIDATE,
	LS_SSO_MOVE,
	LS_SSPAN_IS_VALID,
	LS_SSPAN_CREATE,
	LS_SSPAN_FROM_STRING,
	LS_SSPAN_FROM_SHORT_STRING,
	LS_SSPAN_FROM_SSO,
	LS_SSPAN_FROM_CHARS,
	LS_SSPAN_FROM_CSTR,
	LS_SSPAN_INVALIDATE,
	LS_BBUF_CREATE_WITH_INIT_CAP,
	LS_BBUF_FROM_SSPAN,
	LS_BBUF_CLONE,
	CREATE_THEN_APPEND,
	CREATE_WIC_THEN_APPEND,
	CREATE_THEN_INSERT,
	CREATE_WIC_THEN_INSERT,
	CREATE_AND_APPEND,
	CREATE_WIC_AND_APPEND,
	CREATE_AND_INSERT,
	CREATE_WIC_AND_INSERT,
	LS_BBUF_DESTROY,
	LS_BBUF_INVALIDATE,
	LS_BBUF_MOVE,

	NFUNCTIONS
};

static const char *FUNC_NAMES[NFUNCTIONS] = {
	[LS_STRING_IS_VALID]              = "ls_string_is_valid",
	[LS_STRING_CREATE]                = "ls_string_create",
	[LS_STRING_CLONE]                 = "ls_string_clone",
	[LS_STRING_FROM_SHORT_STRING]     = "ls_string_from_short_string",
	[LS_STRING_FROM_SSO]              = "ls_string_from_sso",
	[LS_STRING_FROM_SSPAN]            = "ls_string_from_sspan",
	[LS_STRING_FROM_CHARS]            = "ls_string_from_chars",
	[LS_STRING_FROM_CSTR]             = "ls_string_from_cstr",
	[LS_STRING_DESTROY]               = "ls_string_destroy",
	[LS_STRING_INVALIDATE]            = "ls_string_invalidate",
	[LS_STRING_MOVE]                  = "ls_string_move",
	[LS_SHORT_STRING_IS_VALID]        = "ls_short_string_is_valid",
	[LS_SHORT_STRING_GET_BYTES]       = "ls_short_string_get_bytes",
	[LS_SHORT_STRING_CREATE]          = "ls_short_string_create",
	[LS_SHORT_STRING_FROM_STRING]     = "ls_short_string_from_string",
	[LS_SHORT_STRING_FROM_SSO]        = "ls_short_string_from_sso",
	[LS_SHORT_STRING_FROM_SSPAN]      = "ls_short_string_from_sspan",
	[LS_SHORT_STRING_FROM_CHARS]      = "ls_short_string_from_chars",
	[LS_SHORT_STRING_FROM_CSTR]       = "ls_short_string_from_cstr",
	[LS_SHORT_STRING_INVALIDATE]      = "ls_short_string_invalidate",
	[LS_SSO_GET_TYPE]                 = "ls_sso_get_type",
	[LS_SSO_IS_VALID]                 = "ls_sso_is_valid",
	[LS_SSO_GET_BYTES]                = "ls_sso_get_bytes",
	[LS_SSO_CREATE]                   = "ls_sso_create",
	[LS_SSO_FROM_STRING]              = "ls_sso_from_string",
	[LS_SSO_FROM_SHORT_STRING]        = "ls_sso_from_short_string",
	[LS_SSO_CLONE]                    = "ls_sso_clone",
	[LS_SSO_FROM_SSPAN]               = "ls_sso_from_sspan",
	[LS_SSO_FROM_CHARS]               = "ls_sso_from_chars",
	[LS_SSO_FROM_CSTR]                = "ls_sso_from_cstr",
	[LS_SSO_DESTROY]                  = "ls_sso_destroy",
	[LS_SSO_INVALIDATE]               = "ls_sso_invalidate",
	[LS_SSO_MOVE]                     = "ls_sso_move",
	[LS_SSPAN_IS_VALID]               = "ls_sspan_is_valid",
	[LS_SSPAN_CREATE]                 = "ls_sspan_create",
	[LS_SSPAN_FROM_STRING]            = "ls_sspan_from_string",
	[LS_SSPAN_FROM_SHORT_STRING]      = "ls_sspan_from_short_string",
	[LS_SSPAN_FROM_SSO]               = "ls_sspan_from_sso",
	[LS_SSPAN_FROM_CHARS]             = "ls_sspan_from_chars",
	[LS_SSPAN_FROM_CSTR]              = "ls_sspan_from_cstr",
	[LS_SSPAN_INVALIDATE]             = "ls_sspan_invalidate",
	[LS_BBUF_CREATE_WITH_INIT_CAP]    = "ls_bbuf_create_with_init_cap",
	[LS_BBUF_FROM_SSPAN]              = "ls_bbuf_from_sspan",
	[LS_BBUF_CLONE]                   = "ls_bbuf_clone",
	[CREATE_THEN_APPEND]              = "(create bbuf), append",
	[CREATE_WIC_THEN_APPEND]          = "(create bbuf w/ init cap), append",
	[CREATE_THEN_INSERT]              = "(create bbuf), insert",
	[CREATE_WIC_THEN_INSERT]          = "(create bbuf w/ init cap), insert",
	[CREATE_AND_APPEND]               = "create bbuf + append",
	[CREATE_WIC_AND_APPEND]           = "create bbuf w/ init cap + append",
	[CREATE_AND_INSERT]               = "create bbuf + insert",
	[CREATE_WIC_AND_INSERT]           = "create bbuf w/ init cap + insert",
	[LS_BBUF_DESTROY]                 = "ls_bbuf_destroy",
	[LS_BBUF_INVALIDATE]              = "ls_bbuf_invalidate",
	[LS_BBUF_MOVE]                    = "ls_bbuf_move",
};

#define MAX_LEN 32
static const size_t LEN_TAGS[] = { 0, 7, 8, 15, 16, 23, 24, 31, MAX_LEN };
enum {
	NLEN_TAGS = NELEMS(LEN_TAGS),
	NBENCHMARKS = NFUNCTIONS * NLEN_TAGS,
};

static clock_t benchmarks[NFUNCTIONS][NLEN_TAGS];

int main(void)
{
	static char max_len_str[MAX_LEN + 1];
	for (size_t len = 0; len < MAX_LEN; ++len) {
		size_t one_based_idx = len + 1;
		size_t digit = one_based_idx % 10;
		max_len_str[len] = digit + '0';
	}

	char str[MAX_LEN + 1] = { 0 };
	for (size_t len_tag = 0; len_tag < NLEN_TAGS; ++len_tag) {
		size_t len = LEN_TAGS[len_tag];

		memcpy(str, max_len_str, len);

		fprintf(stderr, "Benchmarking string \"%s\" (length %zu)\n", str, len);
		benchmark_text(str, len_tag);
	}


	size_t sum = 0;
	for (size_t i = 0; i < NFUNCTIONS; ++i) {
		for (size_t j = 0; j < NLEN_TAGS; ++j) {
			sum += benchmarks[i][j];
		}
	}
	size_t avg = sum / NBENCHMARKS;
	size_t norm_factor = size_max(avg, 1);

	puts("== Raw Benchmarks ==\n");
	print_benchmarks(1);

	putchar('\n');

	puts("== Normalized Benchmarks ==\n");
	printf("-- Normalization Factor (Average): %zu --\n", norm_factor);
	print_benchmarks(norm_factor);

	return 0;
}

void print_benchmarks(size_t norm_factor)
{
	size_t max_func_name_len = 0;
	for (size_t func = 0; func < NFUNCTIONS; ++func) {
		size_t len = strlen(FUNC_NAMES[func]);

		max_func_name_len = size_max(max_func_name_len, len);
	}

	printf("%-*s : ", (int)max_func_name_len, "LENGTH");
	for (size_t len_tag = 0; len_tag < NLEN_TAGS; ++len_tag) {
		size_t len = LEN_TAGS[len_tag];

		printf("%8zu", len);
	}
	putchar('\n');

	for (size_t func = 0; func < NFUNCTIONS; ++func) {
		printf("%-*s : ", (int)max_func_name_len, FUNC_NAMES[func]);
		for (size_t len_tag = 0; len_tag < NLEN_TAGS; ++len_tag) {
			if (norm_factor == 1) {
				printf("%8ld", (long)benchmarks[func][len_tag]);
			} else {
				printf("% 8.*lf", PRECISION, (double)benchmarks[func][len_tag] / norm_factor);
			}
		}
		putchar('\n');
	}
}

void benchmark_text(const char *cstr, size_t len_tag_idx)
{
	size_t len = strlen(cstr);
	const LSByte *bytes = (const LSByte *)cstr;

	LSShortString short_string = ls_short_string_create(bytes, len);
	LSSSOString sso = ls_sso_create(bytes, len);
	LSString string = ls_string_create(bytes, len);
	LSStringSpan sspan = ls_sspan_create(bytes, len);
	LSByteBuffer bbuf = ls_bbuf_from_sspan(sspan);

	volatile int vol_int;
	(void)vol_int;

	// ### LSString ###

	// warm up memory
	FOREACH (LSString, iter, arrays.strings) {
		*iter = ls_string_create(bytes, len);
	}
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	FOREACH (LSString, iter, arrays.strings) {
		*iter = ls_string_create(bytes, len);
	}
	BENCHMARK(LS_STRING_IS_VALID, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				vol_int = ls_string_is_valid(*iter);
			});
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(LS_STRING_CREATE, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				*iter = ls_string_create(bytes, len);
			});
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(LS_STRING_CLONE, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				*iter = ls_string_clone(string);
			});
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(LS_STRING_FROM_SHORT_STRING, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				*iter = ls_string_from_short_string(short_string);
		 	});
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(LS_STRING_FROM_SSO, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				*iter = ls_string_from_sso(sso);
			});
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(LS_STRING_FROM_SSPAN, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				*iter = ls_string_from_sspan(sspan);
			});
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(LS_STRING_FROM_CHARS, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				*iter = ls_string_from_chars(cstr, len);
			});
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(LS_STRING_FROM_CSTR, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				*iter = ls_string_from_cstr(cstr);
			});
	BENCHMARK(LS_STRING_DESTROY, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				ls_string_destroy(iter);
			});

	BENCHMARK(LS_STRING_INVALIDATE, len_tag_idx,
			FOREACH (LSString, iter, arrays.strings){
				ls_string_invalidate(iter);
			});

	FOREACH (LSString, iter, arrays.strings){
		*iter = ls_string_create(bytes, len);
	}
	BENCHMARK(LS_STRING_MOVE, len_tag_idx,
			LSString tmp = ls_string_move(&arrays.strings[0]);
			for (size_t i = 0; i < NELEMS(arrays.strings) - 1; ++i){
				LSString *dest = &arrays.strings[i];
				LSString *src = &arrays.strings[i + 1];
				*dest = ls_string_move(src);
			}
			LSString *last = &arrays.strings[NELEMS(arrays.strings) - 1];
			*last = ls_string_move(&tmp);
			);
	FOREACH (LSString, iter, arrays.strings) {
		ls_string_destroy(iter);
	}

	// ### LSShortString ###

	// warm up memory
	FOREACH (LSShortString, iter, arrays.short_strings) {
		*iter = ls_short_string_create(bytes, len);
	}

	FOREACH (LSShortString, iter, arrays.short_strings) {
		*iter = ls_short_string_create(bytes, len);
	}
	BENCHMARK(LS_SHORT_STRING_IS_VALID, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				vol_int = ls_short_string_is_valid(*iter);
			});

	BENCHMARK(LS_SHORT_STRING_GET_BYTES, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				ls_short_string_get_bytes(iter);
			});

	BENCHMARK(LS_SHORT_STRING_CREATE, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				*iter = ls_short_string_create(bytes, len);
			});

	BENCHMARK(LS_SHORT_STRING_FROM_STRING, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				*iter = ls_short_string_from_string(string);
			});

	BENCHMARK(LS_SHORT_STRING_FROM_SSO, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				*iter = ls_short_string_from_sso(sso);
			});

	BENCHMARK(LS_SHORT_STRING_FROM_SSPAN, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				*iter = ls_short_string_from_sspan(sspan);
			});

	BENCHMARK(LS_SHORT_STRING_FROM_CHARS, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				*iter = ls_short_string_from_chars(cstr, len);
			});

	BENCHMARK(LS_SHORT_STRING_FROM_CSTR, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				*iter = ls_short_string_from_cstr(cstr);
			});

	BENCHMARK(LS_SHORT_STRING_INVALIDATE, len_tag_idx,
			FOREACH (LSShortString, iter, arrays.short_strings){
				ls_short_string_invalidate(iter);
			});

	// ### LSSSOString ###

	// warm up memory
	FOREACH (LSSSOString, iter, arrays.ssos) {
		*iter = ls_sso_create(bytes, len);
	}
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	FOREACH (LSSSOString, iter, arrays.ssos) {
		*iter = ls_sso_create(bytes, len);
	}
	BENCHMARK(LS_SSO_GET_TYPE, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				vol_int = ls_sso_get_type(*iter);
			});
	BENCHMARK(LS_SSO_IS_VALID, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				vol_int = ls_sso_is_valid(*iter);
			});
	BENCHMARK(LS_SSO_GET_BYTES, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				ls_sso_get_bytes(iter);
			});
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(LS_SSO_CREATE, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				*iter = ls_sso_create(bytes, len);
			});
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(LS_SSO_FROM_STRING, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				*iter = ls_sso_from_string(string);
			});
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(LS_SSO_FROM_SHORT_STRING, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				*iter = ls_sso_from_short_string(short_string);
			});
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(LS_SSO_CLONE, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				*iter = ls_sso_clone(sso);
			});
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(LS_SSO_FROM_SSPAN, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				*iter = ls_sso_from_sspan(sspan);
			});
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(LS_SSO_FROM_CHARS, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				*iter = ls_sso_from_chars(cstr, len);
			});
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(LS_SSO_FROM_CSTR, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				*iter = ls_sso_from_cstr(cstr);
			});

	BENCHMARK(LS_SSO_DESTROY, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				ls_sso_destroy(iter);
			});

	BENCHMARK(LS_SSO_INVALIDATE, len_tag_idx,
			FOREACH (LSSSOString, iter, arrays.ssos) {
				ls_sso_invalidate(iter);
			});

	FOREACH (LSSSOString, iter, arrays.ssos){
		*iter = ls_sso_create(bytes, len);
	}
	BENCHMARK(LS_SSO_MOVE, len_tag_idx,
			LSSSOString tmp = ls_sso_move(&arrays.ssos[0]);
			for (size_t i = 0; i < NELEMS(arrays.ssos) - 1; ++i){
				LSSSOString *dest = &arrays.ssos[i];
				LSSSOString *src = &arrays.ssos[i + 1];
				*dest = ls_sso_move(src);
			}
			LSSSOString *last = &arrays.ssos[NELEMS(arrays.ssos) - 1];
			*last = ls_sso_move(&tmp);
			);
	FOREACH (LSSSOString, iter, arrays.ssos) {
		ls_sso_destroy(iter);
	}

	// ### LSStringSpan

	// warm up memory
	FOREACH (LSStringSpan, iter, arrays.sspans) {
		*iter = ls_sspan_create(bytes, len);
	}

	BENCHMARK(LS_SSPAN_IS_VALID, len_tag_idx,
			FOREACH (LSStringSpan, iter, arrays.sspans) {
				vol_int = ls_sspan_is_valid(*iter);
			});

	BENCHMARK(LS_SSPAN_CREATE, len_tag_idx,
			FOREACH (LSStringSpan, iter, arrays.sspans) {
				*iter = ls_sspan_create(bytes, len);
			});

	BENCHMARK(LS_SSPAN_FROM_STRING, len_tag_idx,
			FOREACH (LSStringSpan, iter, arrays.sspans) {
				*iter = ls_sspan_from_string(string);
			});

	BENCHMARK(LS_SSPAN_FROM_SHORT_STRING, len_tag_idx,
			FOREACH (LSStringSpan, iter, arrays.sspans) {
				*iter = ls_sspan_from_short_string(&short_string);
			});

	BENCHMARK(LS_SSPAN_FROM_SSO, len_tag_idx,
			FOREACH (LSStringSpan, iter, arrays.sspans) {
				*iter = ls_sspan_from_sso(&sso);
			});

	BENCHMARK(LS_SSPAN_FROM_CHARS, len_tag_idx,
			FOREACH (LSStringSpan, iter, arrays.sspans) {
				*iter = ls_sspan_from_chars(cstr, len);
			});

	BENCHMARK(LS_SSPAN_FROM_CSTR, len_tag_idx,
			FOREACH (LSStringSpan, iter, arrays.sspans) {
				*iter = ls_sspan_from_cstr(cstr);
			});

	BENCHMARK(LS_SSPAN_INVALIDATE, len_tag_idx,
			FOREACH (LSStringSpan, iter, arrays.sspans) {
				ls_sspan_invalidate(iter);
			});

	FOREACH (LSByteBuffer, iter, arrays.bbufs){
		*iter = ls_bbuf_create();
	}
	BENCHMARK(LS_BBUF_MOVE, len_tag_idx,
			LSByteBuffer tmp = ls_bbuf_move(&arrays.bbufs[0]);
			for (size_t i = 0; i < NELEMS(arrays.bbufs) - 1; ++i){
				LSByteBuffer *dest = &arrays.bbufs[i];
				LSByteBuffer *src = &arrays.bbufs[i + 1];
				*dest = ls_bbuf_move(src);
			}
			LSByteBuffer *last = &arrays.bbufs[NELEMS(arrays.bbufs) - 1];
			*last = ls_bbuf_move(&tmp);
			);
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(LS_BBUF_CREATE_WITH_INIT_CAP, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				*iter = ls_bbuf_create_with_init_cap(len);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(LS_BBUF_FROM_SSPAN, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				*iter = ls_bbuf_from_sspan(sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(LS_BBUF_CLONE, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				*iter = ls_bbuf_clone(bbuf);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		*iter = ls_bbuf_create();
	}
	BENCHMARK(CREATE_THEN_APPEND, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		*iter = ls_bbuf_create_with_init_cap(sspan.len);
	}
	BENCHMARK(CREATE_WIC_THEN_APPEND, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		*iter = ls_bbuf_create();
	}
	BENCHMARK(CREATE_THEN_INSERT, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		*iter = ls_bbuf_create_with_init_cap(sspan.len);
	}
	BENCHMARK(CREATE_WIC_THEN_INSERT, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(CREATE_AND_APPEND, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				*iter = ls_bbuf_create();
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(CREATE_WIC_AND_APPEND, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				*iter = ls_bbuf_create_with_init_cap(sspan.len);
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(CREATE_AND_INSERT, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				*iter = ls_bbuf_create();
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(CREATE_WIC_AND_INSERT, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				*iter = ls_bbuf_create_with_init_cap(sspan.len);
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, arrays.bbufs) {
		*iter = ls_bbuf_clone(bbuf);
	}
	BENCHMARK(LS_BBUF_DESTROY, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				ls_bbuf_destroy(iter);
			});

	BENCHMARK(LS_BBUF_INVALIDATE, len_tag_idx,
			FOREACH (LSByteBuffer, iter, arrays.bbufs) {
				ls_bbuf_invalidate(iter);
			});

	ls_string_destroy(&string);
	ls_sso_destroy(&sso);
	ls_bbuf_destroy(&bbuf);
}

size_t size_max(size_t a, size_t b)
{
	return a > b ? a : b;
}
