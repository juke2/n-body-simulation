if [ "$#" -ne 1 ]; then
    echo "Invalid number of arguments. Usage: $0 <num_particles>"
    exit 1
fi
make
python3 ./src/get_celestial_bodies_data_from_nasa.py $1
./bin/main.out