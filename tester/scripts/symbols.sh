#!/usr/bin/env bash
# Le sujet n'autorise dans lex.yy.c que <stdio.h>, <string.h>, malloc, realloc,
# calloc et free. On compile le lex.yy.c de plusieurs .l et on liste les
# symboles non definis (nm -u) qui ne sont pas dans lib/allowed_symbols.txt.

SYMBOL_SOURCES=(
	"matching/default_echo"
	"actions/yymore"
	"actions/yyless"
	"actions/input"
	"actions/unput_many"
	"actions/reject"
	"actions/array_yymore"
	"actions/begin_action"
	"actions/long_token"
	"regex/trail_var"
	"regex/bol_eol"
	"regex/keywords"
	"startcond/excl"
)

run_symbols() {
	begin_cat symbols
	local allowed; allowed=$(newwork)/allowed
	grep -v '^#' "$ROOT/lib/allowed_symbols.txt" | sed '/^$/d' > "$allowed"
	local t name w bad
	for t in "${SYMBOL_SOURCES[@]}"; do
		name=${t//\//_}
		[ -n "$ONLY" ] && [ "$ONLY" != "$name" ] && continue
		w=$(newwork)
		cp "$ROOT/tests/$t.l" "$w/"
		run_lex "$w" "$(basename "$t").l"
		if [ "$LEX_RC" -ne 0 ]; then res_ko "$name" "lex.yy.c de $t" "ft_lex echoue (exit $LEX_RC)"; continue; fi
		if ! (cd "$w" && $CC -c -O0 -fno-stack-protector -U_FORTIFY_SOURCE -w -o yy.o lex.yy.c > cc.log 2>&1); then
			res_ko "$name" "lex.yy.c de $t" "lex.yy.c ne compile pas" "$(head -5 "$w/cc.log")"; continue
		fi
		bad=$(nm -u "$w/yy.o" | undef_syms |
			grep -vxF -f "$allowed" | grep -vxE 'yywrap|main' | sort -u)
		if [ -n "$bad" ]; then
			res_ko "$name" "lex.yy.c de $t" "lex.yy.c appelle des fonctions interdites" "$bad"
		else
			res_ok
		fi
	done
	end_cat
}
