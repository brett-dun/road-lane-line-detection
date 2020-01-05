
#ifndef LANE_LINES_H
#define LANE_LINES_H


#include <string>


void annotateImage(const std::string& inputFileName, const std::string& outputFileName = "out.png");
void annotateVideo(const std::string& inputFileName, const std::string& outputFileName = "out.avi");


#endif