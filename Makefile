make :
	gcc -Wall -Wextra -lm -fsanitize=address -o lmlc main.c ocaml.c lexer.c parser.c lambda.c
run :
	./lmlc