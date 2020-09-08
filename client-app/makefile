
mv = MOVE

TITLE = switch-remote-play-client-console
VERSION = 0.1

SOURCES := $(wildcard source/*.c*)
INCLUDES = include
OBJECTS_DIR = obj
OBJECTS := $(wildcard $(OBJECTS_DIR)/*.o)
OFILES_SRC	:=	$(patsubst source/%, $(OBJECTS_DIR)/%, $(SOURCES:.cpp=.o))
EXECUTABLES := $(wildcard *$(TITLE).exe)

BASEFLAGS = -I$(INCLUDES) -D__PREPROC_DEFINETEST__

DEBUG_COMPILE_FLAGS = -g -Wall -fexceptions -ffunction-sections
RELEASE_COMPILE_FLAGS = -O2 -fno-rtti -fno-exceptions

COMPILE_FLAGS = $(BASEFLAGS) $(DEBUG_COMPILE_FLAGS)

LIBS = -lgdi32 -lws2_32

.SILENT:

.PHONY: clean
clean:
ifeq ($(EXECUTABLES),)
else
	@echo "Cleaning $(EXECUTABLES)"
	del $(EXECUTABLES)
endif
ifeq ($(OBJECTS),)
else
	@echo "Cleaning $(OBJECTS)"
	del $(OBJECTS_DIR)\*.o
endif
	
	
debug: COMPILE_FLAGS = $(BASEFLAGS) $(DEBUG_COMPILE_FLAGS)
debug: clean compile
	@echo "Built debug executable"
	
release: COMPILE_FLAGS = $(BASEFLAGS) $(RELEASE_COMPILE_FLAGS)
release: clean compile
	@echo "Built release executable"
	
production: release
	@echo "Stripping unneeded data from $(TITLE).exe"
	strip $(TITLE).exe --strip-unneeded
	@echo "Production executable complete."
	
$(TITLE).exe : $(OFILES_SRC)
	@echo "compiled $^ and linked into $@"
	g++ $(BASEFLAGS) $^ -o $@ $(LIBS)

compile: $(TITLE).exe
	if not exist $(OBJECTS_DIR) ( mkdir $(OBJECTS_DIR) )
	@echo "Build $^ with flags: $(COMPILE_FLAGS)"

# rules section to let it know how to deal with .o patterns
# $< means it's dependency or pre-req, the thing on the right of ':'
# $@ means the target output, the thing on the left of ':'
# % means the input value that will be parsed and filtered from left side to right side. Acts as a pattern key
obj/%.o : source/%.cpp
	echo "compiling $@ from $<"
	g++ $(COMPILE_FLAGS) -o $@ -c $<
  
%.o : %.c
	gcc -g -o $@ -c $<