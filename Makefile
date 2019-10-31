# based on https://bitbucket.org/neilb/genmake/src/default/srcinc/makefile
# found via https://latedev.wordpress.com/2014/11/08/generic-makefiles-with-gcc-and-gnu-make/

SHELL := /bin/sh

PRODUCT := n-colorcode

BINDIR := bin
INCDIR := include
SRCDIR := src
OBJDIR := obj

CC := gcc
LD := gcc
INCDIRS := -I$(INCDIR)
CCFLAGS := -std=c11 -Wall -Wextra -Wpedantic
LDFLAGS := -fsanitize=address -lncurses

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))
DEPFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.d,$(SRCFILES))

$(BINDIR)/$(PRODUCT): $(OBJFILES)
	$(LD) $(CCFLAGS) -O2 $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CCFLAGS) $(INCDIRS) -c $< -o $@

$(OBJDIR)/%.d: $(SRCDIR)/%.c
	$(CC) $(CCFLAGS) $(INCDIRS) -MM $< \
	| tr '\n\r\\' ' ' \
	| sed -e 's%^%$@ %' -e 's% % $(OBJDIR)/%'\
	> $@

$(BINDIR)/$(PRODUCT)-debug: $(SRCFILES)
	$(CC) $(CCFLAGS) -g -O0 $(INCDIRS) $^ $(LDFLAGS) -o $@

.PHONY: clean depends debug
clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(PRODUCT)

depends:
	rm -f $(OBJDIR)/*.d
	$(MAKE) $(DEPFILES)

debug:
	$(MAKE) $(BINDIR)/$(PRODUCT)-debug

-include $(DEPFILES)
