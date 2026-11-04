# Testeur ft_lex

Une suite de tests pour ft_lex : environ 400 tests, avec Valgrind, fuzzing et bonus.
Ce que fait `lex` (flex 2.6.4) sert de référence attendue, triée selon ce que POSIX exige vraiment.

## Utilisation

Depuis `tester/`, avec `ft_lex` et `libl.a` à la racine du dépôt :

```sh
make              # tous les tests obligatoires
make parse        # une catégorie
make vparse       # la même chose sous valgrind (ft_lex et scanners générés)
make v            # tout l'obligatoire sous valgrind
make one T=regex/trail_var   # un seul test
make ext          # bonus : extensions flex
make compress COMPRESS_FLAG=-C   # bonus compression, avec TON option
make fuzz         # mutations aléatoires des .l : ft_lex ne doit jamais planter
make fuzzdiff     # entrées aléatoires : ton scanner contre celui de lex
```

Si tes binaires sont ailleurs : `make FT_LEX=../build/ft_lex LIBL=../libl/libl.a`.

Chaque test affiche `N.OK`, `N.KO` ou `N.WARN`. Les détails (fichier `.l`, entrée, diff attendu/obtenu) sont écrits dans `logs/<catégorie>.log`.

## Ce qui est vérifié

| Catégorie | Contenu |
|---|---|
| `parse` | structure du `.l` : `%%`, `%{ %}`, définitions, `%s`/`%x`, actions, erreurs |
| `regex` | opérateurs ERE, priorités, `{n,m}`, classes, échappements, chaînes, `^`, `$`, `/` |
| `startcond` | conditions inclusives et exclusives, `BEGIN`, `<A,B>` |
| `matching` | plus longue correspondance, `ECHO` par défaut, octets spéciaux, 300 règles |
| `actions` | `yytext`, `yyleng`, `yymore`, `yyless`, `input`, `unput`, `REJECT`, `yywrap`, `yyin`, `%array`/`%pointer`, entrée de 1 Mo, token de 100 000 caractères |
| `robust` | entrées hostiles : ft_lex peut refuser, mais **ne doit pas planter** |
| `cli` | `-t`, `-n`, `-v`, plusieurs fichiers concaténés, `-`, stdin, fichiers invalides |
| `libl` | `main` et `yywrap` de ta libl, et la possibilité de remplacer l'un sans l'autre |
| `symbols` | `lex.yy.c` et la libl n'appellent que stdio, string, malloc, realloc, calloc et free |

Un test de type « fichier `.l` » fonctionne ainsi :
1. ft_lex est lancé sur le fichier.
2. Son code de sortie est comparé à celui de lex.
3. Si le fichier est valide, `lex.yy.c` est compilé avec ta libl, lancé sur chaque `nom.in*`, et sa sortie est comparée **octet par octet** à celle du scanner de lex.

Pour un fichier invalide, ton message d'erreur n'est pas comparé à celui de lex, car le format est libre. Il doit seulement contenir `nom.l:<ligne>`, sinon le test passe en WARN.

## Étiquettes

Chaque test a une étiquette, dans la 2e colonne de `tests/<cat>/index` :

| Étiquette | Signification | Si ft_lex diffère de lex |
|---|---|---|
| `posix` | le comportement est défini par POSIX | **KO** |
| `flex` | POSIX ne dit rien ; c'est ce que fait lex, et c'est raisonnable | WARN |
| `undefined` | POSIX dit « undefined » ou « unspecified » | rien : seul un crash, un timeout ou une erreur Valgrind est KO |
| `ext` | extension flex, hors sujet (bonus) | KO, mais seulement dans `make ext` |

Un crash (signal), un timeout ou une erreur Valgrind est **toujours** KO, quelle que soit l'étiquette.

## Pièges que ce testeur fait ressortir

- **La libl** doit mettre `main` et `yywrap` dans **deux fichiers objets séparés**. Sinon, un utilisateur qui définit son propre `yywrap` tire quand même le `.o` qui contient `main`, et obtient un symbole en double (tests `libl/user_yywrap` et `libl/user_main`).
- **`lex.yy.c` ne doit appeler ni `exit`, ni `isatty`, ni `errno`.** Le sujet n'autorise que `<stdio.h>`, `<string.h>` et les quatre fonctions d'allocation.
- **Les noms internes de ton scanner** doivent commencer par `yy` ou `YY`. Le test `actions/namespace` déclare `state`, `buf`, `table`… côté utilisateur.

## Ajouter un test

1. Crée `tests/<cat>/<nom>.l`, et éventuellement `<nom>.in`, `<nom>.in2`…
2. Ajoute la ligne `<nom><TAB><étiquette><TAB><description>` dans `tests/<cat>/index`.
3. Lance `make golden` pour régénérer `expected/` avec lex.

## Vérifier le testeur lui-même

`make selftest` lance toute la suite en prenant lex comme ft_lex, avec une libl de référence. Tout passe, sauf `symbols` : le code généré par flex appelle `exit` et `isatty`, ce que le sujet t'interdit.
