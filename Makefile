SUBDIRS = p4-softswitch/ sai_adapter/ 
all : $(SUBDIRS)
.PHONY : all $(SUBDIRS)

# need to add sudo make install for bmv2
p4-softswitch/ :
	cd p4-softswitch && ./install_deps.sh && ./autogen.sh && ./configure && cd ..
	$(MAKE) -C $@

sai-adapter/ :
	cd sai_adapter && ./install_deps.sh && cd ..
	$(MAKE) -C $@


lacp-teaming/ :
	$(MAKE) -C $@