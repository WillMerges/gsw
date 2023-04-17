### Make libraries in subdirs ###

all: build copy

build:
	-$(MAKE) -C logging all
	-$(MAKE) -C time all

copy:
	rm -rf bin || true > /dev/null
	mkdir bin
	-cp */*.so bin/
	-cp dev/*/*.so bin/


clean:
	-$(MAKE) -C logging clean
	-$(MAKE) -C time clean
	rm -r bin
