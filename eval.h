#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <functional>
#include <numbers>

namespace eval
{
double eval(std::string expr);

void handel_functions(std::vector<std::string> &tokens, int i, std::function<double(double)> f);
int get_last_bracket_pos(std::vector<std::string> s, int start);
void eval_and_replace_until_next_bracket(std::vector<std::string> &tokens, int i);

std::string get_last_error();
int good();

std::vector<std::string> eval(std::vector<std::string> tokens);
std::vector<std::string> tokenize(const std::string_view &s);
} // namespace eval
