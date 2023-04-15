#pragma once

#include <string>
#include <cstdio>
/*
Nonzero to allow multi-line value parsing, in the style of Python's
ConfigParser. If allowed, ini_parse() will call the handler with the same
name for each subsequent line parsed.
*/
#ifndef INI_ALLOW_MULTILANE
#define INI_ALLOW_MULTILANE 1
#endif

/* 
Nonzero to allow a UTF-8 BOM sequence (0xEF 0xBB 0xBF) at the start of
the file. See http://code.google.com/p/inih/issues/detail?id=21 
*/
#ifndef INI_ALLOW_BOM
#define INI_ALLOW_BOM 1
#endif

/* Nonzero to use stack, zero to use heap (malloc/free). */
#ifndef INI_USE_STACK
#define INI_USE_STACK 1
#endif

/* Stop parsing on first error (default is to keep parsing). */
#ifndef INI_STOP_ON_FIRST_ERROR
#define INI_STOP_ON_FIRST_ERROR 0
#endif

/* Maximum line length for any line in INI file. */
#ifndef INI_MAX_LINE
#define INI_MAX_LINE 200
#endif

#if !INI_USE_STACK
#include <stdlib.h>
#endif

#define MAX_SECTION 50
#define MAX_NAME 50


namespace IniReaderHelperLibrary
{

    /* Parse given INI-style file. May have [section]s, name=value pairs
       (whitespace stripped), and comments starting with ';' (semicolon). Section
       is "" if name=value pair parsed before any section heading. name:value
       pairs are also supported as a concession to Python's ConfigParser.
       For each name=value pair parsed, call handler function with given user
       pointer as well as section, name, and value (data only valid for duration
       of handler call). Handler should return nonzero on success, zero on error.
       Returns 0 on success, line number of first error on parse error (doesn't
       stop on first error), -1 on file open error, or -2 on memory allocation
       error (only when INI_USE_STACK is zero).
    */
    int ini_parse(
        const char* filename,
        int (*handler)(void* user, const char* section,const char* name, const char* value),
        void* user
        );

    /* Same as ini_parse(), but takes a FILE* instead of filename. This doesn't
      close the file when it's finished -- the caller must do that. */
    int ini_parse_file(
        FILE* file,
        int (*handler)(void* user, const char* section, const char* name, const char* value),
        void* user
        );

    /* Strip whitespace chars off end of given string, in place. Return s. */
    static char* rstrip(char* s)
    {
        char* p = s + strlen(s);
        while (p > s && isspace((unsigned char)(*--p)))
            *p = '\0';
        return s;
    }

    /* Return pointer to first non-whitespace char in given string. */
    static char* lskip(const char* s)
    {
        while (*s && isspace((unsigned char)(*s)))
            s++;
        return (char*)s;
    }

    /* Return pointer to first char c or ';' comment in given string, or pointer to
       null at end of string if neither found. ';' must be prefixed by a whitespace
       character to register as a comment. */
    static char* find_char_or_comment(const char* s, char c)
    {
        int was_whitespace = 0;
        while (*s && *s != c && !(was_whitespace && *s == ';')) {
            was_whitespace = isspace((unsigned char)(*s));
            s++;
        }
        return (char*)s;
    }

    /* Version of strncpy that ensures dest (size bytes) is null-terminated. */
    static char* strncpy0(char* dest, const char* src, size_t size)
    {
        strncpy(dest, src, size);
        dest[size - 1] = '\0';
        return dest;
    }
}