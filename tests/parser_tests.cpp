#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "r2d2/ast.hpp"
#include "r2d2/lexer.hpp"
#include "r2d2/parser.hpp"

namespace {

int failures = 0;

void check(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FALHA: " << message << '\n';
        ++failures;
    }
}

bd1::ParseResult parse(std::string_view source) {
    std::istringstream input{std::string(source)};
    const bd1::LexResult lexical = bd1::Lexer{}.scan(input);
    check(lexical.success(), "fonte do teste deve ser lexicamente valida");
    return bd1::Parser{}.parse(lexical.tokens);
}

void test_complete_control_grammar() {
    const auto result = parse(
        "mission main() {\n"
        "  int steps = 0;\n"
        "  bool arrived = false;\n"
        "  if (front_clear()) { move(1); }\n"
        "  else if (at_goal()) { stop(); }\n"
        "  else { turn_left(); }\n"
        "  while (!arrived) { steps++; continue; }\n"
        "  repeat 2 { move(1); }\n"
        "  for (int i = 0; i < 3; i++) { turn_right(); }\n"
        "  for (;;) { break; }\n"
        "  do { steps -= 1; } while (steps > 0);\n"
        "  switch (steps) {\n"
        "    case 0: stop(); break;\n"
        "    case 1: move(1);\n"
        "    default: return;\n"
        "  }\n"
        "  return steps;\n"
        "}\n");

    check(result.success(), "gramatica completa deve ser aceita");
    check(result.ast != nullptr && result.ast->kind == bd1::AstKind::Mission,
          "raiz da AST deve ser Mission");
    if (result.ast != nullptr) {
        const std::string tree = bd1::ast_to_string(*result.ast);
        check(tree.find("If [4:3]") != std::string::npos,
              "AST deve conter if");
        check(tree.find("While [7:3]") != std::string::npos,
              "AST deve conter while");
        check(tree.find("Repeat [8:3]") != std::string::npos,
              "AST deve conter repeat");
        check(tree.find("For [9:3]") != std::string::npos,
              "AST deve conter for");
        check(tree.find("DoWhile [11:3]") != std::string::npos,
              "AST deve conter do-while");
        check(tree.find("Switch [12:3]") != std::string::npos,
              "AST deve conter switch");
    }
}

void test_expression_precedence() {
    const auto result = parse(
        "mission main() {\n"
        "  bool result = 1 + 2 * 3 == 7 && true || false;\n"
        "}\n");

    check(result.success(), "expressao valida deve ser aceita");
    if (result.ast != nullptr) {
        const std::string tree = bd1::ast_to_string(*result.ast);
        const auto logical_or = tree.find("BinaryExpression ||");
        const auto logical_and = tree.find("BinaryExpression &&");
        const auto equality = tree.find("BinaryExpression ==");
        const auto addition = tree.find("BinaryExpression +");
        const auto multiplication = tree.find("BinaryExpression *");
        check(logical_or < logical_and && logical_and < equality &&
                  equality < addition && addition < multiplication,
              "AST deve respeitar precedencia dos operadores");
    }
}

void test_multiple_syntax_errors_and_no_partial_ast() {
    const auto result = parse(
        "mission main() {\n"
        "  int steps = ;\n"
        "  move(1)\n"
        "  turn_left();\n"
        "  stop()\n"
        "}\n");

    check(!result.success(), "fonte sintaticamente invalida deve falhar");
    check(result.diagnostics.size() >= 2,
          "recuperacao deve relatar mais de um erro sintatico");
    check(result.ast == nullptr, "AST parcial nao deve ser exposta");
    if (!result.diagnostics.empty()) {
        check(result.diagnostics.front().code == "S001",
              "primeiro diagnostico deve usar o codigo S001");
    }
}

}  // namespace

int main() {
    test_complete_control_grammar();
    test_expression_precedence();
    test_multiple_syntax_errors_and_no_partial_ast();

    if (failures != 0) {
        std::cerr << failures << " teste(s) falharam\n";
        return 1;
    }

    std::cout << "todos os testes sintaticos passaram\n";
    return 0;
}
