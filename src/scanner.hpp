#ifndef R2D2_SCANNER_HPP
#define R2D2_SCANNER_HPP

#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

#include <cstddef>
#include <istream>
#include <string_view>
#include <vector>

#include "r2d2/diagnostic.hpp"
#include "r2d2/token.hpp"

class Bd1Scanner final : public yyFlexLexer {
public:
    explicit Bd1Scanner(std::istream* input);

    int yylex() override;

    [[nodiscard]] const bd1::Token& current_token() const noexcept;
    [[nodiscard]] const std::vector<bd1::Diagnostic>& diagnostics() const noexcept;

private:
    void begin_lexeme(std::string_view text);
    void set_token(bd1::TokenKind kind, std::string_view lexeme);
    void set_integer(std::string_view lexeme);
    void set_unterminated_comment();
    void report(std::string code, std::string message,
                bd1::SourceLocation location);

    std::size_t line_{1};
    std::size_t column_{1};
    bd1::SourceLocation lexeme_location_{};
    bd1::SourceLocation comment_location_{};
    bd1::Token current_token_{};
    std::vector<bd1::Diagnostic> diagnostics_;
};

#endif
