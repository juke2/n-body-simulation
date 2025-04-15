import numpy as np
import sys
import pandas as pd
import subprocess
import matplotlib as mpl
import shutil
import os
import matplotlib.pyplot as plt
from math import log10, ceil



draw_3d = True
if(draw_3d):
    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')
else:
    fig, ax = plt.subplots()

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
def colorizer(x):
    if x in list(planetary_colors): # list(dict) returns list of keys for some reason...
        return planetary_colors[x]
    else:
        return "#cc6600"




col_names = ["tick","id","mass","xpos","ypos","zpos","xvel","yvel","zvel"]
input_datum = pd.read_csv("./data/out.dat", sep=" ",names=col_names,header=None)
input_datum["mass"] = input_datum["mass"].apply(lambda x: log10( x/3.30e+21 + 1)) # divide by slightly less than mercury's mass
input_datum["color"] = input_datum["id"].apply(colorizer)

size_max_tick = len(str(max(input_datum["tick"]+1)))
num_ticks = max(input_datum["tick"]) + 1
num_slices = ceil(num_ticks / 1000)
size_num_slices = len(str(num_slices))
for image_slice in range(0,num_slices):
    os.mkdir(f"./images/{image_slice:0>{num_slices}}")
    for tick in range(image_slice*1000,(image_slice+1)*1000):
        if(tick < num_ticks):
            print(f"img {tick:0>{size_max_tick}}")
            ax.set_axis_off()
            if(draw_3d):
                data = input_datum[(input_datum["tick"] == tick) & (input_datum["mass"] > 0)][["mass","xpos","ypos","zpos","color"]]
                ax.scatter(xs=data["xpos"],ys=data["ypos"],zs=data["zpos"],s=data["mass"],c=data["color"])
                fig.savefig(f"images/{image_slice:0>{num_slices}}/{tick:0>{size_max_tick}}.png")
                ax.clear()
            else:
                data = input_datum[(input_datum["tick"] == tick) & (input_datum["mass"] > 0)][["mass","xpos","ypos"]]
                ax.scatter(x=data["xpos"],y=data["ypos"],s=data["mass"])
                fig.savefig(f"images/{image_slice:0>{num_slices}}/{tick:0>{size_max_tick}}.png")
                ax.clear()


fname_list = [x for x in os.listdir("./images") if not "." in x] # really hope you don't put anything without an extension here!
fname_list.sort()
for dir in fname_list:
    input = f"""ffmpeg \
    -y \
    -pattern_type glob \
    -i './images/{dir}/*.png' \
    -loop 0 \
    -delay 0 \
    -filter_complex "[0:v] split [a][b];[a] palettegen [p];[b][p] paletteuse;" \
    -colorspace 1 \
    -color_primaries 1 \
    -color_trc 1 \
    ./images/{dir}.gif"""
    subprocess.run(input, stdout=sys.stdout, shell=True)


input_gif_concat = f"""ffmpeg \
    -f gif \
    -y \
    -i "concat:{"|".join([f"./images/{dir}.gif" for dir in fname_list])}" \
    -c copy ./images/output.gif"""
subprocess.run(input_gif_concat, stdout=sys.stdout, shell=True)

for dir in fname_list:
    shutil.rmtree(f"./images/{dir}")
    os.remove(f"./images/{dir}.gif")
