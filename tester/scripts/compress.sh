#!/usr/bin/env bash
# Bonus "Compression" : avec COMPRESS_FLAG, lex.yy.c doit etre au moins 2x plus
# petit qu'sans, et le scanner doit toujours donner la meme sortie.

run_compress() {
	begin_cat compress
	if [ -z "$COMPRESS_FLAG" ]; then
		printf '%sCOMPRESS_FLAG non defini (ex. make compress COMPRESS_FLAG=-C)%s' "$YELLOW" "$RESET"
		end_cat; return
	fi
	require_libl
	local name tag desc w s1 s2 exp=$ROOT/expected/compress
	while IFS=$'\t' read -r name tag desc <&3; do
		[ -z "$name" ] && continue
		w=$(newwork); cp "$ROOT/tests/compress/$name.l" "$w/"
		run_lex "$w" -t "$name.l"; cp "$w/lex.stdout" "$w/plain.c"
		local why; why=$(crash_reason "$LEX_RC" ft_lex)
		[ -n "$why" ] && { res_ko "$name" "$desc" "$why (sans option)"; continue; }
		run_lex "$w" -t "$COMPRESS_FLAG" "$name.l"; cp "$w/lex.stdout" "$w/packed.c"
		why=$(crash_reason "$LEX_RC" ft_lex)
		[ -n "$why" ] && { res_ko "$name" "$desc" "$why (avec $COMPRESS_FLAG)"; continue; }
		[ "$LEX_RC" -eq 0 ] || { res_ko "$name" "$desc" "ft_lex $COMPRESS_FLAG echoue (exit $LEX_RC)"; continue; }
		s1=$(wc -c < "$w/plain.c"); s2=$(wc -c < "$w/packed.c")
		if [ "$s2" -eq 0 ] || [ $((s1 * 10 / s2)) -lt 20 ]; then
			res_ko "$name" "$desc" "gain insuffisant : $s1 -> $s2 octets (il faut au moins x2)"; continue
		fi
		if ! (cd "$w" && $CC -w -o scan packed.c "$LIBL" > cc.log 2>&1); then
			res_ko "$name" "$desc" "la version compressee ne compile pas" "$(head -5 "$w/cc.log")"; continue
		fi
		run_scan "$w" "$ROOT/tests/compress/$name.in"
		if ! cmp -s "$w/scan.stdout" "$exp/$name.in.expected"; then
			res_ko "$name" "$desc" "la version compressee ne donne pas la meme sortie que lex" \
				"$(diff <(cat -A "$exp/$name.in.expected") <(cat -A "$w/scan.stdout") | head -10)"
			continue
		fi
		res_ok
		printf '%s(%s -> %s)%s ' "$BLUE" "$s1" "$s2" "$RESET"
	done 3< "$ROOT/tests/compress/index"
	end_cat
}
