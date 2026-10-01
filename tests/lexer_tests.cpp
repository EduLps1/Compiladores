#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "r2d2/lexer.hpp"
#include "r2d2/token.hpp"

namespace {

int failures = 0;

void check(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FALHA: " << message << '\n';
        ++failures;
    }
}

bd1::LexResult scan(std::string_view source) {
    std::istringstream input{std::string(source)};
    return bd1::Lexer{}.scan(input);
}

void expect_kinds(const bd1::LexResult& result,
                  const std::vector<bd1::TokenKind>& expected,
                  std::string_view test_name) {
    check(result.tokens.size() == expected.size(),
          std::string(test_name) + ": quantidade de tokens");

    const std::size_t count =
        result.tokens.size() < expected.size() ? result.tokens.size()
                                               : expected.size();
    for (std::size_t index = 0; index < count; ++index) {
        check(result.tokens[index].kind == expected[index],
              std::string(test_name) + ": token " + std::to_string(index));
    }
}

void test_keywords() {
    const auto result = scan(
        "mission main int bool true false if else switch case default "
        "while do for repeat break continue return move turn_left turn_right "
        "stop front_clear at_goal");

    expect_kinds(
        result,
        {bd1::TokenKind::KwMission, bd1::TokenKind::KwMain,
         bd1::TokenKind::KwInt, bd1::TokenKind::KwBool,
         bd1::TokenKind::KwTrue, bd1::TokenKind::KwFalse,
         bd1::TokenKind::KwIf, bd1::TokenKind::KwElse,
         bd1::TokenKind::KwSwitch, bd1::TokenKind::KwCase,
         bd1::TokenKind::KwDefault, bd1::TokenKind::KwWhile,
         bd1::TokenKind::KwDo, bd1::TokenKind::KwFor,
         bd1::TokenKind::KwRepeat, bd1::TokenKind::KwBreak,
         bd1::TokenKind::KwContinue, bd1::TokenKind::KwReturn,
         bd1::TokenKind::KwMove, bd1::TokenKind::KwTurnLeft,
         bd1::TokenKind::KwTurnRight, bd1::TokenKind::KwStop,
         bd1::TokenKind::KwFrontClear, bd1::TokenKind::KwAtGoal},
        "palavras reservadas");
    check(result.success(), "palavras reservadas sem diagnosticos");
}

void test_operators_and_longest_match() {
    const auto result = scan(
        "+ - * / % ++ -- = += -= *= /= %= == != < <= > >= && || ! "
        "( ) { } ; , :");

    expect_kinds(
        result,
        {bd1::TokenKind::Plus, bd1::TokenKind::Minus,
         bd1::TokenKind::Star, bd1::TokenKind::Slash,
         bd1::TokenKind::Percent, bd1::TokenKind::Increment,
         bd1::TokenKind::Decrement, bd1::TokenKind::Assign,
         bd1::TokenKind::PlusAssign, bd1::TokenKind::MinusAssign,
         bd1::TokenKind::StarAssign, bd1::TokenKind::SlashAssign,
         bd1::TokenKind::PercentAssign, bd1::TokenKind::Equal,
         bd1::TokenKind::NotEqual, bd1::TokenKind::Less,
         bd1::TokenKind::LessEqual, bd1::TokenKind::Greater,
         bd1::TokenKind::GreaterEqual, bd1::TokenKind::LogicalAnd,
         bd1::TokenKind::LogicalOr, bd1::TokenKind::LogicalNot,
         bd1::TokenKind::LeftParen, bd1::TokenKind::RightParen,
         bd1::TokenKind::LeftBrace, bd1::TokenKind::RightBrace,
         bd1::TokenKind::Semicolon, bd1::TokenKind::Comma,
         bd1::TokenKind::Colon},
        "operadores");
    check(result.success(), "operadores sem diagnosticos");
}

void test_identifiers_case_and_integer_value() {
    const auto result = scan("move Move move_robot robot_1 _count 42");
    expect_kinds(result,
                 {bd1::TokenKind::KwMove, bd1::TokenKind::Identifier,
                  bd1::TokenKind::Identifier, bd1::TokenKind::Identifier,
                  bd1::TokenKind::Identifier, bd1::TokenKind::Integer},
                 "identificadores");
    check(result.tokens.back().integer_value == std::int64_t{42},
          "valor inteiro processado");
}

void test_locations_and_comments() {
    const auto result = scan(
        "// cabecalho\n"
        "/* comentario\n"
        "   de bloco */\n"
        "  move(12);\n");

    expect_kinds(result,
                 {bd1::TokenKind::KwMove, bd1::TokenKind::LeftParen,
                  bd1::TokenKind::Integer, bd1::TokenKind::RightParen,
                  bd1::TokenKind::Semicolon},
                 "comentarios");
    check(result.tokens[0].location.line == 4 &&
              result.tokens[0].location.column == 3,
          "linha e coluna apos comentarios");
    check(result.tokens[2].location.line == 4 &&
              result.tokens[2].location.column == 8,
          "posicao do inteiro");
}

void test_multiple_invalid_characters() {
    const auto result = scan("move(1); @ # stop();");
    check(!result.success(), "entrada invalida deve falhar");
    check(result.diagnostics.size() == 2,
          "deve relatar todos os caracteres invalidos");
    check(result.diagnostics[0].code == "L001" &&
              result.diagnostics[1].code == "L001",
          "codigo de caractere invalido");
    check(result.tokens.back().kind == bd1::TokenKind::Semicolon,
          "lexer deve continuar depois dos erros");
}

void test_integer_overflow() {
    const auto result = scan("9223372036854775808");
    check(result.diagnostics.size() == 1, "overflow gera um diagnostico");
    check(result.diagnostics[0].code == "L002", "codigo de overflow");
    check(result.tokens.size() == 1 &&
              result.tokens[0].kind == bd1::TokenKind::Invalid,
          "inteiro fora do limite gera token invalido");
}

void test_unterminated_comment() {
    const auto result = scan("move(1);\n  /* comentario");
    check(result.diagnostics.size() == 1,
          "comentario aberto gera um diagnostico");
    check(result.diagnostics[0].code == "L003",
          "codigo de comentario nao terminado");
    check(result.diagnostics[0].location.line == 2 &&
              result.diagnostics[0].location.column == 3,
          "posicao inicial do comentario nao terminado");
}

}  // namespace

int main() {
    test_keywords();
    test_operators_and_longest_match();
    test_identifiers_case_and_integer_value();
    test_locations_and_comments();
    test_multiple_invalid_characters();
    test_integer_overflow();
    test_unterminated_comment();

    if (failures != 0) {
        std::cerr << failures << " teste(s) falharam\n";
        return 1;
    }

    std::cout << "todos os testes lexicos passaram\n";
    return 0;
}
