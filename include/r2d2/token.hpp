#ifndef R2D2_TOKEN_HPP
#define R2D2_TOKEN_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace bd1 {

struct SourceLocation {
    std::size_t line{1};
    std::size_t column{1};
};

enum class TokenKind : int {
    Eof = 0,
    Invalid = 256,

    Identifier,
    Integer,

    KwMission,
    KwMain,
    KwInt,
    KwBool,
    KwTrue,
    KwFalse,
    KwIf,
    KwElse,
    KwSwitch,
    KwCase,
    KwDefault,
    KwWhile,
    KwDo,
    KwFor,
    KwRepeat,
    KwBreak,
    KwContinue,
    KwReturn,

    KwMove,
    KwTurnLeft,
    KwTurnRight,
    KwStop,
    KwFrontClear,
    KwAtGoal,

    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Increment,
    Decrement,

    Assign,
    PlusAssign,
    MinusAssign,
    StarAssign,
    SlashAssign,
    PercentAssign,

    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    LogicalAnd,
    LogicalOr,
    LogicalNot,

    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Semicolon,
    Comma,
    Colon,
};

struct Token {
    TokenKind kind{TokenKind::Invalid};
    std::string lexeme;
    SourceLocation location;
    std::optional<std::int64_t> integer_value;
};

[[nodiscard]] std::string_view token_kind_name(TokenKind kind);

}  // namespace bd1

#endif
