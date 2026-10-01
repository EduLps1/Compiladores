#include "r2d2/lexer.hpp"

#include "scanner.hpp"

namespace bd1 {

LexResult Lexer::scan(std::istream& input) const {
    Bd1Scanner scanner(&input);
    LexResult result;

    while (scanner.yylex() != static_cast<int>(TokenKind::Eof)) {
        result.tokens.push_back(scanner.current_token());
    }

    result.diagnostics = scanner.diagnostics();
    return result;
}

}  // namespace bd1
