#include "parsetext.h"

std::vector<code_word> readFile(const std::string& file_path)
{
  std::string line;
  std::ifstream data_file(file_path);

  std::vector<code_word> text;

  while(getline(data_file, line))
    parseForward(line, text);

  return text;
}

void parseForward(const std::string& text,
	                std::vector<code_word>& codewords)
{
  for (size_t i = 0; i < text.length(); ++i)
    codewords.push_back(ascii_to_code_word(text[i]));

  // need to add a line break;
  codewords.push_back(10);

  return;
}

code_word ascii_to_code_word(const char c)
{
  const unsigned int k = (unsigned int)c;
  const code_word cw = k;
  return cw;
}



std::string parseBackwards(const std::vector<code_word>& codewords)
{
  std::string text = "";
  for (size_t i = 0; i < codewords.size(); ++i)
    text.append(1, code_word_to_ascii(codewords[i]));

  return text;
}

char code_word_to_ascii(const code_word& cw)
{
  const unsigned long ul = cw.to_ulong();
  const char c = (char)ul;

  return c;
}