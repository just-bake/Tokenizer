#ifndef JTOK_TOKENIZER_H
#define JTOK_TOKENIZER_H

#ifdef __cplusplus
extern "C" {
	#endif

	#include <stddef.h> // for size_t

	typedef struct jtok_Tokenizer jtok_Tokenizer;

	/**
	 * Token types.
	 */
	typedef enum {
		JTOK_TOKEN_UNKNOWN = 0,
		JTOK_TOKEN_IDENTIFIER,
		JTOK_TOKEN_NUMBER,
		JTOK_TOKEN_STRING,
		JTOK_TOKEN_SYMBOL,
		JTOK_TOKEN_EOF,
		JTOK_TOKEN_USER = 1000  // Start custom tokens here
	} jtok_TokenType;

	/**
	 * Token structure.
	 */
	typedef struct {
		jtok_TokenType type;
		const char* text;
		size_t length;
	} jtok_Token;

	/**
	 * Create a tokenizer from a string.
	 */
	jtok_Tokenizer* jtok_create_from_string(const char* source);

	/**
	 * Create a tokenizer from a file.
	 */
	jtok_Tokenizer* jtok_create_from_file(const char* filename);

	/**
	 * Free tokenizer resources.
	 */
	void jtok_destroy(jtok_Tokenizer* tokenizer);

	/**
	 * Register a custom token string and associate it with a type.
	 *
	 * @param tokenizer Tokenizer instance.
	 * @param string Token string (e.g. "if", "+", "while").
	 * @param type Must be >= JTOK_TOKEN_USER.
	 * @return 0 on success, non-zero on error.
	 */
	int jtok_define_token(jtok_Tokenizer* tokenizer, const char* string, jtok_TokenType type);

	/**
	 * Get the next token.
	 */
	jtok_Token jtok_next(jtok_Tokenizer* tokenizer);

	#ifdef __cplusplus
}
#endif

#endif // JTOK_TOKENIZER_H
