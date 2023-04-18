# Make all subdirs

all:
	-$(MAKE) -C lib all
	-$(MAKE) -C daemons all
	-$(MAKE) -C app all

clean:
	-$(MAKE) -C lib clean
	-$(MAKE) -C daemons clean
	-$(MAKE) -C app clean
