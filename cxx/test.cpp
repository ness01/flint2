#include <iostream>

#define INSTRUMENT
#define EXPRESSION_TEMPLATES
#define MOVE_SEMANTICS
#include "header.h"

int
main()
{
    flint::fmpz a = 4;
    flint::fmpz b = 5;
    flint::fmpz c = 6;
    flint::fmpz d = 7;
    flint::fmpz e = 8;
    flint::fmpz f = 9;
    std::cout << "Hello word" << std::endl;
    //flint::fmpz res = a + b*c;
    //flint::fmpz res = ((((a + b) + c) + d) + e) + f;
    flint::fmpz res = a + (b + (c + (d + (e + f))));
    //flint::fmpz res = a*b + c*d + e*f;
    //flint::fmpz res = a + b;
    std::cout << res << std::endl;
    return 0;
}
