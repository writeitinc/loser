#ifndef loser_h
#define loser_h

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

#endif // loser_h
