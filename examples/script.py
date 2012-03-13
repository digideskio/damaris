import damaris

for v in damaris.get_variables():
  print "has var",v.name,
  chunks = v.chunks
  if len(chunks) == 0:
    print "no chunks"
  else:
    print len(chunks),"chunks"

  for c in v.chunks:
    print "it ",c.iteration, "from",c.lower_bounds,c.upper_bounds,"= \n",c.data


print "Cleaning everything in the variables\n"
damaris.clear()
