CXX := clang++
CXXFLAGS := -Wall -Wextra -Werror -pedantic -std=c++20 -fopenmp
RELEASEFLAGS := -O3

# List of source files
SRCS := main.cc io.cc
HEADERS := io.h collision.h sim_validator.h

# Object files
OBJS := $(SRCS:.cc=.o)

.PHONY: all clean bonus

all: release

# List of executables (actual binary names)
EXECUTABLES := sim
PERF_EXECUTABLES := $(EXECUTABLES:%=%.perf)
BONUS_EXECUTABLES := sim_bonus
BONUS_PERF_EXECUTABLES := $(BONUS_EXECUTABLES:%=%.perf)

TARGETS := $(EXECUTABLES) 
PERF_TARGETS := $(PERF_EXECUTABLES)
BONUS_TARGETS := $(BONUS_EXECUTABLES) 
BONUS_PERF_TARGETS := $(BONUS_PERF_EXECUTABLES)

release: $(TARGETS) $(PERF_TARGETS)

# How to compile .o and .o.perf object files
%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -DCHECK=1 $(RELEASEFLAGS) -c $< -o $@
%.o.perf: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -DCHECK=0 $(RELEASEFLAGS) -c $< -o $@

# How to compile non-perf and perf executables
$(EXECUTABLES): %: %.o io.o sim_validator.a
	$(CXX) $(CXXFLAGS) -DCHECK=1 $(RELEASEFLAGS) -o $@ $^
$(PERF_EXECUTABLES): %.perf: %.o.perf io.o
	$(CXX) $(CXXFLAGS) -DCHECK=0 $(RELEASEFLAGS) -o $@ $^

clean:
	$(RM) *.o *.o.perf $(EXECUTABLES) $(PERF_EXECUTABLES) bonus bonus.perf


bonus: $(BONUS_TARGETS) $(BONUS_PERF_TARGETS)
%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -DCHECK=1 $(RELEASEFLAGS) -c $< -o $@
%.o.perf: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -DCHECK=0 $(RELEASEFLAGS) -c $< -o $@

$(BONUS_EXECUTABLES): %: %.o io.o sim_validator.a
	$(CXX) $(CXXFLAGS) -DCHECK=1 $(RELEASEFLAGS) -o bonus $^
$(BONUS_PERF_EXECUTABLES): %.perf: %.o.perf io.o
	$(CXX) $(CXXFLAGS) -DCHECK=0 $(RELEASEFLAGS) -o bonus.perf $^