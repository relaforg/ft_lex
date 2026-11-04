#!/usr/bin/env bash
# Usage : scripts/run.sh categorie...
# Variables : FT_LEX, LIBL, VALGRIND=1, ONLY=nom_du_test, COMPRESS_FLAG
DIR=$(dirname "$0")
. "$DIR/common.sh"
. "$DIR/cases.sh"
. "$DIR/cli.sh"
. "$DIR/libl.sh"
. "$DIR/symbols.sh"
. "$DIR/compress.sh"

require_ft_lex
if [ "$VALGRIND" = 1 ] && ! command -v valgrind > /dev/null; then
	echo "valgrind introuvable" >&2; exit 2
fi

printf '%sft_lex%s : %s\n%slibl%s   : %s\n' "$BOLD" "$RESET" "$FT_LEX" "$BOLD" "$RESET" "$LIBL"
[ "$VALGRIND" = 1 ] && printf '%s(sous valgrind)%s\n' "$YELLOW" "$RESET"
printf '\n'

for cat in "$@"; do
	case $cat in
		cli) run_cli ;;
		libl) run_libl ;;
		symbols) run_symbols ;;
		compress) run_compress ;;
		*)
			if [ -f "$ROOT/tests/$cat/index" ]; then
				[ "$cat" != robust ] && require_libl
				run_cases "$cat"
			else
				echo "categorie inconnue : $cat" >&2
			fi ;;
	esac
done

rm -rf "$WORK"/t.*
printf '\n%sTOTAL%s : %s%d OK%s, %s%d KO%s, %s%d WARN%s\n' "$BOLD" "$RESET" \
	"$GREEN" "$TOTAL_OK" "$RESET" "$RED" "$TOTAL_KO" "$RESET" "$YELLOW" "$TOTAL_WARN" "$RESET"
[ $TOTAL_KO -eq 0 ]
