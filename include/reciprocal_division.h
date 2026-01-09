/*
 * Original License:
    ============================================================================
    * File    : integer_division_uint16.h
    * Purpose : macros for integer division of uint16 values
    ------------------------------------------------------------------------------
    * Version : 1
    * Date    : 15th June 2009
    * Author  : Alan Bowens
    ------------------------------------------------------------------------------
    * Based on the article "Division of integers by constants" by Nigel Jones,
    * http://www.embeddedgurus.net/stack-overflow/2009/06/division-of-integers-by-constants.html
    * This article is also cross posted at his website: www.rmbconsulting.us.
    *
    * The above article was in turn based upon a paper by Doug Jones available 
    * at http://www.cs.uiowa.edu/~jones/bcd/divide.html
    *
    * The latest version of this file may be downloaded from my blog 
    * at http://codereview.blogspot.com/.
    *
    * This code is supplied as is. It comes with no warranty and no guarantee. Use 
    * it at your own risk. You are free to use it and modify it in any way you see 
    * fit, provided that this notice is included in its entirety.
    ============================================================================
 * ******************************************************************************
 * 
 * we take only what we need but there is a really nice method to learn!!!! 
 * 
 * original:     http://homepage.divms.uiowa.edu/~jones/bcd/divide.html#times
 * nice summary: https://web.archive.org/web/20240326221708/https://embeddedgurus.com/stack-overflow/2009/06/division-of-integers-by-constants/
 * 
 * code: (proper fork needed but later)
 *  https://github.com/stevenhoving/integer_division/
 */
#define U16REC2(A, M, S) (uint16_t)((((((uint32_t)(A) * (uint32_t)(M)) >> 16u) + (A)) >> 1u) >> (S))
#define U16DIVBY32767(A)    U16REC2(A, 0x0003u, 14u)

