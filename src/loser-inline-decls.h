#define LS_LINK(T) LS_LINKAGE T

LS_LINK(bool) ls_string_is_valid(LSString string);
LS_LINK(bool) ls_short_string_is_valid(LSShortString short_string);
LS_LINK(const LSByte *)ls_short_string_get_bytes(
		const LSShortString *short_string);
LS_LINK(bool) ls_sspan_is_valid(LSStringSpan sspan);
LS_LINK(bool) ls_bbuf_is_valid(LSByteBuffer bbuf);
LS_LINK(LSSSOStringType) ls_sso_get_type(LSSSOString sso);
LS_LINK(bool) ls_sso_is_valid(LSSSOString sso);
LS_LINK(const LSByte *)ls_sso_get_bytes(const LSSSOString *sso);

LS_LINK(LSStringSpan) ls_sspan_create(const LSByte *start, size_t len);

LS_LINK(void) ls_string_invalidate(LSString *string);
LS_LINK(void) ls_short_string_invalidate(LSShortString *short_string);
LS_LINK(void) ls_sso_invalidate(LSSSOString *sso);
LS_LINK(void) ls_sspan_invalidate(LSStringSpan *sspan);
LS_LINK(void) ls_bbuf_invalidate(LSByteBuffer *bbuf);

LS_LINK(LSString) ls_string_move(LSString *string);
LS_LINK(LSSSOString) ls_sso_move(LSSSOString *sso);
LS_LINK(LSByteBuffer) ls_bbuf_move(LSByteBuffer *bbuf);

LS_LINK(LSString) ls_bbuf_finalize(LSByteBuffer *bbuf);

LS_LINK(LSString) ls_string_from_sso(LSSSOString sso);

LS_LINK(LSShortString) ls_short_string_from_sso(LSSSOString sso);

LS_LINK(LSSSOString) ls_sso_from_short_string(LSShortString short_string);

LS_LINK(LSStringSpan) ls_sspan_from_string(LSString string);
LS_LINK(LSStringSpan) ls_sspan_from_short_string(
		const LSShortString *short_string);
LS_LINK(LSStringSpan) ls_sspan_from_sso(const LSSSOString *sso);
LS_LINK(LSStringSpan) ls_sspan_from_bbuf(LSByteBuffer bbuf);
LS_LINK(LSStringSpan) ls_sspan_from_chars(const char *chars, size_t len);
LS_LINK(LSStringSpan) ls_sspan_from_cstr(const char *cstr);

LS_LINK(LSString) ls_string_substr(LSString string, size_t start, size_t len);
LS_LINK(LSStringSpan) ls_string_subspan(LSString string, size_t start,
		size_t len);
LS_LINK(LSString) ls_sspan_substr(LSStringSpan sspan, size_t start, size_t len);
LS_LINK(LSStringSpan) ls_sspan_subspan(LSStringSpan sspan, size_t start,
		size_t len);

LS_LINK(LSStatus) ls_bbuf_append_short_string(LSByteBuffer *bbuf,
		LSShortString short_string);
LS_LINK(LSStatus) ls_bbuf_append_sso(LSByteBuffer *bbuf, LSSSOString sso);
LS_LINK(LSStatus) ls_bbuf_insert_short_string(LSByteBuffer *bbuf, size_t idx,
		LSShortString short_string);
LS_LINK(LSStatus) ls_bbuf_insert_sso(LSByteBuffer *bbuf, size_t idx,
		LSSSOString sso);

#undef LS_LINK
