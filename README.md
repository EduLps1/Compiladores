# R2-D2 Compiler

Compilador educacional da linguagem **BD-1**, uma linguagem de alto nível com
sintaxe inspirada em C++ para controlar um robô em uma grade 2D.

O projeto evolui por etapas da disciplina de Compiladores. A candidata à
`v0.1.0` implementa exclusivamente a **análise léxica**. Ainda não há parser,
validação semântica, execução do robô ou interface gráfica.

## Fluxo atual

```text
programa.bd1 → lexer Flex/C++ → tokens ou diagnósticos léxicos
```

O caminho planejado para versões futuras é:

```text
BD-1 → front-end → representação intermediária → C++ → g++ → executável
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

Nesta versão, o exemplo é convertido em tokens. Sua estrutura e seu significado
serão verificados nas versões sintática e semântica.

## Compilar e testar localmente

Requisitos: CMake 3.20+, compilador C++20, Flex 2.6+ e os cabeçalhos de
desenvolvimento do Flex (`libfl-dev` em Debian/Ubuntu).

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Executar o analisador:

```bash
./build/r2d2c tokens examples/navigation.bd1
./build/r2d2c tokens examples/lexical-errors.bd1
./build/r2d2c --version
```

## Executar com Docker

O Docker evita instalar Flex e CMake diretamente na máquina:

```bash
docker build --target test -t r2d2-compiler:test .
docker build -t r2d2-compiler:0.1.0 .
docker run --rm \
  -v "$PWD:/workspace:ro" \
  r2d2-compiler:0.1.0 tokens /workspace/examples/navigation.bd1
```

## Documentação

- [Especificação léxica](docs/lexical-specification.md)
- [Roteiro de demonstração e validação](docs/lexical-demo.md)
- [Histórico de versões](CHANGELOG.md)

## Roadmap

- `v0.1.0`: análise léxica com Flex e C++;
- `v0.2.0`: gramática, análise sintática com Bison e AST;
- `v0.3.0`: tabela de símbolos, escopos e análise semântica;
- `v0.4.0`: representação intermediária e geração de C++;
- versões posteriores: simulador e interface visual.
