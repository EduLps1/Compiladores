%skeleton "lalr1.cc"
%require "3.8"
%define api.namespace {bd1}
%define api.parser.class {GeneratedParser}
%define api.value.type variant
%define api.token.constructor
%define parse.error detailed
%define parse.lac full
%locations

%parse-param { bd1::ParserDriver& driver }
%lex-param { bd1::ParserDriver& driver }

%code requires {
    #include <cstdint>
    #include <string>

    #include "r2d2/ast.hpp"

    namespace bd1 {
    class ParserDriver;
    }
}

%code {
    #include "parser_driver.hpp"

    #include <utility>

    static bd1::GeneratedParser::symbol_type yylex(bd1::ParserDriver& driver) {
        return driver.next_symbol();
    }

    static bd1::SourceLocation source_location(
        const bd1::GeneratedParser::location_type& location) {
        return {static_cast<std::size_t>(location.begin.line),
                static_cast<std::size_t>(location.begin.column)};
    }

    static bd1::AstPtr make_clause(
        std::string name, bd1::AstPtr child,
        const bd1::GeneratedParser::location_type& location) {
        auto clause = bd1::make_ast(bd1::AstKind::Clause,
                                    source_location(location), std::move(name));
        bd1::add_child(*clause, std::move(child));
        return clause;
    }

    static bd1::AstPtr make_binary(
        std::string operation, bd1::AstPtr left, bd1::AstPtr right,
        const bd1::GeneratedParser::location_type& location) {
        auto node = bd1::make_ast(bd1::AstKind::BinaryExpression,
                                  source_location(location),
                                  std::move(operation));
        bd1::add_child(*node, std::move(left));
        bd1::add_child(*node, std::move(right));
        return node;
    }
}

%token KW_MISSION "mission"
%token KW_MAIN "main"
%token KW_INT "int"
%token KW_BOOL "bool"
%token KW_TRUE "true"
%token KW_FALSE "false"
%token KW_IF "if"
%token KW_ELSE "else"
%token KW_SWITCH "switch"
%token KW_CASE "case"
%token KW_DEFAULT "default"
%token KW_WHILE "while"
%token KW_DO "do"
%token KW_FOR "for"
%token KW_REPEAT "repeat"
%token KW_BREAK "break"
%token KW_CONTINUE "continue"
%token KW_RETURN "return"
%token KW_MOVE "move"
%token KW_TURN_LEFT "turn_left"
%token KW_TURN_RIGHT "turn_right"
%token KW_STOP "stop"
%token KW_FRONT_CLEAR "front_clear"
%token KW_AT_GOAL "at_goal"

%token <std::string> IDENTIFIER "identifier"
%token <std::int64_t> INTEGER "integer"

%token PLUS "+"
%token MINUS "-"
%token STAR "*"
%token SLASH "/"
%token PERCENT "%"
%token INCREMENT "++"
%token DECREMENT "--"
%token ASSIGN "="
%token PLUS_ASSIGN "+="
%token MINUS_ASSIGN "-="
%token STAR_ASSIGN "*="
%token SLASH_ASSIGN "/="
%token PERCENT_ASSIGN "%="
%token EQUAL "=="
%token NOT_EQUAL "!="
%token LESS "<"
%token LESS_EQUAL "<="
%token GREATER ">"
%token GREATER_EQUAL ">="
%token LOGICAL_AND "&&"
%token LOGICAL_OR "||"
%token LOGICAL_NOT "!"
%token LEFT_PAREN "("
%token RIGHT_PAREN ")"
%token LEFT_BRACE "{"
%token RIGHT_BRACE "}"
%token SEMICOLON ";"
%token COMMA ","
%token COLON ":"

%type <bd1::AstPtr> mission block statement declaration declaration_core
%type <bd1::AstPtr> assignment assignment_core update update_core
%type <bd1::AstPtr> robot_command if_statement optional_else
%type <bd1::AstPtr> while_statement repeat_statement for_statement
%type <bd1::AstPtr> for_initializer_optional expression_optional
%type <bd1::AstPtr> for_update_optional do_while_statement
%type <bd1::AstPtr> switch_statement switch_section literal
%type <bd1::AstPtr> break_statement continue_statement return_statement
%type <bd1::AstPtr> optional_return_expression optional_initializer
%type <bd1::AstPtr> expression logical_or logical_and equality comparison
%type <bd1::AstPtr> additive multiplicative unary primary
%type <bd1::AstList> statement_list switch_sections
%type <std::string> type assignment_operator update_operator

%start program
%expect 0

%%

program
    : mission YYEOF
      {
          driver.set_ast(std::move($1));
      }
    ;

mission
    : KW_MISSION KW_MAIN LEFT_PAREN RIGHT_PAREN block
      {
          $$ = bd1::make_ast(bd1::AstKind::Mission, source_location(@1), "main");
          bd1::add_child(*$$, std::move($5));
      }
    ;

block
    : LEFT_BRACE statement_list RIGHT_BRACE
      {
          $$ = bd1::make_ast(bd1::AstKind::Block, source_location(@1));
          bd1::add_children(*$$, std::move($2));
      }
    ;

statement_list
    : %empty
      {
          $$ = bd1::AstList{};
      }
    | statement_list statement
      {
          $$ = std::move($1);
          if ($2 != nullptr) {
              $$.push_back(std::move($2));
          }
      }
    ;

statement
    : declaration        { $$ = std::move($1); }
    | assignment         { $$ = std::move($1); }
    | update             { $$ = std::move($1); }
    | robot_command      { $$ = std::move($1); }
    | if_statement       { $$ = std::move($1); }
    | while_statement    { $$ = std::move($1); }
    | repeat_statement   { $$ = std::move($1); }
    | for_statement      { $$ = std::move($1); }
    | do_while_statement { $$ = std::move($1); }
    | switch_statement   { $$ = std::move($1); }
    | break_statement    { $$ = std::move($1); }
    | continue_statement { $$ = std::move($1); }
    | return_statement   { $$ = std::move($1); }
    | block              { $$ = std::move($1); }
    | error SEMICOLON
      {
          yyerrok;
          $$ = nullptr;
      }
    ;

declaration
    : declaration_core SEMICOLON { $$ = std::move($1); }
    ;

declaration_core
    : type IDENTIFIER optional_initializer
      {
          $$ = bd1::make_ast(bd1::AstKind::VariableDeclaration,
                             source_location(@1), $1 + " " + $2);
          bd1::add_child(*$$, std::move($3));
      }
    ;

type
    : KW_INT  { $$ = std::string{"int"}; }
    | KW_BOOL { $$ = std::string{"bool"}; }
    ;

optional_initializer
    : %empty            { $$ = nullptr; }
    | ASSIGN expression { $$ = std::move($2); }
    ;

assignment
    : assignment_core SEMICOLON { $$ = std::move($1); }
    ;

assignment_core
    : IDENTIFIER assignment_operator expression
      {
          $$ = bd1::make_ast(bd1::AstKind::Assignment, source_location(@1),
                             $1 + " " + $2);
          bd1::add_child(*$$, std::move($3));
      }
    ;

assignment_operator
    : ASSIGN         { $$ = std::string{"="}; }
    | PLUS_ASSIGN    { $$ = std::string{"+="}; }
    | MINUS_ASSIGN   { $$ = std::string{"-="}; }
    | STAR_ASSIGN    { $$ = std::string{"*="}; }
    | SLASH_ASSIGN   { $$ = std::string{"/="}; }
    | PERCENT_ASSIGN { $$ = std::string{"%="}; }
    ;

update
    : update_core SEMICOLON { $$ = std::move($1); }
    ;

update_core
    : IDENTIFIER update_operator
      {
          $$ = bd1::make_ast(bd1::AstKind::Update, source_location(@1),
                             $1 + $2);
      }
    ;

update_operator
    : INCREMENT { $$ = std::string{"++"}; }
    | DECREMENT { $$ = std::string{"--"}; }
    ;

robot_command
    : KW_MOVE LEFT_PAREN expression RIGHT_PAREN SEMICOLON
      {
          $$ = bd1::make_ast(bd1::AstKind::Move, source_location(@1));
          bd1::add_child(*$$, std::move($3));
      }
    | KW_TURN_LEFT LEFT_PAREN RIGHT_PAREN SEMICOLON
      { $$ = bd1::make_ast(bd1::AstKind::TurnLeft, source_location(@1)); }
    | KW_TURN_RIGHT LEFT_PAREN RIGHT_PAREN SEMICOLON
      { $$ = bd1::make_ast(bd1::AstKind::TurnRight, source_location(@1)); }
    | KW_STOP LEFT_PAREN RIGHT_PAREN SEMICOLON
      { $$ = bd1::make_ast(bd1::AstKind::Stop, source_location(@1)); }
    ;

if_statement
    : KW_IF LEFT_PAREN expression RIGHT_PAREN block optional_else
      {
          $$ = bd1::make_ast(bd1::AstKind::If, source_location(@1));
          bd1::add_child(*$$, make_clause("Condition", std::move($3), @3));
          bd1::add_child(*$$, make_clause("Then", std::move($5), @5));
          bd1::add_child(*$$, std::move($6));
      }
    ;

optional_else
    : %empty
      { $$ = nullptr; }
    | KW_ELSE block
      { $$ = make_clause("Else", std::move($2), @1); }
    | KW_ELSE if_statement
      { $$ = make_clause("Else", std::move($2), @1); }
    ;

while_statement
    : KW_WHILE LEFT_PAREN expression RIGHT_PAREN block
      {
          $$ = bd1::make_ast(bd1::AstKind::While, source_location(@1));
          bd1::add_child(*$$, make_clause("Condition", std::move($3), @3));
          bd1::add_child(*$$, make_clause("Body", std::move($5), @5));
      }
    ;

repeat_statement
    : KW_REPEAT expression block
      {
          $$ = bd1::make_ast(bd1::AstKind::Repeat, source_location(@1));
          bd1::add_child(*$$, make_clause("Count", std::move($2), @2));
          bd1::add_child(*$$, make_clause("Body", std::move($3), @3));
      }
    ;

for_statement
    : KW_FOR LEFT_PAREN for_initializer_optional SEMICOLON
      expression_optional SEMICOLON for_update_optional RIGHT_PAREN block
      {
          $$ = bd1::make_ast(bd1::AstKind::For, source_location(@1));
          bd1::add_child(*$$, make_clause("Initializer", std::move($3), @2));
          bd1::add_child(*$$, make_clause("Condition", std::move($5), @5));
          bd1::add_child(*$$, make_clause("Update", std::move($7), @7));
          bd1::add_child(*$$, make_clause("Body", std::move($9), @9));
      }
    ;

for_initializer_optional
    : %empty          { $$ = nullptr; }
    | declaration_core { $$ = std::move($1); }
    | assignment_core  { $$ = std::move($1); }
    | update_core      { $$ = std::move($1); }
    ;

expression_optional
    : %empty     { $$ = nullptr; }
    | expression { $$ = std::move($1); }
    ;

for_update_optional
    : %empty         { $$ = nullptr; }
    | assignment_core { $$ = std::move($1); }
    | update_core     { $$ = std::move($1); }
    ;

do_while_statement
    : KW_DO block KW_WHILE LEFT_PAREN expression RIGHT_PAREN SEMICOLON
      {
          $$ = bd1::make_ast(bd1::AstKind::DoWhile, source_location(@1));
          bd1::add_child(*$$, make_clause("Body", std::move($2), @2));
          bd1::add_child(*$$, make_clause("Condition", std::move($5), @5));
      }
    ;

switch_statement
    : KW_SWITCH LEFT_PAREN expression RIGHT_PAREN LEFT_BRACE
      switch_sections RIGHT_BRACE
      {
          $$ = bd1::make_ast(bd1::AstKind::Switch, source_location(@1));
          bd1::add_child(*$$, make_clause("Expression", std::move($3), @3));
          bd1::add_children(*$$, std::move($6));
      }
    ;

switch_sections
    : %empty
      { $$ = bd1::AstList{}; }
    | switch_sections switch_section
      {
          $$ = std::move($1);
          $$.push_back(std::move($2));
      }
    ;

switch_section
    : KW_CASE literal COLON statement_list
      {
          $$ = bd1::make_ast(bd1::AstKind::SwitchCase, source_location(@1));
          bd1::add_child(*$$, make_clause("Label", std::move($2), @2));
          bd1::add_children(*$$, std::move($4));
      }
    | KW_DEFAULT COLON statement_list
      {
          $$ = bd1::make_ast(bd1::AstKind::SwitchDefault,
                             source_location(@1));
          bd1::add_children(*$$, std::move($3));
      }
    ;

literal
    : INTEGER
      { $$ = bd1::make_ast(bd1::AstKind::IntegerLiteral,
                           source_location(@1), std::to_string($1)); }
    | KW_TRUE
      { $$ = bd1::make_ast(bd1::AstKind::BooleanLiteral,
                           source_location(@1), "true"); }
    | KW_FALSE
      { $$ = bd1::make_ast(bd1::AstKind::BooleanLiteral,
                           source_location(@1), "false"); }
    ;

break_statement
    : KW_BREAK SEMICOLON
      { $$ = bd1::make_ast(bd1::AstKind::Break, source_location(@1)); }
    ;

continue_statement
    : KW_CONTINUE SEMICOLON
      { $$ = bd1::make_ast(bd1::AstKind::Continue, source_location(@1)); }
    ;

return_statement
    : KW_RETURN optional_return_expression SEMICOLON
      {
          $$ = bd1::make_ast(bd1::AstKind::Return, source_location(@1));
          bd1::add_child(*$$, std::move($2));
      }
    ;

optional_return_expression
    : %empty     { $$ = nullptr; }
    | expression { $$ = std::move($1); }
    ;

expression
    : logical_or { $$ = std::move($1); }
    ;

logical_or
    : logical_and { $$ = std::move($1); }
    | logical_or LOGICAL_OR logical_and
      { $$ = make_binary("||", std::move($1), std::move($3), @2); }
    ;

logical_and
    : equality { $$ = std::move($1); }
    | logical_and LOGICAL_AND equality
      { $$ = make_binary("&&", std::move($1), std::move($3), @2); }
    ;

equality
    : comparison { $$ = std::move($1); }
    | equality EQUAL comparison
      { $$ = make_binary("==", std::move($1), std::move($3), @2); }
    | equality NOT_EQUAL comparison
      { $$ = make_binary("!=", std::move($1), std::move($3), @2); }
    ;

comparison
    : additive { $$ = std::move($1); }
    | comparison LESS additive
      { $$ = make_binary("<", std::move($1), std::move($3), @2); }
    | comparison LESS_EQUAL additive
      { $$ = make_binary("<=", std::move($1), std::move($3), @2); }
    | comparison GREATER additive
      { $$ = make_binary(">", std::move($1), std::move($3), @2); }
    | comparison GREATER_EQUAL additive
      { $$ = make_binary(">=", std::move($1), std::move($3), @2); }
    ;

additive
    : multiplicative { $$ = std::move($1); }
    | additive PLUS multiplicative
      { $$ = make_binary("+", std::move($1), std::move($3), @2); }
    | additive MINUS multiplicative
      { $$ = make_binary("-", std::move($1), std::move($3), @2); }
    ;

multiplicative
    : unary { $$ = std::move($1); }
    | multiplicative STAR unary
      { $$ = make_binary("*", std::move($1), std::move($3), @2); }
    | multiplicative SLASH unary
      { $$ = make_binary("/", std::move($1), std::move($3), @2); }
    | multiplicative PERCENT unary
      { $$ = make_binary("%", std::move($1), std::move($3), @2); }
    ;

unary
    : primary { $$ = std::move($1); }
    | LOGICAL_NOT unary
      {
          $$ = bd1::make_ast(bd1::AstKind::UnaryExpression,
                             source_location(@1), "!");
          bd1::add_child(*$$, std::move($2));
      }
    | MINUS unary
      {
          $$ = bd1::make_ast(bd1::AstKind::UnaryExpression,
                             source_location(@1), "-");
          bd1::add_child(*$$, std::move($2));
      }
    ;

primary
    : INTEGER
      { $$ = bd1::make_ast(bd1::AstKind::IntegerLiteral,
                           source_location(@1), std::to_string($1)); }
    | KW_TRUE
      { $$ = bd1::make_ast(bd1::AstKind::BooleanLiteral,
                           source_location(@1), "true"); }
    | KW_FALSE
      { $$ = bd1::make_ast(bd1::AstKind::BooleanLiteral,
                           source_location(@1), "false"); }
    | IDENTIFIER
      { $$ = bd1::make_ast(bd1::AstKind::Identifier,
                           source_location(@1), std::move($1)); }
    | KW_FRONT_CLEAR LEFT_PAREN RIGHT_PAREN
      { $$ = bd1::make_ast(bd1::AstKind::SensorCall,
                           source_location(@1), "front_clear"); }
    | KW_AT_GOAL LEFT_PAREN RIGHT_PAREN
      { $$ = bd1::make_ast(bd1::AstKind::SensorCall,
                           source_location(@1), "at_goal"); }
    | LEFT_PAREN expression RIGHT_PAREN
      { $$ = std::move($2); }
    ;

%%

void bd1::GeneratedParser::error(const location_type& location,
                                 const std::string& message) {
    driver.report(location, message);
}
