#include "r2d2/token.hpp"

namespace bd1 {

std::string_view token_kind_name(TokenKind kind) {
    switch (kind) {
        case TokenKind::Eof: return "EOF";
        case TokenKind::Invalid: return "INVALID";
        case TokenKind::Identifier: return "IDENTIFIER";
        case TokenKind::Integer: return "INTEGER";
        case TokenKind::KwMission: return "KW_MISSION";
        case TokenKind::KwMain: return "KW_MAIN";
        case TokenKind::KwInt: return "KW_INT";
        case TokenKind::KwBool: return "KW_BOOL";
        case TokenKind::KwTrue: return "KW_TRUE";
        case TokenKind::KwFalse: return "KW_FALSE";
        case TokenKind::KwIf: return "KW_IF";
        case TokenKind::KwElse: return "KW_ELSE";
        case TokenKind::KwSwitch: return "KW_SWITCH";
        case TokenKind::KwCase: return "KW_CASE";
        case TokenKind::KwDefault: return "KW_DEFAULT";
        case TokenKind::KwWhile: return "KW_WHILE";
        case TokenKind::KwDo: return "KW_DO";
        case TokenKind::KwFor: return "KW_FOR";
        case TokenKind::KwRepeat: return "KW_REPEAT";
        case TokenKind::KwBreak: return "KW_BREAK";
        case TokenKind::KwContinue: return "KW_CONTINUE";
        case TokenKind::KwReturn: return "KW_RETURN";
        case TokenKind::KwMove: return "KW_MOVE";
        case TokenKind::KwTurnLeft: return "KW_TURN_LEFT";
        case TokenKind::KwTurnRight: return "KW_TURN_RIGHT";
        case TokenKind::KwStop: return "KW_STOP";
        case TokenKind::KwFrontClear: return "KW_FRONT_CLEAR";
        case TokenKind::KwAtGoal: return "KW_AT_GOAL";
        case TokenKind::Plus: return "PLUS";
        case TokenKind::Minus: return "MINUS";
        case TokenKind::Star: return "STAR";
        case TokenKind::Slash: return "SLASH";
        case TokenKind::Percent: return "PERCENT";
        case TokenKind::Increment: return "INCREMENT";
        case TokenKind::Decrement: return "DECREMENT";
        case TokenKind::Assign: return "ASSIGN";
        case TokenKind::PlusAssign: return "PLUS_ASSIGN";
        case TokenKind::MinusAssign: return "MINUS_ASSIGN";
        case TokenKind::StarAssign: return "STAR_ASSIGN";
        case TokenKind::SlashAssign: return "SLASH_ASSIGN";
        case TokenKind::PercentAssign: return "PERCENT_ASSIGN";
        case TokenKind::Equal: return "EQUAL";
        case TokenKind::NotEqual: return "NOT_EQUAL";
        case TokenKind::Less: return "LESS";
        case TokenKind::LessEqual: return "LESS_EQUAL";
        case TokenKind::Greater: return "GREATER";
        case TokenKind::GreaterEqual: return "GREATER_EQUAL";
        case TokenKind::LogicalAnd: return "LOGICAL_AND";
        case TokenKind::LogicalOr: return "LOGICAL_OR";
        case TokenKind::LogicalNot: return "LOGICAL_NOT";
        case TokenKind::LeftParen: return "LEFT_PAREN";
        case TokenKind::RightParen: return "RIGHT_PAREN";
        case TokenKind::LeftBrace: return "LEFT_BRACE";
        case TokenKind::RightBrace: return "RIGHT_BRACE";
        case TokenKind::Semicolon: return "SEMICOLON";
        case TokenKind::Comma: return "COMMA";
        case TokenKind::Colon: return "COLON";
    }
    return "UNKNOWN";
}

}  // namespace bd1
