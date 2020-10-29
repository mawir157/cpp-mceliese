#pragma once

#include "matrix.h"
#include "encode.h"

class LinearCode
{
	public:
		LinearCode(const matrix& M, const size_t mn_code_width);
		~LinearCode();

		// getters
		matrix get_gen_mat()   const { return mv_generator; }
		matrix get_check_mat() const { return mv_check; }
		size_t get_code_with() const { return mn_code_width; }
		size_t get_max_error() const { return mn_max_errors; }

		// [en/de]-crypting
		std::vector<code_word> encode_message(const std::vector<code_word>& message);
		std::vector<code_word> decode_message(const std::vector<code_word>& message);

		// printing code
		void print();
	
	private:
		matrix mv_generator;
		matrix mv_check;
		syndrome_table mm_syndromes;
		size_t mn_code_width; // a '4 bit -> 8 bit' code has mn_code_width = 4;
		size_t mn_max_errors;

		// [en/de]-crypting
		code_word encode_symbol(const code_word r);
		code_word decode_symbol(const code_word r);

		unsigned int calc_minimum_weight();
};
