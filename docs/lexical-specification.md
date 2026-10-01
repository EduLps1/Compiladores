# Especificação léxica da BD-1

## 1. Objetivo

O analisador léxico recebe caracteres de um arquivo `.bd1` e produz uma
sequência de tokens. Espaços e comentários são consumidos, mas não geram
tokens. Cada token mantém o lexema e a posição em que começou.

```text
caracteres → Flex → token(tipo, lexema, linha, coluna, valor opcional)
```

A linguagem diferencia maiúsculas e minúsculas. `move` é uma palavra
reservada, enquanto `Move` é um identificador.

## 2. Palavras reservadas

| Grupo | Lexemas |
|---|---|
| Estrutura | `mission`, `main` |
| Tipos e valores | `int`, `bool`, `true`, `false` |
| Decisão | `if`, `else`, `switch`, `case`, `default` |
| Repetição | `while`, `do`, `for`, `repeat` |
| Controle | `break`, `continue`, `return` |
| Ações do robô | `move`, `turn_left`, `turn_right`, `stop` |
| Sensores | `front_clear`, `at_goal` |

O reconhecimento léxico dessas palavras não significa que sua combinação já
seja aceita. A gramática sintática disponível desde a `v0.2.0` define onde
cada token pode aparecer.

## 3. Identificadores e literais

### Identificadores

```regex
[A-Za-z_][A-Za-z0-9_]*
```

Exemplos válidos: `steps`, `robot_1`, `_counter` e `Move`.

Exemplos inválidos como identificador: `1robot`, `posição` e `robot-name`.

### Inteiros

```regex
[0-9]+
```

Inteiros são decimais sem sinal e precisam caber em 64 bits com sinal. O sinal
de `-10` é reconhecido separadamente como `MINUS` seguido de `INTEGER(10)`,
permitindo que o parser diferencie negação de subtração.

Literais `string` não fazem parte da versão atual da BD-1.

## 4. Operadores

| Categoria | Lexemas |
|---|---|
| Aritméticos | `+`, `-`, `*`, `/`, `%` |
| Incremento | `++`, `--` |
| Atribuição | `=`, `+=`, `-=`, `*=`, `/=`, `%=` |
| Comparação | `==`, `!=`, `<`, `<=`, `>`, `>=` |
| Lógicos | `&&`, `\|\|`, `!` |

O Flex aplica o maior casamento. Portanto, `>=` produz um único token
`GREATER_EQUAL`, e `move_robot` produz um `IDENTIFIER`, não `KW_MOVE` seguido
de outro identificador.

## 5. Delimitadores

| Lexema | Token |
|---|---|
| `(` | `LEFT_PAREN` |
| `)` | `RIGHT_PAREN` |
| `{` | `LEFT_BRACE` |
| `}` | `RIGHT_BRACE` |
| `;` | `SEMICOLON` |
| `,` | `COMMA` |
| `:` | `COLON` |

## 6. Espaços e comentários

Espaços, tabulações, `CR` e quebras de linha são ignorados, mas atualizam a
posição dos tokens seguintes.

São aceitos comentários no estilo C++:

```c
// comentário até o fim da linha

/* comentário
   com várias linhas */
```

Comentários de bloco não são aninhados. Um bloco não terminado gera o
diagnóstico `L003` na posição em que `/*` começou.

## 7. Diagnósticos

| Código | Significado | Recuperação |
|---|---|---|
| `L001` | Caractere não reconhecido | Consome o caractere e continua |
| `L002` | Inteiro fora do intervalo de 64 bits | Produz `INVALID` e continua |
| `L003` | Comentário de bloco não terminado | Produz `INVALID` e encerra no EOF |

O analisador reúne todos os erros recuperáveis. Se houver pelo menos um
diagnóstico, a CLI termina com código diferente de zero.

## 8. Limite da etapa léxica

A `v0.1.0` responde apenas:

> Quais tokens existem no arquivo e onde eles aparecem?

Ela não responde se os tokens formam um programa válido, se uma variável foi
declarada ou se os tipos são compatíveis. Na versão atual, a análise sintática
já verifica a primeira questão; declaração, escopo e tipos permanecem para a
análise semântica.
