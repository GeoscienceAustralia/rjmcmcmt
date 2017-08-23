SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .cpp .o
.DEFAULT_GOAL := allclean

executable =  $(exedir)/generate_synthetic_data.exe

includes += -I$(srcdir)
includes += -I$(cpputilssrc)
includes += -I$(LIBRJMCMC_ROOT)/include

#cxxflags  += -DUSEGLOBALSTACKTRACE
cxxflags   += -D_MPI_ENABLED
libs       = -L$(LIBRJMCMC_ROOT)/lib -lrjmcmc

objects += $(cpputilssrc)/general_utils.o
objects += $(cpputilssrc)/file_utils.o
objects += $(srcdir)/generate_synthetic_data.o

%.o : %.cpp
	@echo ' '
	@echo 'Compiling ' $<
	$(mpicxx) -c $(includes) $(cxxflags) $< -o $@

compile: $(objects)

link: $(objects)
	mkdir -p $(exedir)
	@echo ' '
	@echo Linking
	$(mpicxx) $(objects) $(libs) -o $(executable)
	chmod g+rx $(executable)
	chmod o+rx $(executable)

clean:
	@echo ' '
	@echo Cleaning
	rm -f $(objects)
	rm -f $(executable)

all: compile link
allclean: clean compile link

