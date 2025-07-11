//
// Created by justi on 2025-07-10.
//
#include <assert.h>
#include "jtok/tokenizer.h"

int main(void) {
	jtok_Tokenizer* t = jtok_create_from_file("test.txt");
	assert(t != NULL);
	jtok_destroy(t);
	return 0;
}
