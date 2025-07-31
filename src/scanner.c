#include <tree_sitter/parser.h>
#include <wctype.h>

enum TokenType
{
    _MACRO_HASH,
};

void *tree_sitter_sqf_external_scanner_create() { return NULL; }
void tree_sitter_sqf_external_scanner_destroy(void *p) {}
void tree_sitter_sqf_external_scanner_reset(void *p) {}
unsigned tree_sitter_sqf_external_scanner_serialize(void *p, char *b) { return 0; }
void tree_sitter_sqf_external_scanner_deserialize(void *p, const char *b, unsigned n) {}

bool tree_sitter_sqf_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols)
{
    if (valid_symbols[_MACRO_HASH])
    {
        // Skip initial whitespace which is not at a new line
        while (iswspace(lexer->lookahead) && lexer->get_column(lexer) > 0)
        {
            lexer->advance(lexer, true);
        }

        // Check if we found a new line
        if (lexer->get_column(lexer) == 0)
        {
            // Skip leading whitespace. Note: this may contain additional newlines.
            while (iswspace(lexer->lookahead))
            {
                lexer->advance(lexer, true);
            }

            // The next character is the first non-whitespace character of a new line. Check if it's
            // a hash.
            if (lexer->lookahead == '#')
            {
                lexer->advance(lexer, false);

                // Check if there is a lowercase letter directly after the hash. This is checked
                // here because whitespace or comments are not allowed after the hash and
                // token.immediate() does not work for the macro name for some reason.
                if (iswlower(lexer->lookahead))
                {
                    // Accept the token
                    lexer->result_symbol = _MACRO_HASH;
                    return true;
                }
            }
        }
    }
    return false;
}