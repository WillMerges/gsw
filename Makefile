# Make all subdirs

all:
	-$(MAKE) -C lib all
	-$(MAKE) -C daemons all

clean:
	-$(MAKE) -C lib clean
	-$(MAKE) -C daemons clean
