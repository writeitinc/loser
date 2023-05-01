#ifndef loser_h
#define loser_h

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef unsigned char LSByte;

typedef struct LSString {
	size_t len;
	const LSByte *bytes;
} LSString;

typedef struct LSStringSpan {
	size_t len;
	const LSByte *start;
} LSStringSpan;

typedef struct LSByteBuffer {
	size_t len;
	size_t cap;
	LSByte *bytes;
} LSByteBuffer;

// Validity Checks

static inline bool LS_STRING_VALID(LSString string);
static inline bool LS_SSPAN_VALID(LSStringSpan sspan);
static inline bool LS_BBUF_VALID(LSByteBuffer bbuf);

// Base Constructors/Destructors

LSString ls_string_create(const LSByte *bytes, size_t len);
void ls_string_destroy(LSString *string);

static inline LSStringSpan ls_sspan_create(const LSByte *bytes, size_t len);

// Convenience Constructors

static inline LSString ls_string_clone(LSString string);
static inline LSString ls_string_from_sspan(LSStringSpan sspan);
static inline LSString ls_string_from_chars(const char *chars, size_t len);
static inline LSString ls_string_from_cstr(const char *cstr);

static inline LSStringSpan ls_sspan_from_string(LSString string);
static inline LSStringSpan ls_sspan_from_chars(const char *chars, size_t len);
static inline LSStringSpan ls_sspan_from_cstr(const char *cstr);

// Substring/Subspan Constructors

static inline LSString ls_string_substr(LSString string, size_t start,
		size_t len);
static inline LSStringSpan ls_string_subspan(LSString string, size_t start,
		size_t len);

static inline LSString ls_sspan_substr(LSStringSpan sspan, size_t start,
		size_t len);
static inline LSStringSpan ls_sspan_subspan(LSStringSpan sspan, size_t start,
		size_t len);

// Inline Definitions

static inline bool LS_STRING_VALID(LSString string)
{
	return string.bytes != NULL;
}

static inline bool LS_SSPAN_VALID(LSStringSpan sspan)
{
	return sspan.start != NULL;
}

static inline bool LS_BBUF_VALID(LSByteBuffer bbuf)
{
	return bbuf.bytes != NULL;
}

static inline LSStringSpan ls_sspan_create(const LSByte *bytes, size_t len)
{
	return (LSStringSpan){
		.start = bytes,
		.len = len
	};
}

static inline LSString ls_string_clone(LSString string)
{
	return ls_string_create(string.bytes, string.len);
}

static inline LSString ls_string_from_sspan(LSStringSpan sspan)
{
	return ls_string_create(sspan.start, sspan.len);
}

static inline LSString ls_string_from_chars(const char *chars, size_t len)
{
	return ls_string_create((const LSByte *)chars, len);
}

static inline LSString ls_string_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return (LSString){ 0 };
	}

	return ls_string_from_chars(cstr, strlen(cstr));
}

static inline LSStringSpan ls_sspan_from_string(LSString string)
{
	return (LSStringSpan){
		.start = string.bytes,
		.len = string.len
	};
}

static inline LSStringSpan ls_sspan_from_chars(const char *chars, size_t len)
{
	return (LSStringSpan){
		.start = (const LSByte *)chars,
		.len = len
	};
}

static inline LSStringSpan ls_sspan_from_cstr(const char *cstr)
{
	if (cstr == NULL) {
		return (LSStringSpan){ 0 };
	}

	return ls_sspan_from_chars(cstr, strlen(cstr));
}

static inline LSString ls_string_substr(LSString string, size_t start,
		size_t len)
{
	return ls_string_from_sspan(ls_string_subspan(string, start, len));
}

static inline LSStringSpan ls_string_subspan(LSString string, size_t start,
		size_t len)
{
	return ls_sspan_subspan(ls_sspan_from_string(string), start, len);
}

static inline LSString ls_sspan_substr(LSStringSpan sspan, size_t start,
		size_t len)
{
	return ls_string_from_sspan(ls_sspan_subspan(sspan, start, len));
}

static inline LSStringSpan ls_sspan_subspan(LSStringSpan sspan, size_t start,
		size_t len)
{
	if (!LS_SSPAN_VALID(sspan)
			|| start > sspan.len
			|| len > sspan.len
			|| start > sspan.len - len) {
		return (LSStringSpan){ 0 };
	}

	return ls_sspan_create(&sspan.start[start], len);
}

#endif // loser_h
