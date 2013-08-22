/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2009 William Hart

******************************************************************************/

#ifndef USTREAM_H
#define USTREAM_H

#include <gmp.h>

#include "flint.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
    int (*puts)(void*, const char*);
    int (*uputc)(void*, char);
    int (*put_mpz)(void*, int, const mpz_t);
    int (*put_ulong)(void*, ulong);
    int (*put_slong)(void*, slong);

    int (*get_slong)(void*, slong*);
    int (*get_ulong)(void*, ulong*);
    int (*ugetc)(void*);
    int (*get_mpz)(void*, int, mpz_t);

    int (*eof)(void*);
} ustream_funcs;

typedef struct
{
    void* data;
    const ustream_funcs* funcs;
} ustream;

/* NB: reversed arguments, just like puts */
static __inline__ int ustream_puts(const char* s, ustream stream)
{
    return stream.funcs->puts(stream.data, s);
}
/* NB: reversed arguments, just like putc */
static __inline__ int ustream_putc(char s, ustream stream)
{
    return stream.funcs->uputc(stream.data, s);
}
static __inline__ int ustream_put_mpz(ustream stream, int i, const mpz_t s)
{
    return stream.funcs->put_mpz(stream.data, i, s);
}
static __inline__ int ustream_out_str(ustream stream, int i, const mpz_t s)
{
    return ustream_put_mpz(stream, i, s);
}

static __inline__ int ustream_put_ulong(ustream stream, ulong s)
{
    return stream.funcs->put_ulong(stream.data, s);
}
static __inline__ int ustream_put_slong(ustream stream, slong s)
{
    return stream.funcs->put_slong(stream.data, s);
}

static __inline__ int ustream_get_slong(ustream stream, slong* s)
{
    return stream.funcs->get_slong(stream.data, s);
}
static __inline__ int ustream_get_ulong(ustream stream, ulong* s)
{
    return stream.funcs->get_ulong(stream.data, s);
}

static __inline__ int ustream_get_mpz(ustream stream, int i, mpz_t s)
{
    return stream.funcs->get_mpz(stream.data, i, s);
}
static __inline__ int ustream_mpz_inp_str(mpz_t s, ustream stream, int i)
{
    return ustream_get_mpz(stream, i, s);
}

static __inline__ int ustream_getc(ustream stream)
{
    return stream.funcs->ugetc(stream.data);
}

static __inline__ int ustream_eof(ustream stream)
{
    return stream.funcs->eof(stream.data);
}

extern const ustream_funcs __ustream_funcs_c_impl;

static __inline__ ustream FILE2ustream(FILE* file)
{
    ustream res;
    res.data = file;
    res.funcs = &__ustream_funcs_c_impl;
    return res;
}

#ifdef __cplusplus
}
#endif

#endif
