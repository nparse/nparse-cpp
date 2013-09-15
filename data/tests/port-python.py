#!/usr/bin/python
import sys
import nparse

np = nparse.Parser()

print 'version:             ', np.version()
print 'load:                ', np.load('port.ng')
print 'parse:               ', np.parse('hello world')
print 'status:              ', np.status_str()

print 'message count:       ', np.get_message_count()
print 'iteration count:     ', np.get_iteration_count()
print 'trace count:         ', np.get_trace_count()
print 'state pool usage:    ', np.get_pool_usage(), 'bytes'
print 'state pool capacity: ', np.get_pool_capacity(), 'bytes'

while np.next():
	sys.stdout.write(':')
	while np.step():
		sys.stdout.write( ')' * np.shift() )
		node = np.node()
		if node:
			sys.stdout.write( ' (' + node )
		text = np.text()
		if text:
			sys.stdout.write( ' ' + text )
	sys.stdout.write( ')' * np.shift() )
	print

for i in range(0, np.get_message_count()):
	print '\t', np.get_message(i)
	print '\tin', np.get_location(i) 

print '-' * 20
np.rewind()
np.next()
print np.get('z')
