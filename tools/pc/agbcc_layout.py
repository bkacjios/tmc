#!/usr/bin/env python3
"""Make host GCC lay out structs, unions and constant data the way agbcc does.

agbcc rounds the size and alignment of every struct and union up to a multiple
of 4 bytes (ARM STRUCTURE_SIZE_BOUNDARY = 32) unless the type is packed. Host
GCC has no option for this, so this filter rewrites preprocessed C (stdin to
stdout) by inserting __attribute__((aligned(4))) into every struct or union
definition that comes from the project's own files and is not packed.

Code from system headers (absolute paths in line markers) is left untouched.
"""

import re
import sys

LINE_MARKER = re.compile(r'^#\s*\d+\s+"([^"]*)"((?:\s+\d+)*)\s*$')
KEYWORD = re.compile(r"\b(struct|union)\b")
IDENT_OR_SPACE = re.compile(r"\s*(?:[A-Za-z_]\w*\s*)?")
ATTRIBUTE = "__attribute__((aligned(4)))"


def split_regions(lines):
    """Yield (is_system, text) chunks, keeping line markers in the output."""
    system = False
    chunk = []
    for line in lines:
        m = LINE_MARKER.match(line)
        if m:
            if chunk:
                yield system, "".join(chunk)
                chunk = []
            yield True, line  # emit marker unchanged
            # Flag 3 also marks lines that expand a system header's macro, so
            # go by the file: project files are relative paths.
            system = m.group(1).startswith("/")
            continue
        chunk.append(line)
    if chunk:
        yield system, "".join(chunk)


def matching_brace(text, start):
    depth = 0
    for i in range(start, len(text)):
        c = text[i]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i
    return -1


def is_packed_after(text, close):
    # Attributes after the closing brace, e.g. "} __attribute__((packed)) Name;"
    tail = text[close + 1 : close + 200]
    m = re.match(r"\s*((?:__attribute__\s*\(\(.*?\)\)\s*)*)", tail, re.S)
    return m is not None and "packed" in m.group(1)


def is_packed_before(text, kw_end, brace):
    return "packed" in text[kw_end:brace]


# A top-level "const T name[];" is a tentative definition. GCC emits the object
# where it first appears, agbcc where it is defined, which changes the order of
# .rodata. Declaring it extern keeps agbcc's order.
TENTATIVE_CONST_ARRAY = re.compile(r"^(const\b[^;=(){}]*\[\s*\]\s*;)", re.M)


# Function-local statics: GCC emits them in reverse order, agbcc in declaration
# order where the function is. Hoist them to file scope in front of their
# function, renamed so they stay unique.
FUNCTION_START = re.compile(r"\)\s*\{")


def skip_literal(text, i):
    """If text[i] opens a string or char literal, returns the index of its closing quote."""
    quote = text[i]
    if quote not in "\"'":
        return i
    j = i + 1
    while j < len(text) and text[j] != quote:
        j += 2 if text[j] == "\\" else 1
    return j


def statement_end(text, start):
    depth = 0
    i = start
    while i < len(text):
        i = skip_literal(text, i)
        c = text[i]
        if c in "{(":
            depth += 1
        elif c in "})":
            depth -= 1
        elif c == ";" and depth == 0:
            return i + 1
        i += 1
    return -1


def hoist_local_statics(text):
    out = []
    pos = 0
    depth = 0
    i = 0
    func_start = None  # index in text where the current top-level item starts
    item_start = 0
    while i < len(text):
        i = skip_literal(text, i)
        c = text[i]
        if c == "{":
            if depth == 0:
                func_start = item_start
                body_start = i
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0 and func_start is not None:
                body = text[body_start : i + 1]
                header = text[func_start:body_start]
                if header.rstrip().endswith(")") and "static" in body:
                    hoisted, body = extract_statics(body, header)
                    if hoisted:
                        out.append(text[pos:func_start])
                        out.append(hoisted)
                        out.append(header)
                        out.append(body)
                        pos = i + 1
                func_start = None
                item_start = i + 1
        elif c == ";" and depth == 0:
            item_start = i + 1
        elif c == "#" and (i == 0 or text[i - 1] == "\n"):
            # line markers between items
            nl = text.find("\n", i)
            if depth == 0:
                item_start = nl + 1
            i = nl
        i += 1
    out.append(text[pos:])
    return "".join(out)


STATIC_KEYWORD = re.compile(r"(?<=[{;}])\s*static\b")


def find_local_static(body, start=0):
    """Returns (start, end, name) of the next "static ... const ... = ...;" statement."""
    for m in STATIC_KEYWORD.finditer(body, start):
        decl_start = m.start() + len(m.group(0)) - len("static")
        end = statement_end(body, decl_start)
        if end < 0:
            continue
        stmt = body[decl_start:end]
        eq = stmt.find("=")
        if eq < 0:
            continue
        decl = stmt[:eq]
        if not re.search(r"\bconst\b", decl):
            continue
        bracket = decl.find("[")
        head = decl[:bracket] if bracket >= 0 else decl
        names = re.findall(r"[A-Za-z_]\w*", head)
        if not names:
            continue
        return decl_start, end, names[-1]
    return None


def rename(text, old, new, count=0):
    """Renames identifier old to new, except where it is a struct member."""
    def repl(m):
        before = text[: m.start()].rstrip()
        if before.endswith(".") or before.endswith("->"):
            return m.group(0)
        return new
    return re.sub(r"\b%s\b" % re.escape(old), repl, text, count=count)


def extract_statics(body, header):
    m = re.search(r"(\w+)\s*\(", header)
    func = m.group(1) if m else "f"
    hoisted = []
    if re.search(r"\btypedef\b", body):
        return "", body  # the statics may use the local types
    while True:
        found = find_local_static(body)
        if found is None:
            break
        start, end, name = found
        new_name = "%s__%s" % (name, func)
        decl = rename(body[start:end], name, new_name, count=1)
        rest = rename(body[end:], name, new_name)
        body = body[:start] + rest
        hoisted.append(decl + "\n")
    return "".join(hoisted), body


# A trailing zero-length array member ("u8 data[0];"): agbcc stores the
# initializer data for it, GCC drops it. A flexible array member keeps it.
TRAILING_ZERO_ARRAY = re.compile(r"(\b(?!return\b)[A-Za-z_]\w*\s+[A-Za-z_]\w*\s*)\[\s*0\s*\]\s*;(\s*\})")


def rewrite(text):
    text = TENTATIVE_CONST_ARRAY.sub(r"extern \1", text)
    text = TRAILING_ZERO_ARRAY.sub(r"\1[];\2", text)
    text = hoist_local_statics(text)
    out = []
    pos = 0
    for m in KEYWORD.finditer(text):
        if m.start() < pos:
            continue
        kw_end = m.end()
        # optional attributes and tag between keyword and '{'
        j = kw_end
        while True:
            a = re.match(r"\s*__attribute__\s*\(\(.*?\)\)", text[j:], re.S)
            if not a:
                break
            j += a.end()
        t = IDENT_OR_SPACE.match(text, j)
        j = t.end()
        if j >= len(text) or text[j] != "{":
            continue  # forward declaration or usage, not a definition
        close = matching_brace(text, j)
        if close < 0:
            continue
        if is_packed_after(text, close) or is_packed_before(text, kw_end, j):
            continue
        out.append(text[pos:kw_end])
        out.append(" " + ATTRIBUTE)
        pos = kw_end
    out.append(text[pos:])
    return "".join(out)


def main():
    lines = sys.stdin.readlines()
    # Group consecutive project-code chunks so definitions spanning #line
    # markers inside the same file still match their braces.
    result = []
    pending = []
    for system, chunk in split_regions(lines):
        if LINE_MARKER.match(chunk) or not system:
            pending.append((system, chunk))
            continue
        if pending:
            result.append(flush(pending))
            pending = []
        result.append(chunk)
    if pending:
        result.append(flush(pending))
    sys.stdout.write("".join(result))


def flush(pending):
    # Rewrite project code only; markers are passed through verbatim. Markers
    # are replaced with placeholders so brace matching sees continuous code.
    markers = []
    parts = []
    for system, chunk in pending:
        if LINE_MARKER.match(chunk):
            markers.append(chunk)
            parts.append("\0%d\0\n" % (len(markers) - 1))
        else:
            parts.append(chunk)
    text = rewrite("".join(parts))
    return re.sub(r"\0(\d+)\0\n", lambda m: markers[int(m.group(1))], text)


if __name__ == "__main__":
    main()
