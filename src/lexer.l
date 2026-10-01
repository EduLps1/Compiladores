%option c++ noyywrap nounput noinput nodefault
%option yyclass="Bd1Scanner"
%x BLOCK_COMMENT

%{
#include "scanner.hpp"

#include <string>
#include <string_view>

#undef YY_DECL
#define YY_DECL int Bd1Scanner::yylex()
#define YY_USER_ACTION begin_lexeme(std::string_view(yytext, yyleng));
#define RETURN_TOKEN(kind)                                                    \
    do {                                                                      \
        set_token((kind), std::string_view(yytext, yyleng));                  \
        return static_cast<int>(kind);                                        \
    } while (false)
%}

DIGIT       [0-9]
LETTER      [A-Za-z_]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
SPACE       [ \t\r\n]+

%%

{SPACE}             { /* Separadores nao produzem tokens. */ }
"//"[^\n]*         { /* Comentario de linha ignorado. */ }
"/*"                { comment_location_ = lexeme_location_; BEGIN(BLOCK_COMMENT); }
<BLOCK_COMMENT>"*/" { BEGIN(INITIAL); }
<BLOCK_COMMENT>(.|\n) { /* Conteudo do comentario ignorado. */ }
<BLOCK_COMMENT><<EOF>> {
    set_unterminated_comment();
    BEGIN(INITIAL);
    return static_cast<int>(bd1::TokenKind::Invalid);
}

"mission"           { RETURN_TOKEN(bd1::TokenKind::KwMission); }
"main"              { RETURN_TOKEN(bd1::TokenKind::KwMain); }
"int"               { RETURN_TOKEN(bd1::TokenKind::KwInt); }
"bool"              { RETURN_TOKEN(bd1::TokenKind::KwBool); }
"true"              { RETURN_TOKEN(bd1::TokenKind::KwTrue); }
"false"             { RETURN_TOKEN(bd1::TokenKind::KwFalse); }
"if"                { RETURN_TOKEN(bd1::TokenKind::KwIf); }
"else"              { RETURN_TOKEN(bd1::TokenKind::KwElse); }
"switch"            { RETURN_TOKEN(bd1::TokenKind::KwSwitch); }
"case"              { RETURN_TOKEN(bd1::TokenKind::KwCase); }
"default"           { RETURN_TOKEN(bd1::TokenKind::KwDefault); }
"while"             { RETURN_TOKEN(bd1::TokenKind::KwWhile); }
"do"                { RETURN_TOKEN(bd1::TokenKind::KwDo); }
"for"               { RETURN_TOKEN(bd1::TokenKind::KwFor); }
"repeat"            { RETURN_TOKEN(bd1::TokenKind::KwRepeat); }
"break"             { RETURN_TOKEN(bd1::TokenKind::KwBreak); }
"continue"          { RETURN_TOKEN(bd1::TokenKind::KwContinue); }
"return"            { RETURN_TOKEN(bd1::TokenKind::KwReturn); }

"move"              { RETURN_TOKEN(bd1::TokenKind::KwMove); }
"turn_left"         { RETURN_TOKEN(bd1::TokenKind::KwTurnLeft); }
"turn_right"        { RETURN_TOKEN(bd1::TokenKind::KwTurnRight); }
"stop"              { RETURN_TOKEN(bd1::TokenKind::KwStop); }
"front_clear"       { RETURN_TOKEN(bd1::TokenKind::KwFrontClear); }
"at_goal"           { RETURN_TOKEN(bd1::TokenKind::KwAtGoal); }

"++"                { RETURN_TOKEN(bd1::TokenKind::Increment); }
"--"                { RETURN_TOKEN(bd1::TokenKind::Decrement); }
"+="                { RETURN_TOKEN(bd1::TokenKind::PlusAssign); }
"-="                { RETURN_TOKEN(bd1::TokenKind::MinusAssign); }
"*="                { RETURN_TOKEN(bd1::TokenKind::StarAssign); }
"/="                { RETURN_TOKEN(bd1::TokenKind::SlashAssign); }
"%="                { RETURN_TOKEN(bd1::TokenKind::PercentAssign); }
"=="                { RETURN_TOKEN(bd1::TokenKind::Equal); }
"!="                { RETURN_TOKEN(bd1::TokenKind::NotEqual); }
"<="                { RETURN_TOKEN(bd1::TokenKind::LessEqual); }
">="                { RETURN_TOKEN(bd1::TokenKind::GreaterEqual); }
"&&"                { RETURN_TOKEN(bd1::TokenKind::LogicalAnd); }
"||"                { RETURN_TOKEN(bd1::TokenKind::LogicalOr); }

"+"                 { RETURN_TOKEN(bd1::TokenKind::Plus); }
"-"                 { RETURN_TOKEN(bd1::TokenKind::Minus); }
"*"                 { RETURN_TOKEN(bd1::TokenKind::Star); }
"/"                 { RETURN_TOKEN(bd1::TokenKind::Slash); }
"%"                 { RETURN_TOKEN(bd1::TokenKind::Percent); }
"="                 { RETURN_TOKEN(bd1::TokenKind::Assign); }
"<"                 { RETURN_TOKEN(bd1::TokenKind::Less); }
">"                 { RETURN_TOKEN(bd1::TokenKind::Greater); }
"!"                 { RETURN_TOKEN(bd1::TokenKind::LogicalNot); }
"("                 { RETURN_TOKEN(bd1::TokenKind::LeftParen); }
")"                 { RETURN_TOKEN(bd1::TokenKind::RightParen); }
"{"                 { RETURN_TOKEN(bd1::TokenKind::LeftBrace); }
"}"                 { RETURN_TOKEN(bd1::TokenKind::RightBrace); }
";"                 { RETURN_TOKEN(bd1::TokenKind::Semicolon); }
","                 { RETURN_TOKEN(bd1::TokenKind::Comma); }
":"                 { RETURN_TOKEN(bd1::TokenKind::Colon); }

{DIGIT}+ {
    set_integer(std::string_view(yytext, yyleng));
    return static_cast<int>(current_token_.kind);
}

{IDENTIFIER}         { RETURN_TOKEN(bd1::TokenKind::Identifier); }

. {
    const std::string invalid(yytext, yyleng);
    set_token(bd1::TokenKind::Invalid, invalid);
    report("L001", "caractere invalido '" + invalid + "'", lexeme_location_);
    return static_cast<int>(bd1::TokenKind::Invalid);
}

<<EOF>>              { return static_cast<int>(bd1::TokenKind::Eof); }

%%
