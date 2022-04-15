#include "eval.h"

namespace eval
{
std::string last_error = "no error occured";
std::vector<std::string> valid_tokens = {"+", "-", "*", "/", "^", "(", ")", "sin", "cos", "tan", "ln", "log", "abs", "pi", "e"};

std::vector<std::string> tokenize(const std::string_view &s)
{
    std::vector<std::string> tokens;
    std::string expr = s.data();
    expr.erase(std::remove(expr.begin(), expr.end(), ' '), expr.end()); // remove all spaces
    int counter = 0;

    for (auto x : expr)
    {
        if (x == '(')
            counter++;
        if (x == ')')
            counter--;
    }

    if (counter < 0)
    {
        last_error = "TOKENIZE__MISSING_OPEN_BRACKET";
        return {};
    }
    else if (counter > 0)
    {
        last_error = "TOKENIZE__MISSING_CLOSE_BRACKET";
        return {};
    }

    int matched = 0;
    while (expr.size() > 0)
    {
        if (std::isdigit(expr[0]) || expr[0] == '.')
        {
            matched = 1;
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
                matched = 1;
                tokens.push_back(i);
                expr.erase(0, i.size());
                break;
            }
        }
        if (!matched)
        {
            last_error = "TOKENIZE__UNRECOGNIZED_TOKEN";
            return {};
        }
    }

    for (int i = 0; i < tokens.size(); i++) // idk if it works or is right, seems to do its job just fine
    {
        if (tokens[i] == "(" && (tokens[i + 1] == "+" || tokens[i + 1] == "-" || tokens[i + 1] == "*" || tokens[i + 1] == "/"))
        {
            last_error = "TOKENIZE__OPERATOR_AFTER_OPEN_BRACKET";
            return {};
        }
        if (tokens[i] == ")" && (tokens[i - 1] == "+" || tokens[i - 1] == "-" || tokens[i - 1] == "*" || tokens[i - 1] == "/"))
        {
            last_error = "TOKENIZE__OPERATOR_BEFORE_CLOSE_BRACKET";
            return {};
        }
    }
    return tokens;
}

double eval(std::string expr) { return eval(tokenize(expr)).size() ? std::stod(eval(tokenize(expr))[0]) : 0; }

std::vector<std::string> eval(std::vector<std::string> tokens)
{
    if (last_error != "no error occured")
    {
        return {};
    }
    // constants
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] == "pi")
            tokens[i] = std::to_string(std::numbers::pi);
        if (tokens[i] == "e")
            tokens[i] = std::to_string(std::numbers::e);
    }

    // functions
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i] == "sin")
        {
            handel_functions(tokens, i, [](double x) { return std::cos(x); });
            i = -1;
        }
        else if (tokens[i] == "cos")
        {
            handel_functions(tokens, i, [](double x) { return std::cos(x); });
            i = -1;
        }
        else if (tokens[i] == "tan")
        {
            handel_functions(tokens, i, [](double x) { return std::tan(x); });
            i = -1;
        }
        else if (tokens[i] == "abs")
        {
            handel_functions(tokens, i, [](double x) { return std::abs(x); });
            i = -1;
        }
        else if (tokens[i] == "ln")
        {
            handel_functions(tokens, i, [](double x) { return std::log(x); });
            i = -1;
        }
        else if (tokens[i] == "log")
        {
            handel_functions(tokens, i, [](double x) { return std::log10(x); });
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
            if (tokens[i] == "/" && tokens[i + 1] == "0")
            {
                last_error = "EVAL__DIVIDE_BY_ZERO";
                return {};
            }
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
    last_error = "EVAL__NO_CLOSING_BRACKET";
    return -1;
}

void eval_and_replace_until_next_bracket(std::vector<std::string> &tokens, int i)
{
    int j = get_last_bracket_pos(tokens, i);
    if (j - i <= 2)
    {
        tokens.erase(tokens.begin() + i, tokens.begin() + j);
        return;
    }
    std::vector<std::string> sub_tokens = std::vector<std::string>(tokens.begin() + i + 1, tokens.begin() + j - 1);
    std::string sub_token_eval = eval(sub_tokens).at(0);
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

std::string get_last_error()
{
    std::string backup = last_error;
    last_error = "no error occured";
    return backup;
}

int good() { return last_error == "no error occured"; }

} // namespace eval
