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
#include <stdlib.h>

#include "ustream.h"

int
main()
{
    FILE* f;
    slong n;

    printf("c_imp....");

    f = fopen("ustream_c_impl_test", "w+");
    if (!f)
    {
	printf("Error: unable to open file for writing.\n");
	abort();
    }

    ustream_put_slong(FILE2ustream(f), 25);
    ustream_puts("\nfoo\n", FILE2ustream(f));

    fflush(f);
    fclose(f);

    f = fopen("ustream_c_impl_test", "r");
    ustream_get_slong(FILE2ustream(f), &n);
    if(n != 25)
    {
	printf("Error: read %ld, expected 25\n", n);
	abort();
    }

    /* TODO more */

    fclose(f);
    if(remove("ustream_c_impl_test"))
    {
	printf("Error, unable to delete file ustream_c_impl_test\n");
	abort();
    }

    printf("PASS\n");
    return 0;
}
