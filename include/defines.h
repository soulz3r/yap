#pragma once

// unsigned int types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// signed int types
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

// floating point types
typedef float f32;
typedef double f64;

// boolean types
typedef int b32;
typedef char b8;

#define TRUE 1
#define FALSE 0

// platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define YAP_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif

// Linux OS
#elif defined(__linux__) || defined(__gnu_linux__)
#define YAP_PLATFORM_LINUX 1

#if defined(__ANDROID__)
#define YAP_PLATFORM_ANDROID 1
#endif

#elif defined(__unix__)
// Catch anything not caught by the above.
#define YAP_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define YAP_PLATFORM_POSIX 1
#endif
