
#ifndef __FSEX_COMMON_H
#define __FSEX_COMMON_H

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef signed int int32;
typedef signed short int16;
typedef signed char int8;

int32 val32_lsn(uint8 *);
int32 val32_be(uint8 *);
int32 val32_le(uint8 *);
int32 val24_le(uint8 *);
int16 val16_le(uint8 *);
int write32_le(int, uint32);
int write16_le(int, uint16);

int suffix(char *, char *);
int has_suffix(char *);
uint8 *mapfile(char *);
char *base_name(char *, char *, char *);


/* You'll need an ANSI terminal to use these :\ */
#ifdef _TRACE
#  include <stdio.h>
#  ifdef __GNUC__
#    define _D_INFO "\x1b[33m"
#    define _D_CRIT "\x1b[31m"
#    define _D_WARN "\x1b[36m"
#    define _D(args...) do { \
        printf("\x1b[33m%s \x1b[37m[%s:%d] " _D_INFO, __PRETTY_FUNCTION__, \
		__FILE__, __LINE__); printf (args); printf ("\x1b[0m\n"); \
        } while (0)
#  else
#    define _D_INFO "I: "
#    define _D_CRIT "C: "
#    define _D_WARN "W: "
#    define _D fflush(stdout); printf("\n%s:%d: ", __FILE__, __LINE__); printf
#  endif /* __GNUC__ */
#else /* !_TRACE */
#  define _D_INFO
#  define _D_CRIT
#  define _D_WARN
#  ifdef _D
#    undef _D
#  endif
#  if defined(__GNUC__) /* && !defined(MACOSX) */
#    define _D(args...)
#  else
     void _D(char *, ...);
#  endif
#endif /* _TRACE */

#endif
