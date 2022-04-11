#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <functional>

std::vector<std::string> valid_tokens = {"+", "-", "*", "/", "^", "(", ")", "sin", "cos", "tan", "pi"};

void handel_functions(std::vector<std::string> &tokens, int i, std::function<double(double)> f);
int get_last_bracket_pos(std::vector<std::string> s, int start);
void eval_and_replace_until_next_bracket(std::vector<std::string> &tokens, int i);
std::vector<std::string> tokenize(const std::string_view &s);
std::vector<std::string> eval(std::vector<std::string> tokens);

int main(int argc, char const *argv[])
{
    std::cout << eval(tokenize("8/2*sin(2+2*tan((2))/2)*2^cos(3.5+5)"))[0];
    return 0;
}

std::vector<std::string> eval(std::vector<std::string> tokens)
{
    // constants
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {

        if (tokens[i] == "pi")
        {
            tokens[i] = std::to_string(std::numbers::pi);
        }
    }

    // functions
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i] == "sin")
        {
            handel_functions(tokens, i, sin);
            i = -1;
        }
        else if (tokens[i] == "cos")
        {
            handel_functions(tokens, i, cos);
            i = -1;
        }
        else if (tokens[i] == "tan")
        {
            handel_functions(tokens, i, tan);
            i = -1;
        }
    }

    // parentheses
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i] == "(")
            eval_and_replace_until_next_bracket(tokens, i--);
    }

    // exponent
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {

        if (tokens[i] == "^")
        {
            double a = std::stod(tokens[i - 1]);
            double b = std::stod(tokens[i + 1]);
            tokens[i - 1] = std::to_string(pow(a, b));
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i = -1;
        }
    }

    // divide and muliply
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i] == "*" || tokens[i] == "/")
        {
            double a = std::stod(tokens[i - 1]);
            double b = std::stod(tokens[i + 1]);
            tokens[i - 1] = tokens[i] == "*" ? std::to_string(a * b) : std::to_string(a / b);
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i = -1;
        }
    }

    // addition and subtraction
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i] == "+" || tokens[i] == "-")
        {
            double a = std::stod(tokens[i - 1]);
            double b = std::stod(tokens[i + 1]);
            tokens[i - 1] = tokens[i] == "+" ? std::to_string(a + b) : std::to_string(a - b);
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i = -1;
        }
    }

    return tokens;
}

std::vector<std::string> tokenize(const std::string_view &s)
{
    std::vector<std::string> tokens;                                    // vector of tokens
    std::string expr = s.data();                                        // string to be tokenized
    expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end()); // remove spaces
    while (expr.size() > 0)                                             //
    {

        if (std::isdigit(expr[0]) || expr[0] == '.')
        {
            std::string number = "";
            while (std::isdigit(expr[0]) || expr[0] == '.')
            {
                number += expr[0];
                expr.erase(0, 1);
            }
            tokens.push_back(number);
        }

        for (auto i : valid_tokens)
        {
            if (expr.starts_with(i))
            {
                tokens.push_back(i);
                expr.erase(0, i.size());
                break;
            }
        }
    }
    return tokens;
}

int get_last_bracket_pos(std::vector<std::string> s, int start)
{
    int count = 0;
    for (int i = start; i < s.size(); i++)
    {
        if (s[i] == "(")
            count++;
        else if (s[i] == ")")
            count--;
        if (count == 0)
            return i + 1;
    }
    return -1;
}

void eval_and_replace_until_next_bracket(std::vector<std::string> &tokens, int i)
{
    int j = get_last_bracket_pos(tokens, i);
    std::vector<std::string> sub_tokens = std::vector<std::string>(tokens.begin() + i + 1, tokens.begin() + j - 1);
    std::string sub_token_eval = eval(sub_tokens)[0];
    tokens.erase(tokens.begin() + i, tokens.begin() + j);
    tokens.insert(tokens.begin() + i, sub_token_eval);
}

void handel_functions(std::vector<std::string> &tokens, int i, std::function<double(double)> f)
{
    eval_and_replace_until_next_bracket(tokens, i + 1);
    double num_val_before = std::stod(tokens[i + 1]);

    double num_val_after = f(num_val_before);

    tokens[i] = std::to_string(num_val_after);
    tokens.erase(tokens.begin() + i + 1);
}