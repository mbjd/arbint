/*
 * best testing suite ever
 * http://www.jera.com/techinfo/jtns/jtn002.html
 *
 * Slightly improved by me:
 *  - Prints a dot for each successful assertion
 *  - Counts assertions too instead of tests only
 */

#define mu_assert(message, test)                        \
	do                                              \
	{                                               \
		if (!(test))                            \
		{                                       \
			return "Test failed: " message; \
		}                                       \
		else                                    \
		{                                       \
			assertions_run++;               \
			fputc('.', stdout);             \
		}                                       \
	} while (0)

#define mu_run_test(test)                    \
	do                                   \
	{                                    \
		char* message = test();      \
		printf(" ");                 \
		tests_run++;                 \
		if (message)                 \
		{                            \
			fputc('\n', stdout); \
			return message;      \
		}                            \
	} while (0)

extern int tests_run;
extern int assertions_run;
