# Testeur ft_lex — a cloner/copier a la racine du depot ft_lex (dossier tester/)
#
#   make            tests obligatoires (= make m)
#   make <cat>      une categorie : parse regex startcond matching actions robust cli libl symbols
#   make v / v<cat> idem sous valgrind (ft_lex et scanners generes)
#   make ext        bonus : extensions flex (non exigees par le sujet)
#   make compress   bonus : compression (COMPRESS_FLAG=ton_option)
#   make memfail    securite memoire : fait echouer chaque malloc un par un (ASan)
#                   MEMFAIL_FILES="x.l" MEMFAIL_MODES="once from" MEMFAIL_MAX=300
#   make fuzz       fuzzing par mutation des .l (FUZZ_N=500)
#   make fuzzdiff   fuzzing differentiel des scanners contre lex (necessite lex)
#   make golden     regenere expected/ avec lex (necessite lex)
#   make selftest   lance la suite sur lex lui-meme (valide le testeur)
#   make one T=cat/nom   un seul test
#   make p2 / vp2   uniquement les tests de la phase 2 (liste : phases/phase2.list)
#
# Variables : FT_LEX=../ft_lex  LIBL=../libl.a  CC=cc  COMPRESS_FLAG=
#             VERBOSE=0 (grille compacte)  DETAIL_LINES=12 (lignes de detail par echec)

FT_LEX ?= ../ft_lex
LIBL ?= ../libl.a
export FT_LEX LIBL CC COMPRESS_FLAG FUZZ_N FUZZ_SEED REF_LEX VERBOSE DETAIL_LINES
export MEMFAIL_FILES MEMFAIL_MODES MEMFAIL_MAX MEMFAIL_JOBS

MANDATORY := parse regex startcond matching actions robust cli libl symbols
CATS := $(MANDATORY) ext compress
RUN := ./scripts/run.sh

all: m

m:
	@$(RUN) $(MANDATORY)

$(CATS):
	@$(RUN) $@

v vm:
	@VALGRIND=1 $(RUN) $(MANDATORY)

$(addprefix v,$(CATS)):
	@VALGRIND=1 $(RUN) $(@:v%=%)

one:
	@ONLY=$(notdir $(T)) $(RUN) $(patsubst %/,%,$(dir $(T)))

# Phase 2 : seules les categories a base de .l ont des tests selectionnes
P2_LIST := $(CURDIR)/phases/phase2.list
P2_CATS := parse regex matching actions robust

p2:
	@SELECT=$(P2_LIST) $(RUN) $(P2_CATS)

vp2:
	@SELECT=$(P2_LIST) VALGRIND=1 $(RUN) $(P2_CATS)

memfail:
	@$(RUN) memfail

fuzz:
	@python3 scripts/fuzz.py mutate

fuzzdiff: stub
	@python3 scripts/fuzz.py diff

golden:
	@./scripts/golden.sh

stub:
	@bash -c '. scripts/common.sh && build_stub_libl'

selftest: stub
	@FT_LEX=lex LIBL=.work/stub/libl.a $(RUN) $(MANDATORY) ext || true
	@echo "(selftest : seuls 'symbols' et quelques WARN sont attendus en echec, flex utilise exit/isatty)"

clean:
	rm -rf .work logs

.PHONY: all m v vm one p2 vp2 memfail fuzz fuzzdiff golden stub selftest clean $(CATS) $(addprefix v,$(CATS))
