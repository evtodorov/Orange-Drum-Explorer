default: sc1 sc2 sc3

sc1: Solver.o scenario1.o
	$(CXX) -o sc1 scenario1.o Solver.o $(STDFLAGS) $(CUSTOMFLAGS) -fopt-info-optall=sc1_link.opt

sc2: Solver.o scenario2.o
	$(CXX) -o sc2 scenario2.o Solver.o $(STDFLAGS) $(CUSTOMFLAGS) -fopt-info-optall=sc2_link.opt

sc3: Solver.o scenario3.o
	$(CXX) -o sc3 scenario3.o Solver.o $(STDFLAGS) $(CUSTOMFLAGS) -fopt-info-optall=sc3_link.opt

scenario1.o: ../scenario1.cpp
	$(CXX) -c -o scenario1.o ../scenario1.cpp $(STDFLAGS) $(CUSTOMFLAGS) $(INCLUDE_DIR) -fopt-info-optall=sc1_compile.opt

scenario2.o: ../scenario2.cpp
	$(CXX) -c -o scenario2.o ../scenario2.cpp $(STDFLAGS) $(CUSTOMFLAGS) $(INCLUDE_DIR) -fopt-info-optall=sc2_compile.opt

scenario3.o: ../scenario3.cpp
	$(CXX) -c -o scenario3.o ../scenario3.cpp $(STDFLAGS) $(CUSTOMFLAGS) $(INCLUDE_DIR) -fopt-info-optall=sc3_compile.opt

Solver.o: ../../lib/Solver.cpp
	$(CXX) -c -o Solver.o ../../lib/Solver.cpp $(STDFLAGS) $(CUSTOMFLAGS) $(INCLUDE_DIR) -fopt-info-optall=solver_compile.opt

run: run1 run2 run3

run1: export GMON_OUT_PREFIX = profiling_sc1
run1: 	
	./sc1
	@gprof sc1 profiling_sc1* > report_sc1.txt

run2: export GMON_OUT_PREFIX=profiling_sc2
run2:
	./sc2
	@gprof sc2 profiling_sc2* > report_sc2.txt

run3: export GMON_OUT_PREFIX=profiling_sc3
run3:
	./sc3
	@gprof sc3 profiling_sc3* > report_sc3.txt
clean:
	rm -f sc* *.o *.out *.txt profiling* *.opt