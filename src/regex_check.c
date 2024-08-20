#include "regex_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#include "macro_funcs.h"

// check if string match to specific template
bool
correctness_check(const char *str, const char *regstr)
{
	int        stat;           // regexec return status
	regex_t    regex;          // regex struct
	char       errmsg[255];    // regexec error msg
	
	if (regcomp(&regex, regstr, REG_EXTENDED))
		EHDLR("regcomp");

	stat = regexec(&regex, str, 0, NULL, 0);
	if (0 == stat) {
		regfree(&regex);
		return true;
	} else if (REG_NOMATCH == stat) {
		regfree(&regex);
		return false;
	} else {
		regerror(stat, &regex, errmsg, sizeof(errmsg));
		regfree(&regex);

		fprintf(stderr, "Regex match failed: %s\n", errmsg);
		exit(EXIT_FAILURE);
	}
}
