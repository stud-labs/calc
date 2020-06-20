#include "calc.h"
#include "parser.hpp"
#include <cstdio>
#include <cstdlib>
#include <new>

void no_memory(void)
{
    std::printf("Ran out of memory! Exiting...\n");
    std::exit(EXIT_FAILURE);
}

int main(void){
    std::set_new_handler(no_memory);
    printf("> ");
    return yyparse();
}
