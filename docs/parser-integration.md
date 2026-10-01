# Integração lexer-parser

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

O lexer termina primeiro e entrega seu vetor completo. Se houver erro léxico,
o parser não é executado, pois um token inválido tornaria o diagnóstico
sintático enganoso. Sem erros léxicos, `ParserDriver` fornece um símbolo por
vez ao código gerado pelo Bison, preservando valor, linha e coluna.

O Bison usa análise LALR(1): mantém uma pilha de estados e decide entre
deslocar o próximo símbolo ou reduzir uma sequência por uma regra. A opção
`%expect 0` faz a geração falhar se surgir qualquer conflito de
deslocamento/redução ou redução/redução não previsto.

## Diagnósticos e recuperação

Erros léxicos usam códigos `Lxxx`; erros sintáticos usam `Sxxx`. A regra de
recuperação descarta uma instrução inválida até um ponto e vírgula seguro e
continua a análise. Assim, uma execução pode apresentar mais de um problema.
Quando existe qualquer erro sintático, a AST parcial é descartada.

## Responsabilidade de cada comando

- `tokens`: mostra a saída do Flex, preservando a validação da `v0.1.0`;
- `parse`: executa lexer, adaptador e parser e informa aceitação ou erros;
- `ast`: executa o mesmo fluxo e também imprime a AST válida;
- `ast --trace`: mostra as transições e contagens da integração;
- `demo syntax`: usa uma fonte embutida e compara a AST obtida com uma saída
  fixa conhecida.
