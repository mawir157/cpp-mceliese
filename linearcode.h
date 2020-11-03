#pragma once

#include "matrix.h"
#include "encode.h"

class LinearCode
{
	public:
		LinearCode(const matrix& M, const uint64_t width);
		~LinearCode();

		// getters
		matrix get_gen_mat()    const { return mv_generator; }
		// matrix get_check_mat()  const { return mv_check; }
		size_t get_code_with()  const { return mn_code_width; }
		size_t get_max_error()  const { return mn_max_errors; }
		size_t code_word_size() const { return mv_generator.size(); }

		// setters
		void set_decode_flag(const bool b) { mb_can_decode = b; }

		// swap function
		void swapColumns(const size_t c1, const size_t c2);
		void replaceRow(const size_t r, const code_word wd);

		// [en/de]-crypting
		std::vector<code_word> encode_message(const std::vector<code_word>& message) const;
		std::vector<code_word> decode_message(const std::vector<code_word>& message) const;

		// printing code
		void print() const;
	
	private:
		matrix mv_generator;
		matrix mv_check;
		syndrome_table mm_syndromes;
		size_t mn_code_width; // a '4 bit -> 8 bit' code has mn_code_width = 4;
		size_t mn_max_errors;
		bool mb_can_decode; // flag gets set to to false during building public key

		// [en/de]-crypting
		code_word encode_symbol(const code_word r) const;
		code_word decode_symbol(const code_word r) const;

		size_t calc_minimum_weight() const;
};
