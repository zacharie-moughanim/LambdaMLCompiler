# λMLCompiler

## Description

λMLCompiler est un projet de compilateur d'un fragment pur de Caml vers du λ-calcul.
Le compilateur est entièrement codé en C, et un interpréteur de λ-terme est disponible (codé en OCaml).

## Les langages

### Langage source : Fragment Pur de Caml
On considère le fragment du langage Caml suivant :
```
S ::= let x = M in N | M N | fun x -> M | (M, N) | M :: N | [M1;M2;...;Mn]
     | M @ N | M ^ N | M + N | M - N | M * N | M / N | M && | (M || N)
     | M < N | M <= N | M > N | M >= N | M <> N | M = N
     | if C then M else N | if C then M | (match C with | P1  -> M1 ... | Pn -> Mn)
     | chaîne de caractère | entiers | true | false | (M)
```
#### Idées d'améliorations
Dans un premier temps, on souhaiterait implémenter en C une infératrice de types, en effet, on verra plus tard que cela peut éviter beaucoup de bugs qui ne seront pas détéctés, et qui produira des λ-termes qui ne vérifient pas la spécification du programme Caml en entrée.
On pourrait imaginer prendre en compte les types enregistrements et les types algébriques.

### Langage objet : λ-Calcul
Le langage pris en compte est le λ-calcul originel :
```
L ::= x | M N | λ x. M
```

On s'autorisera, lorsqu'il est pertinent, d'écrire `λ x1. λ x2. ... λ xn. M` de façon plus compacte : `λ x1, x2, ..., xn. M`

#### Représentation des objets Caml :
- Entiers naturels, booléens, couples : Représentation de Church, respectivement l'entier $n$, les booléens `true` et `false`, le couple `⟨a, b⟩`, sont représentés par `λ s, z. s^n z`, `λ t, e. t`, `λ t, e. e` et `λ z. z a b`.
- Entiers relatifs : Le couple d'entiers naturels `⟨n, m⟩` représente `n - m`
- Liste : Une liste est représenté par sa fonction fold right : `[x1; x2; ...; xn]` est représenté par `λ op init. op (x1 op (x2 op (... (op xn init)) ...)`
#### Idées d'améliorations
- Prise en compte des flottants. <br/>
  Le problème qui émerge est qu'implémenter les opérations sur les flottants "classiques" devient très compliqué avec des λ-termes.<br/>
  Une piste possible, et qui se situe plus dans l'esprit du λ-calcul, serait de représenter des rationnels. Le rationnel $\frac p q$ peut se voir en λ-calcul comme le couple `⟨⟨n, m⟩, q⟩`, avec `p = n - m` relatif et `q` naturel non nul.<br/>
  Le problème de cet alternative est que pour représenter les flottants Caml, il peut être nécessaire de passer par une représentation `〈p, 10^18〉` avec `p` de l'orde de $1$ à $10^{18}$. Représenter $10^{18}$ en unaire ne semble pas réalisable raisonnablement.
- En conclusion du point précédent, on pourrait envisager de passer de la représentation de Church des naturels à une représentation sous forme de suite finie de bits.
- De plus, ces représentations mènent à un constat : La liste vide `[]`, l'entier naturel $0$ et le booléen `false` ont la même représentation en tant que λ-terme ! C'est l'une des raisons principales pour laquelle il serait intéréssant d'implémenter l'inférence de type dans le pré-traitement du code Caml.  

## Exécution

`./lmlc -o dest_file src_file`

Le dest_file contient un fichier très facilement parsable par le code OCaml, l'enchaînement compilateur/interpréteur doit être fait manuellement, des améliorations sont à venir sur ce point.
