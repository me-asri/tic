# Target name
TARGET := tic

# Source directories
SRCDIR := src
INCDIR := inc
VIEWDIR := view
VIEW_INCDIR := $(VIEWDIR)/__inc__

# File extensions
SRCEXT := cpp
DEPEXT := h
FLEXT := fl
OBJEXT := o

# Compiler options
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Werror
CXXFLAGS_REL := -O3 -s
CXXFLAGS_DBG := -Og -g -DDEBUG
INC := -I$(INCDIR)
LIB :=

# FLTK options
FLUID := fluid
FLTKFLAGS :=

# Object directories
OBJDIR := obj
OBJDIR_DBG := obj_dbg
VIEW_OBJDIR := $(VIEWDIR)/__obj__
VIEW_OBJDIR_DBG := $(VIEWDIR)/__obj_dbg__
BINDIR := bin
BINDIR_DBG := bin_dbg

# Add .exe suffix to binaries built for Windows
ifeq ($(OS),Windows_NT)
	TARGET := $(addsuffix .exe,$(TARGET))
endif

# Use appropriate flags and object directories for debug builds
ifeq ($(DEBUG),1)
	CXXFLAGS += $(CXXFLAGS_DBG)

	OBJDIR := $(OBJDIR_DBG)
	VIEW_OBJDIR := $(VIEW_OBJDIR_DBG)
	BINDIR := $(BINDIR_DBG)
else
	CXXFLAGS += $(CXXFLAGS_REL)
endif

# Globbing
SRCS := $(wildcard $(SRCDIR)/*.$(SRCEXT) $(SRCDIR)/**/*.$(SRCEXT))
DEPS := $(wildcard $(INCDIR)/*.$(DEPEXT) $(INCDIR)/**/*.$(DEPEXT))
OBJS := $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SRCS:.$(SRCEXT)=.$(OBJEXT)))
BIN := $(BINDIR)/$(TARGET)

# Append FLTK specific flags
CXXFLAGS += $(shell fltk-config --cxxflags $(FLTKFLAGS))
LIB += $(shell fltk-config --ldflags $(FLTKFLAGS))
INC += -I$(VIEW_INCDIR)

# Append FLUID objects
FLS := $(wildcard $(VIEWDIR)/*.$(FLEXT) $(VIEWDIR)/**/*.$(FLEXT))
OBJS += $(patsubst $(VIEWDIR)/%, $(VIEW_OBJDIR)/%, $(FLS:.$(FLEXT)=.$(OBJEXT)))

export DEBUG CXX CXXFLAGS FLUID SRCEXT DEPEXT OBJEXT FLEXT

.PHONY: all view run clean cleaner

all: view
	@echo + Building $(TARGET)
	@$(MAKE) $(BIN)

view:
	@echo + Building views
	@$(MAKE) -C $(VIEWDIR)

$(BIN): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CXX) -o $(BIN) $^ $(LIB)
	@echo + Built $(TARGET)

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT) $(DEPS)
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(INC)

run: all
	@echo + Launching $(TARGET)
	@cd $(BINDIR) ;\
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(OBJDIR_DBG) $(BINDIR) $(BINDIR_DBG)

cleaner: clean
	@$(MAKE) -C $(VIEWDIR) clean
