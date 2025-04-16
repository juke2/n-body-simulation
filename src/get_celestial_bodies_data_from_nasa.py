import requests
import pandas
import random
import re
import sys
from math import *

solar_system_mass = 1.99e+30 # in kg
au_to_km_conv = 1.496e+8
year_to_sec = 3.154e+7
G = 6.674e-20
def norm_3d(x,y,z):
    return sqrt(pow(x,2) + pow(y,2) + pow(z,2))
    

def parse_api_return(text_out):
    data = re.findall(r" A= (?P<apothem>[0-9.]*)| PER= (?P<period>[0-9.]*)|\$\$SOE(?P<csv_data>(?s:.)*)\$\$EOE",text_out)
    # print(data)
    semi_major = (float(data[0][0]) * au_to_km_conv) if data[0][0] else -1   
    period = float(data[1][1]) * year_to_sec if data[1][1] else -1   
    csv_data = data[2][2]
    keys = ["EPOCH","datetime","x","y","z","vx","vy","vz"]
    data_dict = {key : (float(dat) if key != "datetime" else dat) for key,dat in zip(keys,csv_data[0:-2].split(", "))}
    mass = (G * ((semi_major ** 3) * 4 * (pi ** 2) / ((period ** 2) * G) - solar_system_mass)) if (semi_major != -1 and period != -1) else 53590252 # random value that's probably good enough
    # these values are basically bullshit but it gets me something close to the expected
    # at least locations are accurate...
    # print(f"{mass}")
    if(mass < 0):
        print("AAAAAAA OH GOD NO HOW IS MASS NEGATIVE WHAT THE HECK")
    return mass, data_dict["x"],data_dict["y"], data_dict["z"], data_dict["vx"], data_dict["vy"], data_dict["vz"]



def parse_planet_data(text_out):
    data = re.findall(r"Mass[, ]+(?P<mass>[x0-9^(kg)=.~, ]+)|\$\$SOE(?P<csv_data>(?s:.)+)\$\$EOE",text_out)
    # print(re.search(r"(Mass)",text_out))
    # print(data)
    raw_mass = data[0][0]
    csv_data = data[1][1]
    exponent = float(re.search(r"\^([0-9]*)",raw_mass).group(1))
    mass = 10 ** exponent * float(raw_mass.split("=")[1].replace("~",""))
    keys = ["EPOCH","datetime","x","y","z","vx","vy","vz"]
    # print(csv_data)
    data_dict = {key : (float(dat.replace(",","")) if key != "datetime" else dat) for key,dat in zip(keys,csv_data.rstrip().split(", "))}
    # print(data_dict)
    return mass, data_dict["x"],data_dict["y"], data_dict["z"], data_dict["vx"], data_dict["vy"], data_dict["vz"]

    

planetary_ids = {
    "sol":10,
    "mercury":199,
    "venus":299,
    "luna":301,
    "earth":399,
    "mars":499,
    "jupiter":599,
    "saturn":699,
    "uranus":799,
    "neptune":899,
    # "pluto":134340 L
}


def main(num_asteroids_wanted: int) -> None:
    with open("./data/asteroids.csv", "r") as f:
        asteriod_ids = pandas.Series(f.read().splitlines(),dtype=int)
    planet_arr = []

    random_selection = random.sample(asteriod_ids.to_list(),num_asteroids_wanted)
    cm_cords = [0,0,0]
    mass_sum = 0

    with open("./data/in.dat","w") as f:
        f.write(str(len(planetary_ids) + num_asteroids_wanted))
        planet_number = 0
        for planet_name,body_id in planetary_ids.items():
            param = [
                "https://ssd.jpl.nasa.gov/api/horizons.api?format=text",
                f"COMMAND='{body_id}'",
                "OBJ_DATA='YES'",
                "MAKE_EPHEM='YES'",
                "EPHEM_TYPE='VECTORS'",
                "VEC_TABLE='2'",
                "CENTER='@ssb'",
                "START_TIME='2006-01-01'",
                "STOP_TIME='2006-01-02'",
                "STEP_SIZE='2d'",
                "CSV_FORMAT='YES'",
                "ANG_FORMAT='DEG'"
                ]
            out_data = requests.get("&".join(param))
            # print(out_data.text)
            planet_arr = parse_planet_data(out_data.text)
            f.write("\n" + " ".join((str(x) for x in planet_arr)))
        

        
        # calculate vars with respect to center of mass 
        for i,body_id in enumerate(random_selection):
            param = [
                "https://ssd.jpl.nasa.gov/api/horizons.api?format=text",
                f"COMMAND='DES={body_id}'",
                "OBJ_DATA='YES'",
                "MAKE_EPHEM='YES'",
                "EPHEM_TYPE='VECTORS'",
                "VEC_TABLE='2'",
                "CENTER='@ssb'",
                "START_TIME='2006-01-01'",
                "STOP_TIME='2006-01-02'",
                "STEP_SIZE='2d'",
                "CSV_FORMAT='YES'",
                "ANG_FORMAT='DEG'"
                ]
            out_data = requests.get("&".join(param))
            # print(out_data.text)
            print(f"API CALL {i} FINISHED")
            parse_api_return(out_data.text)
            f.write("\n" + " ".join((str(x) for x in parse_api_return(out_data.text))))
            # with open("./data/example_api_out.txt","w") as exp_out:
            #     exp_out.write(out_data.text)

if __name__ == "__main__":
    if(len(sys.argv) != 2):
        print(f"Error: invalid number of arguments provided. Expected 2, recieved {len(sys.argv)}. Usage: {sys.argv[0]} <num_particles>")
    else:
        num_asteroids = int(sys.argv[1])
        main(num_asteroids)





