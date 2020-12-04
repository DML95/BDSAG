## VARIABLES

CXX=g++
CXXFLAGS=-O3 -std=c++17 -Wall
LDFLAGS=-static

DIRSRC = src
DIROBJ = obj
DIRBIN = bin
DIRDEP = dep

PROGRAM=BDSAG
LDLIBS=-lOpenCL -lmicrohttpd
PRIORITY_FILES=src/console/log.cpp

## LOGICA

#detectamos el sistema operativo
ifeq ($(OS),Windows_NT)
    OS_NAME := Windows
else
    OS_NAME := $(shell uname)
endif

$(info OS_NAME = $(OS_NAME))

#definimos la s condiciones de compilacion dependiendo del sistema operativo
ifeq ($(OS_NAME),Windows)
LDLIBS+=-lws2_32
PROGRAM:=$(addsuffix .exe,$(PROGRAM))
define MKDIR_OS
	-mkdir $(subst /,\,$(1))
endef
define RM_OS
	del /Q /S $(subst /,\,$(1))
endef
else ifeq ($(OS_NAME),Linux)
$(error Version incompatible con Linux)
define MKDIR_OS
	-mkdir -p $(1)
endef
define RM_OS
	rm $(1)
endef
else
$(error Sistema operativo no valido)
endif

FILE_BIN=$(DIRBIN)/$(PROGRAM)

$(info LDLIBS = $(LDLIBS))
$(info FILE_BIN = $(FILE_BIN))

#listamos los archivos de codigo
SRCS = $(wildcard $(DIRSRC)/*.cpp $(DIRSRC)/*/*.cpp)
$(info SRCS sin prorizan = $(SRCS))

#chequeamos que los archivos a prorizar existan
ifeq ($(filter $(PRIORITY_FILES),$(SRCS)),)
$(error Archivos a prorizar no validos)
endif
#priorizamos la compilacion de ciertos archivos
SRCS:=$(filter-out $(PRIORITY_FILES),$(SRCS)) $(PRIORITY_FILES)
$(info SRCS priorizado = $(SRCS))

#optenemos los objetos
OBJS = $(SRCS:$(DIRSRC)/%.cpp=$(DIROBJ)/%.o)
$(info OBJS = $(OBJS))

#optenemos las dependencias
DEPS = $(SRCS:$(DIRSRC)/%.cpp=$(DIRDEP)/%.d)
$(info DEPS = $(DEPS))

## TARGETS

#targets que no son archivos
.PHONY: all clean

#inicio del make
all: $(FILE_BIN)

#archivos a linkar
$(FILE_BIN): $(OBJS)
	$(CXX) $^ -o $@ $(LDLIBS) $(LDFLAGS)

#incluimos las dependencias
-include $(DEPS)

#patron de objetos a compilar
$(DIROBJ)/%.o: $(DIRSRC)/%.cpp
#	creamos los subdirectorios y generamos los archivos de dependencias
	$(call MKDIR_OS,$(@D))
	$(CXX) -MM $< > $(@:$(DIROBJ)/%.o=$(DIRDEP)/%.d)
#	creamos los subdirectorios y compilamos los objetos
	$(call MKDIR_OS,$(@D:$(DIROBJ)%=$(DIRDEP)%))
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(DIRSRC)

#limpiamos los archivos antiguos
clean: $(DIROBJ) $(DIRDEP) $(FILE_BIN)
	$(call RM_OS,$^)
