#!/usr/bin/env bash
# Fonctions partagees par tous les scripts du testeur.

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)

abspath() { case "$1" in /*) printf '%s' "$1" ;; *) printf '%s/%s' "$PWD" "$1" ;; esac; }

# Une commande (ex. FT_LEX=lex) est cherchee dans le PATH, un chemin est rendu absolu.
resolve_cmd() {
	case "$1" in
		*/*) abspath "$1" ;;
		*) command -v "$1" || printf '%s' "$1" ;;
	esac
}

FT_LEX=$(resolve_cmd "${FT_LEX:-$ROOT/../ft_lex}")
LIBL=$(abspath "${LIBL:-$ROOT/../libl.a}")
REF_LEX=$(resolve_cmd "${REF_LEX:-lex}")
CC=${CC:-cc}
VALGRIND=${VALGRIND:-0}
LEX_TIMEOUT=${LEX_TIMEOUT:-10}
RUN_TIMEOUT=${RUN_TIMEOUT:-5}
LOGDIR=$ROOT/logs
WORK=${WORK:-$ROOT/.work}
STUB_LIBL=$ROOT/.work/stub/libl.a

mkdir -p "$LOGDIR" "$WORK"

if [ -t 1 ] || [ -n "$FORCE_COLOR" ]; then
	GREEN=$'\e[32m'; RED=$'\e[31m'; YELLOW=$'\e[33m'; BLUE=$'\e[36m'; BOLD=$'\e[1m'; RESET=$'\e[0m'
else
	GREEN=; RED=; YELLOW=; BLUE=; BOLD=; RESET=
fi

VG_LEX=(valgrind -q --error-exitcode=99 --leak-check=full
	--errors-for-leak-kinds=definite,indirect,possible
	--show-leak-kinds=definite,indirect,possible)
VG_RUN=(valgrind -q --error-exitcode=99 --leak-check=full --errors-for-leak-kinds=definite)

# ---------------------------------------------------------------- rapport
CAT=""; CAT_IDX=0; CAT_OK=0; CAT_KO=0; CAT_WARN=0; CAT_COL=0
TOTAL_OK=0; TOTAL_KO=0; TOTAL_WARN=0

begin_cat() {
	CAT=$1; CAT_IDX=0; CAT_OK=0; CAT_KO=0; CAT_WARN=0; CAT_COL=0
	LOG=$LOGDIR/$CAT.log
	: > "$LOG"
	printf '%s%-10s%s: ' "$BOLD$BLUE" "$CAT" "$RESET"
}

_tick() {
	CAT_IDX=$((CAT_IDX + 1))
	if [ $CAT_COL -ge 12 ]; then printf '\n%12s' ''; CAT_COL=0; fi
	CAT_COL=$((CAT_COL + 1))
}

# res_ok
res_ok() { _tick; CAT_OK=$((CAT_OK + 1)); printf '%s%d.OK%s ' "$GREEN" "$CAT_IDX" "$RESET"; }

# res_ko NAME DESC REASON [DETAIL]
res_ko() {
	_tick; CAT_KO=$((CAT_KO + 1)); printf '%s%d.KO%s ' "$RED" "$CAT_IDX" "$RESET"
	_log KO "$@"
}
# res_warn NAME DESC REASON [DETAIL]
res_warn() {
	_tick; CAT_WARN=$((CAT_WARN + 1)); printf '%s%d.WARN%s ' "$YELLOW" "$CAT_IDX" "$RESET"
	_log WARN "$@"
}
_log() {
	{
		printf '[%s] #%d %s/%s : %s\n' "$1" "$CAT_IDX" "$CAT" "$2" "$3"
		[ -f "$ROOT/tests/$CAT/$2.l" ] && printf '    fichier : tests/%s/%s.l\n' "$CAT" "$2"
		printf '    raison  : %s\n' "$4"
		[ -n "$5" ] && printf '%s\n' "$5" | sed 's/^/    | /'
		printf '\n'
	} >> "$LOG"
}

end_cat() {
	printf '\n%12s%s%d OK%s' '' "$GREEN" "$CAT_OK" "$RESET"
	[ $CAT_KO -gt 0 ] && printf ', %s%d KO%s' "$RED" "$CAT_KO" "$RESET"
	[ $CAT_WARN -gt 0 ] && printf ', %s%d WARN%s' "$YELLOW" "$CAT_WARN" "$RESET"
	[ $((CAT_KO + CAT_WARN)) -gt 0 ] && printf '  -> details : logs/%s.log' "$CAT"
	printf '\n'
	TOTAL_OK=$((TOTAL_OK + CAT_OK)); TOTAL_KO=$((TOTAL_KO + CAT_KO)); TOTAL_WARN=$((TOTAL_WARN + CAT_WARN))
}

# ---------------------------------------------------------------- execution
# run_lex DIR ARGS... : lance ft_lex dans DIR, stdin = /dev/null sauf si STDIN_FILE
# -> LEX_RC, DIR/lex.stdout, DIR/lex.stderr
run_lex() {
	local dir=$1; shift
	local vg=()
	[ "$VALGRIND" = 1 ] && vg=("${VG_LEX[@]}")
	(
		cd "$dir" || exit 98
		timeout -k 2 "$LEX_TIMEOUT" "${vg[@]}" "$FT_LEX" "$@" \
			< "${STDIN_FILE:-/dev/null}" > lex.stdout 2> lex.stderr
	)
	LEX_RC=$?
}

# run_scan DIR INPUT : lance DIR/scan -> SCAN_RC, DIR/scan.stdout
run_scan() {
	local dir=$1 input=$2
	local vg=()
	[ "$VALGRIND" = 1 ] && vg=("${VG_RUN[@]}")
	(cd "$dir" && timeout -k 2 "$RUN_TIMEOUT" "${vg[@]}" ./scan < "$input" > scan.stdout 2> scan.stderr)
	SCAN_RC=$?
}

# crash_reason RC WHO STDERR_FILE : affiche une raison si RC indique un plantage
crash_reason() {
	local rc=$1 who=$2 err=$3
	if [ "$rc" -eq 124 ] || [ "$rc" -eq 137 ]; then
		printf '%s : TIMEOUT (boucle infinie ou trop lent)' "$who"
	elif [ "$rc" -eq 99 ] && [ "$VALGRIND" = 1 ]; then
		printf '%s : erreur Valgrind (memoire ou fuite)' "$who"
	elif [ "$rc" -gt 128 ]; then
		local sig=$((rc - 128)) sname
		sname=$(kill -l "$sig" 2>/dev/null)
		printf '%s : CRASH (signal %s%s)' "$who" "$sig" "${sname:+ SIG$sname}"
	fi
}

head_file() { [ -f "$1" ] && head -c "${2:-1500}" "$1"; }

require_ft_lex() {
	if [ ! -x "$FT_LEX" ]; then
		printf '%sft_lex introuvable : %s%s\n' "$RED" "$FT_LEX" "$RESET" >&2
		printf 'Compile ton projet ou passe FT_LEX=chemin/vers/ft_lex\n' >&2
		exit 2
	fi
}

require_libl() {
	if [ ! -f "$LIBL" ]; then
		printf '%slibl introuvable : %s%s\n' "$RED" "$LIBL" "$RESET" >&2
		printf 'Compile ta libl ou passe LIBL=chemin/vers/libl.a\n' >&2
		exit 2
	fi
}

build_stub_libl() {
	[ -f "$STUB_LIBL" ] && return
	mkdir -p "$(dirname "$STUB_LIBL")"
	local d; d=$(dirname "$STUB_LIBL")
	$CC -c -o "$d/libmain.o" "$ROOT/lib/stub/libmain.c" &&
	$CC -c -o "$d/libyywrap.o" "$ROOT/lib/stub/libyywrap.c" &&
	ar rcs "$STUB_LIBL" "$d/libmain.o" "$d/libyywrap.o"
}

newwork() { mktemp -d "$WORK/t.XXXXXX"; }

# nm : retire le @VERSION (glibc) et le _ initial ajoute par macOS
strip_sym() {
	if [ "$(uname)" = Darwin ]; then sed 's/@.*//;s/^_//'; else sed 's/@.*//'; fi
}

# filtre la sortie de `nm -u` (Linux : "U sym", macOS : "sym", archives : "x.o:")
undef_syms() {
	awk '$1 == "U" { print $2; next } NF == 1 && $1 !~ /:$/ { print $1 }' | strip_sym
}
