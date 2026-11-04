#!/usr/bin/env bash
# Regenere expected/ a partir de la reference (REF_LEX, par defaut `lex`).
# Usage : scripts/golden.sh [categorie...]
. "$(dirname "$0")/common.sh"

command -v "$REF_LEX" > /dev/null || { echo "reference introuvable : $REF_LEX" >&2; exit 2; }
build_stub_libl || { echo "impossible de compiler la libl de reference" >&2; exit 2; }

cats=("$@")
[ ${#cats[@]} -eq 0 ] && cats=(parse regex startcond matching actions robust ext compress)

problems=0
for cat in "${cats[@]}"; do
	src=$ROOT/tests/$cat; exp=$ROOT/expected/$cat
	[ -f "$src/index" ] || continue
	rm -rf "$exp"; mkdir -p "$exp"
	n=0
	while IFS=$'\t' read -r name tag desc <&3; do
		[ -z "$name" ] && continue
		[ "$tag" = undefined ] && continue
		n=$((n + 1))
		w=$(newwork)
		cp "$src/$name.l" "$w/"
		(cd "$w" && timeout 20 "$REF_LEX" "$name.l" < /dev/null > /dev/null 2> lexerr)
		rc=$?
		if [ $rc -ne 0 ]; then
			echo 1 > "$exp/$name.status"
			cp "$w/lexerr" "$exp/$name.lexerr"
			if compgen -G "$src/$name.in*" > /dev/null; then
				echo "  [$cat/$name] lex echoue mais le test a des entrees" >&2; problems=$((problems + 1))
			fi
			rm -rf "$w"; continue
		fi
		echo 0 > "$exp/$name.status"
		if compgen -G "$src/$name.in*" > /dev/null; then
			if ! (cd "$w" && $CC -w -o scan lex.yy.c "$STUB_LIBL" > cc.log 2>&1); then
				echo "  [$cat/$name] le lex.yy.c de reference ne compile pas :" >&2
				head -3 "$w/cc.log" | sed 's/^/      /' >&2
				problems=$((problems + 1)); rm -rf "$w"; continue
			fi
			for in in "$src/$name".in*; do
				suf=${in##*.}
				(cd "$w" && timeout 10 ./scan < "$in" > "$exp/$name.$suf.expected" 2> /dev/null)
				echo $? > "$exp/$name.$suf.rc"
			done
		fi
		rm -rf "$w"
	done 3< "$src/index"
	printf '%-10s : %d attendus generes\n' "$cat" "$n"
done
[ $problems -gt 0 ] && echo "$problems probleme(s) dans le corpus" >&2
exit $((problems > 0))
