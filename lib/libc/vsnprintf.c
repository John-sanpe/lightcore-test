/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <string.h>
#include <kernel.h>
#include <export.h>

enum flags {
    FL_ZERO         = 0x01,     /* Zero modifier        */
    FL_MINUS        = 0x02,     /* Minus modifier       */
    FL_PLUS         = 0x04,     /* Plus modifier        */
    FL_TICK         = 0x08,     /* ' modifier           */
    FL_SPACE        = 0x10,     /* Space modifier       */
    FL_HASH         = 0x20,     /* # modifier           */
    FL_SIGNED       = 0x40,     /* Number is signed     */
    FL_UPPER        = 0x80,     /* Upper case digits    */
};

/*
 * These may have to be adjusted on certain implementations
 */
enum ranks {
    rank_char       = -2,
    rank_short      = -1,
    rank_int        =  0,
    rank_long       =  1,
    rank_longlong   =  2,
};

#define MIN_RANK        rank_char
#define MAX_RANK        rank_longlong
#define INTMAX_RANK     rank_longlong
#define SIZE_T_RANK     rank_long
#define PTRDIFF_T_RANK  rank_long

#define EMIT(x) ({ if(o<n) {*q++ = (x);} o++; })

static size_t format_int(char *q, size_t n, uintmax_t val, enum flags flags,
                         int base, int width, int prec)
{
    static const char lcdigits[] = "0123456789abcdef";
    static const char ucdigits[] = "0123456789ABCDEF";
    const char *digits;
    int ndigits = 0, nchars;
    int tickskip, b4tick;
    int minus = 0;
    uintmax_t tmpval;
    size_t o = 0, oo;
    char *qq;

    /*
     * Select type of digits
     */
    digits = (flags & FL_UPPER) ? ucdigits : lcdigits;

    /*
     * If signed, separate out the minus
     */
    if ((flags & FL_SIGNED) && ((intmax_t)val < 0)) {
        minus = 1;
        val = (uintmax_t)(-(intmax_t)val);
    }

    /*
     * Count the number of digits needed.  This returns zero for 0
     */
    tmpval = val;
    while (tmpval) {
        tmpval /= base;
        ndigits++;
    }

    /*
     * Adjust ndigits for size of output
     */
    if ((flags & FL_HASH) && (base == 8)) {
        if (prec < ndigits + 1)
            prec = ndigits + 1;
    }

    if (ndigits < prec) /* Mandatory number padding */
        ndigits = prec;
    else if (val == 0) /* Zero still requires space */
        ndigits = 1;

    /*
     * For ', figure out what the skip should be
     */
    if (flags & FL_TICK)
        tickskip = (base == 16) ? 4 : 3;
    else /* No tick marks */
        tickskip = ndigits;

    /*
     * Tick marks aren't digits, but generated by the number converter
     */
    ndigits += (ndigits - 1) / tickskip;

    /*
     * Now compute the number of nondigits
     */
    nchars = ndigits;

    if (minus || (flags & (FL_PLUS | FL_SPACE)))
        nchars++; /* Need space for sign */

    if ((flags & FL_HASH) && (base == 16)) {
        nchars += 2; /* Add 0x for hex */
        width += 2;
    }

    /*
     * Emit early space padding
     */
    if (!(flags & (FL_MINUS | FL_ZERO)) && (width > nchars)) {
        while (width > nchars) {
            EMIT(' ');
            width--;
        }
    }

    /*
     * Emit nondigits
     */
    if (minus)
        EMIT('-');
    else if (flags & FL_PLUS)
        EMIT('+');
    else if (flags & FL_SPACE)
        EMIT(' ');

    if ((flags & FL_HASH) && (base == 16)) {
        EMIT('0');
        EMIT((flags & FL_UPPER) ? 'X' : 'x');
    }

    /*
     * Emit zero padding
     */
    if (((flags & (FL_MINUS | FL_ZERO)) == FL_ZERO) && (width > ndigits)) {
        while (width > nchars) {
            EMIT('0');
            width--;
        }
    }

    /*
     * Generate the number.  This is done from right to left
     */
    q += ndigits;   /* Advance the pointer to end of number */
    o += ndigits;
    qq = q;
    oo = o;         /* Temporary values */

    b4tick = tickskip;
    while (ndigits > 0) {
        if (!b4tick--) {
            qq--;
            oo--;
            ndigits--;
            if (oo < n)
                *qq = '_';
            b4tick = tickskip - 1;
        }
        qq--;
        oo--;
        ndigits--;
        if (oo < n)
            *qq = digits[val % base];
        val /= base;
    }

    /*
     * Emit late space padding
     */
    while ((flags & FL_MINUS) && (width > nchars)) {
        EMIT(' ');
        width--;
    }

    return o;
}

int vsnprintf(char *buf, size_t n, const char *fmt, va_list args)
{
    const char * p = fmt;
    char ch;
    char *q = buf;
    size_t o = 0;   /* Number of characters output */
    uintmax_t val = 0;
    int rank = rank_int; /* Default rank */
    int width = 0;
    int prec = -1;
    int base;
    size_t sz;
    enum flags flags = 0;
    enum {
        st_normal,      /* Ground state */
        st_flags,       /* Special flags */
        st_width,       /* Field width */
        st_prec,        /* Field precision */
        st_modifiers,   /* Length or conversion modifiers */
    } state = st_normal;
    const char * sarg; /* %s string argument */
    char carg;   /* %c char argument */
    int slen;   /* String length */

    while ((ch = *p++)) {
        switch (state) {
            case st_normal:
                if (ch == '%') {
                    state = st_flags;
                    flags = 0;
                    rank = rank_int;
                    width = 0;
                    prec = -1;
                }
                else
                    EMIT(ch);
                break;

            case st_flags:
                switch (ch) {
                    case '-':
                        flags |= FL_MINUS;
                        break;
                    case '+':
                        flags |= FL_PLUS;
                        break;
                    case '\'':
                        flags |= FL_TICK;
                        break;
                    case ' ':
                        flags |= FL_SPACE;
                        break;
                    case '#':
                        flags |= FL_HASH;
                        break;
                    case '0':
                        flags |= FL_ZERO;
                        break;
                    default:
                        state = st_width;
                        p--; /* Process this character again */
                        break;
                }
                break;

            case st_width:
                if (ch >= '0' && ch <= '9')
                    width = width * 10 + (ch - '0');
                else if (ch == '*') {
                    width = va_arg(args, int);
                    if (width < 0) {
                        width = -width;
                        flags |= FL_MINUS;
                    }
                } else if (ch == '.') {
                    prec = 0; /* Precision given */
                    state = st_prec;
                } else {
                    state = st_modifiers;
                    p--;  /* Process this character again */
                }
                break;

            case st_prec:
                if (ch >= '0' && ch <= '9')
                    prec = prec * 10 + (ch - '0');
                else if (ch == '*') {
                    prec = va_arg(args, int);
                    if (prec < 0)
                        prec = -1;
                } else {
                    state = st_modifiers;
                    p--;  /* Process this character again */
                }
                break;

            case st_modifiers: {
                switch (ch) {
                    /*
                     * Length modifiers - nonterminal sequences
                     */
                    case 'h': /* Shorter rank */
                        rank--;
                        break;

                    case 'l': /* Longer rank */
                        rank++;
                        break;

                    case 'j':
                        rank = INTMAX_RANK;
                        break;

                    case 'z':
                        rank = SIZE_T_RANK;
                        break;

                    case 't':
                        rank = PTRDIFF_T_RANK;
                        break;

                    case 'L':
                    case 'q':
                        rank += 2;
                        break;

                    default: {
                        /*
                         * Next state will be normal
                         */
                        state = st_normal;

                        /*
                         * Canonicalize rank
                         */
                        if (rank < MIN_RANK)
                            rank = MIN_RANK;

                        else if (rank > MAX_RANK)
                            rank = MAX_RANK;

                        switch (ch) {
                            case 'P': /* Upper case pointer */
                                flags |= FL_UPPER;
                                break;

                            case 'p': /* Pointer */
                                base = 16;
                                prec = (8 * sizeof(void *) + 3) / 4;
                                flags |= FL_HASH;
                                val = (uintmax_t)(uintptr_t)
                                va_arg(args, void *);
                                goto is_integer;

                            case 'd': /* Signed decimal output */
                            case 'i':
                                base = 10;
                                flags |= FL_SIGNED;
                                switch (rank) {
                                    case rank_char:
                                        /* Yes, all these casts are needed */
                                        val = (uintmax_t)(intmax_t)
                                        (signed char)
                                        va_arg(args, signed int);
                                        break;
                                    case rank_short:
                                        val = (uintmax_t)(intmax_t)
                                        (signed short)
                                        va_arg(args, signed int);
                                        break;
                                    case rank_int:
                                        val = (uintmax_t)(intmax_t)
                                        va_arg(args, signed int);
                                        break;
                                    case rank_long:
                                        val = (uintmax_t)(intmax_t)
                                        va_arg(args, signed long);
                                        break;
                                    case rank_longlong:
                                        val = (uintmax_t)(intmax_t)
                                        va_arg(args,
                                                signed long long);
                                        break;
                                }
                                goto is_integer;

                            case 'o': /* Octal */
                                base = 8;
                                goto is_unsigned;

                            case 'u': /* Unsigned decimal */
                                base = 10;
                                goto is_unsigned;

                            case 'X': /* Upper case hexadecimal */
                                flags |= FL_UPPER;
                                base = 16;
                                goto is_unsigned;

                            case 'x': /* Hexadecimal */
                                base = 16;
                                goto is_unsigned;

                                is_unsigned: switch (rank) {
                                    case rank_char:
                                        val = (uintmax_t)
                                        (unsigned char)
                                        va_arg(args, unsigned int);
                                        break;
                                    case rank_short:
                                        val = (uintmax_t)
                                        (unsigned short)
                                        va_arg(args, unsigned int);
                                        break;
                                    case rank_int:
                                        val = (uintmax_t)
                                        va_arg(args, unsigned int);
                                        break;
                                    case rank_long:
                                        val = (uintmax_t)
                                        va_arg(args, unsigned long);
                                        break;
                                    case rank_longlong:
                                        val = (uintmax_t)
                                        va_arg(args, unsigned long long);
                                        break;
                                }

                                is_integer: sz = format_int(q, (o < n) ? n - o : 0, val,
                                        flags, base, width, prec);
                                q += sz;
                                o += sz;
                                break;

                            case 'c': /* Character */
                                carg = (char)va_arg(args, int);
                                sarg = &carg;
                                slen = 1;
                                goto is_string;

                            case 's': /* String */
                                sarg = va_arg(args, const char *);
                                sarg = sarg ? sarg : "(null)";
                                slen = strlen(sarg);

                                is_string: {
                                    char sch;
                                    int i;

                                    if (prec != -1 && slen > prec)
                                        slen = prec;

                                    if (width > slen && !(flags & FL_MINUS)) {
                                        char pad = (flags & FL_ZERO) ? '0' : ' ';
                                        while (width > slen) {
                                            EMIT(pad);
                                            width--;
                                        }
                                    }
                                    for (i = slen; i; i--) {
                                        sch = *sarg++;
                                        EMIT(sch);
                                    }
                                    if (width > slen && (flags & FL_MINUS)) {
                                        while (width > slen) {
                                            EMIT(' ');
                                            width--;
                                        }
                                    }
                                }
                                break;

                            case 'n': {
                                /*
                                 * Output the number of characters written
                                 */
                                switch (rank) {
                                    case rank_char:
                                        *va_arg(args, signed char *) = o;
                                        break;
                                    case rank_short:
                                        *va_arg(args, signed short *) = o;
                                        break;
                                    case rank_int:
                                        *va_arg(args, signed int *) = o;
                                        break;
                                    case rank_long:
                                        *va_arg(args, signed long *) = o;
                                        break;
                                    case rank_longlong:
                                        *va_arg(args, signed long long *) = o;
                                        break;
                                }
                            }
                                break;

                            default:  /* Anything else, including % */
                                EMIT(ch);
                                break;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    /*
     * Null-terminate the string
     */
    if (o < n) /* No overflow */
        *q = '\0';

    else if (n > 0) /* Overflow - terminate at end of buffer */
        buf[n - 1] = '\0';

    return o;
}
EXPORT_SYMBOL(vsnprintf);
