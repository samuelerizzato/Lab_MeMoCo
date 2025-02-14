#ifndef CPLEXSOLVER_H
#define CPLEXSOLVER_H

#include "cpxmacro.h"
#include <vector>

class CplexSolver
{
public:
	void setupLP(CEnv env, Prob lp, int N, const std::vector<std::vector<double>>& c);
	void solve(CEnv env, Prob lp);
	std::vector<std::vector<int>> getMapX();
	std::vector<std::vector<int>> getMapY();

private:
	std::vector<std::vector<int>> map_x;
	std::vector<std::vector<int>> map_y;
};

#endif // !CPLEXSOLVER_H
