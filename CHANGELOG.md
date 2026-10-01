# Histórico de versões

Todas as mudanças relevantes do R2-D2 Compiler serão registradas neste arquivo.
O projeto segue o versionamento semântico.

## [0.1.0] — candidata, ainda não publicada

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
