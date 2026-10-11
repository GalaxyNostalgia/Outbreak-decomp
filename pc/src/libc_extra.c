/* C library functions the PS2's libc has and the host's doesn't. */
#include <ctype.h>

/* Sony libc: upper-cases ASCII letters in place, returns its argument. */
char *strupr(char *s) {
    for (char *p = s; *p; p++)
        if (*p >= 'a' && *p <= 'z') *p = (char)(*p - 0x20);
    return s;
}
