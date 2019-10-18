SCAD=78leds.scad
TXT=78leds.txt
DXF=78leds.dxf
SCRIPT=triangle.py

all: $(DXF) $(TXT)

$(SCAD) $(TXT): $(SCRIPT)
	./$(SCRIPT) $(SCAD) $(TXT)

$(DXF): $(SCAD)
	openscad $(SCAD) -o $(DXF)

clean:
	rm -f $(SCAD) $(TXT) $(DXF)
