# Roteiro de demonstração da análise sintática

## 1. Validar a instalação

```bash
./build/r2d2c --version
ctest --test-dir build --output-on-failure
```

O primeiro comando deve mostrar `r2d2c 0.2.1`. O segundo verifica lexer,
parser, precedência, recuperação de erros e saídas exatas do CLI.

## 2. Mostrar a integração completa

```bash
./build/r2d2c ast --trace examples/syntax-valid.bd1
```

Explique a sequência do log: o Flex reconhece os tokens, o adaptador os
converte para símbolos Bison, o parser LALR(1) aceita a entrada e as ações da
gramática constroem a AST. A árvore mostra nós de declaração, repetição,
condição, comando e expressão com suas posições na fonte.

Para o exemplo versionado, o início da saída deve confirmar:

```text
[trace] lexer: 56 token(s), 0 erro(s)
[trace] adapter: 56 token(s) preparados para o Bison
[trace] parser: entrada aceita
[trace] ast: raiz Mission criada
```

## 3. Mostrar recuperação de erros

```bash
./build/r2d2c parse examples/syntax-errors.bd1
```

A saída deve conter três diagnósticos (`S001`, `S002` e `S003`) em uma única
execução. Não é impressa uma AST parcial.

## 4. Demonstrar saída esperada versus obtida

```bash
./build/r2d2c demo syntax
```

Esse caso é independente de arquivos externos: a fonte está atribuída no
próprio executável. Depois da integração, o programa imprime a AST realmente
produzida e termina com:

```text
=== validacao automatica ===
esperado == obtido: PASS
```

Isso comprova que o resultado observado não é apenas uma ilustração no
documento; ele foi calculado pelo lexer e pelo parser da versão atual.
