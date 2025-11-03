#include <stdio.h>
#include "lexer.h"
#include "utils.h"
#include "parser.h"

int main() {
    char *src = loadFile("1.q");  // încarcă fișierul sursă
    tokenize(src);                 // analizează lexical
    showTokens();                  // afișează toți tokenii
    parse();
    return 0;
}
