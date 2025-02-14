#ifndef WEIGHTSFILEMANAGER_H
#define WEIGHTSFILEMANAGER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>

class WeightsFileManager
{
public:
	WeightsFileManager(int matrixSize);
	void createWeights(int nPoints, int version);

private:
	std::vector<std::pair<double, double>> createRectangles(unsigned int n, int maxRange);
	std::vector<double> computeDistances(const std::vector<std::pair<double, double>>& points);

	void writeWeightsToFile(const std::string& filename, const std::vector<double>& weights, int nCols);

	const int LINE_LENGTH = 5;
	const int SIDE_LENGTH = 200;
	std::vector<std::vector<bool>> posTable;
};

#endif // !WEIGHTSFILEMANAGER_H

