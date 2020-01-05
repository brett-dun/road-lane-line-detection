

#include "linearRegression.hpp"

#include <numeric>


//Sources:
//https://www.statisticshowto.datasciencecentral.com/probability-and-statistics/regression-analysis/find-a-linear-regression-equation/
//https://stackoverflow.com/questions/18939869/how-to-get-the-slope-of-a-linear-regression-line-using-c

void linearRegression(std::vector<float> &x, std::vector<float> &y, float &m, float &b)
{

	const int n = x.size();

	//handle case where only 1 point is given
	if (n == 1)
	{
		//todo: what would be appropriate? maybe pretend the other point is (0,0)
		return;
	}

	//sum(x_i)
	const float sumX = std::accumulate(x.begin(), x.end(), 0.0);

	//sum(y_i)
	const float sumY = std::accumulate(y.begin(), y.end(), 0.0);

	//n * sum((x_i)^2)
	const float sumXX = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);

	//sum((x_i)*(y_i))
	const float sumXY = std::inner_product(x.begin(), x.end(), y.begin(), 0.0);

	//sum(x_i)^2
	const float sumXSquared = sumX * sumX;

	//n * sum((x_i)^2) - sum(x_i)^2
	const float denominator = (n* sumXX) - sumXSquared;
	//todo: check that denominator is not 0?
	
	//slope
	m = ((n * sumXY) - (sumX * sumY)) / denominator;

	//intercept
	b = ((sumY * sumXX) - (sumX * sumXY)) / denominator;

}