#include <tree_sitter/parser.h>
#include <wctype.h>

enum TokenType
{
    _MACRO_HASH,
    _MACRO_WHITESPACE,
};

void *tree_sitter_sqf_external_scanner_create() { return NULL; }
void tree_sitter_sqf_external_scanner_destroy(void *p) {}
void tree_sitter_sqf_external_scanner_reset(void *p) {}
unsigned tree_sitter_sqf_external_scanner_serialize(void *p, char *b) { return 0; }
void tree_sitter_sqf_external_scanner_deserialize(void *p, const char *b, unsigned n) {}

bool tree_sitter_sqf_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols)
{

    if (valid_symbols[_MACRO_WHITESPACE])
    {
        // _MACRO_WHITESPACE is a special kind of whitespace used in macros where all newlines must
        // be escaped and there must not be comments. It must also contain at least one whitespace
        // character that is not an escaped newline.
        lexer->mark_end(lexer);
        bool at_least_one_char = false;
        while (true)
        {
            // If we encounter a newline we must not accept this as macro whitespace at all
            if (lexer->lookahead == '\n')
            {
                break;
            }
            // If we encounter a whitespace character other than a newline, continue scanning. Also
            // mark that at least one such character exists.
            else if (iswspace(lexer->lookahead))
            {
                at_least_one_char = true;
                lexer->advance(lexer, true);
                lexer->mark_end(lexer);
            }
            // If we encounter a slash, check if it begins a comment. If it does, we must not accept
            // this macro whitespace at all. If it does not, the macro whitespace has ended.
            else if (lexer->lookahead == '/')
            {
                lexer->advance(lexer, true);
                if (lexer->lookahead == '*' || lexer->lookahead == '/')
                {
                    break;
                }
                else
                {
                    if (at_least_one_char)
                    {
                        lexer->result_symbol = _MACRO_WHITESPACE;
                        return true;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            // If we encounter a backslash, check if it begins a newline escape sequence. If it
            // does, continue scanning. If it does not, the macro whitespace has ended.
            else if (lexer->lookahead == '\\')
            {
                lexer->advance(lexer, true);
                if (lexer->lookahead == '\r')
                {
                    lexer->advance(lexer, true);
                }
                if (lexer->lookahead == '\n')
                {
                    lexer->advance(lexer, true);
                    lexer->mark_end(lexer);
                }
                else
                {
                    if (at_least_one_char)
                    {
                        lexer->result_symbol = _MACRO_WHITESPACE;
                        return true;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            // If we encounter a non-whitespace character other than a slash or a backslash, the
            // macro whitespace has ended.
            else
            {
                if (at_least_one_char)
                {
                    lexer->result_symbol = _MACRO_WHITESPACE;
                    return true;
                }
                else
                {
                    break;
                }
            }
        }
    }
    // Only check for _MACRO_HASH if the position was not valid for _MACRO_WHITESPACE, because macro
    // hashes are never legal for the positions where macro whitespace is expected.
    else if (valid_symbols[_MACRO_HASH])
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