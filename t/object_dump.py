import inspect
import itertools
import os
import re
import subprocess
import sys


def test_match(pattern, testname):
    top = os.path.join(os.path.dirname(__file__), '..')
    cmd = [os.path.join(top, 'mouselisp'), '--test-object-dump', testname]
    raw_out = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
    out = raw_out.decode('utf-8')
    match = re.match(pattern, out, re.MULTILINE)
    if match is None:
        fname = os.path.basename(__file__)
        lineno = inspect.currentframe().f_back.f_lineno
        print(f'{fname}: {lineno}: test failed: {testname}', file=sys.stderr)
        sys.exit(1)
    return match


def test(b):
    if not b:
        fname = os.path.basename(__file__)
        lineno = inspect.currentframe().f_back.f_lineno
        print(f'{fname}: {lineno}: test failed.', file=sys.stderr)
        sys.exit(1)


# nil
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ NIL\n'
    r'--------------------\n'
    r'\Z',
    'nil'
)

# number
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ NUMBER: 42\n'
    r'--------------------\n'
    r'\Z',
    'number'
)

# bool
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ BOOL: true\n'
    r'--------------------\n'
    r'\Z',
    'bool'
)

# name
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ NAME: dog\n'
    r'--------------------\n'
    r'\Z',
    'name'
)

# cons
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ CONS:\n'
    r'  [0-9a-f]+ NUMBER: -42\n'
    r'  [0-9a-f]+ BOOL: false\n'
    r'--------------------\n'
    r'\Z',
    'cons'
)

# builtin function
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ BUILTIN FUNCTION: [0-9a-f]+\n'
    r'--------------------\n'
    r'\Z',
    'builtin-function'
)

# normal function
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ NORMAL FUNCTION \(ARGS, BODY\):\n'
    r'  [0-9a-f]+ CONS:\n'
    r'    [0-9a-f]+ NAME: x\n'
    r'    [0-9a-f]+ NIL\n'
    r'  [0-9a-f]+ NAME: x\n'
    r'--------------------\n'
    r'\Z',
    'normal-function'
)

# pointer
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ POINTER: [0-9a-f]+\n'
    r'--------------------\n'
    r'\Z',
    'pointer'
)

# macro
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ MACRO \(ARGS, BODY\):\n'
    r'  [0-9a-f]+ CONS:\n'
    r'    [0-9a-f]+ NAME: x\n'
    r'    [0-9a-f]+ NIL\n'
    r'  [0-9a-f]+ NAME: x\n'
    r'--------------------\n'
    r'\Z',
    'macro'
)

# same reference
m = test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'(?P<cons>[0-9a-f]+) CONS:\n'
    r'  (?P<same1>[0-9a-f]+) BOOL: true\n'
    r'  (?P<same2>[0-9a-f]+) \(dup\)\n'
    r'--------------------\n'
    r'\Z',
    'same-reference'
)
test(m['cons'] != m['same1'])
test(m['cons'] != m['same2'])
test(m['same1'] == m['same2'])

# circular list
m = test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'(?P<list1>[0-9a-f]+) CONS:\n'
    r'  (?P<elelm1>[0-9a-f]+) NAME: 1st\n'
    r'  (?P<list2>[0-9a-f]+) CONS:\n'
    r'    (?P<elelm2>[0-9a-f]+) NAME: 2nd\n'
    r'    (?P<list3>[0-9a-f]+) CONS:\n'
    r'      (?P<elem3>[0-9a-f]+) NAME: 3rd\n'
    r'      (?P<circular>[0-9a-f]+) \(dup\)\n'
    r'--------------------\n'
    r'\Z',
    'circular-list'
)
mkv = m.groupdict()
for t in itertools.combinations(mkv, 2):
    if set(t) == {'list1', 'circular'}:
        test(mkv[t[0]] == mkv[t[1]])
    else:
        test(mkv[t[0]] != mkv[t[1]])
