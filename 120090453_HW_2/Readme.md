# Readme of CSC4140 Assignment II

## 1. How to compile the code?
### Step 1
Please change the working directory under the root directory.

    cd .
### Step 2
Use the following command to compile `main.cpp` file.

    cmake .
    make
Then you get `compile_run.sh` .
## 2. How to run the compile file?
### Run with GUI
If you don't need to set any parameters, just directly run `compile_run.sh` with the following command:

    ./compile_run.sh

or run with the following command:

    ./compile_run.sh -GUI

The default rotation angle is `0` and the rotation axis is `(0, 0, 1)`. If you want to set angle `ANGLE`:

    ./compile_run.sh -GUI ANGLE

If you want set both the angle and the axis:

    ./compile_run.sh -GUI Angle Rotation_Asix[0] Rotation_Asix[1] Rotation_Asix[2]
where axis is described as a vector of `(Rotation_Asix[0], Rotation_Asix[1], Rotation_Asix[2])`.


### Run without GUI
When the program runs without GUI, it would save the result as an image.

The image's path is under the root directory `./`.

If you don't need to set any parameters, just directly run `compile_run.sh` with the following command:

    ./compile_run.sh -s


The default rotation angle is `0` and the rotation axis is `(0, 0, 1)`. The default image file is saved as `result.png`. If you want to change file name or path:

    ./compile_run.sh -s FILE_NAME

If you want to set angle:

    ./compile_run.sh -s FILE_NAME ANGLE

If you want change both the angle and the axis:

    ./compile_run.sh -s FILE_NAME ANGLE Rotation_Asix[0] Rotation_Asix[1] Rotation_Asix[2]
where axis is described as a vector of `(Rotation_Asix[0], Rotation_Asix[1], Rotation_Asix[2])`.