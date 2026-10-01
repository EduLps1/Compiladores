# R2-D2 Compiler

Compilador educacional da linguagem **BD-1**, uma linguagem de alto nível com
sintaxe inspirada em C++ para controlar um robô em uma grade 2D.

A versão atual, `v0.2.1`, implementa as duas primeiras etapas do front-end:
**análise léxica** com Flex e **análise sintática** com Bison. Um programa
válido é transformado em uma Árvore Sintática Abstrata (AST), que será a
entrada da futura análise semântica.

## Estado do projeto

| Versão | Etapa | Situação |
| --- | --- | --- |
| `v0.1.0` | Análise léxica | Concluída |
| `v0.2.0` | Análise sintática | Concluída |
| `v0.2.1` | Consolidação da documentação sintática | Versão atual |
| `v0.3.0` | Análise semântica | Próxima etapa |
| `v0.4.0` | Middle-end e geração de C++ | Planejada |

Ainda não fazem parte do compilador: tabela de símbolos, validação de tipos,
geração de código, execução do robô e interface gráfica.

## Arquitetura atual

```text
arquivo .bd1
    │
    ▼
lexer Flex (src/lexer.l)
    │ bd1::Token + diagnósticos Lxxx
    ▼
adaptador ParserDriver
    │ símbolos aceitos pelo Bison
    ▼
parser Bison LALR(1) (src/parser.y)
    │ diagnósticos Sxxx ou ações da gramática
    ▼
AST com valores, linha e coluna
    │
    └── futura análise semântica
```

O lexer é executado primeiro e produz o vetor completo de tokens. Se existir
qualquer erro léxico, o parser não é chamado. Caso os tokens sejam válidos, o
`ParserDriver` converte cada `TokenKind` no símbolo correspondente do Bison,
preservando valores e posições da fonte.

## Análise léxica

O analisador gerado pelo Flex reconhece:

- palavras reservadas da BD-1;
- identificadores e literais inteiros;
- literais booleanos `true` e `false`;
- operadores simples e compostos;
- delimitadores e pontuação;
- comentários de linha e de bloco;
- posição de linha e coluna de cada token.

Os erros léxicos usam códigos `Lxxx`. O lexer continua após caracteres
inválidos sempre que possível, permitindo apresentar vários problemas na
mesma execução.

Para visualizar a tokenização:

```bash
./build/r2d2c tokens examples/navigation.bd1
```

## Especificação sintática

O parser é gerado pelo Bison usando o algoritmo **LALR(1)**. A gramática é
livre de contexto e sua implementação executável está em `src/parser.y`.

Resumo em EBNF:

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
assignment    = identifier,
                ("=" | "+=" | "-=" | "*=" | "/=" | "%="), expression ;
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

### Estruturas reconhecidas

- `mission main()` como ponto de entrada;
- blocos obrigatórios nas estruturas de controle;
- declarações `int` e `bool`, com inicialização opcional;
- atribuições `=`, `+=`, `-=`, `*=`, `/=` e `%=`;
- atualizações pós-fixadas `++` e `--`;
- comandos `move`, `turn_left`, `turn_right` e `stop`;
- sensores `front_clear()` e `at_goal()`;
- `if`, `else`, `else if`, `while`, `repeat`, `for` e `do/while`;
- `switch`, `case`, `default`, `break`, `continue` e `return`.

### Precedência de expressões

Da menor para a maior precedência:

1. `||`;
2. `&&`;
3. `==` e `!=`;
4. `<`, `<=`, `>` e `>=`;
5. `+` e `-`;
6. `*`, `/` e `%`;
7. operadores unários `!` e `-`;
8. literais, identificadores, sensores e expressões entre parênteses.

## Integração Flex–Bison

O Flex e o Bison não compartilham diretamente o mesmo tipo de token. A classe
`ParserDriver` realiza essa integração:

1. recebe o vetor de `bd1::Token` produzido pelo lexer;
2. converte `TokenKind` para os construtores `GeneratedParser::make_*`;
3. transfere o valor de identificadores e inteiros;
4. converte linha e coluna para a localização usada pelo Bison;
5. recebe a AST criada pelas ações da gramática;
6. reúne e numera os diagnósticos sintáticos.

A diretiva `%expect 0` faz a geração falhar caso a gramática passe a possuir
algum conflito de deslocamento/redução ou redução/redução não previsto.

## Árvore Sintática Abstrata

A árvore de parse registra todos os símbolos da derivação. A AST remove
detalhes auxiliares, como parênteses e ponto e vírgula, e preserva somente os
elementos necessários às próximas etapas.

Exemplo produzido para `repeat steps { move(1); }`:

```text
Repeat [4:5]
|-- Count [4:12]
|   `-- Identifier steps [4:12]
`-- Body [4:18]
    `-- Block [4:18]
        `-- Move [5:9]
            `-- Integer 1 [5:14]
```

Todos os nós mantêm `[linha:coluna]`, permitindo que a análise semântica gere
diagnósticos apontando para a posição original do código.

## Diagnósticos e recuperação

Os diagnósticos sintáticos usam códigos `S001`, `S002`, `S003` e assim por
diante. O parser tenta se recuperar no próximo ponto e vírgula seguro, podendo
informar mais de um erro na mesma execução. Se existir qualquer erro, a AST
parcial é descartada.

Exemplo:

```text
erro sintatico [2:17] S001: encontrado ;
erro sintatico [4:5] S002: encontrado turn_left; esperado ;
erro sintatico [6:1] S003: encontrado }; esperado ;
analise sintatica: 3 erro(s)
```

## Exemplo completo

```c
mission main() {
    int steps = 0;
    bool arrived = false;

    while (!arrived) {
        if (front_clear()) {
            move(1);
            steps++;
        } else {
            turn_right();
        }

        arrived = at_goal();
    }

    stop();
}
```

## Comandos do CLI

```bash
# Mantém a análise léxica da v0.1.0
./build/r2d2c tokens examples/navigation.bd1

# Valida a sintaxe sem imprimir a árvore
./build/r2d2c parse examples/syntax-valid.bd1

# Valida a sintaxe e imprime a AST
./build/r2d2c ast examples/syntax-valid.bd1

# Mostra lexer, adaptador, Bison e AST
./build/r2d2c ast --trace examples/syntax-valid.bd1

# Demonstra recuperação de erros
./build/r2d2c parse examples/syntax-errors.bd1

# Executa uma fonte embutida e compara esperado versus obtido
./build/r2d2c demo syntax

# Mostra a versão
./build/r2d2c --version
```

O comando `demo syntax` termina com uma verificação automática:

```text
=== validacao automatica ===
esperado == obtido: PASS
```

## Compilar e testar localmente

Requisitos: CMake 3.20+, compilador C++20, Flex 2.6+, Bison 3.8+ e os
cabeçalhos de desenvolvimento do Flex (`libfl-dev` em Debian/Ubuntu).

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

A suíte possui oito testes para lexer, parser, precedência, recuperação de
erros, versão e comparação exata das saídas do CLI.

## Executar com Docker

```bash
docker build --target test -t r2d2-compiler:test .
docker build -t r2d2-compiler:0.2.1 .
docker run --rm \
  -v "$PWD:/workspace:ro" \
  r2d2-compiler:0.2.1 ast /workspace/examples/syntax-valid.bd1
```

## Limite entre sintaxe e semântica

Uma construção como `int value = true;` possui formato sintaticamente válido,
mas tipos incompatíveis. Da mesma forma, `break;` possui formato válido mesmo
fora de um laço. Essas regras pertencem à análise semântica e serão tratadas
na próxima etapa.

## Documentação detalhada

- [Especificação léxica](docs/lexical-specification.md)
- [Especificação sintática](docs/syntax-specification.md)
- [Integração entre Flex, Bison e AST](docs/parser-integration.md)
- [Roteiro de demonstração sintática](docs/syntax-demo.md)
- [Roteiro de demonstração léxica](docs/lexical-demo.md)
- [Histórico de versões](CHANGELOG.md)

## Próxima etapa

A `v0.3.0` iniciará a análise semântica sobre a AST, incluindo tabela de
símbolos, escopos, declaração e uso de variáveis, verificação de tipos e
validação contextual de comandos de controle.
