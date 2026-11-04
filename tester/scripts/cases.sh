#!/usr/bin/env bash
# Categories basees sur des fichiers .l : tests/<cat>/index + expected/<cat>/
#
# Etiquettes (2e colonne de index) :
#   posix      comportement defini par POSIX : toute difference est un KO
#   flex       non impose par POSIX, mais c'est ce que fait lex : difference = WARN
#   undefined  POSIX ne definit pas le resultat : seul un crash/timeout/fuite est KO
#   ext        extension flex (bonus) : difference = KO, dans `make ext` seulement

# fail_for TAG ... : KO pour posix/ext, WARN pour flex
fail_for() {
	local tag=$1; shift
	if [ "$tag" = flex ]; then res_warn "$@"; else res_ko "$@"; fi
}

case_one() {
	local cat=$1 name=$2 tag=$3 desc=$4
	local src=$ROOT/tests/$cat exp=$ROOT/expected/$cat
	local w; w=$(newwork)
	cp "$src/$name.l" "$w/"

	run_lex "$w" "$name.l"
	local why; why=$(crash_reason "$LEX_RC" ft_lex)
	if [ -n "$why" ]; then
		res_ko "$name" "$desc" "$why" "$(head_file "$w/lex.stderr")"
		return
	fi
	if [ "$tag" = undefined ]; then
		res_ok
		return
	fi

	if [ ! -f "$exp/$name.status" ]; then
		res_warn "$name" "$desc" "pas d'attendu : lance 'make golden'"
		return
	fi
	local want; want=$(cat "$exp/$name.status")

	if [ "$want" != 0 ]; then
		if [ "$LEX_RC" -eq 0 ]; then
			fail_for "$tag" "$name" "$desc" "ft_lex accepte ce fichier (exit 0) alors que lex le refuse" \
				"message de lex : $(head -3 "$exp/$name.lexerr" 2>/dev/null)"
		elif ! grep -Eqs "$name\.l:[0-9]+" "$w/lex.stderr" "$w/lex.stdout"; then
			res_warn "$name" "$desc" "erreur bien detectee, mais le message ne contient pas '$name.l:<ligne>'" \
				"$(head_file "$w/lex.stderr" 400)"
		else
			res_ok
		fi
		return
	fi

	if [ "$LEX_RC" -ne 0 ]; then
		fail_for "$tag" "$name" "$desc" "ft_lex refuse ce fichier (exit $LEX_RC) alors que lex l'accepte" \
			"$(head_file "$w/lex.stderr" 600)"
		return
	fi
	if [ ! -f "$w/lex.yy.c" ]; then
		fail_for "$tag" "$name" "$desc" "exit 0 mais aucun lex.yy.c genere"
		return
	fi

	local inputs=("$src/$name".in*)
	[ -e "${inputs[0]}" ] || { res_ok; return; }

	if ! (cd "$w" && $CC -w -o scan lex.yy.c "$LIBL" > cc.log 2>&1); then
		fail_for "$tag" "$name" "$desc" "lex.yy.c ne compile pas (cc -w lex.yy.c libl)" "$(head -15 "$w/cc.log")"
		return
	fi

	local in suf
	for in in "${inputs[@]}"; do
		suf=${in##*.}
		run_scan "$w" "$in"
		why=$(crash_reason "$SCAN_RC" "scanner genere")
		if [ -n "$why" ]; then
			res_ko "$name" "$desc" "$why (entree $suf)" "$(head_file "$w/scan.stderr" 600)"
			return
		fi
		local want_rc; want_rc=$(cat "$exp/$name.$suf.rc")
		if ! cmp -s "$w/scan.stdout" "$exp/$name.$suf.expected" || [ "$SCAN_RC" != "$want_rc" ]; then
			local d
			d=$(printf 'entree (%s) : %s\n' "$suf" "$(head -c 200 "$in" | cat -A | head -3)"
				printf 'exit attendu %s, obtenu %s\n' "$want_rc" "$SCAN_RC"
				printf -- '--- attendu (lex)   +++ obtenu (ft_lex), vu avec cat -A\n'
				diff <(cat -A "$exp/$name.$suf.expected") <(cat -A "$w/scan.stdout") | head -20)
			fail_for "$tag" "$name" "$desc" "le scanner genere ne donne pas la meme sortie que celui de lex" "$d"
			return
		fi
	done
	res_ok
}

run_cases() {
	local cat=$1
	begin_cat "$cat"
	local name tag desc
	while IFS=$'\t' read -r name tag desc <&3; do
		[ -z "$name" ] && continue
		[ -n "$ONLY" ] && [ "$ONLY" != "$name" ] && continue
		case_one "$cat" "$name" "$tag" "$desc"
	done 3< "$ROOT/tests/$cat/index"
	end_cat
}
