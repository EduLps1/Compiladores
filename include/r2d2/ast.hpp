#ifndef R2D2_AST_HPP
#define R2D2_AST_HPP

#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "r2d2/token.hpp"

namespace bd1 {

enum class AstKind {
    Mission,
    Block,
    VariableDeclaration,
    Assignment,
    Update,
    Move,
    TurnLeft,
    TurnRight,
    Stop,
    If,
    While,
    Repeat,
    For,
    DoWhile,
    Switch,
    SwitchCase,
    SwitchDefault,
    Break,
    Continue,
    Return,
    IntegerLiteral,
    BooleanLiteral,
    Identifier,
    SensorCall,
    UnaryExpression,
    BinaryExpression,
    Clause,
};

struct AstNode {
    AstKind kind;
    SourceLocation location;
    std::string value;
    std::vector<std::unique_ptr<AstNode>> children;
};

using AstPtr = std::unique_ptr<AstNode>;
using AstList = std::vector<AstPtr>;

[[nodiscard]] AstPtr make_ast(AstKind kind, SourceLocation location,
                              std::string value = {});
void add_child(AstNode& parent, AstPtr child);
void add_children(AstNode& parent, AstList children);

[[nodiscard]] std::string_view ast_kind_name(AstKind kind);
[[nodiscard]] std::string ast_to_string(const AstNode& root);
void print_ast(const AstNode& root, std::ostream& output);

}  // namespace bd1

#endif
