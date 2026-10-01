#ifndef R2D2_DIAGNOSTIC_HPP
#define R2D2_DIAGNOSTIC_HPP

#include <string>

#include "r2d2/token.hpp"

namespace bd1 {

struct Diagnostic {
    std::string code;
    std::string message;
    SourceLocation location;
};

[[nodiscard]] std::string format_diagnostic(const Diagnostic& diagnostic);

}  // namespace bd1

#endif
