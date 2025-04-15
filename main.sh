make
python3 ./src/get_celestial_bodies_data_from_nasa.py
$(BIN_DIR)main.out
python3 process_data_for_opengl.py
$(BIN_DIR)draw.out