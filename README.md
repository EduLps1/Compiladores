# R2-D2 Compiler

Compilador educacional da linguagem **BD-1**, uma linguagem de alto nível com
sintaxe inspirada em C++ para controlar um robô em uma grade 2D.

A candidata à `v0.2.0` implementa o front-end léxico e sintático. O Flex
reconhece os tokens, um adaptador os entrega ao parser LALR(1) gerado pelo
Bison e o resultado válido é convertido em uma árvore sintática abstrata
(AST). Análise semântica, geração de código, execução do robô e interface
gráfica ainda não fazem parte desta versão.

## Fluxo atual

```text
arquivo BD-1
    ↓
lexer Flex → tokens ou erros léxicos
    ↓
adaptador Token → símbolos Bison
    ↓
parser Bison LALR(1) → erros sintáticos ou AST
```

## Exemplo BD-1

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

## Compilar e testar localmente

Requisitos: CMake 3.20+, compilador C++20, Flex 2.6+, Bison 3.8+ e os
cabeçalhos de desenvolvimento do Flex (`libfl-dev` em Debian/Ubuntu).

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Comandos principais:

```bash
./build/r2d2c tokens examples/navigation.bd1
./build/r2d2c parse examples/syntax-valid.bd1
./build/r2d2c ast examples/syntax-valid.bd1
./build/r2d2c ast --trace examples/syntax-valid.bd1
./build/r2d2c parse examples/syntax-errors.bd1
./build/r2d2c demo syntax
./build/r2d2c --version
```

`parse` apenas informa se a entrada foi aceita. `ast` imprime a estrutura que
será consumida pela futura análise semântica. `--trace` torna explícitas as
etapas da integração, e `demo syntax` compara uma AST real com o resultado
esperado embutido no executável.

## Executar com Docker

O Docker evita instalar a cadeia de compilação diretamente na máquina:

```bash
docker build --target test -t r2d2-compiler:test .
docker build -t r2d2-compiler:0.2.0 .
docker run --rm \
  -v "$PWD:/workspace:ro" \
  r2d2-compiler:0.2.0 ast /workspace/examples/syntax-valid.bd1
```

## Documentação

- [Especificação léxica](docs/lexical-specification.md)
- [Especificação sintática](docs/syntax-specification.md)
- [Integração entre Flex, Bison e AST](docs/parser-integration.md)
- [Roteiro de demonstração sintática](docs/syntax-demo.md)
- [Roteiro de demonstração léxica](docs/lexical-demo.md)
- [Histórico de versões](CHANGELOG.md)

## Roadmap

- `v0.1.0`: análise léxica com Flex e C++;
- `v0.2.0`: gramática, análise sintática com Bison e AST;
- `v0.3.0`: tabela de símbolos, escopos e análise semântica;
- `v0.4.0`: representação intermediária e geração de C++;
- versões posteriores: simulador e interface visual.
