/*
 * TODO:
 *  - make sure we support utf-8 properly.
 *  - consider comment blocks.
 *  - fix the pattern look-up problem listed bellow.
 */

#include <stdio.h>
#include <stdlib.h>

#define KSIZE 1024
#define KSIZE64 65536

#define STATE_NORMAL 0x0
#define STATE_CURLY_BRACKET 0x1
#define STATE_ROUND_BRACKET 0x2
#define STATE_SQUARE_BRACKET 0x3

#define STATE_DOUBLE_QUOTE 0x1
#define STATE_SINGLE_QUOTE 0x2
#define STATE_SLASH 0x3
#define STATE_BACKSLASH 0x4
#define STATE_PRINTING 0x8

#define DEBUG 0

typedef struct {
	const char * function;
	const char * f;
	int argument;
} Pattern;


typedef struct {
	int b;
	int argument;
	int state;
	int counter;
	const Pattern * pattern;
} ParserInfo;

int main(const int argc, const char * * argv) {

	char * a = NULL;
	char * b = NULL;

	char buffer[KSIZE + 1] = {0};
	char * c;

	Pattern patterns[KSIZE];
	int patternLength = 0;

	ParserInfo stack[KSIZE];
	ParserInfo * info = NULL;
	int stackLength = 0;

	int i;
	int j;

	int skip;
	int state = 0;

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

			if (b) {
				*b++ = *c;
				i = b - a;

				if (i && i % KSIZE64 == 0) {

					if (DEBUG) {
						fputs("resizing\n", stderr);
					}

					a = realloc(a, (i + KSIZE64) * sizeof(char));
					b = a + i;
				}
			}

			if (state & STATE_BACKSLASH) {
				state &= ~STATE_BACKSLASH;
				continue;
			}

			switch (*c) {
			case '\\':
				if (state & (STATE_SLASH | STATE_SINGLE_QUOTE | STATE_DOUBLE_QUOTE)) {
					state |= STATE_BACKSLASH;
				}

				break;

			case '/':
				if (state & STATE_SLASH) {
					state &= ~STATE_SLASH;

				} else {
					state |= STATE_SLASH;
				}

				break;

			case '\'':
				if (state & STATE_SINGLE_QUOTE) {
					state &= ~STATE_SINGLE_QUOTE;

				} else {
					state |= STATE_SINGLE_QUOTE;
				}

				break;

			case '"':
				if (state & STATE_DOUBLE_QUOTE) {
					state &= ~STATE_DOUBLE_QUOTE;

				} else {
					state |= STATE_DOUBLE_QUOTE;
				}

				break;

			case ',':
				if (info && info->state == STATE_NORMAL) {
					info->argument++;

					if (info->argument == info->pattern->argument) {
						if (!b) {
							if (DEBUG) {
								fputs("creating buffer\n", stderr);
							}

							b = a = malloc(sizeof(char) * KSIZE64);
						}

						info->b = b - a;

					} else if (info->argument == info->pattern->argument + 1) {
						*(b - 1) = 0;
						fputs(a + info->b, stdout);
						fputc('\n', stdout);

						if (info->b) {
							*(b - 1) = ',';

						} else {
							free(a);
							b = a = NULL;
						}
					}
				}
				break;

			case '(':
				if (info) {
					switch (info->state) {
					case STATE_ROUND_BRACKET:
						info->counter++;

					case STATE_NORMAL:
						info->counter = 1;
						info->state = STATE_ROUND_BRACKET;
						break;
					}
				}

				if (stackLength == KSIZE) {
					fputs("stack reached limit, ignoring new entries.", stderr);

				} else {
					for (i = 0; i < patternLength; i++) {
						if (!(*(patterns[i].f))) {
							stack[stackLength].argument = 1;
							stack[stackLength].b = 0;
							stack[stackLength].state = STATE_NORMAL;
							stack[stackLength].counter = 0;
							stack[stackLength].pattern = &patterns[i];

							if (patterns[i].argument == 1) {
								if (!b) {
									if (DEBUG) {
										fputs("creating buffer\n", stderr);
									}

									b = a = malloc(sizeof(char) * KSIZE64);
								}

								stack[stackLength].b = b - a;
							}

							info = &stack[stackLength++];
						}
					}
				}
				break;

			case ')':
				if (info) {

					switch (info->state) {
					case STATE_NORMAL:
						if (info->argument == info->pattern->argument) {
							*(b - 1) = 0;
							fputs(a + info->b, stdout);
							fputc('\n', stdout);

							if (info->b) {
								*(b - 1) = ')';

							} else {
								free(a);
								b = a = NULL;
							}
						}

						info = (--stackLength) ? &stack[stackLength - 1] : NULL;
						break;

					case STATE_ROUND_BRACKET:
						if ( ! --info->counter) {
							info->state = STATE_NORMAL;
						}
						break;
					}
				}

				break;

			case '[':
				if (info) {
					switch (info->state) {
					case STATE_SQUARE_BRACKET:
						info->counter++;

					case STATE_NORMAL:
						info->counter = 1;
						info->state = STATE_SQUARE_BRACKET;
						break;
					}
				}
				break;

			case ']':
				if (info && info->state == STATE_SQUARE_BRACKET && ! --info->counter) {
					info->state = STATE_NORMAL;
				}
				break;

			case '{':
				if (info) {
					switch (info->state) {
					case STATE_CURLY_BRACKET:
						info->counter++;

					case STATE_NORMAL:
						info->counter = 1;
						info->state = STATE_CURLY_BRACKET;
						break;
					}
				}
				break;

			case '}':
				if (info && info->state == STATE_CURLY_BRACKET && ! --info->counter) {
					info->state = STATE_NORMAL;
				}
				break;

			default:
				//TODO: that's still broken.
				for (i = 0; i < patternLength; i++) {
					if (*c == *(patterns[i].f)) {
						patterns[i].f++;
					} else {
						patterns[i].f = patterns[i].function;
					}
				}
				break;
			}
		}
	}

	return 0;
}
