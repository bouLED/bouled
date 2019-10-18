C_INCLUDES+=-I$(BOULED)/simulation
C_SOURCES+=$(BOULED)/simulation/controller_constants.c
$(COMMON)/controller.c: $(BOULED)/simulation/controller_constants.h

$(BOULED)/simulation/controller_constants.h:
	cd $(BOULED)/simulation && make controller_constants.h

$(BOULED)/simulation/controller_constants.c: $(BOULED)/simulation/controller_constants.h
	cd $(BOULED)/simulation && make controller_constants.c
