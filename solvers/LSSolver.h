#ifndef LSSOLVER_H
#define LSSOLVER_H

#include <vector>
#include "TSPSolution.h"

struct TSPMove
{
	int substring_init;
	int substring_end;
};

class LSSolver
{
public:
	LSSolver();
	bool initRnd(TSPSolution& sol) const;
	bool initBestInsertion(const std::vector<std::vector<double>>& weights, TSPSolution& sol) const;
	double evaluate(const TSPSolution& sol, const std::vector<std::vector<double>>& weights) const;
	TSPSolution solve(const std::vector<std::vector<double>> &weights, const TSPSolution& initSol) const;
	TSPSolution rndMultiStartSolve(const std::vector<std::vector<double>>& weights, const TSPSolution& initSol, int numStarts) const;
private:
	TSPSolution apply2optMove(const TSPSolution& tspSol, const TSPMove& move) const;
	double findFirstImprovementNeighbour(const std::vector<std::vector<double>>& weights, const TSPSolution& currSol, TSPMove& move) const;
	std::pair<int, int> findNearestNodesIndeces(const std::vector<int>& nodesToVisit, const std::vector<std::vector<double>>& weights) const;
};

#endif // !LSSOLVER_H



