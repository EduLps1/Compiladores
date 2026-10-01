# Especificação sintática da BD-1

## Objetivo da versão 0.2.0

A análise sintática verifica se a sequência de tokens produzida pelo lexer
obedece à estrutura da linguagem. Ela não decide ainda se uma variável foi
declarada, se os tipos são compatíveis ou se um `break` está dentro de um
laço; essas são regras semânticas da próxima etapa.

A gramática executável está em `src/parser.y`. Ela é livre de contexto e foi
escrita para o gerador Bison com o esqueleto C++ `lalr1.cc`.

## EBNF resumida

```ebnf
program       = "mission", "main", "(", ")", block ;
block         = "{", { statement }, "}" ;

statement     = declaration, ";"
              | assignment, ";"
              | update, ";"
              | command
              | if | while | repeat | for | doWhile | switch
              | "break", ";" | "continue", ";"
              | "return", [ expression ], ";"
              | block ;

declaration   = ("int" | "bool"), identifier,
                [ "=", expression ] ;
assignment    = identifier, ("=" | "+=" | "-=" | "*=" | "/=" | "%="),
                expression ;
update        = identifier, ("++" | "--") ;

command       = "move", "(", expression, ")", ";"
              | ("turn_left" | "turn_right" | "stop"), "(", ")", ";" ;

if            = "if", "(", expression, ")", block,
                [ "else", (block | if) ] ;
while         = "while", "(", expression, ")", block ;
repeat        = "repeat", expression, block ;
for           = "for", "(", [ initializer ], ";",
                [ expression ], ";", [ update ], ")", block ;
doWhile       = "do", block, "while", "(", expression, ")", ";" ;
switch        = "switch", "(", expression, ")", "{",
                { ("case", literal | "default"), ":", { statement } }, "}" ;
```

Expressões são associativas à esquerda e seguem, da menor para a maior
precedência: `||`, `&&`, `== !=`, `< <= > >=`, `+ -`, `* / %`, e os unários
`! -`. Parênteses podem alterar essa ordem. Literais inteiros e booleanos,
identificadores e os sensores `front_clear()` e `at_goal()` são expressões
primárias.

## Árvore de parse e AST

A árvore de parse contém todos os símbolos e detalhes gramaticais, como
parênteses e ponto e vírgula. A AST remove esses elementos auxiliares e mantém
apenas a estrutura relevante para as próximas etapas. Por exemplo,
`move(1);` vira um nó `Move` com um filho `Integer 1`.

O CLI imprime a AST porque ela será a entrada da análise semântica. Cada nó
preserva `[linha:coluna]`, permitindo diagnósticos precisos nas próximas
versões.

## Limite entre sintaxe e semântica

O parser aceita, por exemplo, `int value = true;` e `break;` fora de um laço
porque ambos têm formato sintático válido. A futura `v0.3.0` deverá rejeitá-los
por incompatibilidade de tipo e contexto de controle, respectivamente.
