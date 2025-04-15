{ pkgs ? import <nixpkgs> {} }:
    
pkgs.mkShell {

  nativeBuildInputs = with pkgs.buildPackages; [ 
    gcc
    gdb
    (python311.withPackages (python-pkgs: with python-pkgs; [
      matplotlib
      numpy
      pandas
      wand
      requests
    ]))
    cpulimit
    bash
    ffmpeg_6-full
    gnumake
    libGL
    libGLU
    libglvnd
		mesa
    freeglut
  ];

  buildInputs = with pkgs; [
		libGL
    libGLU
		libglvnd
		mesa
    freeglut
  ];
  LD_LIBRARY_PATH = with pkgs; lib.makeLibraryPath [
		libGL
    libGLU
  ];
  CC="gcc";
  CFLAGS = "-I${pkgs.mesa.dev}/include:${pkgs.freeglut.dev}/include -Wall";
  LDFLAGS = "-L${pkgs.mesa.dev}/lib:${pkgs.freeglut.dev}/lib";
  LDLIBS = "-lGL -lGLU -lglut -lm";
  LIBGL_DRIVERS_PATH = "/run/opengl-driver/lib/dri";
  DEBUG_FLAGS = "-g -O0";
  BUILD_DIR = "./build/";
  BIN_DIR = "./bin/";
}
