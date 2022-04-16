#include "eval.h"

namespace eval
{

std::string last_error = "no error occured";
std::vector<std::string> valid_tokens = {"+", "-", "*", "/", "^", "(", ")", "sin", "cos", "tan", "ln", "log", "abs", "pi", "e"};

std::vector<Token> lex(const std::string_view &s) // actually a lexer now
{
    std::vector<Token> tokens;
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
        last_error = "SYNTAX_TOKENIZE__MISSING_OPEN_BRACKET";
        return {};
    }
    else if (counter > 0)
    {
        last_error = "SYNTAX_TOKENIZE__MISSING_CLOSE_BRACKET";
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
            tokens.push_back({number, true});
        }

        for (auto i : valid_tokens)
        {
            if (expr.starts_with(i))
            {
                matched = 1;
                tokens.push_back({i, false});
                expr.erase(0, i.size());
                break;
            }
        }
        if (!matched)
        {
            last_error = "SYNTAX_TOKENIZE__UNRECOGNIZED_TOKEN";
            return {};
        }
    }

    // if a minus is before a number, make it negative and remove the minus
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].token == "-" && tokens[i + 1].is_value && !tokens[i - 1].is_value)
        {
            tokens[i + 1].token = "-" + tokens[i + 1].token;
            tokens.erase(tokens.begin() + i);
        }
        // if a plus is before a number, remove the plus
        if (tokens[i].token == "+" && tokens[i + 1].is_value && !tokens[i - 1].is_value)
            tokens.erase(tokens.begin() + i);
    }

    // if there are multiple minuses, combine them into one
    // bruh makes no sense
    // bool worked = false;
    // do
    // {
    //     for (int i = 0; i < tokens.size() - 1; i++)
    //     {
    //         if (tokens[i].token == "-" && tokens[i + 1].token == "-")
    //         {
    //             worked = true;
    //             tokens[i].token = "+";
    //             tokens.erase(tokens.begin() + i + 1);
    //             break;
    //         }
    //         if (tokens[i].token == "+" && tokens[i + 1].token == "+")
    //         {
    //             worked = true;
    //             tokens[i].token = "-";
    //             tokens.erase(tokens.begin() + i + 1);
    //             break;
    //         }
    //     }
    //     worked = false;
    // } while (worked);

    for (int i = 0; i < tokens.size(); i++) // idk if it works or is right, seems to do its job just fine
    {
        if (tokens[i].token == "(" && (tokens[i + 1].token == "+" || tokens[i + 1].token == "-" || tokens[i + 1].token == "*" || tokens[i + 1].token == "/"))
        {
            last_error = "SYNTAX_TOKENIZE__OPERATOR_AFTER_OPEN_BRACKET";
            return {};
        }
        if (tokens[i].token == ")" && (tokens[i - 1].token == "+" || tokens[i - 1].token == "-" || tokens[i - 1].token == "*" || tokens[i - 1].token == "/"))
        {
            last_error = "SYNTAX_TOKENIZE__OPERATOR_BEFORE_CLOSE_BRACKET";
            return {};
        }
    }

    for (int i = 1; i < tokens.size() - 1; i++) // idk if it works or is right, seems to do its job just fine
    {
        if (tokens[i].token == "(" && tokens[i - 1].is_value)
            tokens.insert(tokens.begin() + i, {"*", false});
        if (tokens[i].token == ")" && tokens[i + 1].is_value)
            tokens.insert(tokens.begin() + i + 1, {"*", false});
    }
    return tokens;
}

double eval(std::string expr) { return eval(lex(expr)).size() ? std::stod(eval(lex(expr))[0].token) : 0; }

std::vector<Token> eval(std::vector<Token> tokens)
{
    if (last_error != "no error occured")
    {
        return {};
    }
    // constants
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].token == "pi")
            tokens[i].token = std::to_string(std::numbers::pi);
        if (tokens[i].token == "e")
            tokens[i].token = std::to_string(std::numbers::e);
    }

    // functions
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i].token == "sin")
        {
            handel_functions(tokens, i, [](double x) { return std::cos(x); });
            i = -1;
        }
        else if (tokens[i].token == "cos")
        {
            handel_functions(tokens, i, [](double x) { return std::cos(x); });
            i = -1;
        }
        else if (tokens[i].token == "tan")
        {
            handel_functions(tokens, i, [](double x) { return std::tan(x); });
            i = -1;
        }
        else if (tokens[i].token == "abs")
        {
            handel_functions(tokens, i, [](double x) { return std::abs(x); });
            i = -1;
        }
        else if (tokens[i].token == "ln")
        {
            handel_functions(tokens, i, [](double x) { return std::log(x); });
            i = -1;
        }
        else if (tokens[i].token == "log")
        {
            handel_functions(tokens, i, [](double x) { return std::log10(x); });
            i = -1;
        }
    }

    // parentheses
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i].token == "(")
            eval_and_replace_until_next_bracket(tokens, i--);
    }

    // exponent
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {

        if (tokens[i].token == "^")
        {
            double a = std::stod(tokens[i - 1].token);
            double b = std::stod(tokens[i + 1].token);
            tokens[i - 1].token = std::to_string(pow(a, b));
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i = -1;
        }
    }

    // divide and muliply
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i].token == "*" || tokens[i].token == "/")
        {
            if (tokens[i].token == "/" && !std::abs(std::stod(tokens[i + 1].token)))
            {
                last_error = "SEMANTIC_EVAL__DIVIDE_BY_ZERO";
                return {};
            }
            double a = std::stod(tokens[i - 1].token);
            double b = std::stod(tokens[i + 1].token);
            tokens[i - 1].token = tokens[i].token == "*" ? std::to_string(a * b) : std::to_string(a / b);
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i = -1;
        }
    }

    // addition and subtraction
    for (int i = 0; tokens.size() > 1 && i < tokens.size() - 1; i++)
    {
        if (tokens[i].token == "+" || tokens[i].token == "-")
        {
            double a = std::stod(tokens[i - 1].token);
            double b = std::stod(tokens[i + 1].token);
            tokens[i - 1].token = tokens[i].token == "+" ? std::to_string(a + b) : std::to_string(a - b);
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i = -1;
        }
    }

    return tokens;
}

int get_last_bracket_pos(std::vector<Token> s, int start)
{
    int count = 0;
    for (int i = start; i < s.size(); i++)
    {
        if (s[i].token == "(")
            count++;
        else if (s[i].token == ")")
            count--;
        if (count == 0)
            return i + 1;
    }
    last_error = "SYNTAX_EVAL__NO_CLOSING_BRACKET";
    return -1;
}

void eval_and_replace_until_next_bracket(std::vector<Token> &tokens, int i)
{
    int j = get_last_bracket_pos(tokens, i);
    if (j - i <= 2)
    {
        tokens.erase(tokens.begin() + i, tokens.begin() + j);
        return;
    }
    std::vector<Token> sub_tokens = std::vector<Token>(tokens.begin() + i + 1, tokens.begin() + j - 1);
    Token sub_token_eval = eval(sub_tokens).at(0);
    tokens.erase(tokens.begin() + i, tokens.begin() + j);
    tokens.insert(tokens.begin() + i, sub_token_eval);
}

void handel_functions(std::vector<Token> &tokens, int i, std::function<double(double)> f)
{
    eval_and_replace_until_next_bracket(tokens, i + 1);
    double num_val_before = std::stod(tokens[i + 1].token);

    double num_val_after = f(num_val_before);

    tokens[i].token = std::to_string(num_val_after);
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
