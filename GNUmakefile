.PHONY: all
all:
	$(MAKE) -C cxu
	$(MAKE) -C cxm


.PHONY: check
check: all
	$(MAKE) -C cxu_test
	cd cxu_test ; ./cxu_test
	$(MAKE) -C cxm_test
	cd cxm_test ; ./cxm_test


.PHONY: distcheck
distcheck:
	echo "nope"


.PHONY: clean
clean:
	$(MAKE) -C cxu clean
	$(MAKE) -C cxu_test clean
	$(MAKE) -C cxm clean
	$(MAKE) -C cxm_test clean


