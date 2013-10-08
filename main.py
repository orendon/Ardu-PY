import ArduPy

Ard=ArduPy.Arduino()
print Ard.voltageRead(5)
print "Tone!"
print Ard.toneGen(13,220,1)