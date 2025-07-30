#include <tree_sitter/parser.h>
#include <wctype.h>

enum TokenType
{
    _MACRO_LINE_BEGIN,
};

void *tree_sitter_sqf_external_scanner_create() { return NULL; }
void tree_sitter_sqf_external_scanner_destroy(void *p) {}
void tree_sitter_sqf_external_scanner_reset(void *p) {}
unsigned tree_sitter_sqf_external_scanner_serialize(void *p, char *b) { return 0; }
void tree_sitter_sqf_external_scanner_deserialize(void *p, const char *b, unsigned n) {}

bool tree_sitter_sqf_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols)
{
    if (valid_symbols[_MACRO_LINE_BEGIN])
    {
        // _MACRO_LINE_BEGIN should match an empty string at the beginning of a line with a macro
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
                // Accept the token
                lexer->result_symbol = _MACRO_LINE_BEGIN;
                return true;
            }
        }
    }
    return false;
}