#pragma once

#include "matrix.h"

std::vector<code_word> readFile(const std::string& file_path);
void parseForward(const std::string& text,
                  std::vector<code_word>& codewords);
code_word ascii_to_code_word(const char c);

std::string parseBackwards(const std::vector<code_word>& codewords);
char code_word_to_ascii(const code_word& cw);
