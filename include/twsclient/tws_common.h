#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __cplusplus
#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS  extern "C" {
#endif
#ifndef __END_DECLS
#define __END_DECLS    }
#endif
#else
#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS
#endif
#ifndef __END_DECLS
#define __END_DECLS
#endif
#endif


/* define export */
#ifdef _WIN32
# if defined(BUILDING_TWS_SHARED)
#   define EXPORT __declspec(dllexport)
# else
#   define EXPORT /* nothing */
# endif
#elif __GNUC__ >= 4
# define EXPORT __attribute__((visibility("default")))
#else
# define EXPORT /* nothing */
#endif

/* custom memory manage */
#include <stdlib.h>

#ifdef USE_JEMALLOC
#define sk_malloc   je_malloc
#define sk_calloc   je_calloc
#define sk_free     je_free
#else
#define sk_malloc   malloc
#define sk_calloc   calloc
#define sk_free     free
#define sk_strdup   strdup
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
#define PLATFORM_WIN
#else
#define PLATFORM_POSIX
#endif

#if defined(__LP64__)
#define PLATFORM_x64
#else
#define PLATFORM_x32
#endif

/* Provide definitions for some commonly used macros.
 *  Some of them are only provided if they haven't already
 *  been defined. It is assumed that if they are already
 *  defined then the current definition is correct.
 */
#ifndef NULL
#  ifdef __cplusplus
#  define NULL        (0L)
#  else /* !__cplusplus */
#  define NULL        ((void*) 0)
#  endif /* !__cplusplus */
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif

#if defined(__STRICT_ANSI__) || defined(_MSC_VER)
# define inline __inline
#endif

#define UNUSED(x) (void)x

#ifdef PLATFORM_WIN
#define ascii_strcasecmp(x,y) _stricmp(x,y)
#define ascii_strncasecmp(x,y,z) _strnicmp(x,y,z)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#else
#define ascii_strcasecmp(x,y) strcasecmp(x,y)
#define ascii_strncasecmp(x,y,z) strncasecmp(x,y,z)
#endif

#ifdef __cplusplus
}
#endif
#endif // __COMMON_H__
