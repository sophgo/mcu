TARGET = sm5-pcie-signal-debugger

all: $(TARGET).pdf

%.pdf: %.odt FORCE
	rm -rf $@
	libreoffice --headless --convert-to pdf $<
	while [ ! -f $@ ]; do sleep 1; done

clean:
	rm -rf *.pdf

FORCE:

.PHYON: FORCE clean
