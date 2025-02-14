#ifndef TSPSOLUTION_H
#define TSPSOLUTION_H

#include <vector>
#include <iostream>

class TSPSolution
{
public:
	std::vector<int> sequence;

    TSPSolution(int numNodes);

    TSPSolution(const TSPSolution& tspSol);

    TSPSolution& operator=(const TSPSolution& other);

    void print() const;
};

#endif // !TSPSOLUTION_H
