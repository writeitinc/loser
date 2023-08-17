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

#define FOREACH_AOU(U, T, member, ident, arr) \
	for (T *ident = &(arr)[0].member; (U *)ident != &(arr)[NELEMS(arr)]; ident = (T *)(((U *)ident) + 1))

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
} uoa;

typedef union StringUnion {
	LSString string;
	LSShortString short_string;
	LSSSOString sso;
	LSStringSpan sspan;
	LSByteBuffer bbuf;
} StringUnion;
static StringUnion aou[NITERATIONS];

enum Function {
	UOA_LS_STRING_IS_VALID = 0,
	UOA_LS_STRING_CREATE,
	UOA_LS_STRING_CLONE,
	UOA_LS_STRING_FROM_SHORT_STRING,
	UOA_LS_STRING_FROM_SSO,
	UOA_LS_STRING_FROM_SSPAN,
	UOA_LS_STRING_FROM_CHARS,
	UOA_LS_STRING_FROM_CSTR,
	UOA_LS_STRING_DESTROY,
	UOA_LS_STRING_INVALIDATE,
	UOA_LS_STRING_MOVE,
	UOA_LS_SHORT_STRING_IS_VALID,
	UOA_LS_SHORT_STRING_GET_BYTES,
	UOA_LS_SHORT_STRING_CREATE,
	UOA_LS_SHORT_STRING_FROM_STRING,
	UOA_LS_SHORT_STRING_FROM_SSO,
	UOA_LS_SHORT_STRING_FROM_SSPAN,
	UOA_LS_SHORT_STRING_FROM_CHARS,
	UOA_LS_SHORT_STRING_FROM_CSTR,
	UOA_LS_SHORT_STRING_INVALIDATE,
	UOA_LS_SSO_GET_TYPE,
	UOA_LS_SSO_IS_VALID,
	UOA_LS_SSO_GET_BYTES,
	UOA_LS_SSO_CREATE,
	UOA_LS_SSO_FROM_STRING,
	UOA_LS_SSO_FROM_SHORT_STRING,
	UOA_LS_SSO_CLONE,
	UOA_LS_SSO_FROM_SSPAN,
	UOA_LS_SSO_FROM_CHARS,
	UOA_LS_SSO_FROM_CSTR,
	UOA_LS_SSO_DESTROY,
	UOA_LS_SSO_INVALIDATE,
	UOA_LS_SSO_MOVE,
	UOA_LS_SSPAN_IS_VALID,
	UOA_LS_SSPAN_CREATE,
	UOA_LS_SSPAN_FROM_STRING,
	UOA_LS_SSPAN_FROM_SHORT_STRING,
	UOA_LS_SSPAN_FROM_SSO,
	UOA_LS_SSPAN_FROM_CHARS,
	UOA_LS_SSPAN_FROM_CSTR,
	UOA_LS_SSPAN_INVALIDATE,
	UOA_LS_BBUF_CREATE_WITH_INIT_CAP,
	UOA_LS_BBUF_FROM_SSPAN,
	UOA_LS_BBUF_CLONE,
	UOA_CREATE_THEN_APPEND,
	UOA_CREATE_WIC_THEN_APPEND,
	UOA_CREATE_THEN_INSERT,
	UOA_CREATE_WIC_THEN_INSERT,
	UOA_CREATE_AND_APPEND,
	UOA_CREATE_WIC_AND_APPEND,
	UOA_CREATE_AND_INSERT,
	UOA_CREATE_WIC_AND_INSERT,
	UOA_LS_BBUF_DESTROY,
	UOA_LS_BBUF_INVALIDATE,
	UOA_LS_BBUF_MOVE,

	AOU_LS_STRING_IS_VALID,
	AOU_LS_STRING_CREATE,
	AOU_LS_STRING_CLONE,
	AOU_LS_STRING_FROM_SHORT_STRING,
	AOU_LS_STRING_FROM_SSO,
	AOU_LS_STRING_FROM_SSPAN,
	AOU_LS_STRING_FROM_CHARS,
	AOU_LS_STRING_FROM_CSTR,
	AOU_LS_STRING_DESTROY,
	AOU_LS_STRING_INVALIDATE,
	AOU_LS_STRING_MOVE,
	AOU_LS_SHORT_STRING_IS_VALID,
	AOU_LS_SHORT_STRING_GET_BYTES,
	AOU_LS_SHORT_STRING_CREATE,
	AOU_LS_SHORT_STRING_FROM_STRING,
	AOU_LS_SHORT_STRING_FROM_SSO,
	AOU_LS_SHORT_STRING_FROM_SSPAN,
	AOU_LS_SHORT_STRING_FROM_CHARS,
	AOU_LS_SHORT_STRING_FROM_CSTR,
	AOU_LS_SHORT_STRING_INVALIDATE,
	AOU_LS_SSO_GET_TYPE,
	AOU_LS_SSO_IS_VALID,
	AOU_LS_SSO_GET_BYTES,
	AOU_LS_SSO_CREATE,
	AOU_LS_SSO_FROM_STRING,
	AOU_LS_SSO_FROM_SHORT_STRING,
	AOU_LS_SSO_CLONE,
	AOU_LS_SSO_FROM_SSPAN,
	AOU_LS_SSO_FROM_CHARS,
	AOU_LS_SSO_FROM_CSTR,
	AOU_LS_SSO_DESTROY,
	AOU_LS_SSO_INVALIDATE,
	AOU_LS_SSO_MOVE,
	AOU_LS_SSPAN_IS_VALID,
	AOU_LS_SSPAN_CREATE,
	AOU_LS_SSPAN_FROM_STRING,
	AOU_LS_SSPAN_FROM_SHORT_STRING,
	AOU_LS_SSPAN_FROM_SSO,
	AOU_LS_SSPAN_FROM_CHARS,
	AOU_LS_SSPAN_FROM_CSTR,
	AOU_LS_SSPAN_INVALIDATE,
	AOU_LS_BBUF_CREATE_WITH_INIT_CAP,
	AOU_LS_BBUF_FROM_SSPAN,
	AOU_LS_BBUF_CLONE,
	AOU_CREATE_THEN_APPEND,
	AOU_CREATE_WIC_THEN_APPEND,
	AOU_CREATE_THEN_INSERT,
	AOU_CREATE_WIC_THEN_INSERT,
	AOU_CREATE_AND_APPEND,
	AOU_CREATE_WIC_AND_APPEND,
	AOU_CREATE_AND_INSERT,
	AOU_CREATE_WIC_AND_INSERT,
	AOU_LS_BBUF_DESTROY,
	AOU_LS_BBUF_INVALIDATE,
	AOU_LS_BBUF_MOVE,

	NFUNCTIONS
};

static const char *FUNC_NAMES[NFUNCTIONS] = {
	[UOA_LS_STRING_IS_VALID]              = "[uoa]ls_string_is_valid",
	[UOA_LS_STRING_CREATE]                = "[uoa]ls_string_create",
	[UOA_LS_STRING_CLONE]                 = "[uoa]ls_string_clone",
	[UOA_LS_STRING_FROM_SHORT_STRING]     = "[uoa]ls_string_from_short_string",
	[UOA_LS_STRING_FROM_SSO]              = "[uoa]ls_string_from_sso",
	[UOA_LS_STRING_FROM_SSPAN]            = "[uoa]ls_string_from_sspan",
	[UOA_LS_STRING_FROM_CHARS]            = "[uoa]ls_string_from_chars",
	[UOA_LS_STRING_FROM_CSTR]             = "[uoa]ls_string_from_cstr",
	[UOA_LS_STRING_DESTROY]               = "[uoa]ls_string_destroy",
	[UOA_LS_STRING_INVALIDATE]            = "[uoa]ls_string_invalidate",
	[UOA_LS_STRING_MOVE]                  = "[uoa]ls_string_move",
	[UOA_LS_SHORT_STRING_IS_VALID]        = "[uoa]ls_short_string_is_valid",
	[UOA_LS_SHORT_STRING_GET_BYTES]       = "[uoa]ls_short_string_get_bytes",
	[UOA_LS_SHORT_STRING_CREATE]          = "[uoa]ls_short_string_create",
	[UOA_LS_SHORT_STRING_FROM_STRING]     = "[uoa]ls_short_string_from_string",
	[UOA_LS_SHORT_STRING_FROM_SSO]        = "[uoa]ls_short_string_from_sso",
	[UOA_LS_SHORT_STRING_FROM_SSPAN]      = "[uoa]ls_short_string_from_sspan",
	[UOA_LS_SHORT_STRING_FROM_CHARS]      = "[uoa]ls_short_string_from_chars",
	[UOA_LS_SHORT_STRING_FROM_CSTR]       = "[uoa]ls_short_string_from_cstr",
	[UOA_LS_SHORT_STRING_INVALIDATE]      = "[uoa]ls_short_string_invalidate",
	[UOA_LS_SSO_GET_TYPE]                 = "[uoa]ls_sso_get_type",
	[UOA_LS_SSO_IS_VALID]                 = "[uoa]ls_sso_is_valid",
	[UOA_LS_SSO_GET_BYTES]                = "[uoa]ls_sso_get_bytes",
	[UOA_LS_SSO_CREATE]                   = "[uoa]ls_sso_create",
	[UOA_LS_SSO_FROM_STRING]              = "[uoa]ls_sso_from_string",
	[UOA_LS_SSO_FROM_SHORT_STRING]        = "[uoa]ls_sso_from_short_string",
	[UOA_LS_SSO_CLONE]                    = "[uoa]ls_sso_clone",
	[UOA_LS_SSO_FROM_SSPAN]               = "[uoa]ls_sso_from_sspan",
	[UOA_LS_SSO_FROM_CHARS]               = "[uoa]ls_sso_from_chars",
	[UOA_LS_SSO_FROM_CSTR]                = "[uoa]ls_sso_from_cstr",
	[UOA_LS_SSO_DESTROY]                  = "[uoa]ls_sso_destroy",
	[UOA_LS_SSO_INVALIDATE]               = "[uoa]ls_sso_invalidate",
	[UOA_LS_SSO_MOVE]                     = "[uoa]ls_sso_move",
	[UOA_LS_SSPAN_IS_VALID]               = "[uoa]ls_sspan_is_valid",
	[UOA_LS_SSPAN_CREATE]                 = "[uoa]ls_sspan_create",
	[UOA_LS_SSPAN_FROM_STRING]            = "[uoa]ls_sspan_from_string",
	[UOA_LS_SSPAN_FROM_SHORT_STRING]      = "[uoa]ls_sspan_from_short_string",
	[UOA_LS_SSPAN_FROM_SSO]               = "[uoa]ls_sspan_from_sso",
	[UOA_LS_SSPAN_FROM_CHARS]             = "[uoa]ls_sspan_from_chars",
	[UOA_LS_SSPAN_FROM_CSTR]              = "[uoa]ls_sspan_from_cstr",
	[UOA_LS_SSPAN_INVALIDATE]             = "[uoa]ls_sspan_invalidate",
	[UOA_LS_BBUF_CREATE_WITH_INIT_CAP]    = "[uoa]ls_bbuf_create_with_init_cap",
	[UOA_LS_BBUF_FROM_SSPAN]              = "[uoa]ls_bbuf_from_sspan",
	[UOA_LS_BBUF_CLONE]                   = "[uoa]ls_bbuf_clone",
	[UOA_CREATE_THEN_APPEND]              = "[uoa](create bbuf), append",
	[UOA_CREATE_WIC_THEN_APPEND]          = "[uoa](create bbuf w/ init cap), append",
	[UOA_CREATE_THEN_INSERT]              = "[uoa](create bbuf), insert",
	[UOA_CREATE_WIC_THEN_INSERT]          = "[uoa](create bbuf w/ init cap), insert",
	[UOA_CREATE_AND_APPEND]               = "[uoa]create bbuf + append",
	[UOA_CREATE_WIC_AND_APPEND]           = "[uoa]create bbuf w/ init cap + append",
	[UOA_CREATE_AND_INSERT]               = "[uoa]create bbuf + insert",
	[UOA_CREATE_WIC_AND_INSERT]           = "[uoa]create bbuf w/ init cap + insert",
	[UOA_LS_BBUF_DESTROY]                 = "[uoa]ls_bbuf_destroy",
	[UOA_LS_BBUF_INVALIDATE]              = "[uoa]ls_bbuf_invalidate",
	[UOA_LS_BBUF_MOVE]                    = "[uoa]ls_bbuf_move",

	[AOU_LS_STRING_IS_VALID]              = "[aou]ls_string_is_valid",
	[AOU_LS_STRING_CREATE]                = "[aou]ls_string_create",
	[AOU_LS_STRING_CLONE]                 = "[aou]ls_string_clone",
	[AOU_LS_STRING_FROM_SHORT_STRING]     = "[aou]ls_string_from_short_string",
	[AOU_LS_STRING_FROM_SSO]              = "[aou]ls_string_from_sso",
	[AOU_LS_STRING_FROM_SSPAN]            = "[aou]ls_string_from_sspan",
	[AOU_LS_STRING_FROM_CHARS]            = "[aou]ls_string_from_chars",
	[AOU_LS_STRING_FROM_CSTR]             = "[aou]ls_string_from_cstr",
	[AOU_LS_STRING_DESTROY]               = "[aou]ls_string_destroy",
	[AOU_LS_STRING_INVALIDATE]            = "[aou]ls_string_invalidate",
	[AOU_LS_STRING_MOVE]                  = "[aou]ls_string_move",
	[AOU_LS_SHORT_STRING_IS_VALID]        = "[aou]ls_short_string_is_valid",
	[AOU_LS_SHORT_STRING_GET_BYTES]       = "[aou]ls_short_string_get_bytes",
	[AOU_LS_SHORT_STRING_CREATE]          = "[aou]ls_short_string_create",
	[AOU_LS_SHORT_STRING_FROM_STRING]     = "[aou]ls_short_string_from_string",
	[AOU_LS_SHORT_STRING_FROM_SSO]        = "[aou]ls_short_string_from_sso",
	[AOU_LS_SHORT_STRING_FROM_SSPAN]      = "[aou]ls_short_string_from_sspan",
	[AOU_LS_SHORT_STRING_FROM_CHARS]      = "[aou]ls_short_string_from_chars",
	[AOU_LS_SHORT_STRING_FROM_CSTR]       = "[aou]ls_short_string_from_cstr",
	[AOU_LS_SHORT_STRING_INVALIDATE]      = "[aou]ls_short_string_invalidate",
	[AOU_LS_SSO_GET_TYPE]                 = "[aou]ls_sso_get_type",
	[AOU_LS_SSO_IS_VALID]                 = "[aou]ls_sso_is_valid",
	[AOU_LS_SSO_GET_BYTES]                = "[aou]ls_sso_get_bytes",
	[AOU_LS_SSO_CREATE]                   = "[aou]ls_sso_create",
	[AOU_LS_SSO_FROM_STRING]              = "[aou]ls_sso_from_string",
	[AOU_LS_SSO_FROM_SHORT_STRING]        = "[aou]ls_sso_from_short_string",
	[AOU_LS_SSO_CLONE]                    = "[aou]ls_sso_clone",
	[AOU_LS_SSO_FROM_SSPAN]               = "[aou]ls_sso_from_sspan",
	[AOU_LS_SSO_FROM_CHARS]               = "[aou]ls_sso_from_chars",
	[AOU_LS_SSO_FROM_CSTR]                = "[aou]ls_sso_from_cstr",
	[AOU_LS_SSO_DESTROY]                  = "[aou]ls_sso_destroy",
	[AOU_LS_SSO_INVALIDATE]               = "[aou]ls_sso_invalidate",
	[AOU_LS_SSO_MOVE]                     = "[aou]ls_sso_move",
	[AOU_LS_SSPAN_IS_VALID]               = "[aou]ls_sspan_is_valid",
	[AOU_LS_SSPAN_CREATE]                 = "[aou]ls_sspan_create",
	[AOU_LS_SSPAN_FROM_STRING]            = "[aou]ls_sspan_from_string",
	[AOU_LS_SSPAN_FROM_SHORT_STRING]      = "[aou]ls_sspan_from_short_string",
	[AOU_LS_SSPAN_FROM_SSO]               = "[aou]ls_sspan_from_sso",
	[AOU_LS_SSPAN_FROM_CHARS]             = "[aou]ls_sspan_from_chars",
	[AOU_LS_SSPAN_FROM_CSTR]              = "[aou]ls_sspan_from_cstr",
	[AOU_LS_SSPAN_INVALIDATE]             = "[aou]ls_sspan_invalidate",
	[AOU_LS_BBUF_CREATE_WITH_INIT_CAP]    = "[aou]ls_bbuf_create_with_init_cap",
	[AOU_LS_BBUF_FROM_SSPAN]              = "[aou]ls_bbuf_from_sspan",
	[AOU_LS_BBUF_CLONE]                   = "[aou]ls_bbuf_clone",
	[AOU_CREATE_THEN_APPEND]              = "[aou](create bbuf), append",
	[AOU_CREATE_WIC_THEN_APPEND]          = "[aou](create bbuf w/ init cap), append",
	[AOU_CREATE_THEN_INSERT]              = "[aou](create bbuf), insert",
	[AOU_CREATE_WIC_THEN_INSERT]          = "[aou](create bbuf w/ init cap), insert",
	[AOU_CREATE_AND_APPEND]               = "[aou]create bbuf + append",
	[AOU_CREATE_WIC_AND_APPEND]           = "[aou]create bbuf w/ init cap + append",
	[AOU_CREATE_AND_INSERT]               = "[aou]create bbuf + insert",
	[AOU_CREATE_WIC_AND_INSERT]           = "[aou]create bbuf w/ init cap + insert",
	[AOU_LS_BBUF_DESTROY]                 = "[aou]ls_bbuf_destroy",
	[AOU_LS_BBUF_INVALIDATE]              = "[aou]ls_bbuf_invalidate",
	[AOU_LS_BBUF_MOVE]                    = "[aou]ls_bbuf_move",
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

	// ### [UOA] LSString ###

	// warm up memory
	FOREACH (LSString, iter, uoa.strings) {
		*iter = ls_string_create(bytes, len);
	}
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	FOREACH (LSString, iter, uoa.strings) {
		*iter = ls_string_create(bytes, len);
	}
	BENCHMARK(UOA_LS_STRING_IS_VALID, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				vol_int = ls_string_is_valid(*iter);
			});
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(UOA_LS_STRING_CREATE, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				*iter = ls_string_create(bytes, len);
			});
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(UOA_LS_STRING_CLONE, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				*iter = ls_string_clone(string);
			});
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(UOA_LS_STRING_FROM_SHORT_STRING, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				*iter = ls_string_from_short_string(short_string);
		 	});
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(UOA_LS_STRING_FROM_SSO, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				*iter = ls_string_from_sso(sso);
			});
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(UOA_LS_STRING_FROM_SSPAN, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				*iter = ls_string_from_sspan(sspan);
			});
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(UOA_LS_STRING_FROM_CHARS, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				*iter = ls_string_from_chars(cstr, len);
			});
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	BENCHMARK(UOA_LS_STRING_FROM_CSTR, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				*iter = ls_string_from_cstr(cstr);
			});
	BENCHMARK(UOA_LS_STRING_DESTROY, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				ls_string_destroy(iter);
			});

	BENCHMARK(UOA_LS_STRING_INVALIDATE, len_tag_idx,
			FOREACH (LSString, iter, uoa.strings) {
				ls_string_invalidate(iter);
			});

	FOREACH (LSString, iter, uoa.strings) {
		*iter = ls_string_create(bytes, len);
	}
	BENCHMARK(UOA_LS_STRING_MOVE, len_tag_idx,
			LSString tmp = ls_string_move(&uoa.strings[0]);
			for (size_t i = 0; i < NELEMS(uoa.strings) - 1; ++i) {
				LSString *dest = &uoa.strings[i];
				LSString *src = &uoa.strings[i + 1];
				*dest = ls_string_move(src);
			}
			LSString *last = &uoa.strings[NELEMS(uoa.strings) - 1];
			*last = tmp;
			);
	FOREACH (LSString, iter, uoa.strings) {
		ls_string_destroy(iter);
	}

	// ### [UOA] LSShortString ###

	// warm up memory
	FOREACH (LSShortString, iter, uoa.short_strings) {
		*iter = ls_short_string_create(bytes, len);
	}

	FOREACH (LSShortString, iter, uoa.short_strings) {
		*iter = ls_short_string_create(bytes, len);
	}
	BENCHMARK(UOA_LS_SHORT_STRING_IS_VALID, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				vol_int = ls_short_string_is_valid(*iter);
			});

	BENCHMARK(UOA_LS_SHORT_STRING_GET_BYTES, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				ls_short_string_get_bytes(iter);
			});

	BENCHMARK(UOA_LS_SHORT_STRING_CREATE, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				*iter = ls_short_string_create(bytes, len);
			});

	BENCHMARK(UOA_LS_SHORT_STRING_FROM_STRING, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				*iter = ls_short_string_from_string(string);
			});

	BENCHMARK(UOA_LS_SHORT_STRING_FROM_SSO, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				*iter = ls_short_string_from_sso(sso);
			});

	BENCHMARK(UOA_LS_SHORT_STRING_FROM_SSPAN, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				*iter = ls_short_string_from_sspan(sspan);
			});

	BENCHMARK(UOA_LS_SHORT_STRING_FROM_CHARS, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				*iter = ls_short_string_from_chars(cstr, len);
			});

	BENCHMARK(UOA_LS_SHORT_STRING_FROM_CSTR, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				*iter = ls_short_string_from_cstr(cstr);
			});

	BENCHMARK(UOA_LS_SHORT_STRING_INVALIDATE, len_tag_idx,
			FOREACH (LSShortString, iter, uoa.short_strings) {
				ls_short_string_invalidate(iter);
			});

	// ### [UOA] LSSSOString ###

	// warm up memory
	FOREACH (LSSSOString, iter, uoa.ssos) {
		*iter = ls_sso_create(bytes, len);
	}
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	FOREACH (LSSSOString, iter, uoa.ssos) {
		*iter = ls_sso_create(bytes, len);
	}
	BENCHMARK(UOA_LS_SSO_GET_TYPE, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				vol_int = ls_sso_get_type(*iter);
			});
	BENCHMARK(UOA_LS_SSO_IS_VALID, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				vol_int = ls_sso_is_valid(*iter);
			});
	BENCHMARK(UOA_LS_SSO_GET_BYTES, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				ls_sso_get_bytes(iter);
			});
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(UOA_LS_SSO_CREATE, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				*iter = ls_sso_create(bytes, len);
			});
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(UOA_LS_SSO_FROM_STRING, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				*iter = ls_sso_from_string(string);
			});
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(UOA_LS_SSO_FROM_SHORT_STRING, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				*iter = ls_sso_from_short_string(short_string);
			});
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(UOA_LS_SSO_CLONE, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				*iter = ls_sso_clone(sso);
			});
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(UOA_LS_SSO_FROM_SSPAN, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				*iter = ls_sso_from_sspan(sspan);
			});
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(UOA_LS_SSO_FROM_CHARS, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				*iter = ls_sso_from_chars(cstr, len);
			});
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(UOA_LS_SSO_FROM_CSTR, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				*iter = ls_sso_from_cstr(cstr);
			});

	BENCHMARK(UOA_LS_SSO_DESTROY, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				ls_sso_destroy(iter);
			});

	BENCHMARK(UOA_LS_SSO_INVALIDATE, len_tag_idx,
			FOREACH (LSSSOString, iter, uoa.ssos) {
				ls_sso_invalidate(iter);
			});

	FOREACH (LSSSOString, iter, uoa.ssos) {
		*iter = ls_sso_create(bytes, len);
	}
	BENCHMARK(UOA_LS_SSO_MOVE, len_tag_idx,
			LSSSOString tmp = ls_sso_move(&uoa.ssos[0]);
			for (size_t i = 0; i < NELEMS(uoa.ssos) - 1; ++i) {
				LSSSOString *dest = &uoa.ssos[i];
				LSSSOString *src = &uoa.ssos[i + 1];
				*dest = ls_sso_move(src);
			}
			LSSSOString *last = &uoa.ssos[NELEMS(uoa.ssos) - 1];
			*last = tmp;
			);
	FOREACH (LSSSOString, iter, uoa.ssos) {
		ls_sso_destroy(iter);
	}

	// ### [UOA] LSStringSpan ###

	// warm up memory
	FOREACH (LSStringSpan, iter, uoa.sspans) {
		*iter = ls_sspan_create(bytes, len);
	}

	BENCHMARK(UOA_LS_SSPAN_IS_VALID, len_tag_idx,
			FOREACH (LSStringSpan, iter, uoa.sspans) {
				vol_int = ls_sspan_is_valid(*iter);
			});

	BENCHMARK(UOA_LS_SSPAN_CREATE, len_tag_idx,
			FOREACH (LSStringSpan, iter, uoa.sspans) {
				*iter = ls_sspan_create(bytes, len);
			});

	BENCHMARK(UOA_LS_SSPAN_FROM_STRING, len_tag_idx,
			FOREACH (LSStringSpan, iter, uoa.sspans) {
				*iter = ls_sspan_from_string(string);
			});

	BENCHMARK(UOA_LS_SSPAN_FROM_SHORT_STRING, len_tag_idx,
			FOREACH (LSStringSpan, iter, uoa.sspans) {
				*iter = ls_sspan_from_short_string(&short_string);
			});

	BENCHMARK(UOA_LS_SSPAN_FROM_SSO, len_tag_idx,
			FOREACH (LSStringSpan, iter, uoa.sspans) {
				*iter = ls_sspan_from_sso(&sso);
			});

	BENCHMARK(UOA_LS_SSPAN_FROM_CHARS, len_tag_idx,
			FOREACH (LSStringSpan, iter, uoa.sspans) {
				*iter = ls_sspan_from_chars(cstr, len);
			});

	BENCHMARK(UOA_LS_SSPAN_FROM_CSTR, len_tag_idx,
			FOREACH (LSStringSpan, iter, uoa.sspans) {
				*iter = ls_sspan_from_cstr(cstr);
			});

	BENCHMARK(UOA_LS_SSPAN_INVALIDATE, len_tag_idx,
			FOREACH (LSStringSpan, iter, uoa.sspans) {
				ls_sspan_invalidate(iter);
			});

	// ### [UOA] LSByteBuffer ###

	// warm up memory
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		*iter = ls_bbuf_create();
	}
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(UOA_LS_BBUF_CREATE_WITH_INIT_CAP, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				*iter = ls_bbuf_create_with_init_cap(len);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(UOA_LS_BBUF_FROM_SSPAN, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				*iter = ls_bbuf_from_sspan(sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(UOA_LS_BBUF_CLONE, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				*iter = ls_bbuf_clone(bbuf);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		*iter = ls_bbuf_create();
	}
	BENCHMARK(UOA_CREATE_THEN_APPEND, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		*iter = ls_bbuf_create_with_init_cap(sspan.len);
	}
	BENCHMARK(UOA_CREATE_WIC_THEN_APPEND, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		*iter = ls_bbuf_create();
	}
	BENCHMARK(UOA_CREATE_THEN_INSERT, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		*iter = ls_bbuf_create_with_init_cap(sspan.len);
	}
	BENCHMARK(UOA_CREATE_WIC_THEN_INSERT, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(UOA_CREATE_AND_APPEND, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				*iter = ls_bbuf_create();
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(UOA_CREATE_WIC_AND_APPEND, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				*iter = ls_bbuf_create_with_init_cap(sspan.len);
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(UOA_CREATE_AND_INSERT, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				*iter = ls_bbuf_create();
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(UOA_CREATE_WIC_AND_INSERT, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				*iter = ls_bbuf_create_with_init_cap(sspan.len);
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		*iter = ls_bbuf_clone(bbuf);
	}
	BENCHMARK(UOA_LS_BBUF_DESTROY, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				ls_bbuf_destroy(iter);
			});

	BENCHMARK(UOA_LS_BBUF_INVALIDATE, len_tag_idx,
			FOREACH (LSByteBuffer, iter, uoa.bbufs) {
				ls_bbuf_invalidate(iter);
			});

	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		*iter = ls_bbuf_create();
	}
	BENCHMARK(UOA_LS_BBUF_MOVE, len_tag_idx,
			LSByteBuffer tmp = ls_bbuf_move(&uoa.bbufs[0]);
			for (size_t i = 0; i < NELEMS(uoa.bbufs) - 1; ++i) {
				LSByteBuffer *dest = &uoa.bbufs[i];
				LSByteBuffer *src = &uoa.bbufs[i + 1];
				*dest = ls_bbuf_move(src);
			}
			LSByteBuffer *last = &uoa.bbufs[NELEMS(uoa.bbufs) - 1];
			*last = tmp;
			);
	FOREACH (LSByteBuffer, iter, uoa.bbufs) {
		ls_bbuf_destroy(iter);
	}

	// ### AOU ###

	// warm up memory (only once for AOU)
	FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
		*iter = LS_AN_INVALID_SHORT_STRING;
	}

	// ### [AOU] LSString ###

	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		*iter = ls_string_create(bytes, len);
	}
	BENCHMARK(AOU_LS_STRING_IS_VALID, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				vol_int = ls_string_is_valid(*iter);
			});
	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		ls_string_destroy(iter);
	}

	BENCHMARK(AOU_LS_STRING_CREATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				*iter = ls_string_create(bytes, len);
			});
	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		ls_string_destroy(iter);
	}

	BENCHMARK(AOU_LS_STRING_CLONE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				*iter = ls_string_clone(string);
			});
	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		ls_string_destroy(iter);
	}

	BENCHMARK(AOU_LS_STRING_FROM_SHORT_STRING, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				*iter = ls_string_from_short_string(short_string);
		 	});
	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		ls_string_destroy(iter);
	}

	BENCHMARK(AOU_LS_STRING_FROM_SSO, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				*iter = ls_string_from_sso(sso);
			});
	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		ls_string_destroy(iter);
	}

	BENCHMARK(AOU_LS_STRING_FROM_SSPAN, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				*iter = ls_string_from_sspan(sspan);
			});
	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		ls_string_destroy(iter);
	}

	BENCHMARK(AOU_LS_STRING_FROM_CHARS, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				*iter = ls_string_from_chars(cstr, len);
			});
	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		ls_string_destroy(iter);
	}

	BENCHMARK(AOU_LS_STRING_FROM_CSTR, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				*iter = ls_string_from_cstr(cstr);
			});
	BENCHMARK(AOU_LS_STRING_DESTROY, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				ls_string_destroy(iter);
			});

	BENCHMARK(AOU_LS_STRING_INVALIDATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
				ls_string_invalidate(iter);
			});

	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		*iter = ls_string_create(bytes, len);
	}
	BENCHMARK(AOU_LS_STRING_MOVE, len_tag_idx,
			LSString tmp = ls_string_move(&aou[0].string);
			for (size_t i = 0; i < NELEMS(aou) - 1; ++i) {
				LSString *dest = &aou[i].string;
				LSString *src = &aou[i + 1].string;
				*dest = ls_string_move(src);
			}
			LSString *last = &aou[NELEMS(aou) - 1].string;
			*last = tmp;
			);
	FOREACH_AOU (StringUnion, LSString, string, iter, aou) {
		ls_string_destroy(iter);
	}

	// ### [AOU] LSShortString ###

	FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
		*iter = ls_short_string_create(bytes, len);
	}
	BENCHMARK(AOU_LS_SHORT_STRING_IS_VALID, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				vol_int = ls_short_string_is_valid(*iter);
			});

	BENCHMARK(AOU_LS_SHORT_STRING_GET_BYTES, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				ls_short_string_get_bytes(iter);
			});

	BENCHMARK(AOU_LS_SHORT_STRING_CREATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				*iter = ls_short_string_create(bytes, len);
			});

	BENCHMARK(AOU_LS_SHORT_STRING_FROM_STRING, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				*iter = ls_short_string_from_string(string);
			});

	BENCHMARK(AOU_LS_SHORT_STRING_FROM_SSO, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				*iter = ls_short_string_from_sso(sso);
			});

	BENCHMARK(AOU_LS_SHORT_STRING_FROM_SSPAN, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				*iter = ls_short_string_from_sspan(sspan);
			});

	BENCHMARK(AOU_LS_SHORT_STRING_FROM_CHARS, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				*iter = ls_short_string_from_chars(cstr, len);
			});

	BENCHMARK(AOU_LS_SHORT_STRING_FROM_CSTR, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				*iter = ls_short_string_from_cstr(cstr);
			});

	BENCHMARK(AOU_LS_SHORT_STRING_INVALIDATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSShortString, short_string, iter, aou) {
				ls_short_string_invalidate(iter);
			});

	// ### [AOU] LSSSOString ###

	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		*iter = ls_sso_create(bytes, len);
	}
	BENCHMARK(AOU_LS_SSO_GET_TYPE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				vol_int = ls_sso_get_type(*iter);
			});
	BENCHMARK(AOU_LS_SSO_IS_VALID, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				vol_int = ls_sso_is_valid(*iter);
			});
	BENCHMARK(AOU_LS_SSO_GET_BYTES, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				ls_sso_get_bytes(iter);
			});
	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(AOU_LS_SSO_CREATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				*iter = ls_sso_create(bytes, len);
			});
	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(AOU_LS_SSO_FROM_STRING, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				*iter = ls_sso_from_string(string);
			});
	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(AOU_LS_SSO_FROM_SHORT_STRING, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				*iter = ls_sso_from_short_string(short_string);
			});
	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(AOU_LS_SSO_CLONE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				*iter = ls_sso_clone(sso);
			});
	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(AOU_LS_SSO_FROM_SSPAN, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				*iter = ls_sso_from_sspan(sspan);
			});
	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(AOU_LS_SSO_FROM_CHARS, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				*iter = ls_sso_from_chars(cstr, len);
			});
	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		ls_sso_destroy(iter);
	}

	BENCHMARK(AOU_LS_SSO_FROM_CSTR, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				*iter = ls_sso_from_cstr(cstr);
			});

	BENCHMARK(AOU_LS_SSO_DESTROY, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				ls_sso_destroy(iter);
			});

	BENCHMARK(AOU_LS_SSO_INVALIDATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
				ls_sso_invalidate(iter);
			});

	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		*iter = ls_sso_create(bytes, len);
	}
	BENCHMARK(AOU_LS_SSO_MOVE, len_tag_idx,
			LSSSOString tmp = ls_sso_move(&aou[0].sso);
			for (size_t i = 0; i < NELEMS(aou) - 1; ++i) {
				LSSSOString *dest = &aou[i].sso;
				LSSSOString *src = &aou[i + 1].sso;
				*dest = ls_sso_move(src);
			}
			LSSSOString *last = &aou[NELEMS(aou) - 1].sso;
			*last = tmp;
			);
	FOREACH_AOU (StringUnion, LSSSOString, sso, iter, aou) {
		ls_sso_destroy(iter);
	}

	// ### [AOU] LSStringSpan ###

	BENCHMARK(AOU_LS_SSPAN_IS_VALID, len_tag_idx,
			FOREACH_AOU (StringUnion, LSStringSpan, sspan, iter, aou) {
				vol_int = ls_sspan_is_valid(*iter);
			});

	BENCHMARK(AOU_LS_SSPAN_CREATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSStringSpan, sspan, iter, aou) {
				*iter = ls_sspan_create(bytes, len);
			});

	BENCHMARK(AOU_LS_SSPAN_FROM_STRING, len_tag_idx,
			FOREACH_AOU (StringUnion, LSStringSpan, sspan, iter, aou) {
				*iter = ls_sspan_from_string(string);
			});

	BENCHMARK(AOU_LS_SSPAN_FROM_SHORT_STRING, len_tag_idx,
			FOREACH_AOU (StringUnion, LSStringSpan, sspan, iter, aou) {
				*iter = ls_sspan_from_short_string(&short_string);
			});

	BENCHMARK(AOU_LS_SSPAN_FROM_SSO, len_tag_idx,
			FOREACH_AOU (StringUnion, LSStringSpan, sspan, iter, aou) {
				*iter = ls_sspan_from_sso(&sso);
			});

	BENCHMARK(AOU_LS_SSPAN_FROM_CHARS, len_tag_idx,
			FOREACH_AOU (StringUnion, LSStringSpan, sspan, iter, aou) {
				*iter = ls_sspan_from_chars(cstr, len);
			});

	BENCHMARK(AOU_LS_SSPAN_FROM_CSTR, len_tag_idx,
			FOREACH_AOU (StringUnion, LSStringSpan, sspan, iter, aou) {
				*iter = ls_sspan_from_cstr(cstr);
			});

	BENCHMARK(AOU_LS_SSPAN_INVALIDATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSStringSpan, sspan, iter, aou) {
				ls_sspan_invalidate(iter);
			});

	// ### [AOU] LSByteBuffer ###

	BENCHMARK(AOU_LS_BBUF_CREATE_WITH_INIT_CAP, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				*iter = ls_bbuf_create_with_init_cap(len);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(AOU_LS_BBUF_FROM_SSPAN, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				*iter = ls_bbuf_from_sspan(sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(AOU_LS_BBUF_CLONE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				*iter = ls_bbuf_clone(bbuf);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		*iter = ls_bbuf_create();
	}
	BENCHMARK(AOU_CREATE_THEN_APPEND, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		*iter = ls_bbuf_create_with_init_cap(sspan.len);
	}
	BENCHMARK(AOU_CREATE_WIC_THEN_APPEND, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		*iter = ls_bbuf_create();
	}
	BENCHMARK(AOU_CREATE_THEN_INSERT, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		*iter = ls_bbuf_create_with_init_cap(sspan.len);
	}
	BENCHMARK(AOU_CREATE_WIC_THEN_INSERT, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(AOU_CREATE_AND_APPEND, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				*iter = ls_bbuf_create();
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(AOU_CREATE_WIC_AND_APPEND, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				*iter = ls_bbuf_create_with_init_cap(sspan.len);
				ls_bbuf_append_sspan(iter, sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(AOU_CREATE_AND_INSERT, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				*iter = ls_bbuf_create();
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	BENCHMARK(AOU_CREATE_WIC_AND_INSERT, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				*iter = ls_bbuf_create_with_init_cap(sspan.len);
				ls_bbuf_insert_sspan(iter, 0, sspan);
			});
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		*iter = ls_bbuf_clone(bbuf);
	}
	BENCHMARK(AOU_LS_BBUF_DESTROY, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				ls_bbuf_destroy(iter);
			});

	BENCHMARK(AOU_LS_BBUF_INVALIDATE, len_tag_idx,
			FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
				ls_bbuf_invalidate(iter);
			});

	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		*iter = ls_bbuf_create();
	}
	BENCHMARK(AOU_LS_BBUF_MOVE, len_tag_idx,
			LSByteBuffer tmp = ls_bbuf_move(&aou[0].bbuf);
			for (size_t i = 0; i < NELEMS(aou) - 1; ++i) {
				LSByteBuffer *dest = &aou[i].bbuf;
				LSByteBuffer *src = &aou[i + 1].bbuf;
				*dest = ls_bbuf_move(src);
			}
			LSByteBuffer *last = &aou[NELEMS(aou) - 1].bbuf;
			*last = tmp;
			);
	FOREACH_AOU (StringUnion, LSByteBuffer, bbuf, iter, aou) {
		ls_bbuf_destroy(iter);
	}

	ls_string_destroy(&string);
	ls_sso_destroy(&sso);
	ls_bbuf_destroy(&bbuf);
}

size_t size_max(size_t a, size_t b)
{
	return a > b ? a : b;
}
