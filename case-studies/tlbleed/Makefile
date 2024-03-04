all: madtlb
run: clear-max.txt clear-probe.txt inuse-max.txt inuse-probe.txt

clear-max.txt clear-probe.txt inuse-max.txt inuse-probe.txt &: madtlb measure.sh
	./measure.sh

madtlb: madtlb.c
	$(CC) -ggdb3 -O0 -Wall -o $@ $<

.PHONY: clean cleanall

clean:
	rm -f madtlb

cleanall: clean
	rm -f clear-max.txt clear-probe.txt inuse-max.txt inuse-probe.txt
