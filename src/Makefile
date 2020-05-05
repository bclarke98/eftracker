CC =  g++
CFLAGS = `pkg-config --cflags opencv4`
MAIN = main.o
OBJECTS = $(MAIN) eftpx.o gfx.o
EXE = eftpx
LIBS = `pkg-config --libs opencv4`

$(EXE): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -pthread

run: $(EXE)
	./$(EXE) ${ARGS}

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	-@rm $(EXE) *.o 2>/dev/null || true
