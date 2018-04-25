#ifndef COLOURS_H_
#define COLOURS_H_

// These macros only work on string literals and will generate rubbish in redirected output

#ifdef COLOURS
#define RED(x) "\033[01;31m" x "\033[0m"
#define GREEN(x) "\033[01;32m" x "\033[0m"
#define YELLOW(x) "\033[01;33m" x "\033[0m"
#define BLUE(x) "\033[01;34m" x "\033[0m"
#define MAGENTA(x) "\033[01;35m" x "\033[0m"
#define CYAN(x) "\033[01;36m" x "\033[0m"
#define WHITE(x) "\033[01;37m" x "\033[0m"
#define DARKRED(x) "\033[31m" x "\033[0m"
#define DARKGREEN(x) "\033[32m" x "\033[0m"
#define DARKYELLOW(x) "\033[33m" x "\033[0m"
#define DARKBLUE(x) "\033[34m" x "\033[0m"
#define DARKMAGENTA(x) "\033[35m" x "\033[0m"
#define DARKCYAN(x) "\033[36m" x "\033[0m"
#else
#define RED(x) x
#define GREEN(x) x
#define YELLOW(x) x
#define BLUE(x) x
#define MAGENTA(x) x
#define CYAN(x) x
#define WHITE(x) x
#define DARKRED(x) x
#define DARKGREEN(x) x
#define DARKYELLOW(x) x
#define DARKBLUE(x) x
#define DARKMAGENTA(x) x
#define DARKCYAN(x) x
#endif

#endif /* COLOURS_H_ */
