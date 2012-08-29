/*
 * TODO
 * - fix nested levels
 */

#include <stdio.h>
#define KSIZE 1024

#define STATE_NORMAL 0x0

#define STATE_CURLY_BRACKET 0x1
#define STATE_ROUND_BRACKET 0x2
#define STATE_SQUARE_BRACKET 0x3

#define STATE_DOUBLE_QUOTE 0x100
#define STATE_SINGLE_QUOTE 0x200

#define STATE_BACKSLASH 0x400

#define STATE_PRINTING 0x10000

typedef struct {
	const char * function;
	const char * f;
	int argument;
} Pattern;


typedef struct {
	const char * b;
	int argument;
	int state;
	int counter;
	const Pattern * pattern;
} ParserInfo;

int main(const int argc, const char * * argv) {

	char buffer[KSIZE + 1] = {0};
	char * c;

	Pattern patterns[KSIZE];
	int patternLength = 0;

	ParserInfo stack[KSIZE];
	ParserInfo * info;
	int stackLength = 0;

	int i;
	int j;

	int skip;

	for (i = 1, skip = 0; i + 1 < argc; i += 2, skip = 0) {

		for (j = 0; j < patternLength; j++) {
			if (strcmp(argv[i], patterns[j].function) == 0) {
				fprintf(stderr, "ignoring %s %s because it is duplicated.\n", argv[i], argv[i + 1]);
				skip = 1;
			}
		}

		if (skip) {
			continue;
		}

		patterns[patternLength].f = patterns[patternLength].function = argv[i];
		patterns[patternLength].argument = atoi(argv[i + 1]);

		patternLength++;
	}

	if (patternLength == 0) {
		fprintf(stderr, "USAGE: %s function argument [ function argument ... ]\n", argv[0]);
		return 1;
	}

	while (fgets(buffer, KSIZE, stdin)) {

		for (c = buffer; *c != 0; c++) {

			if (stackLength > 0) {
				info = &stack[stackLength - 1];
				if ((info->state & 0xFFFF) == STATE_NORMAL) {

					if (*c == ',') {
						if (info->argument == info->pattern->argument) {
							info->state &= (~STATE_PRINTING);
							fputc('\n', stdout);
						}

					} else if (*c == ')') {
						if (info->argument == info->pattern->argument) {
							info->state &= (~STATE_PRINTING);
							fputc('\n', stdout);
						}

						stackLength--;
						break;
					}
				}

				if (info->state & STATE_PRINTING) {
					fputc(*c, stdout);
				}

				if (info->state & (0xFF00)) {
					switch (info->state & (0xFF00)) {

					case STATE_BACKSLASH:
						info->state = info->state & (~STATE_BACKSLASH);
						break;

					case STATE_DOUBLE_QUOTE:
						if (*c == '\\') {
							info->state |= STATE_BACKSLASH;

						} else if (*c == '"') {
							info->state = info->state & (~STATE_DOUBLE_QUOTE);
						}

						break;

					case STATE_SINGLE_QUOTE:
						if (*c == '\\') {
							info->state |= STATE_BACKSLASH;

						} else if (*c == '\'') {
							info->state = info->state & (~STATE_SINGLE_QUOTE);
						}

						break;
					}

				} else if (*c == '"') {
					info->state |= STATE_DOUBLE_QUOTE;

				} else if (*c == '\'') {
					info->state |= STATE_SINGLE_QUOTE;

				} else {
					switch (info->state & 0xFFFF) {
					case STATE_ROUND_BRACKET:
						if (*c == '(') {
							info->counter++;

						} else if (*c == ')') {
							info->counter--;
						}

						break;

					case STATE_SQUARE_BRACKET:
						if (*c == '[') {
							info->counter++;

						} else if (*c == ']') {
							info->counter--;
						}

						break;

					case STATE_CURLY_BRACKET:
						if (*c == '{') {
							info->counter++;

						} else if (*c == '}') {
							info->counter--;
						}

						break;

					case STATE_NORMAL:
						switch (*c) {
						case '{':
							info->state |= STATE_CURLY_BRACKET;
							info->counter = 1;
							break;

						case '(':
							info->state |= STATE_ROUND_BRACKET;
							info->counter = 1;
							break;

						case '[':
							info->state |= STATE_SQUARE_BRACKET;
							info->counter = 1;
							break;

						case ',':
							info->argument++;

							if (info->argument == info->pattern->argument) {
								info->state |= STATE_PRINTING;
							}

							break;
						}
					}
				}

				if (info->counter < 1) {
					info->state = (info->state & (~0xFF)) | STATE_NORMAL;
				}
			}

			if (*c == '(') {
				if (stackLength == KSIZE) {
					fputs("stack reached limit, ignoring new entries.", stderr);

				} else {
					for (i = 0; i < patternLength; i++) {
						if (!(*(patterns[i].f))) {
							stack[stackLength].argument = 1;
							stack[stackLength].b = NULL;
							stack[stackLength].state = STATE_NORMAL;
							stack[stackLength].counter = 0;
							stack[stackLength].pattern = &patterns[i];

							if (patterns[i].argument == 1) {
								stack[stackLength].state |= STATE_PRINTING;
							}

							stackLength++;
						}
					}
				}
			}

			for (i = 0; i < patternLength; i++) {
				if (*c == *(patterns[i].f)) {
					patterns[i].f++;
				} else {
					patterns[i].f = patterns[i].function;
				}
			}

		}
	}

	return 0;
}
