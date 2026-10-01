#include "r2d2/parser.hpp"

#include <sstream>

#include "parser_driver.hpp"

namespace bd1 {

ParseResult Parser::parse(const std::vector<Token>& tokens) const {
    ParserDriver driver(tokens);
    GeneratedParser parser(driver);
    const int status = parser.parse();

    ParseResult result;
    result.diagnostics = driver.take_diagnostics();
    if (status == 0 && result.diagnostics.empty()) {
        result.ast = driver.take_ast();
    }
    return result;
}

std::string format_syntax_diagnostic(const SyntaxDiagnostic& diagnostic) {
    std::ostringstream output;
    output << "erro sintatico [" << diagnostic.location.line << ':'
           << diagnostic.location.column << "] " << diagnostic.code << ": "
           << diagnostic.message;
    return output.str();
}

}  // namespace bd1
