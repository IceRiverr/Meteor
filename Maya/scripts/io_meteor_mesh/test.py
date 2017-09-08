
import sys
import os

def splie_space(inStr):
	result = []
	while True:
		inStr.strip()
		pos = inStr.find(" ")
		
		if pos == -1:
			break;
			
		result.append(inStr[0:pos])
		inStr = inStr[pos+1:]
		
	result.append(inStr)
	
	return result

inStr = " pivot  2.703 -0.003  0.000\n"

result = splie_space(inStr)

	
print result
	
print "Hello"
