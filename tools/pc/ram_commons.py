#!/usr/bin/env python3
"""Turn the game's RAM variable declarations into definitions (64-bit build).

On the GBA, RAM variables are only declared in C ("extern Entity gEntities[72];")
and placed at fixed addresses by linker.ld. In the 64-bit PC build their types
with pointers are larger, so they can't keep the GBA addresses. Instead this
filter drops "extern" from each file-scope declaration of one, which makes it
a tentative definition. With -fcommon, the linker allocates each variable once
in .bss, as large as its largest declaration, so variables that several files
declare with different types (shared buffers) get room for every use.

Variables that no C file declares with a size keep their GBA address
(gen_linker.py --provide-ram).

Usage: ram_commons.py <RAM symbol list>   (preprocessed C on stdin/stdout)
"""

import re
import sys

LINE_MARKER = re.compile(r"^#.*$", re.M)
EXTERN = re.compile(r"^(\s*)extern\b")
ARRAY_NAME = re.compile(r"(\w+)\s*(?:\[[^\]]*\]\s*)*$")
FUNCTION_POINTER_NAME = re.compile(r"\(\s*\*\s*(\w+)\s*\)\s*\(")


def declared_name(statement):
    """The variable a single declaration declares, or None."""
    body = statement.rstrip().rstrip(";").strip()
    if "," in body or "=" in body:
        return None
    m = FUNCTION_POINTER_NAME.search(body)
    if m:
        return m.group(1)
    if "(" in body:  # a function
        return None
    if re.search(r"\[\s*\]\s*$", body):  # unsized: it has no size to define it with
        return None
    m = ARRAY_NAME.search(body)
    return m.group(1) if m else None


def rewrite(text, ram):
    out = []
    depth = 0
    start = 0  # start of the current top-level statement
    i = 0
    n = len(text)
    while i < n:
        c = text[i]
        if c == "#" and (i == 0 or text[i - 1] == "\n"):  # line marker
            end = text.find("\n", i)
            i = n if end < 0 else end + 1
            if depth == 0:
                out.append(text[start:i])
                start = i
            continue
        if c in "\"'":
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            i = j + 1
            continue
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                out.append(text[start : i + 1])
                start = i + 1
        elif c == ";" and depth == 0:
            statement = text[start : i + 1]
            m = EXTERN.match(statement)
            if m and declared_name(statement[m.end() :]) in ram:
                statement = m.group(1) + statement[m.end() :]
            out.append(statement)
            start = i + 1
        i += 1
    out.append(text[start:])
    return "".join(out)


def main():
    ram = set(open(sys.argv[1]).read().split())
    sys.stdout.write(rewrite(sys.stdin.read(), ram))


if __name__ == "__main__":
    main()
