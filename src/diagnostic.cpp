#include "r2d2/diagnostic.hpp"

#include <sstream>

namespace bd1 {

std::string format_diagnostic(const Diagnostic& diagnostic) {
    std::ostringstream output;
    output << "erro lexico [" << diagnostic.location.line << ':'
           << diagnostic.location.column << "] " << diagnostic.code << ": "
           << diagnostic.message;
    return output.str();
}

}  // namespace bd1
