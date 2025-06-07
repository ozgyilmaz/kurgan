CC      = gcc
PROF    = -O -g
NOCRYPT =
C_FLAGS = -Wall $(PROF) $(NOCRYPT)
L_FLAGS = $(PROF)
LIBS    = -lcrypt

O_FILES = act_comm.o act_enter.o act_info.o act_move.o act_obj.o act_wiz.o \
          alias.o ban.o comm.o const.o db.o db2.o effects.o fight.o flags.o \
          handler.o healer.o interp.o note.o lookup.o magic.o magic2.o \
          music.o recycle.o save.o scan.o skills.o special.o tables.o \
          update.o

# Vars for dependency files
DEP_FILES = $(O_FILES:.o=.d)

# Default target
kurgan: $(O_FILES)
	rm -f kurgan
	$(CC) $(L_FLAGS) -o kurgan $(O_FILES) $(LIBS)

# Pattern rule for compiling .c to .o with header dependency
%.o: %.c merc.h
	$(CC) -c $(C_FLAGS) $< -o $@
	$(CC) -M $(C_FLAGS) $< > $(<:.c=.d)

# Include dependency files, ignore errors if files don't exist yet
-include $(DEP_FILES)

# Clean target to remove built files
clean:
	rm -f $(O_FILES) $(DEP_FILES) kurgan

.PHONY: clean