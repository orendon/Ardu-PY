import ArduPy

Ard=ArduPy.Arduino()
print Ard.voltajeRead(5)
print "Tone!"
print Ard.toneGen(13,220,1)