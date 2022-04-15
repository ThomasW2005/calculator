#include "eval.h"
#include <iostream>

int main(int argc, char const *argv[])
{

    // std::cout << eval::eval("(1)(/(1))") << '\n';
    // if (!eval::good())
    // std::cout << "error: " << eval::get_last_error();

    bool run = true;
    while (run)
    {
        std::string expr;
        std::getline(std::cin, expr);
        if (expr == "exit")
        {
            run = false;
            break;
        }
        std::cout << eval::eval(expr) << std::endl;
        if (!eval::good())
            std::cout << "error: " << eval::get_last_error() << std::endl;
    }
    // std::cout << eval::eval("8/2*sin(2+2*tan((2))/2)*2^cos(3.5+5)") << '\n';
    // std::cout << eval::eval("01   0 ( )   *1+1") << '\n';
    // std::cout << eval::eval("abs(sin(pi*(7/4)))");
    // if (!eval::good())
    //     std::cout << "error: " << eval::get_last_error();
    return 0;
}