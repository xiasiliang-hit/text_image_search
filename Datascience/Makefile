CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := -L./lib -lpthread -ldl -ggdb `pkg-config --cflags opencv` `pkg-config --libs opencv`
CC_FLAGS := -I./include -w
OUT := bin/proc
remote:
	rsync -auv -e "ssh siliang@ec2 ssh -p 54322" ../Datascience/* localhost:/home/siliang/Datascience > /dev/null; \
	ssh siliang@54.201.29.203 ssh localhost -p 54322 make -C Datascience local
local: $(OBJ_FILES)
	mpic++ -o $(OUT) $^ $(LD_FLAGS)
obj/%.o: src/%.cpp include/%.h
	mpic++ $(CC_FLAGS) -c -o $@ $<
clean:
	rm obj/* bin/* 2> /dev/null ; \
	rsync -av --delete -e "ssh siliang@ec2 ssh -p 54322" ../Datascience/* localhost:/home/siliang/Datascience > /dev/null
run:
	ssh siliang@54.201.29.203 ssh localhost -p 54322 "mpirun --mca mpi_yield_when_idle 1 --mca btl_tcp_if_include eth0 -np 4 --hostfile ~/.mpi_hostfile Datascience/bin/proc"
stop:
	ssh siliang@54.201.29.203 ssh localhost -p 54322 "kill -9 $(pidof proc) 2> /dev/null"; \
