#ifndef R2D2_LEXER_HPP
#define R2D2_LEXER_HPP

#include <istream>
#include <vector>

#include "r2d2/diagnostic.hpp"
#include "r2d2/token.hpp"

namespace bd1 {

struct LexResult {
    std::vector<Token> tokens;
    std::vector<Diagnostic> diagnostics;

    [[nodiscard]] bool success() const noexcept {
        return diagnostics.empty();
    }
};

class Lexer {
public:
    [[nodiscard]] LexResult scan(std::istream& input) const;
};

}  // namespace bd1

#endif
