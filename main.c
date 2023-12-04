#include <stdio.h>
#include "lang.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

extern struct cmd * root;
int yyparse();

int main(int argc, char * * argv) {
  if (argc == 1) {
    printf("Error, not enough arguments!\n");
    return 0;
  }
  if (argc >= 3) {
    printf("Error, too many arguments!\n");
    return 0;
  }
  yyin = fopen(argv[1], "rb");
  if (yyin == NULL) {
    printf("File %s can't be opened.\n", argv[1]);
    return 0;
  }
  yyparse();
  fclose(yyin);
  struct res_prog * r = init_res_prog(root);
  while (! test_end(r)) {
    step(r);
  }
}
