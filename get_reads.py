import sys

if len(sys.argv) != 2:
  print "Usage: python",sys.argv[0], sys.argv[1];
  sys.exit(1);

f = open(sys.argv[1],'r');

for line in f:
  line = line[:-1];
  parts = line.split();
  if int(parts[0]) == 0:
    print parts[3],parts[2];

