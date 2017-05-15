import sys

class Generator(object):

    def __init__(self):
        self.count = 0

    def writeHead(self, t0, v0, t1, v1, op):
        self.count += 1
        _testName = "test%d_%s_%s_%s" % (
                self.count,
                t0,
                'equal_to' if op == '==' else 'not_equal_to',
                t1
            )
        _testInput = "%s%s %s %s%s" % (
                t0, '' if v0 is None else ' '+v0,
                op,
                t1, '' if v1 is None else ' '+v1
            )
        sys.stdout.write("%s:\n" % _testName)
        sys.stdout.write("    input:             \"%s\"\n" % _testInput)

    def writePass(self, t0, v0, t1, v1, op, exp):
        self.writeHead(t0, v0, t1, v1, op)
        sys.stdout.write("    traces:            1\n")
        sys.stdout.write("    check:\n")
        sys.stdout.write("        result:        %s\n" % ('true' if exp else 'false'))
        sys.stdout.write("        type0:         \"%s\"\n" % t0)
        sys.stdout.write("        type1:         \"%s\"\n\n" % t1)

    def wt(self, t0, v0, t1, v1, exp):
        self.writePass(t0, v0, t1, v1, '==', exp)
        self.writePass(t0, v0, t1, v1, '!=', not exp)

    def eq(self, t0, v0, t1, v1):
        self.wt(t0, v0, t1, v1, True)
        if t0 != t1 or v0 != v1:
            self.wt(t1, v1, t0, v0, True)

    def nq(self, t0, v0, t1, v1):
        self.wt(t0, v0, t1, v1, False)
        if t0 != t1 or v0 != v1:
            self.wt(t1, v1, t0, v0, False)

    def er(self, t0, v0, t1, v1, msg = 'std::bad_cast'):
        for (op, line) in (('==', 10), ('!=', 12)):
            self.writeHead(t0, v0, t1, v1, op)
            sys.stdout.write("    error:\n")
            sys.stdout.write("        - [ \"%s\", %d, 36 ]\n\n" % (msg.format(op), line))

    def generate(self):
        sys.stdout.write("script:\n")
        sys.stdout.write("    - comparison1.ng\n\n")

        self.eq('null', None, 'null', None)
        self.nq('null', None, 'boolean', 'false')
        self.nq('null', None, 'boolean', 'true')
        self.nq('null', None, 'integer', '-1')
        self.nq('null', None, 'integer', '0')
        self.nq('null', None, 'integer', '1')
        self.nq('null', None, 'real', '-1.0')
        self.nq('null', None, 'real', '0.0')
        self.nq('null', None, 'real', '1.0')
        self.nq('null', None, 'string', 'false')
        self.nq('null', None, 'string', 'true')
        self.nq('null', None, 'string', 'null')
        self.nq('null', None, 'array', 'i')

        self.eq('boolean', 'false', 'boolean', 'False')
        self.eq('boolean', 'true',  'boolean', 'True')
        self.eq('boolean', 'false', 'boolean', 'no')
        self.eq('boolean', 'true',  'boolean', 'yes')
        self.eq('boolean', 'false', 'boolean', '0')
        self.eq('boolean', 'true',  'boolean', '1')
        self.nq('boolean', 'false', 'boolean', 'True')
        self.nq('boolean', 'true',  'boolean', 'False')
        self.nq('boolean', 'false', 'boolean', 'yes')
        self.nq('boolean', 'true',  'boolean', 'no')
        self.nq('boolean', 'false', 'boolean', '1')
        self.nq('boolean', 'true',  'boolean', '0')
        self.eq('boolean', 'false', 'integer', '0')
        self.nq('boolean', 'true',  'integer', '0')
        self.nq('boolean', 'false', 'integer', '1')
        self.eq('boolean', 'true',  'integer', '1')
        self.nq('boolean', 'false', 'integer', '-1')
        self.eq('boolean', 'true',  'integer', '-1')
        self.eq('boolean', 'false', 'real', '0')
        self.nq('boolean', 'true',  'real', '0')
        self.nq('boolean', 'false', 'real', '1')
        self.eq('boolean', 'true',  'real', '1')
        self.nq('boolean', 'false', 'real', '-1')
        self.eq('boolean', 'true',  'real', '-1')
        self.eq('boolean', 'false', 'string', 'false')
        self.eq('boolean', 'true',  'string', 'true')
        self.eq('boolean', 'true',  'array', 'i')
        self.nq('boolean', 'false', 'array', 'j')

        self.eq('integer', '0',     'integer', '0')
        self.nq('integer', '0',     'integer', '1')
        self.nq('integer', '-1',    'integer', '1')
        self.nq('integer', '-1',    'integer', '0')
        self.nq('integer', '0',     'real',    '0.1')
        self.eq('integer', '0',     'real',    '0.0')
        self.eq('integer', '1',     'real',    '1.0')
        self.eq('integer', '-1',    'real',    '-1.0')
        self.nq('integer', '-1',    'real',    '-0.9')
        self.eq('integer', '0',     'string',  '0')
        self.eq('integer', '10',    'string',  '10')
        self.eq('integer', '-10',   'string',  '-10')
        self.nq('integer', '0',     'string',  '1')
        self.nq('integer', '10',    'string',  '101')
        self.nq('integer', '-10',   'string',  '-101')
        self.er('integer', '0',     'array',   'i')

        self.eq('real',    '0',     'real',    '0')
        self.nq('real',    '0',     'real',    '1')
        self.eq('real',    '0.0',   'real',    '0.0')
        self.nq('real',    '0.0',   'real',    '0.1')
        self.nq('real',  '-3.14e5', 'real',    '-31400')
        self.nq('real',    '0',     'string',  '1')
        self.nq('real',    '1',     'string',  '0')
        self.nq('real',    '0.0',   'string',  '0.0')   # Sic!
        self.nq('real',    '1.234', 'string',  '1.234') # Sic!
        self.nq('real',    '0.1',   'string',  '0.0')
        self.er('real',    '0',     'array',   'i')

        self.er('array', 'i', 'array', 'j', "operator '{}': incomparable operands")

if __name__=='__main__':
    Generator().generate()
