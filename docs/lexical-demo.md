# Demonstração e validação da análise léxica

## Objetivo da demonstração

Comprovar que o R2-D2 Compiler reconhece o vocabulário da BD-1, conserva linha
e coluna, aplica a regra do maior casamento e apresenta múltiplos erros de uma
entrada inválida.

## 1. Construção reproduzível

```bash
docker build --target test -t r2d2-compiler:test .
```

O estágio `test` só termina com sucesso quando o projeto compila e todos os
testes CTest passam.

## 2. Exemplo válido

```bash
docker build -t r2d2-compiler:0.1.0 .
docker run --rm \
  -v "$PWD:/workspace:ro" \
  r2d2-compiler:0.1.0 tokens /workspace/examples/navigation.bd1
```

Pontos para observar:

- `mission`, `while`, `move` e `at_goal` são palavras reservadas;
- `steps` e `arrived` são identificadores;
- `++` é um token, não dois sinais `+`;
- cada linha da saída inclui linha e coluna;
- comentários e espaços não aparecem como tokens.

## 3. Exemplo inválido e recuperação

```bash
docker run --rm \
  -v "$PWD:/workspace:ro" \
  r2d2-compiler:0.1.0 tokens /workspace/examples/lexical-errors.bd1
```

A execução deve informar separadamente os caracteres `@` e `#`, continuar a
varredura e terminar com código de falha.

## 4. Testes locais

Quando CMake, Flex e um compilador C++20 estiverem instalados:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Os testes cobrem palavras reservadas, identificadores, sensibilidade a
maiúsculas, valores inteiros, operadores, comentários, posições, caracteres
inválidos, overflow e comentário não terminado.

## 5. Fronteira entre as etapas

Esta entrada contém somente tokens conhecidos:

```c
int = steps 10;
```

Por isso, o lexer consegue percorrê-la sem erro. A ordem, porém, não representa
uma declaração válida. O parser implementado desde a `v0.2.0` rejeita essa
sequência durante a análise sintática.
