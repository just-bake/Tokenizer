//
// Created by justi on 2025-07-10.
//
#include <stdio.h>
#include <stdlib.h>
#include "jtok/tokenizer.h"

#define TOK_IF      (JTOK_TOKEN_USER + 1)
#define TOK_RETURN  (JTOK_TOKEN_USER + 2)
#define TOK_EQEQ    (JTOK_TOKEN_USER + 3)
#define TOK_LBRACE  (JTOK_TOKEN_USER + 4)
#define TOK_RBRACE  (JTOK_TOKEN_USER + 5)

const char* get_token_name(jtok_TokenType type) {
	switch (type) {
		case JTOK_TOKEN_IDENTIFIER: return "IDENTIFIER";
		case JTOK_TOKEN_NUMBER:     return "NUMBER";
		case JTOK_TOKEN_STRING:     return "STRING";
		case JTOK_TOKEN_SYMBOL:     return "SYMBOL";
		case JTOK_TOKEN_EOF:        return "EOF";
		case JTOK_TOKEN_UNKNOWN:    return "UNKNOWN";
		case TOK_IF:      return "IF";
		case TOK_RETURN:  return "RETURN";
		case TOK_EQEQ:    return "==";
		case TOK_LBRACE:  return "{";
		case TOK_RBRACE:  return "}";
		default:          return "CUSTOM";
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	jtok_Tokenizer* tokenizer = jtok_create_from_file(argv[1]);
	if (!tokenizer) {
		fprintf(stderr, "Failed to open or read file: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	// Register custom tokens
	jtok_define_token(tokenizer, "if",     TOK_IF);
	jtok_define_token(tokenizer, "return", TOK_RETURN);
	jtok_define_token(tokenizer, "==",     TOK_EQEQ);
	jtok_define_token(tokenizer, "{",      TOK_LBRACE);
	jtok_define_token(tokenizer, "}",      TOK_RBRACE);

	jtok_Token tok;
	while ((tok = jtok_next(tokenizer)).type != JTOK_TOKEN_EOF) {
		printf("Token %-10s: '%.*s'\n", get_token_name(tok.type),
			   (int)tok.length, tok.text);
	}

	jtok_destroy(tokenizer);
	return EXIT_SUCCESS;
}
