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
make unit         # tests unitaires de ft_atoi_base, contrat d'ABI compris (vunit : sous valgrind)
make memfail      # sécurité mémoire : fait échouer chaque malloc, un par un (ASan)
make fuzz         # mutations aléatoires des .l : ft_lex ne doit jamais planter
make fuzzdiff     # entrées aléatoires : ton scanner contre celui de lex
```

Si tes binaires sont ailleurs : `make FT_LEX=../build/ft_lex LIBL=../libl/libl.a`.

Chaque test affiche une ligne `OK`/`KO`/`WARN` avec son nom et ce qu'il vérifie. Sous chaque `KO`/`WARN` : le fichier `.l` concerné, la raison, et un extrait du détail (diff attendu/obtenu, stderr…). Le détail complet est dans `logs/<catégorie>.log`.

`VERBOSE=0` remet l'ancienne grille compacte (`1.OK 2.KO …`), `DETAIL_LINES=N` change la taille des extraits (12 par défaut).

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

## Tests unitaires (`make unit`, `make vunit`)

`unit/atoi_base.c` teste `ft_atoi_base` (`../src/ft_atoi_base.s`, autre chemin : `ATOI_SRC=...`) :
conversions, arrêt sur un caractère hors base, signes et bases invalides.

Chaque appel passe par `unit/abi_check.s`. Ce wrapper met une sentinelle dans chaque registre **callee-saved** de l'ABI System V (`rbx`, `rbp`, `r12`-`r15`, `rsp`) et vérifie au retour qu'elle est intacte. Les tests de bout en bout ne voient pas ce bug : il ne casse l'appelant que si le compilateur avait justement rangé une variable dans ce registre, typiquement en `-O2`.

Les chaînes sont copiées dans des buffers `malloc` de la taille exacte. `make vunit` signale donc toute lecture au-delà du `'\0'`, ce qui est impossible avec un littéral rangé en `.rodata`.

## Sécurité mémoire quand malloc échoue (`make memfail`)

Les tests classiques ne passent que par le chemin « tout va bien ». Les chemins d'erreur (un `malloc` qui renvoie `NULL`) ne sont jamais exécutés, et c'est là que se cachent les fuites, les double free et les déréférencements de `NULL`.

`make memfail` :
1. recompile ft_lex dans `.work/memfail/` avec AddressSanitizer + UBSan, en interceptant `malloc`, `calloc`, `realloc`, `strdup`, `strndup` et `fopen` (`lib/failalloc/`, via `ld --wrap`) ;
2. pour chaque `.l`, compte les K allocations d'une exécution normale ;
3. relance ft_lex K fois, en faisant échouer la 1ʳᵉ, puis la 2ᵉ… puis la K-ième allocation.

À chaque essai, ft_lex doit :

| Vérification | Sinon |
|---|---|
| ne pas planter ni boucler | KO |
| n'avoir aucune erreur ASan/UBSan (fuite, double free, use-after-free, accès à `NULL`…) | KO |
| ne pas finir avec `exit 0` en produisant une sortie différente de la normale (l'échec aurait été ignoré) | KO |
| afficher un message d'erreur quand il échoue | WARN |

Chaque KO indique le bug, **l'allocation qu'on a fait échouer** et une commande pour le reproduire, par exemple :

```sh
cd $(mktemp -d) && FAILALLOC_AT=61 FAILALLOC_TRACE=1 <tester>/.work/memfail/ft_lex fichier.l
```

`FAILALLOC_TRACE=1` affiche la pile de l'appel qu'on fait échouer. La même commande sous `gdb` permet de suivre le chemin d'erreur pas à pas. En fin de catégorie, un résumé regroupe les bugs distincts, toutes entrées confondues.

Variables : `MEMFAIL_FILES="a.l b.l"` (défaut : tous les `.l` des catégories obligatoires), `MEMFAIL_MODES="once from"` (`from` : la N-ième allocation **et toutes les suivantes** échouent, ce qui teste le code de nettoyage), `MEMFAIL_MAX=300` (au-delà, les essais sont échantillonnés), `MEMFAIL_JOBS`. Linux uniquement (`ld --wrap`).

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
