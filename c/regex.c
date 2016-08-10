/**
 * @file regex.c Implements basic regular expressions
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <ctype.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define _Bool signed char
#define bool _Bool
#define false 0
#define true 1

/** Get number of elements in an array */
#undef ARRAY_SIZE
#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

typedef unsigned char             uint8_t;
typedef unsigned short int        uint16_t;
typedef unsigned int              uint32_t;
typedef unsigned long long int    uint64_t;
typedef int  pj_ssize_t;

struct pj_str_t
{
    /** Buffer pointer, which is by convention NOT null terminated. */
    char  *ptr;

    /** The length of the string. */
    pj_ssize_t  slen;
};


/** Defines a character range */
struct chr {
	uint8_t min;  /**< Minimum value */
	uint8_t max;  /**< Maximum value */
};


static bool expr_match(struct chr *chrv, uint32_t n, uint8_t c,
		       bool neg)
{
	uint32_t i;

	for (i=0; i<n; i++) {

		if (c < chrv[i].min)
			continue;

		if (c > chrv[i].max)
			continue;

		break;
	}

	return neg ? (i == n) : (i != n);
}


/**
 * Parse a string using basic regular expressions. Any number of matching
 * expressions can be given, and each match will be stored in a "struct pj_str_t"
 * pointer-length type.
 *
 * @param ptr  String to parse
 * @param len  Length of string
 * @param expr Regular expressions string
 *
 * @return 0 if success, otherwise errorcode
 *
 * Example:
 *
 *   We parse the buffer for any numerical values, to get a match we must have
 *   1 or more occurences of the digits 0-9. The result is stored in 'num',
 *   which is of pointer-length type and will point to the first location in
 *   the buffer that contains "42".
 *
 * <pre>
 const char buf[] = "foo 42 bar";
 struct pl num;
 int err = re_regex(buf, strlen(buf), "[0-9]+", &num);

 here num contains a pointer to '42'
 * </pre>
 */
int re_regex(char *ptr, pj_ssize_t len, char *expr, ...)
{
	struct chr chrv[64];
	char *p, *ep;
	bool fm, range = false, ec = false, neg = false, qesc = false;
	uint32_t n = 0;
	va_list ap;
	bool eesc;
	size_t l;

	if (!ptr || !expr)
		return EINVAL;

 again:
	eesc = false;
	fm = false;
	l  = len--;
	p  = ptr++;
	ep = expr;

	va_start(ap, expr);

	if (!l)
		goto out;

	for (; *ep; ep++) {

		if ('\\' == *ep && !eesc) {
			eesc = true;
			continue;
		}

		if (!fm) {

			/* Start of character class */
			if ('[' == *ep && !eesc) {
				n     = 0;
				fm    = true;
				ec    = false;
				neg   = false;
				range = false;
				qesc  = false;
				continue;
			}

			if (!l)
				break;

			if (tolower(*ep) != tolower(*p)) {
				va_end(ap);
				goto again;
			}

			eesc = false;
			++p;
			--l;
			continue;
		}
		/* End of character class */
		else if (ec) {

			uint32_t nm, nmin, nmax;
			struct pj_str_t lpl, *pl = va_arg(ap, struct pj_str_t *);
			bool quote = false, esc = false;

			/* Match 0 or more times */
			if ('*' == *ep) {
				nmin = 0;
				nmax = -1;
			}
			/* Match 1 or more times */
			else if ('+' == *ep) {
				nmin = 1;
				nmax = -1;
			}
			/* Match exactly n times */
			else if ('1' <= *ep && *ep <= '9') {
				nmin = *ep - '0';
				nmax = *ep - '0';
			}
			else
				break;

			fm = false;

			lpl.ptr = p;
			lpl.slen = 0;

			for (nm = 0; l && nm < nmax; nm++, p++, l--, lpl.slen++) {

				if (qesc) {

					if (esc) {
						esc = false;
						continue;
					}

					switch (*p) {

					case '\\':
						esc = true;
						continue;

					case '"':
						quote = !quote;
						continue;
					}

					if (quote)
						continue;
				}

				if (!expr_match(chrv, n, tolower(*p), neg))
					break;
			}

			/* Strip quotes */
			if (qesc && lpl.slen > 1 &&
			    lpl.ptr[0] == '"' && lpl.ptr[lpl.slen - 1] == '"') {

				lpl.ptr += 1;
				lpl.slen -= 2;
				nm    -= 2;
			}

			if ((nm < nmin) || (nm > nmax)) {
				va_end(ap);
				goto again;
			}

			if (pl)
				*pl = lpl;

			eesc = false;
			continue;
		}

		if (eesc) {
			eesc = false;
			goto chr;
		}

		switch (*ep) {

			/* End of character class */
		case ']':
			ec = true;
			continue;

			/* Negate with quote escape */
		case '~':
			if (n)
				break;

			qesc = true;
			neg  = true;
			continue;

			/* Negate */
		case '^':
			if (n)
				break;

			neg = true;
			continue;

			/* Range */
		case '-':
			if (!n || range)
				break;

			range = true;
			--n;
			continue;
		}

	chr:
		chrv[n].max = tolower(*ep);

		if (range)
			range = false;
		else
			chrv[n].min = tolower(*ep);

		if (++n > ARRAY_SIZE(chrv))
			break;
	}
 out:
	va_end(ap);

	if (fm)
		return EINVAL;

	return *ep ? ENOENT : 0;
}


int main()
{
	char * num = "1003 <sip:1003@192.168.100.76>";
	struct pj_str_t dname;

	re_regex(num, strlen(num), "[~ \t\r\n<]+", &dname);
	printf("len:%d\n", dname.slen);
	printf("dname:%.*s\n", dname.slen, dname.ptr);
	return 0;
}