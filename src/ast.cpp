#include "r2d2/ast.hpp"

#include <sstream>
#include <utility>

namespace bd1 {

AstPtr make_ast(AstKind kind, SourceLocation location, std::string value) {
    return std::make_unique<AstNode>(
        AstNode{kind, location, std::move(value), {}});
}

void add_child(AstNode& parent, AstPtr child) {
    if (child != nullptr) {
        parent.children.push_back(std::move(child));
    }
}

void add_children(AstNode& parent, AstList children) {
    for (auto& child : children) {
        add_child(parent, std::move(child));
    }
}

std::string_view ast_kind_name(AstKind kind) {
    switch (kind) {
        case AstKind::Mission: return "Mission";
        case AstKind::Block: return "Block";
        case AstKind::VariableDeclaration: return "VariableDeclaration";
        case AstKind::Assignment: return "Assignment";
        case AstKind::Update: return "Update";
        case AstKind::Move: return "Move";
        case AstKind::TurnLeft: return "TurnLeft";
        case AstKind::TurnRight: return "TurnRight";
        case AstKind::Stop: return "Stop";
        case AstKind::If: return "If";
        case AstKind::While: return "While";
        case AstKind::Repeat: return "Repeat";
        case AstKind::For: return "For";
        case AstKind::DoWhile: return "DoWhile";
        case AstKind::Switch: return "Switch";
        case AstKind::SwitchCase: return "SwitchCase";
        case AstKind::SwitchDefault: return "SwitchDefault";
        case AstKind::Break: return "Break";
        case AstKind::Continue: return "Continue";
        case AstKind::Return: return "Return";
        case AstKind::IntegerLiteral: return "Integer";
        case AstKind::BooleanLiteral: return "Boolean";
        case AstKind::Identifier: return "Identifier";
        case AstKind::SensorCall: return "SensorCall";
        case AstKind::UnaryExpression: return "UnaryExpression";
        case AstKind::BinaryExpression: return "BinaryExpression";
        case AstKind::Clause: return "Clause";
    }
    return "Unknown";
}

namespace {

std::string node_label(const AstNode& node) {
    std::string label;
    if (node.kind == AstKind::Clause) {
        label = node.value;
    } else {
        label = ast_kind_name(node.kind);
        if (!node.value.empty()) {
            label += ' ';
            label += node.value;
        }
    }

    label += " [";
    label += std::to_string(node.location.line);
    label += ':';
    label += std::to_string(node.location.column);
    label += ']';
    return label;
}

void print_node(const AstNode& node, std::ostream& output,
                const std::string& prefix, bool is_last, bool is_root) {
    if (!is_root) {
        output << prefix << (is_last ? "`-- " : "|-- ");
    }
    output << node_label(node) << '\n';

    const std::string child_prefix =
        is_root ? std::string{} : prefix + (is_last ? "    " : "|   ");

    for (std::size_t index = 0; index < node.children.size(); ++index) {
        print_node(*node.children[index], output, child_prefix,
                   index + 1 == node.children.size(), false);
    }
}

}  // namespace

std::string ast_to_string(const AstNode& root) {
    std::ostringstream output;
    print_ast(root, output);
    return output.str();
}

void print_ast(const AstNode& root, std::ostream& output) {
    print_node(root, output, {}, true, true);
}

}  // namespace bd1
