#include "parser_driver.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <utility>

namespace bd1 {

namespace {

std::string translated_message(std::string message) {
    constexpr std::string_view prefix = "syntax error, ";
    if (message.starts_with(prefix)) {
        message.erase(0, prefix.size());
    }

    const auto replace_once = [&message](std::string_view from,
                                         std::string_view to) {
        const std::size_t position = message.find(from);
        if (position != std::string::npos) {
            message.replace(position, from.size(), to);
        }
    };

    replace_once("unexpected ", "encontrado ");
    replace_once(", expecting ", "; esperado ");
    replace_once(" or ", " ou ");
    return message;
}

}  // namespace

ParserDriver::ParserDriver(const std::vector<Token>& tokens) : tokens_(tokens) {}

GeneratedParser::location_type ParserDriver::location_for(
    const Token& token) const {
    GeneratedParser::location_type location;
    location.begin.line = static_cast<int>(token.location.line);
    location.begin.column = static_cast<int>(token.location.column);
    location.end = location.begin;
    location.end.column +=
        static_cast<int>(std::max<std::size_t>(token.lexeme.size(), 1));
    return location;
}

GeneratedParser::location_type ParserDriver::eof_location() const {
    GeneratedParser::location_type location;
    if (!tokens_.empty()) {
        const Token& last = tokens_.back();
        location.begin.line = static_cast<int>(last.location.line);
        location.begin.column = static_cast<int>(
            last.location.column + std::max<std::size_t>(last.lexeme.size(), 1));
    }
    location.end = location.begin;
    return location;
}

GeneratedParser::symbol_type ParserDriver::next_symbol() {
    if (index_ >= tokens_.size()) {
        return GeneratedParser::make_YYEOF(eof_location());
    }

    const Token& token = tokens_[index_++];
    const auto location = location_for(token);

    switch (token.kind) {
        case TokenKind::Identifier:
            return GeneratedParser::make_IDENTIFIER(token.lexeme, location);
        case TokenKind::Integer:
            return GeneratedParser::make_INTEGER(token.integer_value.value_or(0),
                                                 location);
        case TokenKind::KwMission: return GeneratedParser::make_KW_MISSION(location);
        case TokenKind::KwMain: return GeneratedParser::make_KW_MAIN(location);
        case TokenKind::KwInt: return GeneratedParser::make_KW_INT(location);
        case TokenKind::KwBool: return GeneratedParser::make_KW_BOOL(location);
        case TokenKind::KwTrue: return GeneratedParser::make_KW_TRUE(location);
        case TokenKind::KwFalse: return GeneratedParser::make_KW_FALSE(location);
        case TokenKind::KwIf: return GeneratedParser::make_KW_IF(location);
        case TokenKind::KwElse: return GeneratedParser::make_KW_ELSE(location);
        case TokenKind::KwSwitch: return GeneratedParser::make_KW_SWITCH(location);
        case TokenKind::KwCase: return GeneratedParser::make_KW_CASE(location);
        case TokenKind::KwDefault: return GeneratedParser::make_KW_DEFAULT(location);
        case TokenKind::KwWhile: return GeneratedParser::make_KW_WHILE(location);
        case TokenKind::KwDo: return GeneratedParser::make_KW_DO(location);
        case TokenKind::KwFor: return GeneratedParser::make_KW_FOR(location);
        case TokenKind::KwRepeat: return GeneratedParser::make_KW_REPEAT(location);
        case TokenKind::KwBreak: return GeneratedParser::make_KW_BREAK(location);
        case TokenKind::KwContinue: return GeneratedParser::make_KW_CONTINUE(location);
        case TokenKind::KwReturn: return GeneratedParser::make_KW_RETURN(location);
        case TokenKind::KwMove: return GeneratedParser::make_KW_MOVE(location);
        case TokenKind::KwTurnLeft: return GeneratedParser::make_KW_TURN_LEFT(location);
        case TokenKind::KwTurnRight: return GeneratedParser::make_KW_TURN_RIGHT(location);
        case TokenKind::KwStop: return GeneratedParser::make_KW_STOP(location);
        case TokenKind::KwFrontClear: return GeneratedParser::make_KW_FRONT_CLEAR(location);
        case TokenKind::KwAtGoal: return GeneratedParser::make_KW_AT_GOAL(location);
        case TokenKind::Plus: return GeneratedParser::make_PLUS(location);
        case TokenKind::Minus: return GeneratedParser::make_MINUS(location);
        case TokenKind::Star: return GeneratedParser::make_STAR(location);
        case TokenKind::Slash: return GeneratedParser::make_SLASH(location);
        case TokenKind::Percent: return GeneratedParser::make_PERCENT(location);
        case TokenKind::Increment: return GeneratedParser::make_INCREMENT(location);
        case TokenKind::Decrement: return GeneratedParser::make_DECREMENT(location);
        case TokenKind::Assign: return GeneratedParser::make_ASSIGN(location);
        case TokenKind::PlusAssign: return GeneratedParser::make_PLUS_ASSIGN(location);
        case TokenKind::MinusAssign: return GeneratedParser::make_MINUS_ASSIGN(location);
        case TokenKind::StarAssign: return GeneratedParser::make_STAR_ASSIGN(location);
        case TokenKind::SlashAssign: return GeneratedParser::make_SLASH_ASSIGN(location);
        case TokenKind::PercentAssign: return GeneratedParser::make_PERCENT_ASSIGN(location);
        case TokenKind::Equal: return GeneratedParser::make_EQUAL(location);
        case TokenKind::NotEqual: return GeneratedParser::make_NOT_EQUAL(location);
        case TokenKind::Less: return GeneratedParser::make_LESS(location);
        case TokenKind::LessEqual: return GeneratedParser::make_LESS_EQUAL(location);
        case TokenKind::Greater: return GeneratedParser::make_GREATER(location);
        case TokenKind::GreaterEqual: return GeneratedParser::make_GREATER_EQUAL(location);
        case TokenKind::LogicalAnd: return GeneratedParser::make_LOGICAL_AND(location);
        case TokenKind::LogicalOr: return GeneratedParser::make_LOGICAL_OR(location);
        case TokenKind::LogicalNot: return GeneratedParser::make_LOGICAL_NOT(location);
        case TokenKind::LeftParen: return GeneratedParser::make_LEFT_PAREN(location);
        case TokenKind::RightParen: return GeneratedParser::make_RIGHT_PAREN(location);
        case TokenKind::LeftBrace: return GeneratedParser::make_LEFT_BRACE(location);
        case TokenKind::RightBrace: return GeneratedParser::make_RIGHT_BRACE(location);
        case TokenKind::Semicolon: return GeneratedParser::make_SEMICOLON(location);
        case TokenKind::Comma: return GeneratedParser::make_COMMA(location);
        case TokenKind::Colon: return GeneratedParser::make_COLON(location);
        case TokenKind::Invalid: return GeneratedParser::make_YYUNDEF(location);
        case TokenKind::Eof: return GeneratedParser::make_YYEOF(location);
    }
    return GeneratedParser::make_YYUNDEF(location);
}

void ParserDriver::set_ast(AstPtr ast) {
    ast_ = std::move(ast);
}

void ParserDriver::report(const GeneratedParser::location_type& location,
                          const std::string& message) {
    std::ostringstream code;
    code << 'S' << std::setfill('0') << std::setw(3)
         << diagnostics_.size() + 1;

    diagnostics_.push_back(
        {code.str(), translated_message(message),
         {static_cast<std::size_t>(location.begin.line),
          static_cast<std::size_t>(location.begin.column)}});
}

AstPtr ParserDriver::take_ast() {
    return std::move(ast_);
}

std::vector<SyntaxDiagnostic> ParserDriver::take_diagnostics() {
    return std::move(diagnostics_);
}

}  // namespace bd1
