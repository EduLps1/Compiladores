#ifndef R2D2_PARSER_DRIVER_HPP
#define R2D2_PARSER_DRIVER_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "parser_generated.hpp"
#include "r2d2/ast.hpp"
#include "r2d2/parser.hpp"
#include "r2d2/token.hpp"

namespace bd1 {

class ParserDriver {
public:
    explicit ParserDriver(const std::vector<Token>& tokens);

    [[nodiscard]] GeneratedParser::symbol_type next_symbol();
    void set_ast(AstPtr ast);
    void report(const GeneratedParser::location_type& location,
                const std::string& message);

    [[nodiscard]] AstPtr take_ast();
    [[nodiscard]] std::vector<SyntaxDiagnostic> take_diagnostics();

private:
    [[nodiscard]] GeneratedParser::location_type location_for(
        const Token& token) const;
    [[nodiscard]] GeneratedParser::location_type eof_location() const;

    const std::vector<Token>& tokens_;
    std::size_t index_{0};
    AstPtr ast_;
    std::vector<SyntaxDiagnostic> diagnostics_;
};

}  // namespace bd1

#endif
