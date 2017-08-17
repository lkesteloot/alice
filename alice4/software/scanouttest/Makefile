all: out.png all8bits.png ramp.png

all8bits.png: all8bits.ppm
	convert all8bits.ppm all8bits.png

ramp.png: ramp.ppm
	convert ramp.ppm ramp.png

out.png: out.ppm
	convert out.ppm out.png

ramp.ppm: scanouttest
	./scanouttest --justramp > ramp.ppm

all8bits.ppm: scanouttest
	./scanouttest --all8bits > all8bits.ppm

out.ppm: scanouttest
	./scanouttest > out.ppm


