import numpy as np
import pandas as pd
from math import pow, pi, log

planetary_colors = {
    0:"#ffff66",  # sol
    1:"#a3a375", # mercury
    2:"#cccc00", # venus
    3:"#e1e1ea", # luna
    4:"#00cc00", # earth
    5:"#cc0000", # mars
    6:"#ff751a", #jupiter
    7:"#ffcc99", # saturn
    8:"#9900ff", #uranus
    9:"#0000b3", #neptune
}

planetary_rad = {
    0:"696340",  # sol
    1:"2440", # mercury
    2:"6051", # venus
    3:"1737", # luna
    4:"6378", # earth
    5:"3390", # mars
    6:"69911", #jupiter
    7:"58232", # saturn
    8:"25362", #uranus
    9:"24622", #neptune
}
def sizer(id,mass):
    if id in list(planetary_rad): # list(dict) returns list of keys for some reason...
        return planetary_rad[id]
    elif (mass == -1):
        return 0
    else:
        return pow(mass / (4/3 * 2000 * pi),1/3)

def colorizer_red(x):
    if x in list(planetary_colors): # list(dict) returns list of keys for some reason...
        return int(planetary_colors[x][1:3],16)/255.0
    else:
        return int("#cc6600"[1:3],16)/255.0
    
def colorizer_green(x):
    if x in list(planetary_colors): # list(dict) returns list of keys for some reason...
        return int(planetary_colors[x][3:5],16)/255.0
    else:
        return int("#cc6600"[3:5],16)/255.0
    
def colorizer_blue(x):
    if x in list(planetary_colors): # list(dict) returns list of keys for some reason...
        return int(planetary_colors[x][5:7],16)/255.0
    else:
        return int("#cc6600"[5:7],16)/255.0

def convert_au(x):
    return 6.68459e-9 * float(x)

def skew_radius(x):
    id = int(x)
    rad = float(input_datum["radius"][id])
    if(id != 0 and id in list(planetary_rad)):
        return abs(log(float(rad/5),1e40))/3
    return abs(log(float(rad/5),1e40))


col_names = ["tick","id","mass","xpos","ypos","zpos","xvel","yvel","zvel"]
input_datum = pd.read_csv("./data/out.dat", sep=" ",names=col_names,header=None)
input_datum["red"] = (input_datum["id"]).apply(colorizer_red)
input_datum["green"] = (input_datum["id"]).apply(colorizer_green)
input_datum["blue"] = (input_datum["id"]).apply(colorizer_blue)
input_datum["radius"] = pd.Series([sizer(id,mass) for id,mass in list(zip(input_datum["id"],input_datum["mass"]))])
input_datum["xpos"] = (input_datum["xpos"]).apply(convert_au)
input_datum["ypos"] = (input_datum["ypos"]).apply(convert_au)
input_datum["zpos"] = (input_datum["zpos"]).apply(convert_au)
# input_datum["radius"] = (input_datum["radius"]).apply(convert_au)
# rad_sun_au = input_datum["radius"][0]
input_datum["radius"] = (input_datum["id"]).apply(skew_radius)

input_datum.to_csv("./data/input_opengl.dat",index=False,sep=" ",columns=["xpos","ypos","zpos","red","green","blue","radius"])

print("Data conversion for rendering complete.")
