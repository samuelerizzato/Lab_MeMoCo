#include "TSPSolution.h"

TSPSolution::TSPSolution(int numNodes) : sequence(numNodes + 1)
{
    for (int i = 0; i < numNodes; ++i) {
        sequence[i] = i;
    }
    sequence[numNodes] = 0;
}

TSPSolution::TSPSolution(const TSPSolution& tspSol) : sequence(tspSol.sequence.size())
{
    for (int i = 0; i < tspSol.sequence.size(); ++i) {
        sequence[i] = tspSol.sequence[i];
    }
}

TSPSolution& TSPSolution::operator=(const TSPSolution& other)
{
    if (this == &other) return *this;
    for (int i = 0; i < sequence.size(); i++) {
        sequence[i] = other.sequence[i];
    }
    return *this;
}

void TSPSolution::print() const
{
    for (int i = 0; i < sequence.size(); i++) {
        std::cout << sequence[i] << " ";
    }
}
