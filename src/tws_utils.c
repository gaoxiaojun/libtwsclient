#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <twsclient/tws_common.h>

#ifndef NDEBUG
void hexdump(const char *prefix, void *ptr, int buflen)
{
    unsigned char *buf = (unsigned char*)ptr;
    int i, j;
    for (i = 0; i < buflen; i += 16) {
        printf("%s  %06x: ", prefix, i);
        for (j = 0; j < 16; j++)
            if (i+j < buflen)
                printf("%02x ", buf[i+j]);
            else
                printf("   ");
        printf(" ");
        for (j = 0; j < 16; j++)
            if (i+j < buflen)
                printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
        printf("\n");
    }
}
#endif

#ifdef USE_JEMALLOC
char *sk_strdup(const char *src)
{
    char *dst = sk_malloc(strlen(src) + 1);
    strcpy(dst, src);
    return dst;
}
#endif
