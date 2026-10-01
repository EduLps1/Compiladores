#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "r2d2/ast.hpp"
#include "r2d2/diagnostic.hpp"
#include "r2d2/lexer.hpp"
#include "r2d2/parser.hpp"
#include "r2d2/token.hpp"

#ifndef R2D2_VERSION
#define R2D2_VERSION "dev"
#endif

namespace {

constexpr int kUsageError = 64;
constexpr int kInputError = 66;

constexpr std::string_view kDemoSource =
    "mission main() {\n"
    "    int steps = 2;\n"
    "\n"
    "    repeat steps {\n"
    "        move(1);\n"
    "    }\n"
    "\n"
    "    stop();\n"
    "}\n";

constexpr std::string_view kDemoExpectedAst =
    "Mission main [1:1]\n"
    "`-- Block [1:16]\n"
    "    |-- VariableDeclaration int steps [2:5]\n"
    "    |   `-- Integer 2 [2:17]\n"
    "    |-- Repeat [4:5]\n"
    "    |   |-- Count [4:12]\n"
    "    |   |   `-- Identifier steps [4:12]\n"
    "    |   `-- Body [4:18]\n"
    "    |       `-- Block [4:18]\n"
    "    |           `-- Move [5:9]\n"
    "    |               `-- Integer 1 [5:14]\n"
    "    `-- Stop [8:5]\n";

void print_help(std::ostream& output) {
    output
        << "R2-D2 Compiler " << R2D2_VERSION << "\n"
        << "Front-end da linguagem BD-1.\n\n"
        << "Uso:\n"
        << "  r2d2c tokens <arquivo.bd1>       Executa a analise lexica\n"
        << "  r2d2c parse <arquivo.bd1>        Valida a sintaxe\n"
        << "  r2d2c ast <arquivo.bd1>          Exibe a AST\n"
        << "  r2d2c ast --trace <arquivo.bd1>  Exibe integracao e AST\n"
        << "  r2d2c demo syntax               Executa a demonstracao interna\n"
        << "  r2d2c --version                 Mostra a versao\n"
        << "  r2d2c --help                    Mostra esta ajuda\n";
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

bool open_input(const char* path, std::ifstream& input) {
    input.open(path);
    if (input) {
        return true;
    }
    std::cerr << "erro: nao foi possivel abrir '" << path << "'\n";
    return false;
}

int run_tokens(const char* path) {
    std::ifstream input;
    if (!open_input(path, input)) {
        return kInputError;
    }

    const bd1::LexResult result = bd1::Lexer{}.scan(input);

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

int report_frontend_result(const bd1::LexResult& lexical,
                           const bd1::ParseResult* syntax,
                           bool show_ast) {
    for (const auto& diagnostic : lexical.diagnostics) {
        std::cerr << bd1::format_diagnostic(diagnostic) << '\n';
    }
    if (!lexical.success()) {
        std::cerr << "analise lexica: " << lexical.diagnostics.size()
                  << " erro(s)\n"
                  << "analise sintatica: nao executada\n";
        return EXIT_FAILURE;
    }

    for (const auto& diagnostic : syntax->diagnostics) {
        std::cerr << bd1::format_syntax_diagnostic(diagnostic) << '\n';
    }
    if (!syntax->success()) {
        std::cerr << "analise sintatica: " << syntax->diagnostics.size()
                  << " erro(s)\n";
        return EXIT_FAILURE;
    }

    if (show_ast) {
        bd1::print_ast(*syntax->ast, std::cout);
    }
    std::cout << "analise sintatica: sucesso\n";
    return EXIT_SUCCESS;
}

int run_parser(const char* path, bool show_ast, bool trace) {
    std::ifstream input;
    if (!open_input(path, input)) {
        return kInputError;
    }

    if (trace) {
        std::cout << "[trace] arquivo: " << path << '\n'
                  << "[trace] lexer: iniciado\n";
    }
    const bd1::LexResult lexical = bd1::Lexer{}.scan(input);
    if (trace) {
        std::cout << "[trace] lexer: " << lexical.tokens.size()
                  << " token(s), " << lexical.diagnostics.size()
                  << " erro(s)\n";
    }

    if (!lexical.success()) {
        return report_frontend_result(lexical, nullptr, false);
    }

    if (trace) {
        std::cout << "[trace] adapter: " << lexical.tokens.size()
                  << " token(s) preparados para o Bison\n"
                  << "[trace] parser: LALR(1) iniciado\n";
    }
    const bd1::ParseResult syntax = bd1::Parser{}.parse(lexical.tokens);
    if (trace) {
        if (syntax.success()) {
            std::cout << "[trace] parser: entrada aceita\n"
                      << "[trace] ast: raiz "
                      << bd1::ast_kind_name(syntax.ast->kind) << " criada\n";
        } else {
            std::cout << "[trace] parser: entrada rejeitada com "
                      << syntax.diagnostics.size() << " erro(s)\n";
        }
    }

    return report_frontend_result(lexical, &syntax, show_ast);
}

int run_syntax_demo() {
    std::cout << "=== fonte BD-1 embutida ===\n" << kDemoSource
              << "=== integracao lexer-parser ===\n"
              << "[trace] origem: exemplo interno do executavel\n"
              << "[trace] lexer: iniciado\n";

    std::istringstream input{std::string(kDemoSource)};
    const bd1::LexResult lexical = bd1::Lexer{}.scan(input);
    std::cout << "[trace] lexer: " << lexical.tokens.size()
              << " token(s), " << lexical.diagnostics.size() << " erro(s)\n";
    if (!lexical.success()) {
        return report_frontend_result(lexical, nullptr, false);
    }

    std::cout << "[trace] adapter: " << lexical.tokens.size()
              << " token(s) preparados para o Bison\n"
              << "[trace] parser: LALR(1) iniciado\n";
    const bd1::ParseResult syntax = bd1::Parser{}.parse(lexical.tokens);
    if (!syntax.success()) {
        std::cout << "[trace] parser: entrada rejeitada\n";
        return report_frontend_result(lexical, &syntax, false);
    }

    const std::string actual_ast = bd1::ast_to_string(*syntax.ast);
    const bool matches = actual_ast == kDemoExpectedAst;
    std::cout << "[trace] parser: entrada aceita\n"
              << "[trace] ast: raiz " << bd1::ast_kind_name(syntax.ast->kind)
              << " criada\n"
              << "=== AST produzida ===\n"
              << actual_ast
              << "=== validacao automatica ===\n"
              << "esperado == obtido: " << (matches ? "PASS" : "FAIL")
              << '\n';
    return matches ? EXIT_SUCCESS : EXIT_FAILURE;
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
    if (argc == 3 && std::string_view(argv[1]) == "parse") {
        return run_parser(argv[2], false, false);
    }
    if (argc == 3 && std::string_view(argv[1]) == "ast") {
        return run_parser(argv[2], true, false);
    }
    if (argc == 4 && std::string_view(argv[1]) == "ast" &&
        std::string_view(argv[2]) == "--trace") {
        return run_parser(argv[3], true, true);
    }
    if (argc == 3 && std::string_view(argv[1]) == "demo" &&
        std::string_view(argv[2]) == "syntax") {
        return run_syntax_demo();
    }

    print_help(std::cerr);
    return kUsageError;
}
