#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string_view>

#include "r2d2/diagnostic.hpp"
#include "r2d2/lexer.hpp"
#include "r2d2/token.hpp"

#ifndef R2D2_VERSION
#define R2D2_VERSION "dev"
#endif

namespace {

constexpr int kUsageError = 64;
constexpr int kInputError = 66;

void print_help(std::ostream& output) {
    output
        << "R2-D2 Compiler " << R2D2_VERSION << "\n"
        << "Analisador da linguagem BD-1.\n\n"
        << "Uso:\n"
        << "  r2d2c tokens <arquivo.bd1>  Executa a analise lexica\n"
        << "  r2d2c --version             Mostra a versao\n"
        << "  r2d2c --help                Mostra esta ajuda\n";
}

void print_token(const bd1::Token& token) {
    std::cout << std::left << std::setw(20)
              << bd1::token_kind_name(token.kind) << " ["
              << token.location.line << ':' << token.location.column << "] "
              << std::quoted(token.lexeme);

    if (token.integer_value.has_value()) {
        std::cout << " valor=" << *token.integer_value;
    }
    std::cout << '\n';
}

int run_tokens(const char* path) {
    std::ifstream input(path);
    if (!input) {
        std::cerr << "erro: nao foi possivel abrir '" << path << "'\n";
        return kInputError;
    }

    const bd1::Lexer lexer;
    const bd1::LexResult result = lexer.scan(input);

    for (const auto& token : result.tokens) {
        print_token(token);
    }

    for (const auto& diagnostic : result.diagnostics) {
        std::cerr << bd1::format_diagnostic(diagnostic) << '\n';
    }

    if (!result.success()) {
        std::cerr << "analise lexica: " << result.diagnostics.size()
                  << " erro(s)\n";
        return EXIT_FAILURE;
    }

    std::cout << "analise lexica: sucesso (" << result.tokens.size()
              << " token(s))\n";
    return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        print_help(std::cout);
        return EXIT_SUCCESS;
    }

    if (argc == 2 && std::string_view(argv[1]) == "--version") {
        std::cout << "r2d2c " << R2D2_VERSION << '\n';
        return EXIT_SUCCESS;
    }

    if (argc == 3 && std::string_view(argv[1]) == "tokens") {
        return run_tokens(argv[2]);
    }

    print_help(std::cerr);
    return kUsageError;
}
