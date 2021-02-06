export CXX = g++
export INCLUDE_DIR = -I../../lib -I../../lib/ext/adept -I../../lib/ext/eigen
export STDFALGS = "std=c++17"

.PHONY: debug not_optimized optimized fully_optimized

all: debug not_optimized optimized fully_optimized

debug:
	$(MAKE) -C debug CUSTOMFLAGS="-pg -g -Og"

not_optimized:
	$(MAKE) -C not_optimized CUSTOMFLAGS="-O0 -p" 

optimized:
	$(MAKE) -C optimized CUSTOMFLAGS="-O1 -p"

fully_optimized:
	$(MAKE) -C fully_optimized CUSTOMFLAGS="-O3 -p"

runall:
	$(MAKE) -C debug run
	$(MAKE) -C not_optimized run
	$(MAKE) -C optimized run
	$(MAKE) -C fully_optimized run

clean:
	$(MAKE) -C debug clean
	$(MAKE) -C not_optimized clean
	$(MAKE) -C optimized clean
	$(MAKE) -C fully_optimized clean