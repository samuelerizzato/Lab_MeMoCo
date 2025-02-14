#include "LSSolver.h"

LSSolver::LSSolver()
{
	srand(time(NULL));
}

bool LSSolver::initRnd(TSPSolution& sol) const
{
	for (int i = 0; i < sol.sequence.size(); i++)
	{
		int idx1 = rand() % (sol.sequence.size() - 2) + 1;
		int idx2 = rand() % (sol.sequence.size() - 2) + 1;
		int tmp = sol.sequence[idx1];
		sol.sequence[idx1] = sol.sequence[idx2];
		sol.sequence[idx2] = tmp;
	}
	return true;
}

bool LSSolver::initBestInsertion(const std::vector<std::vector<double>>& weights, TSPSolution& sol) const
{
	std::vector<int> nodesToVisit(sol.sequence.begin(), sol.sequence.end() - 1);
	std::pair<int, int> nearestIndeces(findNearestNodesIndeces(nodesToVisit, weights));

	std::vector<int> cycle = { nearestIndeces.first, nearestIndeces.second, nearestIndeces.first };

	if (nearestIndeces.first < nearestIndeces.second)
	{
		std::swap(nearestIndeces.first, nearestIndeces.second);
	}
	nodesToVisit.erase(nodesToVisit.begin() + nearestIndeces.first);
	nodesToVisit.erase(nodesToVisit.begin() + nearestIndeces.second);

	for (int n = 0; n < sol.sequence.size() - 3; n++)
	{
		int rPos = 0;
		int r = nodesToVisit[rPos];
		int bestPos = 1;
		double minValue = weights[cycle[bestPos - 1]][r] + weights[r][cycle[bestPos]] - weights[cycle[bestPos - 1]][cycle[bestPos]];

		for (auto i = 0; i < nodesToVisit.size(); i++)
		{
			for (int j = 1; j < cycle.size() - 1; j++)
			{
				double cost = weights[cycle[j - 1]][nodesToVisit[i]] + weights[nodesToVisit[i]][cycle[j]]
					- weights[cycle[j - 1]][cycle[j]];
				if (cost < minValue)
				{
					bestPos = j;
					rPos = i;
					r = nodesToVisit[i];
					minValue = cost;
				}
			}
		}

		cycle.insert(cycle.begin() + bestPos, r);
		nodesToVisit.erase(nodesToVisit.begin() + rPos);
	}

	sol.sequence = cycle;
	std::cout << "### "; sol.print(); std::cout << " ###" << std::endl;

	return true;
}

double LSSolver::evaluate(const TSPSolution& sol, const std::vector<std::vector<double>>& weights) const
{
	double total = 0.0;
	for (int i = 0; i < sol.sequence.size() - 1; ++i) {
		int from = sol.sequence[i];
		int to = sol.sequence[i + 1];
		total += weights[from][to];
	}
	return total;
}

TSPSolution LSSolver::solve(const std::vector<std::vector<double>>& weights, const TSPSolution& initSol) const
{
	bool pending = true;
	//int iter = 0;
	TSPSolution currSol(initSol);
	TSPMove move;

	while (pending)
	{
		double neighbourValue = findFirstImprovementNeighbour(weights, currSol, move);
		//std::cout << "(" << ++iter << "ts) move " << move.substring_init << " , " << move.substring_end << " improves by " << neighbourValue << std::endl;
		if (neighbourValue < -1e-6)
		{
			currSol = apply2optMove(currSol, move);
		}
		else
		{
			pending = false;
		}
	}

	return currSol;
}

TSPSolution LSSolver::rndMultiStartSolve(const std::vector<std::vector<double>>& weights, const TSPSolution& initSol, int numStarts) const
{
	TSPMove move;
	TSPSolution bestSol(initSol);
	double bestSolValue = evaluate(bestSol, weights);

	for (int i = 0; i < numStarts; ++i)
	{
		bool pending = true;
		TSPSolution currSol(initSol);
		initRnd(currSol);
		double improvement = evaluate(currSol, weights);

		while (pending)
		{
			double neighbourValue = findFirstImprovementNeighbour(weights, currSol, move);
			//std::cout << "(" << i << "ts) move " << move.substring_init << " , " << move.substring_end << " improves by " << neighbourValue << std::endl;
			if (neighbourValue < -1e-6)
			{
				currSol = apply2optMove(currSol, move);
				improvement += neighbourValue;
			}
			else
			{
				pending = false;
			}
		}

		if (improvement < bestSolValue)
		{
			bestSolValue = improvement;
			bestSol = currSol;
			//std::cout << "(" << i << "start) current best value is: " << bestSolValue << '\n';
		}
	}

	return bestSol;
}

TSPSolution LSSolver::apply2optMove(const TSPSolution& tspSol, const TSPMove& move) const
{
	TSPSolution tmpSol(tspSol);
	for (int i = move.substring_init; i <= move.substring_end; i++)
	{
		tmpSol.sequence[i] = tspSol.sequence[move.substring_end - (i - move.substring_init)];
	}
	return tmpSol;
}

double LSSolver::findFirstImprovementNeighbour(const std::vector<std::vector<double>>& weights, const TSPSolution& currSol, TSPMove& move) const
{
	for (int start = 1; start < currSol.sequence.size() - 2; start++)
	{
		int h = currSol.sequence[start - 1];
		int i = currSol.sequence[start];
		for (int end = start + 1; end < currSol.sequence.size() - 1; end++)
		{
			int j = currSol.sequence[end];
			int l = currSol.sequence[end + 1];
			double variation = weights[h][j] + weights[i][l] - weights[h][i] - weights[j][l];
			if (variation < -1e-6)
			{
				move.substring_init = start;
				move.substring_end = end;
				return variation;
			}
		}
	}

	return 0.0;
}

std::pair<int, int> LSSolver::findNearestNodesIndeces(const std::vector<int>& nodesToVisit, const std::vector<std::vector<double>>& weights) const
{
	int idx1 = 0;
	int idx2 = 1;
	double minValue = weights[nodesToVisit[idx1]][nodesToVisit[idx2]] + weights[nodesToVisit[idx2]][nodesToVisit[idx1]];
	for (int i = 0; i < nodesToVisit.size(); i++)
	{
		for (int j = 2; j < nodesToVisit.size(); j++)
		{
			double currValue = weights[nodesToVisit[i]][nodesToVisit[j]] + weights[nodesToVisit[j]][nodesToVisit[i]];
			if (i != j && currValue < minValue)
			{
				idx1 = i;
				idx2 = j;
				minValue = currValue;
			}
		}
	}

	return std::pair<int, int>(idx1, idx2);
}
