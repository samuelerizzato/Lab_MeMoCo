#include "WeightsFileManager.h"

WeightsFileManager::WeightsFileManager(int matrixSize) : posTable(matrixSize, std::vector<bool>(matrixSize, false)) 
{}

void WeightsFileManager::createWeights(int nPoints, int version)
{
	auto rectanglesPoints = createRectangles(nPoints, SIDE_LENGTH);
	std::vector<double> weights = computeDistances(rectanglesPoints);

	const int NAME_SIZE = 512;
	char filename[NAME_SIZE];

	snprintf(filename, NAME_SIZE, "./data/tsp%i.%i.dat", (int)rectanglesPoints.size(), version);
	writeWeightsToFile(filename, weights, rectanglesPoints.size());
}

std::vector<std::pair<double, double>> WeightsFileManager::createRectangles(unsigned int nPoints, int maxRange)
{
	std::vector<std::pair<double, double>> points;
	double cellSize = maxRange / posTable.size();

	srand(time(NULL));

	while (points.size() < nPoints)
	{
		std::pair<int, int> pos(rand() % posTable.size(), rand() % posTable.size());

		int maxLength = std::min(LINE_LENGTH, (int)posTable.size() - pos.first);
		int width = rand() % maxLength + 1;

		maxLength = std::min(LINE_LENGTH, (int)posTable.size() - pos.second);
		int height = rand() % maxLength + 1;

		for (int j = 0; j < width; j++)
		{
			int pointPosX = pos.first + j;
			int pointPosY = pos.second;

			if (!posTable[pointPosX][pointPosY])
			{
				posTable[pointPosX][pointPosY] = true;
				points.push_back(std::make_pair(pointPosX * cellSize, pointPosY * cellSize));
			}

			pointPosY += height - 1;
			if (!posTable[pointPosX][pointPosY])
			{
				posTable[pointPosX][pointPosY] = true;
				points.push_back(std::make_pair(pointPosX * cellSize, pointPosY * cellSize));
			}
		}

		for (int j = 0; j < height; j++)
		{
			int pointPosX = pos.first;
			int pointPosY = pos.second + j;

			if (!posTable[pointPosX][pointPosY])
			{
				posTable[pointPosX][pointPosY] = true;
				points.push_back(std::make_pair(pointPosX * cellSize, pointPosY * cellSize));
			}

			pointPosX += width - 1;

			if (!posTable[pointPosX][pointPosY])
			{
				posTable[pointPosX][pointPosY] = true;
				points.push_back(std::make_pair(pointPosX * cellSize, pointPosY * cellSize));
			}
		}
	}

	if (points.size() > nPoints)
	{
		points.erase(points.begin() + nPoints, points.end());
	}

	return points;
}

std::vector<double> WeightsFileManager::computeDistances(const std::vector<std::pair<double, double>>& points)
{
	int nPoints = points.size();
	std::vector<double> weights(nPoints * nPoints);

	for (int i = 0; i < nPoints; i++)
	{
		for (int j = 0; j < nPoints; j++)
		{
			if (i == j) weights[i * nPoints + j] = 0.0;
			weights[i * nPoints + j] = std::sqrt(
				std::pow(points[i].first - points[j].first, 2) + std::pow(points[i].second - points[j].second, 2));
		}
	}

	return weights;
}

void WeightsFileManager::writeWeightsToFile(const std::string& filename, const std::vector<double>& weights, int nPoints)
{
	std::ofstream weightsFile(filename);

	for (int i = 0; i < nPoints; i++)
	{
		for (int j = 0; j < nPoints; j++)
		{
			weightsFile << weights[i * nPoints + j] << " ";
		}
		weightsFile << "\n";
	}
}