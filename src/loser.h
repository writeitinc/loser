#ifndef loser_h
#define loser_h

#include <stdbool.h>
#include <stddef.h>

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

static inline bool LS_STRING_VALID(LSString string);
static inline bool LS_SSPAN_VALID(LSStringSpan sspan);
static inline bool LS_BBUF_VALID(LSByteBuffer bbuf);

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

#endif // loser_h
