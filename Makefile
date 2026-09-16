export PROJECT_ROOT := $(CURDIR)

all:
	$(MAKE) -C examples

linux:
	$(MAKE) -C examples linux

seccomp-exploit:
	$(MAKE) -C examples seccomp-exploit

clean-exploit:
	$(MAKE) -C $@

clean-tests:
	$(MAKE) -C $@

clean:
	$(MAKE) -C examples clean
