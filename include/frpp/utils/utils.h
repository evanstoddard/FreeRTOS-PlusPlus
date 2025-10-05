/*
 * Copyright (C) Ovyl
 */

/**
 * @file utils.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef frpp_utils_h
#define frpp_utils_h

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define FRPP_MAX(a_, b_) ((a_ > b_) ? a_ : b_)

#if defined(__x86_64__)
#define FRPP_STACK_MIN_ALIGN (8U)
#else
#define FRPP_STACK_MIN_ALIGN (4U)
#endif

#define FRPP_VA_STACK_ALIGN(type_) FRPP_MAX(sizeof(type_), FRPP_STACK_MIN_ALIGN)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* frpp_utils_h */
