# Usage

## Step 1

- Compile the lexer to generate `lexer.c` and `lexer.h`

```bash
flex -DYY_DECL="Token * yylex()" exp.lex
```

## Step 2

- Compile the `lexer.c` and `exp.c` files to generate `exp` binary

```bash
gcc -o exp lexer.c exp.c
```

## Final Step

- Execute the binary

```bash
./exp
```
