#!/usr/bin/env bash
# libl (POSIX) : main() qui appelle yylex(), yywrap() qui renvoie 1.
# Chacune doit pouvoir etre remplacee par l'utilisateur sans conflit de symboles.

libl_build() { # DIR LFILE INPUT EXPECTED
	local dir=$1 l=$2 input=$3 want=$4
	printf '%s' "$input" > "$dir/in.txt"
	run_lex "$dir" "$l"
	[ "$LEX_RC" -eq 0 ] || { L_WHY="ft_lex echoue (exit $LEX_RC)"; L_DETAIL=$(head_file "$dir/lex.stderr" 500); return 1; }
	(cd "$dir" && $CC -w -o scan lex.yy.c "$LIBL" > cc.log 2>&1) ||
		{ L_WHY="l'edition de liens avec la libl echoue"; L_DETAIL=$(head -10 "$dir/cc.log"); return 1; }
	run_scan "$dir" "$dir/in.txt"
	local why; why=$(crash_reason "$SCAN_RC" scanner)
	[ -n "$why" ] && { L_WHY="$why"; return 1; }
	[ "$(cat "$dir/scan.stdout")" = "$want" ] ||
		{ L_WHY="sortie inattendue"; L_DETAIL="attendu : $want
obtenu  : $(head -c 300 "$dir/scan.stdout")"; return 1; }
}

l_exists() { [ -f "$LIBL" ] || { L_WHY="$LIBL introuvable"; return 1; }; }
l_main_and_yywrap() {
	printf '%%%%\n[a-z]+\tprintf("<%%s>", yytext);\n' > "$1/t.l"
	libl_build "$1" t.l 'ab cd' '<ab> <cd>'; }
l_user_yywrap() {
	printf '%%%%\n[a-z]+\tprintf("<%%s>", yytext);\n%%%%\nint yywrap(void) { printf("<W>"); return 1; }\n' > "$1/t.l"
	libl_build "$1" t.l 'ab' '<ab><W>'; }
l_user_main() {
	printf '%%%%\n[a-z]+\tprintf("<%%s>", yytext);\n%%%%\nint main(void) { yylex(); printf("<M>"); return 0; }\n' > "$1/t.l"
	libl_build "$1" t.l 'ab' '<ab><M>'; }
l_user_both() {
	printf '%%%%\n[a-z]+\tprintf("<%%s>", yytext);\n%%%%\nint yywrap(void) { return 1; }\nint main(void) { yylex(); printf("<M>"); return 0; }\n' > "$1/t.l"
	libl_build "$1" t.l 'ab' '<ab><M>'; }
l_main_return_zero() {
	printf '%%%%\n[a-z]+\tprintf("<%%s>", yytext);\n' > "$1/t.l"
	libl_build "$1" t.l 'ab' '<ab>' || return 1
	[ "$SCAN_RC" -eq 0 ] || { L_WHY="le main de la libl renvoie $SCAN_RC au lieu de 0"; return 1; }; }
l_symbols() {
	local bad
	bad=$(nm -u "$LIBL" 2>/dev/null | undef_syms |
		grep -vxF -f <(grep -v '^#' "$ROOT/lib/allowed_symbols.txt" | sed '/^$/d') | grep -vx 'yylex' | sort -u)
	[ -z "$bad" ] || { L_WHY="la libl appelle des fonctions interdites"; L_DETAIL=$bad; return 1; }; }

run_libl() {
	begin_cat libl
	local c name desc w
	for c in \
		"exists|la libl existe ($(basename "$LIBL"))" \
		"main_and_yywrap|scanner sans main ni yywrap : main et yywrap de la libl" \
		"user_yywrap|yywrap utilisateur + main de la libl (pas de doublon)" \
		"user_main|main utilisateur + yywrap de la libl (pas de doublon)" \
		"user_both|main et yywrap utilisateur" \
		"main_return_zero|le main de la libl renvoie 0" \
		"symbols|la libl n'utilise que les fonctions autorisees"; do
		name=${c%%|*}; desc=${c#*|}
		[ -n "$ONLY" ] && [ "$ONLY" != "$name" ] && continue
		w=$(newwork); L_WHY=""; L_DETAIL=""; LEX_RC=0
		if [ "$name" != exists ] && [ ! -f "$LIBL" ]; then res_ko "$name" "$desc" "libl introuvable"; continue; fi
		if "l_$name" "$w"; then res_ok; else res_ko "$name" "$desc" "$L_WHY" "$L_DETAIL"; fi
	done
	end_cat
}
