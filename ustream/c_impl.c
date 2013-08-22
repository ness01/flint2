
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

    Copyright (C) 2013 Tom Bachmann

******************************************************************************/

#include <stdio.h>

#include "ustream.h"

static int cputs(void* d, const char* s)
{
    return fputs(s, (FILE*) d);
}

static int cputc(void* d, char s)
{
    return fputc(s, (FILE*) d);
}

static int cput_mpz(void* d, int b, const mpz_t s)
{
    return (int) mpz_out_str((FILE*) d, b, s);
}

static int cput_ulong(void* d, ulong s)
{
    return fprintf((FILE*) d, "%lu", s);
}

static int cput_slong(void* d, slong s)
{
    return fprintf((FILE*) d, "%ld", s);
}

static int cget_slong(void* d, slong* s)
{
    return fscanf((FILE*) d, "%ld", s);
}

static int cget_ulong(void* d, ulong* s)
{
    return fscanf((FILE*) d, "%lu", s);
}

static int cgetc(void* d)
{
    return fgetc((FILE*) d);
}

static int cget_mpz(void* d, int b, mpz_t s)
{
    return mpz_inp_str(s, (FILE*) d, b);
}

static int ceof(void* d)
{
    return feof((FILE*) d);
}

const ustream_funcs __ustream_funcs_c_impl = {
    &cputs,
    &cputc,
    &cput_mpz,
    &cput_ulong,
    &cput_slong,

    &cget_slong,
    &cget_ulong,
    &cgetc,
    &cget_mpz,

    &ceof
};
