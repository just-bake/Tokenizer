#include "jtok/tokenizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct jtok_Matcher jtok_Matcher;
typedef jtok_Token (*jtok_MatchFunc)(jtok_Tokenizer* tokenizer);

struct jtok_Matcher {
    jtok_MatchFunc match;
    jtok_Matcher* next;
};

typedef struct {
    char* text;
    jtok_TokenType type;
} TokenDef;

struct jtok_Tokenizer {
    const char* source;
    size_t pos;
    size_t length;
    char* buffer;

    TokenDef* defs;
    size_t def_count;
    size_t def_capacity;

    jtok_Matcher* matcher_chain;
};

// ========== Utility ==========

static int is_identifier_start(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

static int is_identifier_part(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static void skip_whitespace(jtok_Tokenizer* t) {
    while (t->pos < t->length &&
          (t->source[t->pos] == ' ' || t->source[t->pos] == '\n' ||
           t->source[t->pos] == '\t' || t->source[t->pos] == '\r')) {
        t->pos++;
    }
}

// ========== Matcher Helpers ==========

static jtok_Token match_custom(jtok_Tokenizer* t) {
    for (size_t i = 0; i < t->def_count; ++i) {
        const char* word = t->defs[i].text;
        size_t len = strlen(word);
        if (strncmp(&t->source[t->pos], word, len) == 0) {
            char next = t->source[t->pos + len];
            if (!is_identifier_part(next)) {
                t->pos += len;
                return (jtok_Token){ t->defs[i].type, &t->source[t->pos - len], len };
            }
        }
    }
    return (jtok_Token){ JTOK_TOKEN_UNKNOWN, NULL, 0 };
}

static jtok_Token match_identifier(jtok_Tokenizer* t) {
    if (!is_identifier_start(t->source[t->pos])) return (jtok_Token){ JTOK_TOKEN_UNKNOWN, NULL, 0 };
    size_t start = t->pos++;
    while (t->pos < t->length && is_identifier_part(t->source[t->pos])) t->pos++;
    return (jtok_Token){ JTOK_TOKEN_IDENTIFIER, &t->source[start], t->pos - start };
}

static jtok_Token match_number(jtok_Tokenizer* t) {
    if (!isdigit((unsigned char)t->source[t->pos])) return (jtok_Token){ JTOK_TOKEN_UNKNOWN, NULL, 0 };
    size_t start = t->pos++;
    while (t->pos < t->length && isdigit((unsigned char)t->source[t->pos])) t->pos++;
    return (jtok_Token){ JTOK_TOKEN_NUMBER, &t->source[start], t->pos - start };
}

static jtok_Token match_string(jtok_Tokenizer* t) {
    if (t->source[t->pos] != '"') return (jtok_Token){ JTOK_TOKEN_UNKNOWN, NULL, 0 };
    size_t start = ++t->pos;
    while (t->pos < t->length && t->source[t->pos] != '"') {
        if (t->source[t->pos] == '\\') t->pos++; // skip escape
        t->pos++;
    }
    size_t len = t->pos - start;
    if (t->pos < t->length) t->pos++; // skip closing quote
    return (jtok_Token){ JTOK_TOKEN_STRING, &t->source[start], len };
}

static jtok_Token match_symbol(jtok_Tokenizer* t) {
    jtok_Token tok = {
        .type = JTOK_TOKEN_SYMBOL,
        .text = &t->source[t->pos],
        .length = 1
    };
    t->pos++;
    return tok;
}

// ========== Matcher Chain ==========

static void jtok_add_matcher(jtok_Tokenizer* tokenizer, jtok_MatchFunc func) {
    jtok_Matcher* matcher = malloc(sizeof(jtok_Matcher));
    matcher->match = func;
    matcher->next = NULL;

    if (!tokenizer->matcher_chain) {
        tokenizer->matcher_chain = matcher;
    } else {
        jtok_Matcher* current = tokenizer->matcher_chain;
        while (current->next) current = current->next;
        current->next = matcher;
    }
}

// ========== Core Tokenizer API ==========

jtok_Token jtok_next(jtok_Tokenizer* t) {
    skip_whitespace(t);

    if (t->pos >= t->length)
        return (jtok_Token){ JTOK_TOKEN_EOF, NULL, 0 };

    for (jtok_Matcher* m = t->matcher_chain; m; m = m->next) {
        size_t old_pos = t->pos;
        jtok_Token tok = m->match(t);
        if (tok.type != JTOK_TOKEN_UNKNOWN) return tok;
        t->pos = old_pos; // backtrack
    }

    // If nothing matched, skip unknown char
    t->pos++;
    return (jtok_Token){ JTOK_TOKEN_UNKNOWN, &t->source[t->pos - 1], 1 };
}

// ========== Construction / Destruction ==========

jtok_Tokenizer* jtok_create_from_string(const char* source) {
    if (!source) return NULL;

    jtok_Tokenizer* t = calloc(1, sizeof(jtok_Tokenizer));
    if (!t) return NULL;

    t->length = strlen(source);
    t->source = source;

    // Register matchers (priority order)
    jtok_add_matcher(t, match_custom);
    jtok_add_matcher(t, match_identifier);
    jtok_add_matcher(t, match_number);
    jtok_add_matcher(t, match_string);
    jtok_add_matcher(t, match_symbol);

    return t;
}

jtok_Tokenizer* jtok_create_from_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);

    jtok_Tokenizer* t = jtok_create_from_string(buffer);
    if (t) t->buffer = buffer;
    else free(buffer);

    return t;
}

void jtok_destroy(jtok_Tokenizer* t) {
    if (!t) return;

    free(t->buffer);

    for (size_t i = 0; i < t->def_count; ++i)
        free(t->defs[i].text);

    free(t->defs);

    jtok_Matcher* m = t->matcher_chain;
    while (m) {
        jtok_Matcher* next = m->next;
        free(m);
        m = next;
    }

    free(t);
}

// ========== Token Definition ==========

int jtok_define_token(jtok_Tokenizer* t, const char* string, jtok_TokenType type) {
    if (!t || !string || type < JTOK_TOKEN_USER) return -1;

    if (t->def_count == t->def_capacity) {
        size_t new_cap = (t->def_capacity == 0) ? 8 : t->def_capacity * 2;
        TokenDef* new_defs = realloc(t->defs, new_cap * sizeof(TokenDef));
        if (!new_defs) return -1;
        t->defs = new_defs;
        t->def_capacity = new_cap;
    }

    t->defs[t->def_count].text = strdup(string);
    t->defs[t->def_count].type = type;
    t->def_count++;

    return 0;
}
