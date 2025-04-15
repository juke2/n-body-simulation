# all: draw_opengl.o
# 	$(CC) -o draw_opengl draw_opengl.c 

objects=nbody_draw.o
output=main.out


all: $(objects) nbody.o main.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(BIN_DIR)draw_opengl.out $(addprefix $(BUILD_DIR),$(objects)) $(BUILD_DIR)main-draw.o
	$(CC) -lm -o $(BIN_DIR)n-body-simulate.out $(BUILD_DIR)nbody.o $(BUILD_DIR)main-n-body.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(BIN_DIR)$(output) $(addprefix $(BUILD_DIR),$(objects)) $(BUILD_DIR)nbody.o $(BUILD_DIR)main.o

nbody_draw.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(BUILD_DIR)nbody_draw.o -c ./src/draw_simulation_opengl.c

nbody.o:
	$(CC) -lm -o $(BUILD_DIR)nbody.o -c ./src/n_body_renderer.c

main.o: nbody_draw.o nbody.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(BUILD_DIR)main.o -c ./src/main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(BUILD_DIR)main-n-body.o -c ./src/main-n-body.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(BUILD_DIR)main-draw.o -c ./src/main-draw.c

debug: $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(BIN_DIR)$(output) $(addprefix $(BUILD_DIR),$(objects)) $(DEBUG_FLAGS)

clean:
	rm -f $(BIN_DIR)*
	rm -f $(BUILD_DIR)*
