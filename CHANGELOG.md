# Histórico de versões

Todas as mudanças relevantes do R2-D2 Compiler serão registradas neste arquivo.
O projeto segue o versionamento semântico.

## [0.2.0] — candidata, ainda não publicada

### Adicionado

- gramática livre de contexto da BD-1 em Bison C++;
- parser LALR(1) integrado ao vetor de tokens produzido pelo Flex;
- AST com valores, linha e coluna para declarações, expressões, comandos e
  estruturas de controle;
- comandos CLI `parse`, `ast`, `ast --trace` e `demo syntax`;
- diagnósticos sintáticos numerados e recuperação de múltiplos erros;
- exemplos de sintaxe válida e inválida;
- testes unitários do parser e testes golden da saída exata do CLI;
- documentação da gramática, da integração e da demonstração.

### Alterado

- versão do executável atualizada para `0.2.0`;
- build CMake, Docker e CI passam a exigir Bison 3.8+;
- biblioteca interna ampliada de lexer para front-end.

## [0.1.0] — publicada

### Adicionado

- especificação léxica da linguagem BD-1;
- analisador léxico em C++ gerado com Flex;
- CLI `r2d2c` com os comandos `tokens`, `--help` e `--version`;
- tokens com categoria, lexema, linha, coluna e valor inteiro processado;
- palavras reservadas de controle e navegação do robô;
- operadores simples e compostos com regra do maior casamento;
- comentários de linha e de bloco no estilo C++;
- recuperação de múltiplos erros léxicos em uma única execução;
- testes automatizados, build CMake, imagem Docker e CI do GitHub Actions.
