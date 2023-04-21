# Make all library subdirs

all: build copy

build:
	-$(MAKE) -C logging all
	-$(MAKE) -C time all
	-$(MAKE) -C tripple_buffer all
	-$(MAKE) -C shm all

copy:
	rm -rf bin || true > /dev/null
	mkdir bin
	-cp */*.so bin/

clean:
	-$(MAKE) -C logging clean
	-$(MAKE) -C time clean
	-$(MAKE) -C triple_buffer clean
	-$(MAKE) -C shm clean
	rm -r bin
