SRCDIR := __src__
BASE_INCDIR := __inc__
SUB_INCDIR := view
INCDIR := $(BASE_INCDIR)/$(SUB_INCDIR)

OBJDIR := __obj__
OBJDIR_DBG := __obj_dbg__

ifeq ($(DEBUG),1)
	OBJDIR := $(OBJDIR_DBG)
endif

FLS := $(wildcard *.$(FLEXT) **/*.$(FLEXT))

SRCS := $(addprefix $(SRCDIR)/, $(FLS:.$(FLEXT)=.$(SRCEXT)))
DEPS := $(addprefix $(INCDIR)/, $(FLS:.$(FLEXT)=.$(DEPEXT)))
OBJS := $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SRCS:.$(SRCEXT)=.$(OBJEXT)))

.PRECIOUS: $(SRCDIR)/%.$(SRCEXT)
.PHONY: all clean

all: $(OBJS)

clean:
	rm -rf $(SRCDIR) $(BASE_INCDIR) $(OBJDIR) $(OBJDIR_DBG)

$(SRCDIR)/%.$(SRCEXT): %.$(FLEXT)
	@mkdir -p $(dir $@)
	@mkdir -p $(patsubst $(SRCDIR)/%, $(INCDIR)/%, $(dir $@))

	$(FLUID) -c -o $@ -h $(patsubst $(SRCDIR)/%, $(INCDIR)/%, $(@:.$(SRCEXT)=.$(DEPEXT))) $<

	@sed -i 's/$(notdir $(<:.$(FLEXT)=))_$(DEPEXT)/$(subst -,_,$(SUB_INCDIR))_$(subst /,_,$(<:.$(FLEXT)=))_$(DEPEXT)/g' $(patsubst $(SRCDIR)/%, $(INCDIR)/%, $(@:.$(SRCEXT)=.$(DEPEXT)))
	@sed -i 's/$(BASE_INCDIR)\///g' $@

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(BASE_INCDIR) $(INC)
