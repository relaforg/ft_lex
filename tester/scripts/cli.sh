#!/usr/bin/env bash
# Ligne de commande POSIX : lex [-t] [-n|-v] [file...]

SAMPLE='%%
[a-z]+	printf("<%s>", yytext);
'
SAMPLE_OUT='<ab> <cd>'

# cli_build DIR CFILE : compile CFILE avec la libl, lance sur "ab cd", verifie la sortie
cli_build() {
	local dir=$1 cfile=$2
	(cd "$dir" && $CC -w -o scan "$cfile" "$LIBL" > cc.log 2>&1) || { CLI_WHY="$cfile ne compile pas"; CLI_DETAIL=$(head -10 "$dir/cc.log"); return 1; }
	printf 'ab cd' > "$dir/in.txt"
	run_scan "$dir" "$dir/in.txt"
	if [ "$(cat "$dir/scan.stdout")" != "$SAMPLE_OUT" ]; then
		CLI_WHY="le scanner genere ne donne pas '$SAMPLE_OUT'"
		CLI_DETAIL="obtenu : $(head -c 200 "$dir/scan.stdout")"
		return 1
	fi
}

# cli_case NAME DESC : appelle la fonction t_NAME, qui renvoie 0 (OK), 1 (KO) ou 2 (WARN)
cli_case() {
	local name=$1 desc=$2
	local w; w=$(newwork)
	CLI_WHY=""; CLI_DETAIL=""; LEX_RC=0
	printf '%s' "$SAMPLE" > "$w/sample.l"
	"t_$name" "$w"
	local r=$?
	local why; why=$(crash_reason "$LEX_RC" ft_lex)
	if [ -n "$why" ]; then res_ko "$name" "$desc" "$why" "$(head_file "$w/lex.stderr")"; return; fi
	case $r in
		0) res_ok ;;
		2) res_warn "$name" "$desc" "$CLI_WHY" "$CLI_DETAIL" ;;
		*) res_ko "$name" "$desc" "$CLI_WHY" "$CLI_DETAIL" ;;
	esac
}

expect_success() {
	[ "$LEX_RC" -eq 0 ] && return 0
	CLI_WHY="exit $LEX_RC au lieu de 0"; CLI_DETAIL=$(head_file "$1/lex.stderr" 600); return 1
}
expect_failure() {
	[ "$LEX_RC" -ne 0 ] && return 0
	CLI_WHY="exit 0 alors qu'une erreur etait attendue"; return 1
}
expect_message() {
	[ -s "$1/lex.stderr" ] || [ -s "$1/lex.stdout" ] && return 0
	CLI_WHY="erreur sans aucun message"; return 2
}

t_file_operand() { run_lex "$1" sample.l; expect_success "$1" || return 1
	[ -f "$1/lex.yy.c" ] || { CLI_WHY="lex.yy.c absent"; return 1; }; cli_build "$1" lex.yy.c; }
t_opt_t() { run_lex "$1" -t sample.l; expect_success "$1" || return 1
	[ -f "$1/lex.yy.c" ] && { CLI_WHY="-t ne doit pas creer lex.yy.c"; return 1; }
	cp "$1/lex.stdout" "$1/out.c"; cli_build "$1" out.c; }
t_stdin_no_operand() { STDIN_FILE=$1/sample.l run_lex "$1"; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_stdin_dash() { STDIN_FILE=$1/sample.l run_lex "$1" -; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_opt_t_stdin() { STDIN_FILE=$1/sample.l run_lex "$1" -t; expect_success "$1" || return 1
	cp "$1/lex.stdout" "$1/out.c"; cli_build "$1" out.c; }
t_multi_files() {
	printf 'L [a-z]\n' > "$1/a.l"
	printf '%%%%\n{L}+\tprintf("<%%s>", yytext);\n' > "$1/b.l"
	run_lex "$1" a.l b.l; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_multi_files_three() {
	printf 'L [a-z]\n' > "$1/a.l"
	printf '%%%%\n{L}+\tprintf("<%%s>", tag(yytext));\n' > "$1/b.l"
	printf '%%%%\nconst char *tag(const char *s) { return s; }\n' > "$1/c.l"
	printf '%%{\nconst char *tag(const char *s);\n%%}\n' > "$1/a0.l"
	run_lex "$1" a0.l a.l b.l c.l; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_multi_dash_and_file() {
	printf 'L [a-z]\n' > "$1/a.l"
	printf '%%%%\n{L}+\tprintf("<%%s>", yytext);\n' > "$1/b.l"
	STDIN_FILE=$1/a.l run_lex "$1" - b.l; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_opt_n() { run_lex "$1" -n sample.l; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_opt_v() { run_lex "$1" -v sample.l; expect_success "$1" || return 1
	cli_build "$1" lex.yy.c || return 1
	[ -s "$1/lex.stdout" ] || [ -s "$1/lex.stderr" ] || { CLI_WHY="-v n'affiche aucune statistique"; return 1; }; }
t_opt_t_v() { run_lex "$1" -t -v sample.l; expect_success "$1" || return 1
	cp "$1/lex.stdout" "$1/out.c"
	cli_build "$1" out.c || { CLI_WHY="avec -t -v, stdout doit rester du C valide (stats sur stderr) : $CLI_WHY"; return 1; }
	[ -s "$1/lex.stderr" ] || { CLI_WHY="-t -v : statistiques attendues sur stderr"; return 1; }; }
t_grouped_opts() { run_lex "$1" -tn sample.l; expect_success "$1" || return 1
	cp "$1/lex.stdout" "$1/out.c"; cli_build "$1" out.c; }
t_double_dash() { run_lex "$1" -- sample.l; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_n_and_v() { run_lex "$1" -n -v sample.l; return 0; }
t_overwrite() { printf 'garbage\n' > "$1/lex.yy.c"; run_lex "$1" sample.l; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_table_sizes_stats() { printf '%%p 3000\n%s' "$SAMPLE" > "$1/t.l"; run_lex "$1" t.l; expect_success "$1" || return 1; cli_build "$1" lex.yy.c; }
t_missing_file() { run_lex "$1" nope.l; expect_failure "$1" || return 1; expect_message "$1"; }
t_directory() { mkdir "$1/dir.l"; run_lex "$1" dir.l; expect_failure "$1" || return 1; expect_message "$1"; }
t_unknown_option() { run_lex "$1" -Z sample.l; expect_failure "$1" || return 1; expect_message "$1"; }
t_unreadable() {
	[ "$(id -u)" = 0 ] && return 0
	chmod 000 "$1/sample.l"; run_lex "$1" sample.l; chmod 644 "$1/sample.l"
	expect_failure "$1" || return 1; expect_message "$1"; }
t_second_file_missing() { run_lex "$1" sample.l nope.l; expect_failure "$1" || return 1; expect_message "$1"; }
t_syntax_error_status() { printf '%%%%\n(ab\tprintf("x");\n' > "$1/bad.l"; run_lex "$1" bad.l; expect_failure "$1"; }
t_empty_stdin() { STDIN_FILE=/dev/null run_lex "$1"; expect_failure "$1"; }

run_cli() {
	require_libl
	begin_cat cli
	local c
	for c in \
		"file_operand|lex fichier.l -> lex.yy.c" \
		"opt_t|-t : C sur stdout, pas de lex.yy.c" \
		"stdin_no_operand|sans operande : lecture sur stdin" \
		"stdin_dash|operande - : lecture sur stdin" \
		"opt_t_stdin|-t avec stdin" \
		"multi_files|deux fichiers concatenes" \
		"multi_files_three|quatre fichiers concatenes (3 sections)" \
		"multi_dash_and_file|- puis un fichier" \
		"opt_n|-n" \
		"opt_v|-v : statistiques" \
		"opt_t_v|-t -v : statistiques sur stderr" \
		"grouped_opts|options groupees -tn" \
		"double_dash|-- fichier.l" \
		"n_and_v|-n -v ensemble (ne doit pas planter)" \
		"overwrite|ecrase un lex.yy.c existant" \
		"table_sizes_stats|%p sans -n" \
		"missing_file|fichier inexistant -> erreur" \
		"directory|dossier en operande -> erreur" \
		"unknown_option|option inconnue -> erreur" \
		"unreadable|fichier illisible -> erreur" \
		"second_file_missing|second fichier inexistant -> erreur" \
		"syntax_error_status|erreur de syntaxe -> exit > 0" \
		"empty_stdin|stdin vide -> erreur"; do
		[ -n "$ONLY" ] && [ "$ONLY" != "${c%%|*}" ] && continue
		cli_case "${c%%|*}" "${c#*|}"
	done
	end_cat
}
