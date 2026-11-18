# Analyse les resultats des essais d'injection (un dossier par essai, lu sur stdin).
# Variable : proj = racine du projet (pour reconnaitre les frames de ses sources)
# Sortie TSV, une ligne par essai :
#   N  RC  MD5  ERREUR  LIEU_ERREUR  ALLOC_FORCEE  MESSAGE(0/1)
# ERREUR      : type d'erreur sanitizer ("" si aucune)
# LIEU_ERREUR : 1re frame dans src/ de la pile de l'erreur (pour une fuite : ou le bloc a ete alloue)
# ALLOC_FORCEE: 1re frame dans src/ de l'allocation qu'on a fait echouer

# "    #2 0x4f2a in add_state /proj/src/dfa.c:68:6" -> "add_state (src/dfa.c:68)"
function srcframe(line,    i, rest, fn, path, parts) {
	i = index(line, proj "/src/")
	if (i == 0 || line !~ /^ *#[0-9]+ /)
		return ""
	rest = line
	sub(/^ *#[0-9]+ 0x[0-9a-f]+ in /, "", rest)
	fn = rest
	sub(/ .*/, "", fn)
	path = substr(line, i + length(proj) + 1)
	split(path, parts, ":")
	return fn " (" parts[1] ":" parts[2] ")"
}

function readline1(file,    l) {
	l = ""
	if ((getline l < file) <= 0)
		l = ""
	close(file)
	return l
}

{
	dir = $0
	n = dir
	sub(/.*\//, "", n)
	rc = readline1(dir "/rc")
	md5 = readline1(dir "/out.md5")
	if (md5 == "")
		md5 = "-"
	kind = ""; loc = ""; inj = ""; msg = 0; state = 0
	f = dir "/stderr"
	while ((getline line < f) > 0) {
		if (line ~ /^failalloc: echec force/) { state = 1; continue }
		if (line ~ /^failalloc: fin/) { state = 0; continue }
		if (state == 1) {
			if (inj == "") inj = srcframe(line)
			continue
		}
		if (kind == "" && match(line, /ERROR: (AddressSanitizer|LeakSanitizer): /)) {
			kind = substr(line, RSTART + RLENGTH)
			if (line ~ /LeakSanitizer/)
				kind = "fuite memoire"
			sub(/ (on|at|for|in) .*/, "", kind)
			sub(/ 0x[0-9a-f].*/, "", kind)
			state = 2
			continue
		}
		if (kind == "" && line ~ /runtime error:/) {
			kind = "UB : " line
			sub(/.*runtime error: /, "UB : ", kind)
			i = index(line, proj "/src/")
			if (i > 0) {
				loc = substr(line, i + length(proj) + 1)
				sub(/: runtime error.*/, "", loc)
			}
			state = 2
			continue
		}
		if (state == 2) {
			if (loc == "") loc = srcframe(line)
			continue
		}
		if (line !~ /^[ \t]*$/)
			msg = 1
	}
	close(f)
	if (rc == "") rc = 255
	if (kind == "") kind = "-"
	if (inj == "") inj = "?"
	if (loc == "") loc = "?"
	printf "%s\t%s\t%s\t%s\t%s\t%s\t%d\n", n, rc, md5, kind, loc, inj, msg
}
