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
        print(f'test failed: {testname}', file=sys.stderr)
        sys.exit(1)
    return match


test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ NIL\n'
    r'--------------------\n'
    r'\Z',
    'valid-nil'
)
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ INVALID NIL\n'
    r'--------------------\n'
    r'\Z',
    'invalid-nil'
)
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ NUMBER: 42\.0+\n'
    r'--------------------\n'
    r'\Z',
    'number'
)
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ BOOL: true\n'
    r'--------------------\n'
    r'\Z',
    'bool'
)
test_match(
    r'\A'
    r'[a-zA-Z0-9_./\-]+: [0-9]+: dump object:\n'
    r'--------------------\n'
    r'[0-9a-f]+ STRING: dog\n'
    r'--------------------\n'
    r'\Z',
    'string'
)
