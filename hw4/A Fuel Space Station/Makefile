# Credits: Karl Gääääääfert



.PHONY: clean build run

DIR_SRC = src/Space
DIR_BUILD = build/classes
DIR_CLASSES = $(DIR_BUILD)/Space

RUNNABLE = Space.Main
SRC_FLAT = $(shell find $(DIR_SRC) -printf '%P\n' -name '*.java')
SRC = $(addprefix $(DIR_SRC)/,$(SRC_FLAT))
CLASSES = $(addprefix $(DIR_CLASSES)/,$(SRC_FLAT:%.java=%.class))

all: clean | build run

clean: 
	-rm -rf $(DIR_BUILD)/*
	
build: $(CLASSES)
	
$(CLASSES): $(SRC)
	javac -Xlint:all -d $(DIR_BUILD) $^

run:
	java -cp $(DIR_BUILD) $(RUNNABLE)
