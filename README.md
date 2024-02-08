# λMLCompiler

## Description

λMLCompiler est un projet de compilateur d'un fragment pur de Caml vers du lambda-calcul.
Le compilateur est entièrement codé en C, et un interpréteur de lambda-terme est disponible (codé en OCaml).

## Les langages

### Fragment Pur de Caml
S ::= let x = M in N | M N | fun x -> M | (M, N) | M :: N | \[M1;M2;...;Mn\]
     | M @ N | M ^ N | M + N | M - N | M * N | M / N | M && | (M || N)
     | M < N | M <= N | M > N | M >= N | M <> N | M = N
     | if C then M else N | if C then M | (match C with | P1  -> M1 ... | Pn -> Mn) | chaîne de caractère | entiers
     | (M)

Entiers naturels, booléens : Représentation de Church
Entiers relatifs : Le couple d'entiers naturels `〈a, b〉` représente `a-b`

Liste : Représenté par leur fonction de fold right : `[x1; x2; ...; xn]` se représente `λ op init. op (x1 op (x2 op (... (op xn init)))`

En cours de développement : prise en compte des flottants.
Problème : Implémentation sous forme de flottants "classiques" compliquées à implémenter avec des lambda-termes.
Piste envisagée : Représenter des rationnels

Format : `λ z. z (λ t. t (λ s, x. s^n x) (λ s, x. s^m x)) (λ s, x. s^l x)` ou `〈〈n, m〉, l〉` avec le couple représentant `n - m` relatif et `l` naturel.
Problème de cette représentation : Pour représenter les flottants classiques, peut-être est il obligatoire de passer par une représentation `〈a, 10^8〉` avec `a` de l'orde de 1 à 10^8. Représenter `10^8` en unaire ne semble pas réalisable.
Alternative : Passer à une représentation des naturels sous forme de liste de bits

### Lambda Calcul
On considère ici le lambda-calcul originel :
`L ::= x | M N | λ x. M`

## Exécution

`./lmlc -o dest_file src_file`
