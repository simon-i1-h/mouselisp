#include <string.h>

#include "mouselisp.h"

ml_string
ml_string_new_str(const char *str)
{
	ml_string ret = (ml_string){0};
	ml_string_concat_string(&ret, str);
	return ret;
}

void
ml_string_concat_char(ml_string *s, int c)
{
	if (s->len + 1 >= s->capacity) {
		/*
		 * capacityは2倍ずつ増やしたいが、最初の0は何を乗じて
		 * も0なので、0の場合のみ最初に8を設定してから2を乗じ
		 * る。
		 */
		s->capacity += (s->capacity == 0) * 8;
		s->capacity *= 2;
		s->str = xgcrealloc(s->str, s->capacity);
	}

	s->str[s->len++] = c;
	s->str[s->len] = '\0';
}

/* TODO: 高効率な実装 */
void
ml_string_concat_string(ml_string *s, const char *str)
{
	size_t len = strlen(str);

	for (size_t i = 0; i < len; i++)
		ml_string_concat_char(s, str[i]);
}
