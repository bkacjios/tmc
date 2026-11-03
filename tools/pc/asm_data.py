#!/usr/bin/env python3
"""Extract the data blocks from a handwritten ARM/Thumb assembly file.

The PC build replaces the code in asm/src/*.s with C, but some of those files
also define data tables that C code uses. This filter (stdin to stdout) keeps
the .include lines and every global label (`name::`) whose contents are data
directives, and drops everything else: instructions, local labels and literal
pools.
"""

import re
import sys

DATA = re.compile(r"^\s*(?:\.(?:byte|2byte|4byte|hword|incbin|space|string)|ptr)\b")
ALIGN = re.compile(r"^\s*\.(align|balign|p2align)\b")
INCLUDE = re.compile(r"^\s*\.include\b")
CONDITIONAL = re.compile(r"^\s*\.(if\w*|else|endif)\b")
GLOBAL_LABEL = re.compile(r"^(\w+)::")
LOCAL_LABEL = re.compile(r"^\s*(\w+):(?!:)")
LABELED_DATA = re.compile(r"^\s*\w+:(?!:)\s*((?:\.(?:byte|2byte|4byte|hword|incbin|space)|ptr)\b.*)$")


def strip_comment(line):
    for marker in ("@", "//"):
        i = line.find(marker)
        if i >= 0:
            line = line[:i]
    return line.rstrip()


def main():
    lines = [strip_comment(l) for l in sys.stdin.read().splitlines()]
    # "label: .4byte x" inside a table is data; the label itself is not needed.
    lines = [LABELED_DATA.sub(r"\1", l) for l in lines]
    statements = [l for l in lines if l.strip()]

    # Walk backwards so a label knows whether data follows it.
    keep = [False] * len(statements)
    data_follows = False
    for i in range(len(statements) - 1, -1, -1):
        s = statements[i]
        if DATA.match(s):
            keep[i] = True
            data_follows = True
        elif GLOBAL_LABEL.match(s):
            keep[i] = data_follows
        elif ALIGN.match(s):
            keep[i] = data_follows
        elif INCLUDE.match(s) or CONDITIONAL.match(s):
            keep[i] = True
        else:
            data_follows = False

    # Data directly after a dropped label belongs to code (literal pools).
    out = []
    in_block = False
    for s, k in zip(statements, keep):
        if GLOBAL_LABEL.match(s):
            in_block = k
        elif LOCAL_LABEL.match(s) or not (DATA.match(s) or ALIGN.match(s)):
            if not (INCLUDE.match(s) or CONDITIONAL.match(s)):
                in_block = False
        if INCLUDE.match(s) or CONDITIONAL.match(s):
            out.append(s)
        elif k and (in_block or GLOBAL_LABEL.match(s)):
            out.append(s)
    # The includes may switch sections (struct macros), so select .rodata after them.
    first_data = next((i for i, s in enumerate(out) if not INCLUDE.match(s)), len(out))
    out.insert(first_data, "\t.section .rodata")
    print("\n".join(out))


if __name__ == "__main__":
    main()
