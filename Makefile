SUBDIRS = sai-adapter/ p4-softswitch/
.PHONY : all $(SUBDIRS)
all : $(SUBDIRS)

sai-adapter/ :
	install_deps
	$(MAKE) -C $@

p4-softswitch/ :
	$(MAKE) -C $@
