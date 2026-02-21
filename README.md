# iCub Image Processing Modules

A collection of YARP-based image processing and visual servoing modules for the iCub humanoid robot platform, developed in February 2016 as part of student work with the iCub simulator.

## Background

The [iCub](https://icub.iit.it/) is an open-source humanoid robot designed by the Istituto Italiano di Tecnologia (IIT) in Genova, Italy. It is used extensively in cognitive and embodied AI research. The robot's software ecosystem is built around [YARP](https://www.yarp.it/) (Yet Another Robot Platform), a middleware framework that provides inter-process communication through a port-based architecture. Processes exchange data -- including images, motor commands, and sensor readings -- by writing to and reading from named YARP ports connected over a software network.

This repository contains small, standalone modules written during hands-on experimentation with the iCub simulator (`iCubSim`). The modules demonstrate fundamental patterns of the YARP workflow: reading camera images from the robot, processing them, and sending motor commands back -- forming a basic visual servoing pipeline.

## Repository Structure

```
.
├── findLocation/          # Blue object detection (C++)
│   ├── findLocation.cpp
│   └── CMakeLists.txt
├── lookAtLocation/        # Head gaze control (C++)
│   ├── lookAtLocation.cpp
│   └── CMakeLists.txt
├── cropImage/             # Image cropping and resizing (C++)
│   ├── cropImage.cpp
│   └── CMakeLists.txt
├── resizeImage/           # Simple image resizing (C++)
│   └── resizeImage.cpp
├── yarp2python.py         # YARP image capture to Python/NumPy
└── python2yarp.py         # NumPy array to YARP image output
```

## Module Descriptions

### findLocation

**File:** `findLocation/findLocation.cpp`

Reads RGB images from the iCub simulator's left camera (`/icubSim/cam/left`) and performs simple color-based object detection. It scans every pixel for "blue-ish" values (where the blue channel exceeds both red and green by a factor of 1.2 plus an offset of 10) and computes the mean (x, y) position of all qualifying pixels. If the number of blue pixels exceeds a minimum threshold, the detected location is published as a 3-element vector `[x, y, 1]` (with confidence = 1) to the output port `/tutorial/target/out`. If no object is found, it outputs `[0, 0, 0]` (confidence = 0).

**YARP ports:**
- Input: `/tutorial/image/in` (receives from `/icubSim/cam/left`)
- Output: `/tutorial/target/out` (publishes target coordinates)

### lookAtLocation

**File:** `lookAtLocation/lookAtLocation.cpp`

Receives target coordinates from `findLocation` and translates them into velocity commands for the iCub's head joints, implementing a basic visual servoing loop. It connects to the iCub simulator's head control board (`/icubSim/head`) via the YARP `remote_controlboard` device driver, acquiring position, velocity, and encoder interfaces. The pixel offset from image center (assumed 320x240) is converted to velocity commands with a proportional gain of 0.1. Joint 3 (tilt) and joint 4 (pan) are driven to center the detected object in the camera frame. Commands are only sent when the confidence value exceeds 0.5; otherwise, velocities are set to zero.

**YARP ports:**
- Input: `/tutorial/target/in` (receives from `/tutorial/target/out`)
- Device: `/icubSim/head` (remote controlboard for head motors)

### cropImage

**File:** `cropImage/cropImage.cpp`

Reads images from a video source (`/movie/out`) and performs a two-step resize operation. First, the 640x480 input image is centrally cropped to 480x480 by removing 80 pixels from each side horizontally. Then the cropped square image is scaled down to 256x256 using YARP's built-in `copy` method. The result is written to an output port. This module was designed for preprocessing camera frames into square, fixed-size images suitable for downstream processing (e.g., neural network input).

**YARP ports:**
- Input: `/image/in` (receives from `/movie/out`)
- Output: `/cropped_image/out`

### resizeImage

**File:** `resizeImage/resizeImage.cpp`

A simpler variant of the cropping module. Reads images from a video source and directly resizes them to 256x256 using YARP's `copy` method, without the intermediate square-cropping step. This module lacks its own `CMakeLists.txt` and appears to be an earlier iteration that was superseded by `cropImage`.

**YARP ports:**
- Input: `/image/in` (receives from `/movie/out`)
- Output: `/resized_image/out`

### yarp2python.py

A Python utility that reads an RGB image from the iCub simulator camera (`/icubSim/cam`) into a NumPy array (shape 240x320x3, uint8) via YARP's Python bindings and displays it using matplotlib. Demonstrates how to bridge YARP's image transport into the Python/NumPy ecosystem for offline analysis or visualization.

### python2yarp.py

The reverse of `yarp2python.py`. Creates a random 240x320 float32 grayscale image as a NumPy array, wraps it in a YARP `ImageFloat`, and sends it to a `yarpview` display instance (`/view01`). Demonstrates publishing image data from Python into the YARP network.

## Dependencies

- **YARP** (Yet Another Robot Platform) -- core middleware; provides networking, image transport, and device drivers. The CMake files use `find_package(YARP REQUIRED)`.
- **CMake** >= 2.8.9 -- build system for the C++ modules.
- **C++ compiler** -- standard C++ toolchain (GCC/Clang).
- **iCub simulator** (`iCubSim`) -- provides the simulated robot with virtual cameras and motor control boards. Part of the broader [iCub software](https://github.com/robotology/icub-main) distribution.
- **Python 2.7** with YARP Python bindings -- for the Python bridge scripts.
- **NumPy** -- used by both Python scripts for image array handling.
- **matplotlib** -- used by `yarp2python.py` for image display.

## Building the C++ Modules

Each C++ module has its own `CMakeLists.txt` and is built independently:

```bash
cd findLocation
mkdir build && cd build
cmake ..
make
```

Repeat for `cropImage` and `lookAtLocation`. The `YARP_DIR` environment variable must point to a valid YARP build/install directory.

## Usage

These modules are designed to run alongside a YARP name server (`yarpserver`) and the iCub simulator. A typical session for the visual servoing demo:

```bash
# Terminal 1: Start the YARP name server
yarpserver

# Terminal 2: Start the iCub simulator
iCub_SIM

# Terminal 3: Run the blue object detector
./findLocation/build/findLocation_exe

# Terminal 4: Run the gaze controller
./lookAtLocation/build/lookAtLocation_exe
```

The `findLocation` and `lookAtLocation` modules together form a closed-loop visual servoing system: the robot detects a blue object in its camera view and moves its head to track it.

## Historical Note

This repository was created on February 18, 2016 as part of early student experimentation with the iCub humanoid robot platform and the YARP middleware. The code reflects tutorial-level exploration of YARP's image transport, color-based vision, and motor control APIs. It preserves the working state of these experiments as originally written.

## License

No license file is included in this repository.
