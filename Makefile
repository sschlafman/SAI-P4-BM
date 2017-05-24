SUBDIRS = sai-adapter/ p4-softswitch/
all : $(SUBDIRS)
.PHONY : all

sai-adapter/ :
	cd sai_adapter && ./install_deps.sh
	$(MAKE) -C $@
	cd ..

p4-softswitch/ :
	cd p4-softswitch
	$(MAKE) -C $@
	cd ..