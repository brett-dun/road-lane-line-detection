

#include <iostream>

// #include "linearRegression.hpp"
#include "laneLines.hpp"


using namespace std;


int main(int argc, char const *argv[])
{
	// quick testing of the linear regression function
	// std::vector<float> x{6,5,11,7,5,4,4};
	// std::vector<float> y{2,3,9,1,8,7,5};

	// float m, b;
	// linearRegression(x, y, m, b);

	// cout << "slope is: " << m << endl;


	// annotateImage("solidWhiteCurve.jpg");
	// annotateVideo("videos/challenge.mp4");
	annotateVideo(argv[1]);

	return 0;
}
