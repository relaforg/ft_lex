#!/usr/bin/env bash
# Un essai d'injection : fait echouer la N-ieme allocation de ft_lex sur un .l.
# Usage (appele par memfail.sh via xargs -P) : memfail_one.sh BIN INPUT OUTDIR MODE N
# Produit dans OUTDIR/N/ : rc, stderr, out.md5 (si lex.yy.c a ete ecrit)
bin=$1 input=$2 outdir=$3 mode=$4 n=$5
d=$outdir/$n
mkdir -p "$d"
cd "$d" || exit 1
FAILALLOC_AT=$n FAILALLOC_MODE=$mode FAILALLOC_TRACE=1 \
	timeout -k 2 "${LEX_TIMEOUT:-10}" "$bin" "$input" < /dev/null > /dev/null 2> stderr
echo $? > rc
if [ -f lex.yy.c ]; then
	md5sum < lex.yy.c | cut -d' ' -f1 > out.md5
	rm -f lex.yy.c
fi
