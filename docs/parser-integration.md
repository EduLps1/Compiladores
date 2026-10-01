# Integração lexer-parser

Este documento descreve a integração utilizada na versão `v0.2.1`. Flex e
Bison geram componentes independentes; a classe `ParserDriver` estabelece o
contrato entre eles sem acoplar o lexer ao código interno gerado pelo Bison.

## Componentes

```text
src/lexer.l
    │ gera tokens bd1::Token
    ▼
bd1::Lexer
    │ vetor de tokens + diagnósticos
    ▼
ParserDriver
    │ converte TokenKind em símbolos GeneratedParser::make_*
    ▼
parser.y → Bison → parser LALR(1) em C++
    │ ações semânticas da gramática
    ▼
bd1::AstNode
```

## Sequência de execução

1. o CLI abre o arquivo `.bd1`;
2. `bd1::Lexer::scan` executa o scanner Flex;
3. o lexer devolve `LexResult`, contendo tokens e diagnósticos;
4. se houver erro léxico, o fluxo termina e o parser não é executado;
5. `bd1::Parser::parse` cria um `ParserDriver` para o vetor válido;
6. o Bison solicita um símbolo por vez por meio de `next_symbol()`;
7. as reduções da gramática constroem os nós da AST;
8. sem erros, o nó `Mission` é devolvido como raiz;
9. com erros sintáticos, os diagnósticos são devolvidos e a AST parcial é
   descartada.

O lexer termina primeiro e entrega seu vetor completo. Se houver erro léxico,
o parser não é executado, pois um token inválido tornaria o diagnóstico
sintático enganoso. Sem erros léxicos, `ParserDriver` fornece um símbolo por
vez ao código gerado pelo Bison, preservando valor, linha e coluna.

## Conversão dos tokens

Exemplos do mapeamento realizado pelo adaptador:

| Saída do Flex | Símbolo entregue ao Bison | Informação preservada |
| --- | --- | --- |
| `KwWhile` | `KW_WHILE` | linha e coluna |
| `Identifier` | `IDENTIFIER` | lexema, linha e coluna |
| `Integer` | `INTEGER` | valor de 64 bits, linha e coluna |
| `LeftParen` | `LEFT_PAREN` | linha e coluna |
| `Eof` | `YYEOF` | posição final |

Esse adaptador permite manter `Token` como uma API estável do lexer enquanto
o Bison utiliza seus construtores de símbolos fortemente tipados.

O Bison usa análise LALR(1): mantém uma pilha de estados e decide entre
deslocar o próximo símbolo ou reduzir uma sequência por uma regra. A opção
`%expect 0` faz a geração falhar se surgir qualquer conflito de
deslocamento/redução ou redução/redução não previsto.

## Diagnósticos e recuperação

Erros léxicos usam códigos `Lxxx`; erros sintáticos usam `Sxxx`. A regra de
recuperação descarta uma instrução inválida até um ponto e vírgula seguro e
continua a análise. Assim, uma execução pode apresentar mais de um problema.
Quando existe qualquer erro sintático, a AST parcial é descartada.

## Log verificável da integração

O comando abaixo torna cada transição visível:

```bash
./build/r2d2c ast --trace examples/syntax-valid.bd1
```

Trecho da saída esperada:

```text
[trace] lexer: iniciado
[trace] lexer: 56 token(s), 0 erro(s)
[trace] adapter: 56 token(s) preparados para o Bison
[trace] parser: LALR(1) iniciado
[trace] parser: entrada aceita
[trace] ast: raiz Mission criada
```

Essa contagem vem da execução real do lexer. A árvore impressa em seguida é a
AST criada pelas ações de redução presentes em `src/parser.y`.

## Responsabilidade de cada comando

- `tokens`: mostra a saída do Flex, preservando a validação da `v0.1.0`;
- `parse`: executa lexer, adaptador e parser e informa aceitação ou erros;
- `ast`: executa o mesmo fluxo e também imprime a AST válida;
- `ast --trace`: mostra as transições e contagens da integração;
- `demo syntax`: usa uma fonte embutida e compara a AST obtida com uma saída
  fixa conhecida.
