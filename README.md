# Road Lane Line Detection

This project is a C++ port of https://github.com/georgesung/road_lane_line_detection and detects lane lines using OpenCV.

Another very similar project is described here: https://towardsdatascience.com/carnd-project-1-lane-lines-detection-a-complete-pipeline-6b815037d02c?#.hcsizymg8

For now, opencv is the only dependency needed to build and run this code.

## Usage
```
./executable [options]

Options:
  -h 				Show this help message and exit.
  -i INPUT_FILE		Input video/image file.
  -o OUTPUT_FILE	Output video/image file.
  -I 				Annotate image (defaults to annotating video)
```

## How it Works
1. Filter out the the parts of the image that aren't white or yellow.
2. Convert to grayscale.
3. Blur the image to reduce noise.
4. Detect edges.
5. Filter out the part of the image that we don't expect the lane in.
6. Identify lines using probabilistic hough lines.
7. Draw lines onto the original image.