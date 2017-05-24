SUBDIRS = sai_adapter/ p4-softswitch/
all : $(SUBDIRS)
.PHONY : all $(SUBDIRS)

sai-adapter/ :
	cd sai_adapter && ./install_deps.sh && cd ..
	$(MAKE) -C $@

p4-softswitch/ :
	$(MAKE) -C $@

lacp-teaming/ :
	$(MAKE) -C $@