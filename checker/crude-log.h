//////////////////////////////////////////////////////////////////////////////
//////////////// A crude logging and printing tool for c/c++ /////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////// Only works on linux system with GCC compiler ////////////////
//////////////////////////////////////////////////////////////////////////////
////// No functions or global variables. Only macros in one header file //////
//////////////////////////////////////////////////////////////////////////////
/////////////////// Author: https://github.com/alexloser /////////////////////
///////////////// License: all free, do what ever you want! //////////////////
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#ifndef CRUDE_SNIPPET_BEGIN
#define CRUDE_SNIPPET_BEGIN ({
#endif
#ifndef CRUDE_SNIPPET_END
#define CRUDE_SNIPPET_END   })
#endif

#ifndef CRUDE_SRC_POS
#define CRUDE_SRC_POS  __FILE__,__LINE__,__FUNCTION__
#endif
#ifndef CRUDE_SRC_POSE
#define CRUDE_SRC_POSE __FILE__,__LINE__,__PRETTY_FUNCTION__
#endif

// Can be changed by user
#ifndef CRUDE_OSTREAM
#define CRUDE_OSTREAM stderr
#endif

#ifndef CRUDE_FASTEST_SNPRINTF
#define CRUDE_FASTEST_SNPRINTF __builtin_snprintf
#endif

#ifndef CRUDE_FASTEST_STRFTIME
#define CRUDE_FASTEST_STRFTIME __builtin_strftime
#endif

#ifndef CRUDE_FASTEST_FPRINTF
#define CRUDE_FASTEST_FPRINTF  __builtin_fprintf
#endif

#ifndef CRUDE_SAFE_LOCALTIME
#define CRUDE_SAFE_LOCALTIME   localtime_r
#endif

// Do not use this in user's code.
#ifndef  __CRUDE_GET_DT__
#define  __CRUDE_GET_DT__(__varbuf__) \
    char __varbuf__[32] = { '[' }; \
    do { \
    struct tm       CRUDE_GET_DT_var_tm; \
    struct timeval  CRUDE_GET_DT_var_timeval; \
    (void)gettimeofday(&CRUDE_GET_DT_var_timeval, NULL); \
    CRUDE_SAFE_LOCALTIME(&CRUDE_GET_DT_var_timeval.tv_sec, &CRUDE_GET_DT_var_tm); \
    CRUDE_FASTEST_STRFTIME(__varbuf__+1,  20, "%F %T",  &CRUDE_GET_DT_var_tm); \
    CRUDE_FASTEST_SNPRINTF(__varbuf__+20, 10, ".%06ld]", CRUDE_GET_DT_var_timeval.tv_usec); \
    } while(0)
#endif //__CRUDE_GET_DT__

//////////////////////////////////////////////////////////////////////////////
/////////// Debug macro defines, can close if CRUDE_NDEBUG defined ///////////
//////////////////////////////////////////////////////////////////////////////
#ifdef  CRUDE_NDEBUG

#define CRUDE_DEBUG_V0(fmt, ...)
#define CRUDE_DEBUG_V1(fmt, ...)
#define CRUDE_DEBUG_V2(fmt, ...)
#define CRUDE_DEBUG_V3(fmt, ...)
#define CRUDE_DEBUG_V4(fmt, ...)
#define CRUDE_DEBUG_V5(fmt, ...)
#define CRUDE_DEBUG(fmt, ...)

#else

#ifndef CRUDE_DEBUG_MACROS
#define CRUDE_DEBUG_MACROS

#define CRUDE_DEBUG_V0(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[DEBUG] " fmt "\n"), ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_DEBUG_V1(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[DEBUG] [%s] " fmt "\n"), __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_DEBUG_V2(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [DEBUG] [%s] " fmt "\n"), __crdlg_dt__, __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_DEBUG_V3(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [DEBUG] [%s] " fmt "\n"), __crdlg_dt__, __PRETTY_FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_DEBUG_V4(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [DEBUG] [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POS, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_DEBUG_V5(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [DEBUG] [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POSE, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_DEBUG(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [DEBUG] " fmt "\n"), __crdlg_dt__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#endif // CRUDE_DEBUG_MACROS
#endif // CRUDE_NDEBUG


//////////////////////////////////////////////////////////////////////////////
////////////////// Logging macro defines, can not be closed //////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef CRUDE_LOG_MACROS
#define CRUDE_LOG_MACROS

/////////////////////////////////// INFO /////////////////////////////////////
#define CRUDE_INFO_V0(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[INFO]  " fmt "\n"), ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_INFO_V1(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[INFO]  [%s] " fmt "\n"), __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_INFO_V2(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [INFO]  [%s] " fmt "\n"), __crdlg_dt__, __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_INFO_V3(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [INFO]  [%s] " fmt "\n"), __crdlg_dt__, __PRETTY_FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_INFO_V4(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [INFO]  [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POS, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_INFO_V5(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [INFO]  [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POSE, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_INFO(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [INFO]  " fmt "\n"), __crdlg_dt__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END


/////////////////////////////////// WARN /////////////////////////////////////
#define CRUDE_WARN_V0(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[WARN]  " fmt "\n"), ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_WARN_V1(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[WARN]  [%s] " fmt "\n"), __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_WARN_V2(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [WARN]  [%s] " fmt "\n"), __crdlg_dt__, __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_WARN_V3(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [WARN]  [%s] " fmt "\n"), __crdlg_dt__, __PRETTY_FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_WARN_V4(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [WARN]  [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POS, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_WARN_V5(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [WARN]  [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POSE, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_WARN(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [WARN]  " fmt "\n"), __crdlg_dt__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END


/////////////////////////////////// ERROR ////////////////////////////////////
#define CRUDE_ERROR_V0(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[ERROR] " fmt "\n"), ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_ERROR_V1(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[ERROR] [%s] " fmt "\n"), __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_ERROR_V2(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [ERROR] [%s] " fmt "\n"), __crdlg_dt__, __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_ERROR_V3(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [ERROR] [%s] " fmt "\n"), __crdlg_dt__, __PRETTY_FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_ERROR_V4(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [ERROR] [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POS, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_ERROR_V5(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [ERROR] [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POSE, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_ERROR(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [ERROR] " fmt "\n"), __crdlg_dt__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END


/////////////////////////////////// FATAL /////////////////////////////////////
#define CRUDE_FATAL_V0(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[FATAL] " fmt "\n"), ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_FATAL_V1(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("[FATAL] [%s] " fmt "\n"), __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_FATAL_V2(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [FATAL] [%s] " fmt "\n"), __crdlg_dt__, __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_FATAL_V3(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [FATAL] [%s] " fmt "\n"), __crdlg_dt__, __PRETTY_FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_FATAL_V4(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [FATAL] [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POS, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_FATAL_V5(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [FATAL] [%s:%d] [%s] " fmt "\n"), __crdlg_dt__, CRUDE_SRC_POSE, ##__VA_ARGS__); \
CRUDE_SNIPPET_END

#define CRUDE_FATAL(fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, ("%s [FATAL] " fmt "\n"), __crdlg_dt__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END


////////////////////////////////// Custom ////////////////////////////////////
#ifndef CRUDE_LOG_V0
#define CRUDE_LOG_V0(level, fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    char __crdlg_vfmt__[256]; \
    strcat(strcat(strcpy(__crdlg_vfmt__, ("[" #level "] ")), (fmt)), "\n"); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, __crdlg_vfmt__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END
#endif//CRUDE_LOG_V0

#ifndef CRUDE_LOG_V1
#define CRUDE_LOG_V1(level, fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    char __crdlg_vfmt__[256]; \
    strcat(strcat(strcpy(__crdlg_vfmt__, ("[" #level "] [%s] ")), (fmt)), "\n"); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, __crdlg_vfmt__, __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END
#endif//CRUDE_LOG_V1

#ifndef CRUDE_LOG_V2
#define CRUDE_LOG_V2(level, fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    char __crdlg_vfmt__[256]; \
    strcat(strcat(strcpy(__crdlg_vfmt__, ("%s [" #level "] [%s] ")), (fmt)), "\n"); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, __crdlg_vfmt__, __crdlg_dt__, __FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END
#endif//CRUDE_LOG_V2

#ifndef CRUDE_LOG_V3
#define CRUDE_LOG_V3(level, fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    char __crdlg_vfmt__[256]; \
    strcat(strcat(strcpy(__crdlg_vfmt__, ("%s [" #level "] [%s] ")), (fmt)), "\n"); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, __crdlg_vfmt__, __crdlg_dt__, __PRETTY_FUNCTION__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END
#endif//CRUDE_LOG_V3

#ifndef CRUDE_LOG_V4
#define CRUDE_LOG_V4(level, fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    char __crdlg_vfmt__[256]; \
    strcat(strcat(strcpy(__crdlg_vfmt__, ("%s [" #level "] [%s:%d] [%s] ")), (fmt)), "\n"); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, __crdlg_vfmt__, __crdlg_dt__, CRUDE_SRC_POS, ##__VA_ARGS__); \
CRUDE_SNIPPET_END
#endif//CRUDE_LOG_V4

#ifndef CRUDE_LOG_V5
#define CRUDE_LOG_V5(level, fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    char __crdlg_vfmt__[256]; \
    strcat(strcat(strcpy(__crdlg_vfmt__, ("%s [" #level "] [%s:%d] [%s] ")), (fmt)), "\n"); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, __crdlg_vfmt__, __crdlg_dt__, CRUDE_SRC_POSE, ##__VA_ARGS__); \
CRUDE_SNIPPET_END
#endif//CRUDE_LOG_V5

#ifndef CRUDE_LOG
#define CRUDE_LOG(level, fmt, ...) \
CRUDE_SNIPPET_BEGIN \
    __CRUDE_GET_DT__(__crdlg_dt__); \
    char __crdlg_vfmt__[256]; \
    strcat(strcat(strcpy(__crdlg_vfmt__, ("%s [" #level "] ")), (fmt)), "\n"); \
    CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, __crdlg_vfmt__, __crdlg_dt__, ##__VA_ARGS__); \
CRUDE_SNIPPET_END
#endif//CRUDE_LOG


#endif // CRUDE_LOG_MACROS
///////////////////////////////////////////////////////////////////////////////


////////////////////// Utility and helper macro defines ///////////////////////
#ifndef CRUDE_LF
#define CRUDE_LF() fputs("\n", CRUDE_OSTREAM)
#define CRUDE_LINEFEED CRUDE_LF
#endif

#ifndef CRUDE_FFLUSH
#define CRUDE_FFLUSH() fflush(CRUDE_OSTREAM)
#endif


#ifndef CRUDE_BUFFERING_MODE
#define CRUDE_FULL_BUFFERED (_IOFBF)
#define CRUDE_LINE_BUFFERED (_IOLBF)
#define CRUDE_NONE_BUFFERED (_IONBF)
#endif

#ifndef CRUDE_SET_BUFFERING_METHOD
#define CRUDE_SET_BUFFERING_METHOD(buf, size, mode) \
CRUDE_SNIPPET_BEGIN \
    switch (mode) { \
        case CRUDE_FULL_BUFFERED: \
        case CRUDE_LINE_BUFFERED: \
            setvbuf(CRUDE_OSTREAM, buf, mode, size); \
            break; \
        case CRUDE_NONE_BUFFERED: \
            CRUDE_WARN_V2("Using no-buffering mode will lead to worse performance!"); \
            setvbuf(CRUDE_OSTREAM, buf, mode, size); \
            break; \
        default: \
            CRUDE_ERROR_V2("Wrong buffering mode for std::setvbuf"); \
            break; \
    } \
CRUDE_SNIPPET_END
#endif


#ifndef CRUDE_FPRINT
#define CRUDE_FPRINT(fmt, ...)   CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, (fmt), ##__VA_ARGS__)
#endif
#ifndef CRUDE_FPRINTLN
#define CRUDE_FPRINTLN(fmt, ...) CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, (fmt "\n"), ##__VA_ARGS__)
#endif

#ifndef CRUDE_PRINT
#define CRUDE_PRINT(cstr)   CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, "%s", (cstr))
#endif
#ifndef CRUDE_PRINTLN
#define CRUDE_PRINTLN(cstr) CRUDE_FASTEST_FPRINTF(CRUDE_OSTREAM, "%s\n", (cstr))
#endif


#ifndef CRUDE_ASFORMAT
#define CRUDE_ASFORMAT_BEGIN(output, format, ...) \
do { \
    char*  output = NULL; \
    size_t output##_size = (size_t)__asprintf(&output, format, ##__VA_ARGS__); \
    if (!output) output##_size = 0;

#define CRUDE_ASFORMAT_END(output) \
    if (output) free(output); \
} while(0);
#endif//CRUDE_ASFORMAT


#ifndef CRUDE_CLOSE
#define CRUDE_CLOSE() \
CRUDE_SNIPPET_BEGIN \
    if (CRUDE_OSTREAM) { \
        CRUDE_FFLUSH(); \
        fclose(CRUDE_OSTREAM); \
    } \
CRUDE_SNIPPET_END
#endif//CRUDE_CLOSE


#ifndef  CRUDE_ASSERT
#define  CRUDE_ASSERT(expr, ...)                       \
if (!(expr)) {                                         \
    CRUDE_ERROR_V3("Assertion: `" #expr "` failed!");  \
    __VA_ARGS__;                                       \
}
#endif// CRUDE_CHKPTR


#ifndef  CRUDE_CHECK_PTR
#define  CRUDE_CHECK_PTR(ptr, ...)                      \
if (((void*)0) == (ptr)) {                              \
    CRUDE_ERROR_V3("Arg `" #ptr "` is null or zero!");  \
    __VA_ARGS__;                                        \
}
#endif// CRUDE_CHECK_PTR


///////////////////////////////////////////////////////////////////////////////
