#ifndef MACRO_FUNCS_H
#define MACRO_FUNCS_H

typedef int make_iso_compilers_happy;

#define REM_NEWLINE(msg, len) { if ('\n' == msg[len - 1]) msg[len - 1] = '\0'; }
#define EHDLR(msg) { perror(msg); exit(EXIT_FAILURE); }

#endif
