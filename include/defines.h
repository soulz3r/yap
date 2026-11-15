#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Boolean type
typedef bool b;

// Unsigned fixed-width integer types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed fixed-width integer types
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Floating-point types
typedef float f32;
typedef double f64;

// Size type
typedef size_t st;

// Platform detection
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
// POSIX
#define YAP_PLATFORM_POSIX 1
#endif
