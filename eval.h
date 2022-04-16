#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <functional>
#include <numbers>

namespace eval
{

struct Token
{
    std::string token;
    bool is_value;
};

double eval(std::string expr);

void handel_functions(std::vector<Token> &tokens, int i, std::function<double(double)> f);
int get_last_bracket_pos(std::vector<Token> s, int start);
void eval_and_replace_until_next_bracket(std::vector<Token> &tokens, int i);

std::string get_last_error();
int good();

std::vector<Token> eval(std::vector<Token> tokens);
std::vector<Token> tokenize(const std::string_view &s);
} // namespace eval
