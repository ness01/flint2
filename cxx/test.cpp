#include <iostream>

#define INSTRUMENT
//#define EXPRESSION_TEMPLATES
#define MOVE_SEMANTICS
#include "header.h"

int
main()
{
    std::cout << "Hello word" << std::endl;
    flint::fmpz a = 4;
    flint::fmpz b = 5;
    flint::fmpz c = 6;
    flint::fmpz res = a + b*c;
    std::cout << res << std::endl;
    return 0;
}
