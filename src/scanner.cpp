#include "scanner.hpp"

#include <charconv>
#include <system_error>
#include <utility>

Bd1Scanner::Bd1Scanner(std::istream* input) : yyFlexLexer(input) {}

const bd1::Token& Bd1Scanner::current_token() const noexcept {
    return current_token_;
}

const std::vector<bd1::Diagnostic>& Bd1Scanner::diagnostics() const noexcept {
    return diagnostics_;
}

void Bd1Scanner::begin_lexeme(std::string_view text) {
    lexeme_location_ = {line_, column_};

    for (const char character : text) {
        if (character == '\n') {
            ++line_;
            column_ = 1;
        } else {
            ++column_;
        }
    }
}

void Bd1Scanner::set_token(bd1::TokenKind kind, std::string_view lexeme) {
    current_token_ = {kind, std::string(lexeme), lexeme_location_, std::nullopt};
}

void Bd1Scanner::set_integer(std::string_view lexeme) {
    std::int64_t value{};
    const auto result = std::from_chars(lexeme.data(),
                                        lexeme.data() + lexeme.size(), value);

    if (result.ec == std::errc::result_out_of_range) {
        set_token(bd1::TokenKind::Invalid, lexeme);
        report("L002", "inteiro fora do intervalo de 64 bits",
               lexeme_location_);
        return;
    }

    set_token(bd1::TokenKind::Integer, lexeme);
    current_token_.integer_value = value;
}

void Bd1Scanner::set_unterminated_comment() {
    current_token_ = {bd1::TokenKind::Invalid, "/*", comment_location_,
                      std::nullopt};
    report("L003", "comentario de bloco nao terminado", comment_location_);
}

void Bd1Scanner::report(std::string code, std::string message,
                        bd1::SourceLocation location) {
    diagnostics_.push_back(
        {std::move(code), std::move(message), location});
}
