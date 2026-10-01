#ifndef R2D2_PARSER_HPP
#define R2D2_PARSER_HPP

#include <string>
#include <vector>

#include "r2d2/ast.hpp"
#include "r2d2/token.hpp"

namespace bd1 {

struct SyntaxDiagnostic {
    std::string code;
    std::string message;
    SourceLocation location;
};

struct ParseResult {
    AstPtr ast;
    std::vector<SyntaxDiagnostic> diagnostics;

    [[nodiscard]] bool success() const noexcept {
        return ast != nullptr && diagnostics.empty();
    }
};

class Parser {
public:
    [[nodiscard]] ParseResult parse(const std::vector<Token>& tokens) const;
};

[[nodiscard]] std::string format_syntax_diagnostic(
    const SyntaxDiagnostic& diagnostic);

}  // namespace bd1

#endif
