default: sc1 sc2

sc1: Solver.o scenario1.o
	$(CXX) -o sc1 scenario1.o Solver.o $(STDFLAGS) $(CUSTOMFLAGS)

sc2: Solver.o scenario2.o
	$(CXX) -o sc2 scenario2.o Solver.o $(STDFLAGS) $(CUSTOMFLAGS)

scenario1.o: ../scenario1.cpp
	$(CXX) -c -o scenario1.o ../scenario1.cpp $(STDFLAGS) $(CUSTOMFLAGS) $(INCLUDE_DIR)

scenario2.o: ../scenario2.cpp
	$(CXX) -c -o scenario2.o ../scenario2.cpp $(STDFLAGS) $(CUSTOMFLAGS) $(INCLUDE_DIR)

Solver.o: ../../lib/Solver.cpp
	$(CXX) -c -o Solver.o ../../lib/Solver.cpp $(STDFLAGS) $(CUSTOMFLAGS) $(INCLUDE_DIR)

run: run1 run2

run1: export GMON_OUT_PREFIX = profiling_sc1
run1: 	
	./sc1
	@gprof sc1 profiling_sc1* > report_sc1.txt
run2: export GMON_OUT_PREFIX=profiling_sc2
run2:
	./sc2
	@gprof sc2 profiling_sc2* > report_sc2.txt

clean:
	rm -f sc* *.o *.out *.txt profiling*