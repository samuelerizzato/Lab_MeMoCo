#include <cstdio>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include "cpxmacro.h"
#include "CplexSolver.h"
#include "LSSolver.h"

using namespace std;

// error status and messagge buffer (from cpxmacro.h)
int status;
char errmsg[BUF_SIZE];

// data
vector<vector<double>> c;

void readWeights(const string& filename, int size)
{
	ifstream weightsFile(filename);
	double weight;

	c.resize(size);
	for (auto& row : c)
	{
		row.resize(size);
	}

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			weightsFile >> weight;
			c[i][j] = weight;
		}
	}

	weightsFile.close();
}

void printVariables(CEnv env, Prob lp, int N, const vector<vector<int>>& map_x, const vector<vector<int>>& map_y)
{
	cout << "----- DECISION VARIABLES -----" << endl;
	int numcols = CPXgetnumcols(env, lp);
	vector<double> varVals(numcols);
	CPXgetx(env, lp, &varVals[0], 0, numcols - 1);

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			int yIndex = map_y[i][j];
			if (yIndex > -1 && varVals[yIndex] > 0)
			{
				cout << "y_" << i << "_" << j << endl;
			}
		}
	}

	for (int i = 0; i < N; i++)
	{
		for (int j = 1; j < N; j++)
		{
			int xIndex = map_x[i][j];
			if (xIndex > -1 && varVals[xIndex] > 0)
			{
				cout << "x_" << i << "_" << j << ": " << varVals[xIndex] << endl;
			}
		}
	}
}

void printLSResults(const LSSolver& ls, const vector<vector<double>>& c, const TSPSolution& result, double duration)
{
	cout << "** Solution found with value: " << ls.evaluate(result, c) << "\n";
	cout << "** Duration: " << duration / 1000000.0 << " seconds" << endl;
	cout << "** Local Search solution: "; result.print(); cout << endl;
}

void solveWithCplex(int N)
{
	///// init
	DECL_ENV(env);
	DECL_PROB(env, lp);

	CplexSolver cpxSolver;
	cpxSolver.setupLP(env, lp, N, c);

	cout << "* Setup done" << endl;
    cout << "Solving problem..." << endl;

	auto startTime = chrono::high_resolution_clock::now();
	cpxSolver.solve(env, lp);
	auto stopTime = chrono::high_resolution_clock::now();

	printVariables(env, lp, N, cpxSolver.getMapX(), cpxSolver.getMapY());

	// print objective function value
	double objval;
	CHECKED_CPX_CALL(CPXgetobjval, env, lp, &objval);
	cout << "Objval: " << objval << endl;

	auto duration = chrono::duration_cast<chrono::microseconds>(stopTime - startTime);
	cout << "Duration: " << duration.count() / 1000000.0 << " seconds" << endl;

	// print overall solution information on a .sol file
	CHECKED_CPX_CALL(CPXsolwrite, env, lp, "tsp_board.sol");
	///// free memory
	CPXfreeprob(env, &lp);
	CPXcloseCPLEX(&env);
}

void solveWithRandomInitLS(const vector<vector<double>>& c)
{
	LSSolver rils;
	TSPSolution initSol(c.size());
	rils.initRnd(initSol);
	cout << "*Solution initialized with value: " << rils.evaluate(initSol, c) << "\n";
	cout << "Solving problem..." << endl;

	auto startTime = chrono::high_resolution_clock::now();
	TSPSolution result = rils.solve(c, initSol);
	auto stopTime = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::microseconds>(stopTime - startTime);
	printLSResults(rils, c, result, duration.count());
}

void solveWithBestInsertionInitLS(const vector<vector<double>>& c)
{
	LSSolver bils;
	TSPSolution initSol(c.size());
	bils.initBestInsertion(c, initSol);
	cout << "*Solution initialized with value: " << bils.evaluate(initSol, c) << "\n";
	cout << "Solving problem..." << endl;

	auto startTime = chrono::high_resolution_clock::now();
	TSPSolution result = bils.solve(c, initSol);
	auto stopTime = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::microseconds>(stopTime - startTime);
	printLSResults(bils, c, result, duration.count());
}

void solveWithRandomMultistartLS(const vector<vector<double>>& c)
{
	LSSolver rmls;
	TSPSolution initSol(c.size());
	
	cout << "Number of starts: ";
	int numStarts;
	cin >> numStarts;
	
	cout << "Solving problem..." << endl;
	auto startTime = chrono::high_resolution_clock::now();
	TSPSolution result = rmls.rndMultiStartSolve(c, initSol, numStarts);
	auto stopTime = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::microseconds>(stopTime - startTime);
	printLSResults(rmls, c, result, duration.count());
}

int main(int argc, char const* argv[])
{
	try
	{
		int version;
		int N;
		cout << "Insert nodes: \n> ";
		cin >> N;
		cout << "Insert version: \n> ";
		cin >> version;

		string filename = "../data/tsp" + to_string(N) + "." + to_string(version) + ".dat";
		readWeights(filename, N);

		cout << "* Weights matrix initialized" << endl;

		cout << "1) Cplex\n";
		cout << "2) Random init. local search\n";
		cout << "3) Best insertion init. local search\n";
		cout << "4) Random init. + multistart local search\n";

		cout << "Choose method [1/2/3/4]: ";
		int method;
		cin >> method;

		switch (method)
		{
			case 1: solveWithCplex(N); break;
			case 2: solveWithRandomInitLS(c); break;
			case 3: solveWithBestInsertionInitLS(c); break;
			case 4: solveWithRandomMultistartLS(c); break;
		}
	}
	catch (exception& e)
	{
		cout << ">>>EXCEPTION: " << e.what() << endl;
	}
	return 0;
}

