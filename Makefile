make :
	gcc -Wall -Wextra -lm -fsanitize=address -o lmlc main.c ocaml.c lexer.c parser.c lambda.c lambda-calculus_interpreter.c utils.c
run :
	./lmlc