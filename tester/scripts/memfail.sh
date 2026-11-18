#!/usr/bin/env bash
# Categorie "memfail" : securite memoire quand malloc echoue.
#
# Principe : on recompile ft_lex avec AddressSanitizer + UBSan et on intercepte
# malloc/calloc/realloc/strdup/strndup/fopen (lib/failalloc). Pour chaque .l,
# on compte les allocations (K), puis on relance ft_lex K fois en faisant
# echouer la 1re, puis la 2e, ... puis la K-ieme. A chaque essai, ft_lex doit :
#   - ne pas planter, ne pas boucler            (sinon KO)
#   - n'avoir aucune erreur memoire ni fuite    (sinon KO : double free, use-after-free...)
#   - ne pas "reussir" (exit 0) avec une sortie differente : l'echec aurait ete
#     ignore silencieusement                    (sinon KO)
#   - afficher un message d'erreur s'il echoue  (sinon WARN)
#
# Variables :
#   MEMFAIL_FILES="a.l b.l"   fichiers a tester (defaut : tous les .l des categories obligatoires)
#   MEMFAIL_MODES="once from" once : seul le N-ieme appel echoue (defaut)
#                             from : le N-ieme et tous les suivants (teste le code de nettoyage)
#   MEMFAIL_MAX=300           au-dela de K allocations, on echantillonne ~MAX essais
#   MEMFAIL_JOBS=nproc        essais en parallele

MEMFAIL_CATS="parse regex startcond actions robust matching"
MEMFAIL_DIR=$WORK/memfail
MEMFAIL_BIN=$MEMFAIL_DIR/ft_lex
PROJ=$(cd "$ROOT/.." && pwd)

# Compile ft_lex instrumente a partir des sources du projet (variables SRCS/VPATH du Makefile)
memfail_build() {
	local srcs vpath files=() s
	srcs=$(make -s -C "$PROJ" -pn 2>/dev/null | sed -n 's/^SRCS := //p' | head -n1)
	vpath=$(make -s -C "$PROJ" -pn 2>/dev/null | sed -n 's/^VPATH := //p' | head -n1)
	vpath=${vpath:-src}
	if [ -z "$srcs" ]; then
		printf '%simpossible de lire SRCS dans %s/Makefile%s\n' "$RED" "$PROJ" "$RESET" >&2
		return 1
	fi
	for s in $srcs; do files+=("$PROJ/$vpath/$s"); done
	mkdir -p "$MEMFAIL_DIR"
	$CC -g -O0 -fno-omit-frame-pointer \
		-fsanitize=address,undefined -fno-sanitize-recover=undefined \
		-std=c99 -D_POSIX_C_SOURCE=200809L -I"$PROJ/includes" \
		"${files[@]}" "$ROOT/lib/failalloc/failalloc.c" \
		-Wl,--wrap=malloc,--wrap=calloc,--wrap=realloc,--wrap=strdup,--wrap=strndup,--wrap=fopen \
		-o "$MEMFAIL_BIN" 2> "$MEMFAIL_DIR/build.log"
}

# ASan n'accepte qu'un binaire nomme exactement "llvm-symbolizer" : si seul
# llvm-symbolizer-NN existe (Ubuntu), on cree un lien avec le bon nom.
# On prefere la version qui correspond a celle de clang.
memfail_symbolizer() {
	local s major
	[ -n "$ASAN_SYMBOLIZER_PATH" ] && return
	command -v llvm-symbolizer > /dev/null && return
	major=$($CC --version 2>/dev/null | sed -n 's/.*clang version \([0-9]*\).*/\1/p' | head -n1)
	s=/usr/bin/llvm-symbolizer-$major
	[ -n "$major" ] && [ -x "$s" ] || s=$(ls /usr/bin/llvm-symbolizer-* 2>/dev/null | sort -V | tail -n1)
	[ -n "$s" ] || return
	mkdir -p "$MEMFAIL_DIR/bin"
	ln -sf "$s" "$MEMFAIL_DIR/bin/llvm-symbolizer"
	export ASAN_SYMBOLIZER_PATH=$MEMFAIL_DIR/bin/llvm-symbolizer
}

memfail_inputs() {
	if [ -n "$MEMFAIL_FILES" ]; then
		local f
		for f in $MEMFAIL_FILES; do abspath "$f"; printf '\n'; done
		return
	fi
	local c
	for c in $MEMFAIL_CATS; do
		find "$ROOT/tests/$c" -maxdepth 1 -name '*.l' 2>/dev/null
	done | sort
}

# memfail_plan K : liste des N a essayer (tous si K <= MEMFAIL_MAX, sinon un echantillon
# regulier qui garde toujours les 50 premiers et les 50 derniers)
memfail_plan() {
	local k=$1 max=${MEMFAIL_MAX:-300} step n
	if [ "$k" -le "$max" ]; then
		seq 1 "$k"
		return
	fi
	step=$(( (k - 100) / (max - 100) + 1 ))
	{
		seq 1 50
		seq 51 "$step" $((k - 50))
		seq $((k - 49)) "$k"
	} | sort -n -u
}

run_memfail() {
	begin_cat memfail
	if [ "$(uname)" = Darwin ]; then
		res_warn build "injection d'echecs malloc" "non supporte sur macOS (ld ne connait pas --wrap)"
		end_cat
		return
	fi
	if ! memfail_build; then
		res_ko build "compilation ASan + failalloc" "echec de compilation" \
			"$(head -n "$DETAIL_LINES" "$MEMFAIL_DIR/build.log")"
		end_cat
		return
	fi
	memfail_symbolizer
	export ASAN_OPTIONS=${ASAN_OPTIONS:-detect_leaks=1:abort_on_error=0}
	export UBSAN_OPTIONS=${UBSAN_OPTIONS:-print_stacktrace=1}
	export LEX_TIMEOUT

	local jobs=${MEMFAIL_JOBS:-$(nproc 2>/dev/null || echo 4)}
	local modes=${MEMFAIL_MODES:-once}
	local -A bug_count=() bug_example=()
	local input name ref k ref_rc ref_md5 mode out tried ko warn detail
	local n rc md5 kind loc inj msg sig reason

	while IFS= read -r input; do
		[ -f "$input" ] || continue
		name=${input#"$ROOT/tests/"}
		name=${name%.l}
		ref=$MEMFAIL_DIR/ref
		rm -rf "$ref"; mkdir -p "$ref"

		# 1) essai de reference, sans injection : compte K et memorise la sortie normale
		(cd "$ref" && FAILALLOC_COUNT_FILE=count timeout -k 2 "$LEX_TIMEOUT" \
			"$MEMFAIL_BIN" "$input" < /dev/null > /dev/null 2> stderr)
		ref_rc=$?
		if [ ! -s "$ref/count" ]; then
			res_ko "$name" "(sans injection)" \
				"le binaire instrumente ne s'est pas termine normalement (rc=$ref_rc)" \
				"$(head -n "$DETAIL_LINES" "$ref/stderr")"
			continue
		fi
		k=$(cat "$ref/count")
		ref_md5=-
		[ -f "$ref/lex.yy.c" ] && ref_md5=$(md5sum < "$ref/lex.yy.c" | cut -d' ' -f1)
		if grep -qE 'ERROR: (Address|Leak)Sanitizer|runtime error:' "$ref/stderr"; then
			res_ko "$name" "(sans injection)" "erreur memoire meme sans echec de malloc" \
				"$(grep -A8 -E 'ERROR: |runtime error:' "$ref/stderr" | head -n "$DETAIL_LINES")"
			continue
		fi

		# 2) un essai par allocation, en parallele
		ko=0; warn=0; tried=0; detail=""
		local -A seen=()
		for mode in $modes; do
			out=$MEMFAIL_DIR/runs/$mode
			rm -rf "$out"; mkdir -p "$out"
			memfail_plan "$k" | xargs -P "$jobs" -n 1 \
				"$ROOT/scripts/memfail_one.sh" "$MEMFAIL_BIN" "$input" "$out" "$mode"

			# 3) verdict de chaque essai
			while IFS=$'\t' read -r n rc md5 kind loc inj msg; do
				tried=$((tried + 1))
				reason=""
				if [ "$kind" != - ]; then
					reason="$kind dans $loc"
				elif [ "$rc" -eq 124 ] || [ "$rc" -eq 137 ]; then
					reason="TIMEOUT"
				elif [ "$rc" -gt 128 ]; then
					reason="CRASH (signal $((rc - 128)))"
				elif [ "$rc" -eq 0 ] && { [ "$ref_rc" -ne 0 ] || [ "$md5" != "$ref_md5" ]; }; then
					reason="echec ignore : exit 0 mais sortie differente de la normale"
				elif [ "$rc" -ne 0 ] && [ "$msg" = 0 ]; then
					sig="WARN|$inj"
					if [ -z "${seen[$sig]}" ]; then
						seen[$sig]=1
						warn=$((warn + 1))
						detail+="WARN  exit $rc sans aucun message d'erreur"$'\n'
						detail+="      malloc qui echoue : $inj"$'\n'
						detail+="      repro : FAILALLOC_AT=$n FAILALLOC_MODE=$mode $MEMFAIL_BIN $input"$'\n'
					fi
					continue
				else
					continue
				fi
				# un meme bug est signale une seule fois par fichier
				sig="$reason"
				[ "$kind" != - ] || sig="$reason|$inj"
				bug_count[$sig]=$(( ${bug_count[$sig]:-0} + 1 ))
				[ -n "${bug_example[$sig]}" ] ||
					bug_example[$sig]="FAILALLOC_AT=$n FAILALLOC_MODE=$mode $MEMFAIL_BIN $input"
				[ -n "${seen[$sig]}" ] && continue
				seen[$sig]=1
				ko=$((ko + 1))
				detail+="KO    $reason"$'\n'
				detail+="      malloc qui echoue : $inj  (essai #$n, mode $mode)"$'\n'
				detail+="      repro : cd \$(mktemp -d) && FAILALLOC_AT=$n FAILALLOC_MODE=$mode FAILALLOC_TRACE=1 $MEMFAIL_BIN $input"$'\n'
			done < <(find "$out" -mindepth 1 -maxdepth 1 -type d  \
				| awk -v proj="$PROJ" -f "$ROOT/scripts/memfail_classify.awk" | sort -n)
			rm -rf "$out"
		done
		unset seen

		if [ "$ko" -gt 0 ]; then
			res_ko "$name" "$k allocs, $tried essais" "$ko bug(s) memoire distinct(s)" "$detail"
		elif [ "$warn" -gt 0 ]; then
			res_warn "$name" "$k allocs, $tried essais" "echec(s) sans message d'erreur" "$detail"
		else
			res_ok "$name" "$k allocs, $tried essais"
		fi
	done < <(memfail_inputs)

	if [ "${#bug_count[@]}" -gt 0 ]; then
		local summary
		summary=$(for sig in "${!bug_count[@]}"; do
				printf '%d\t%s\t%s\n' "${bug_count[$sig]}" "${sig/|/ <- malloc qui echoue : }" "${bug_example[$sig]}"
			done | sort -t$'\t' -k1,1rn | awk -F'\t' '{
				printf "%6d essais  %s\n              ex. : cd $(mktemp -d) && %s\n", $1, $2, $3 }')
		printf '\n  %sBugs distincts (toutes entrees confondues) :%s\n' "$BOLD" "$RESET"
		printf '%s\n' "$summary" | sed 's/^/  /'
		printf '\nBugs distincts (toutes entrees confondues) :\n%s\n' "$summary" >> "$LOG"
	fi
	end_cat
}
