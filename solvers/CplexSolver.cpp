#include "CplexSolver.h"

void CplexSolver::setupLP(CEnv env, Prob lp, int N, const std::vector<std::vector<double>>& c)
{
	int current_var_position = 0; //keep the position of the next variable in the internal array
	const int NAME_SIZE = 512;
	char name[NAME_SIZE];

	map_y.resize(N);
	for (int i = 0; i < N; ++i)
	{
		map_y[i].resize(N);
		for (int j = 0; j < N; ++j)
		{
			map_y[i][j] = -1;
		}
	}

	// add y vars [in the objective function: sum{i,j} c_ij y_ij]
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (i == j) continue;
			char ytype = 'B';
			double lb = 0.0;
			double ub = 1.0;
			snprintf(name, NAME_SIZE, "y_%i_%i", i, j);
			char* yname = (char*)(&name[0]);
			CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &(c[i][j]), &lb, &ub, &ytype, &yname);

			map_y[i][j] = current_var_position++;
		}
	}

	map_x.resize(N);
	for (int i = 0; i < N; i++)
	{
		map_x[i].resize(N);
		for (int j = 0; j < N; j++)
		{
			map_x[i][j] = -1;
		}
	}


	// add x vars
	for (int i = 0; i < N; i++)
	{
		for (int j = 1; j < N; j++)
		{
			if (i == j) continue;
			char xtype = 'C';
			double lb = 0.0;
			double ub = CPX_INFBOUND;
			snprintf(name, NAME_SIZE, "x_%i_%i", i, j);
			char* xName = (char*)(&name[0]);
			double xCoeff = 0.0;
			CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &xCoeff, &lb, &ub, &xtype, &xName);

			map_x[i][j] = current_var_position++;
		}
	}

	// CONSTRAINTS CREATION

	// add (10) constraints [ forall k, sum{i: x_ik exists} x_ik - sum{j: x_kj exists} x_kj == 1 ]
	for (int k = 1; k < N; k++)
	{
		std::vector<int> idx;
		std::vector<double> coef;
		char sense = 'E';
		for (int i = 0; i < N; i++)
		{
			if (map_x[i][k] < 0) continue;

			idx.push_back(map_x[i][k]);
			coef.push_back(1.0);
		}

		for (int j = 1; j < N; j++)
		{
			if (map_x[k][j] < 0) continue;

			idx.push_back(map_x[k][j]);
			coef.push_back(-1.0);
		}

		double rhs = 1.0;
		int matbeg = 0;
		CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, NULL);
	}


	// add (11) constraints [ forall i, sum{j: y_ij exists} y_ij == 1 ]
	for (int i = 0; i < N; i++)
	{
		std::vector<int> idx;
		std::vector<double> coef;
		char sense = 'E';
		for (int j = 0; j < N; j++)
		{
			if (map_y[i][j] < 0) continue;

			idx.push_back(map_y[i][j]);
			coef.push_back(1.0);
		}
		int matbeg = 0;
		double rhs = 1.0;
		CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, NULL);
	}


	// add (12) constraints [ forall j, sum{i: y_ij exists} y_ij == 1 ]
	for (int j = 0; j < N; j++)
	{
		std::vector<int> idx;
		std::vector<double> coef;
		char sense = 'E';
		for (int i = 0; i < N; i++)
		{
			if (map_y[i][j] < 0) continue;

			idx.push_back(map_y[i][j]);
			coef.push_back(1.0);
		}
		int matbeg = 0;
		double rhs = 1.0;
		CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, NULL);
	}



	// add (13) constraints [ forall {(i,j): j != 0} x_ij <= (|N| - 1)*y_ij ]
	for (int i = 0; i < N; i++)
	{
		for (int j = 1; j < N; j++)
		{
			if (map_x[i][j] < 0) continue;
			std::vector<int> idx = { map_x[i][j], map_y[i][j] };
			std::vector<double> coef = { 1.0, 1.0 - N };
			char sense = 'L';
			double rhs = 0.0;
			int matbeg = 0;
			CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], NULL, NULL);
		}
	}

	// print (debug)
	CHECKED_CPX_CALL(CPXwriteprob, env, lp, "tsp_board.lp", NULL);
}

void CplexSolver::solve(CEnv env, Prob lp)
{
	CHECKED_CPX_CALL(CPXmipopt, env, lp);
}

std::vector<std::vector<int>> CplexSolver::getMapX()
{
	return map_x;
}

std::vector<std::vector<int>> CplexSolver::getMapY()
{
	return map_y;
}
