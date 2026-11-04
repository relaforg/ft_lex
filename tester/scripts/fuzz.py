#!/usr/bin/env python3
"""Fuzzing de ft_lex.

  mutate : mute aleatoirement les .l du corpus et verifie que ft_lex ne plante
           jamais (signal, timeout). Les cas fautifs sont gardes dans logs/fuzz/.
  diff   : pour chaque test 'posix' accepte par lex, genere des entrees aleatoires
           et compare le scanner de ft_lex a celui de lex (necessite lex).

Variables : FT_LEX, LIBL, REF_LEX, CC, FUZZ_N (iterations), FUZZ_SEED.
"""
import os
import random
import shutil
import signal
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FT_LEX = os.path.abspath(os.environ.get("FT_LEX", os.path.join(ROOT, "..", "ft_lex")))
if "/" not in os.environ.get("FT_LEX", "/"):
    FT_LEX = shutil.which(os.environ["FT_LEX"]) or os.environ["FT_LEX"]
LIBL = os.path.abspath(os.environ.get("LIBL", os.path.join(ROOT, "..", "libl.a")))
REF_LEX = os.environ.get("REF_LEX", "lex")
CC = os.environ.get("CC", "cc")
N = int(os.environ.get("FUZZ_N", "500"))
SEED = int(os.environ.get("FUZZ_SEED", random.randrange(1 << 30)))
OUT = os.path.join(ROOT, "logs", "fuzz")
STUB = os.path.join(ROOT, ".work", "stub", "libl.a")

TTY = sys.stdout.isatty()
G, R, Y, Z = ("\033[32m", "\033[31m", "\033[33m", "\033[0m") if TTY else ("", "", "", "")

TOKENS = [b"%%", b"%{", b"%}", b"%s", b"%x", b"{", b"}", b"(", b")", b"[", b"]",
          b"[^", b"[:alpha:]", b"\"", b"\\", b"/", b"^", b"$", b"|", b"*", b"+",
          b"?", b"{1,3}", b"{D}", b"<A>", b"<", b">", b"\n", b" ", b"\t", b"\0",
          b"/*", b"*/", b"'", b"yymore();", b"REJECT;", b"\xff"]


def corpus():
    files = []
    for cat in sorted(os.listdir(os.path.join(ROOT, "tests"))):
        d = os.path.join(ROOT, "tests", cat)
        for f in sorted(os.listdir(d)):
            if f.endswith(".l"):
                files.append(os.path.join(d, f))
    return files


def mutate(data, rnd):
    data = bytearray(data)
    for _ in range(rnd.randint(1, 6)):
        op = rnd.randrange(6)
        pos = rnd.randint(0, len(data))
        if op == 0 and data:  # supprime un octet
            del data[min(pos, len(data) - 1)]
        elif op == 1:  # insere un token
            data[pos:pos] = rnd.choice(TOKENS)
        elif op == 2 and data:  # remplace un octet
            data[min(pos, len(data) - 1)] = rnd.randrange(256)
        elif op == 3 and data:  # duplique une tranche
            a = rnd.randint(0, len(data) - 1)
            b = min(len(data), a + rnd.randint(1, 40))
            data[pos:pos] = data[a:b]
        elif op == 4 and data:  # tronque
            data = data[:pos]
        elif op == 5:  # duplique une ligne
            lines = bytes(data).split(b"\n")
            i = rnd.randrange(len(lines))
            lines.insert(i, lines[i])
            data = bytearray(b"\n".join(lines))
    return bytes(data)


def run(cmd, cwd, stdin=None, timeout=10):
    try:
        p = subprocess.run(cmd, cwd=cwd, stdin=stdin or subprocess.DEVNULL,
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeout)
        return p.returncode, p.stdout, p.stderr
    except subprocess.TimeoutExpired:
        return "timeout", b"", b""


def describe(rc):
    if rc == "timeout":
        return "TIMEOUT"
    if isinstance(rc, int) and rc < 0:
        try:
            return "CRASH (%s)" % signal.Signals(-rc).name
        except ValueError:
            return "CRASH (signal %d)" % -rc
    return None


def do_mutate():
    rnd = random.Random(SEED)
    files = corpus()
    os.makedirs(OUT, exist_ok=True)
    bad = 0
    print("fuzz mutate : %d iterations, graine %d" % (N, SEED))
    for i in range(N):
        src = rnd.choice(files)
        data = mutate(open(src, "rb").read(), rnd)
        with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, ".work")) as w:
            open(os.path.join(w, "f.l"), "wb").write(data)
            rc, _, err = run([FT_LEX, "f.l"], w)
            why = describe(rc)
            if why:
                bad += 1
                dst = os.path.join(OUT, "crash_%d_%d.l" % (SEED, i))
                open(dst, "wb").write(data)
                print("\n%s%s%s sur une mutation de %s -> %s" % (R, why, Z, os.path.relpath(src, ROOT), os.path.relpath(dst, ROOT)))
        if (i + 1) % 50 == 0:
            sys.stdout.write("%s%d%s " % (G if not bad else Y, i + 1, Z))
            sys.stdout.flush()
    print("\n%s%d crash/timeout sur %d%s" % (R if bad else G, bad, N, Z))
    return 1 if bad else 0


def build(lex_cmd, libl, w, lfile):
    rc, _, err = run([lex_cmd, lfile], w, timeout=30)
    if rc != 0:
        return False
    rc, _, err = run([CC, "-w", "-o", "scan", "lex.yy.c", libl], w, timeout=60)
    return rc == 0


def random_input(rnd, alphabet, samples):
    if samples and rnd.random() < 0.5:  # mutation d'une entree existante
        return mutate(rnd.choice(samples), rnd)
    n = rnd.randint(0, 60)
    return bytes(rnd.choice(alphabet) for _ in range(n))


def do_diff():
    if not shutil.which(REF_LEX):
        print("reference introuvable : %s" % REF_LEX)
        return 2
    rnd = random.Random(SEED)
    per_test = max(1, N // 50)
    bad = total = 0
    os.makedirs(OUT, exist_ok=True)
    print("fuzz diff : %d entrees par test, graine %d" % (per_test, SEED))
    for cat in ["regex", "startcond", "matching", "actions"]:
        for line in open(os.path.join(ROOT, "tests", cat, "index")):
            name, tag, _ = line.rstrip("\n").split("\t", 2)
            if tag != "posix":
                continue
            d = os.path.join(ROOT, "tests", cat)
            samples = [open(os.path.join(d, f), "rb").read() for f in os.listdir(d)
                       if f.startswith(name + ".in")]
            if not samples or any(len(s) > 4096 for s in samples):
                continue
            src = open(os.path.join(d, name + ".l"), "rb").read()
            alphabet = sorted(set(src.split(b"%%")[1] if b"%%" in src else src) | set(b"".join(samples)) | {10})
            with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, ".work")) as w:
                ref, mine = os.path.join(w, "ref"), os.path.join(w, "mine")
                os.makedirs(ref); os.makedirs(mine)
                for x in (ref, mine):
                    shutil.copy(os.path.join(d, name + ".l"), x)
                if not build(REF_LEX, STUB, ref, name + ".l"):
                    continue
                if not build(FT_LEX, LIBL, mine, name + ".l"):
                    print("%s%s/%s%s : ft_lex ne produit pas de scanner compilable" % (R, cat, name, Z))
                    bad += 1
                    continue
                for k in range(per_test):
                    data = random_input(rnd, alphabet, samples)
                    inp = os.path.join(w, "in")
                    open(inp, "wb").write(data)
                    r1 = run(["./scan"], ref, stdin=open(inp, "rb"), timeout=5)
                    r2 = run(["./scan"], mine, stdin=open(inp, "rb"), timeout=5)
                    total += 1
                    if r1[0] != r2[0] or r1[1] != r2[1]:
                        bad += 1
                        dst = os.path.join(OUT, "diff_%s_%s_%d.in" % (cat, name, k))
                        open(dst, "wb").write(data)
                        print("%sdifference%s %s/%s, entree %r -> %s" % (R, Z, cat, name, data[:60], os.path.relpath(dst, ROOT)))
                        print("    lex    : %r" % r1[1][:120])
                        print("    ft_lex : %r" % (r2[1][:120] if r2[0] != "timeout" else "TIMEOUT"))
                        break
    print("%s%d difference(s) sur %d entrees%s" % (R if bad else G, bad, total, Z))
    return 1 if bad else 0


if __name__ == "__main__":
    os.makedirs(os.path.join(ROOT, ".work"), exist_ok=True)
    mode = sys.argv[1] if len(sys.argv) > 1 else "mutate"
    sys.exit(do_mutate() if mode == "mutate" else do_diff())
