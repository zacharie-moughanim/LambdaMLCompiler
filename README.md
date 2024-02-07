# λMLCompiler

## Description

λMLCompiler est un projet de compilateur d'un fragment pur de Caml vers du lambda-calcul.
Le compilateur est entièrement codé en C, et un interpréteur de lambda-terme est disponible (codé en OCaml).

## Les langages

### Fragment Pur de Caml
S ::= let x = M in N | M N | fun x -> M | (M, N) | M :: N | [M1;M2;...;Mn]
     | M @ N | M ^ N | M + N | M - N | M * N | M / N | M && | (M || N)
     | M < N | M <= N | M > N | M >= N | M <> N | M = N
     | if C then M else N | if C then M | (match C with | P1  -> M1 ... | Pn -> Mn) | chaîne de caractère | entiers
     | (M)

En cours de développement : prise en compte des flottants.

### Lambda Calcul
On considère ici le lambda-calcul originel :
L ::= x | M N | λ x. M

## Exécution

./lmlc -o dest_file src_file
