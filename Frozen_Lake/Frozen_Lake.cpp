#include <Windows.h>
#include <iostream>
#include <ctime>
#include <vector>


#define rows 30
#define columns 30
#define actions 8
#define epochs 50000
#define maxmoves 60
#define epsilondecay 0.000021


using namespace std;

bool gameover ;

double epsilon;
double discount = 0.99;
double learningrate = 0.8;


int boardstate[rows][columns];
int rewardmatrix[rows][columns];
int rewards[rows][columns];
int qtable[rows * columns][actions];

int position[1][2];

void printqtable()
{
	for (int x = 0; x < rows*columns; x++)
	{
		for (int y = 0; y <actions; y++)
		{
			cout << qtable[x][y] << " ";
		}
		cout << endl;
	}
	cout << endl;
}


void printrewards()
{
	for (int x = 0; x < rows; x++)
	{
		for (int y = 0; y < columns; y++)
		{
			cout << rewards[x][y] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void printboard()
{
	for (int x = 0; x < rows; x++)
	{
		for (int y = 0; y< columns; y++)
		{
			if (position[0][0] == x && position[0][1] == y)
				cout << "\033[1;45m" << "  " << "\033[1;49m";
			else if (rewards[x][y] == 1000)
			{
				cout << "\033[1;43m" << "  " << "\033[1;49m";
			}
			else if(boardstate[x][y]==1)
			cout << "\033[1;42m" << "  "<<"\033[1;49m";
			else if (boardstate[x][y] == 0)
			cout << "\033[1;41m" << "  " << "\033[1;49m";
		}
		cout << endl;
	}
	cout << endl;
}

void initiateRewardMatrix()
{
	for (int x = 0; x< rows; x++)
	{
		for (int y = 0; y < columns; y++)
		{

			if (x == rows - 1)
				rewardmatrix[x][y] = 1000;

			else if (boardstate[x][y] == 1)
			{
					rewardmatrix[x][y] = 5;
			}
			else
			{
				rewardmatrix[x][y] = -1000;
			}
		}
	}
}

void initiateqtable()
{
	for (int x = 0; x < rows*columns; x++)
	{
		for (int y = 0; y < actions; y++)
		{
			qtable[x][y] = 0;
		}
	}
}

void initiateBoard()
{
	for (int x = 0; x < rows; x++)
	{
		int broken = (rand() % (columns/4)) + columns/3;
		for (int y = 0; y < columns; y++)
		{
			if (x == 0 && y == 0)
			{
				boardstate[x][y] = 1;
				
			}
			else if (rand()%3 == 0 && broken != 0)
			{
				boardstate[x][y] = 0;
				broken -= 1;
			}
			else
			{
				boardstate[x][y] = 1;
			}
		}
	}
}

bool checkimpossible(int move)
{
	vector<int> impossiblemoves;
	if (position[0][0] == 0)
	{
		impossiblemoves.push_back(0);
		impossiblemoves.push_back(1);
		impossiblemoves.push_back(2);
	}
	if (position[0][1] == 0)
	{
		impossiblemoves.push_back(0);
		impossiblemoves.push_back(7);
		impossiblemoves.push_back(6);
	}

	if (position[0][1] == columns - 1)
	{
		impossiblemoves.push_back(2);
		impossiblemoves.push_back(3);
		impossiblemoves.push_back(4);
	}

	for (int x = 0; x < impossiblemoves.size(); x++)
	{
		if (move == impossiblemoves[x])
			return true;
	}
	return false;
}

int makeMove()
{
	if ((rand()%10)/(double)10 > epsilon)
	{
		//cout << "Optimal\n";
		int max = 3;
		for (int x = 0; x < actions; x++)
		{
			if (qtable[(position[0][0] * columns) + position[0][1]][x] > qtable[(position[0][0] * columns) + position[0][1]][max]&&checkimpossible(actions)==false)
			{
				max = x;
			}
		}
		return max;
	}
	else
	{
		//cout << "Explore: ";
		int random;
		do 
		{
			random = rand() % 8;
		} 
		while(checkimpossible(random)==true);

		return random;
	}
}

void resetEnvironment()
{
	position[0][0] = 0;
	position[0][1] = 0;

	for (int x = 0; x < rows; x++)
	{
		for (int y = 0; y < columns; y++)
		{
			rewards[x][y] = rewardmatrix[x][y];
		}
	}
}

void updateMove(int move)
{
	int prevrow = position[0][0];
	int prevcolumn = position[0][1];

	switch (move)
	{
	case 0:
		position[0][0] -= 1;
		position[0][1] -= 1;
		break;
	case 1:
		position[0][0] -= 1;
		break;
	case 2:
		position[0][0] -= 1;
		position[0][1] += 1;
		break;
	case 3:
		position[0][1] += 1;
		break;
	case 4:
		position[0][0] += 1;
		position[0][1] += 1;
		break;
	case 5:
		position[0][0] += 1;
		break;
	case 6:
		position[0][0] += 1;
		position[0][1] -= 1;
		break;
	case 7:
		position[0][1] -= 1;
		break;
	}

	//Check if out of bounds
	if (position[0][0] < 0 || position[0][1] < 0|| position[0][0] >=rows|| position[0][1] >=columns)
	{
		gameover = true;
		return;
	}

	//Calculate Max Q Value
	int max = 3;
	for (int x = 0; x < actions; x++)
	{
		if (qtable[(position[0][0] * columns) + position[0][1]][x] > qtable[(position[0][0] * columns) + position[0][1]][max])
		{
			max = x;
		}
	}


	//Q Table Update
	qtable[prevrow * columns + prevcolumn][move] = (1-learningrate)*(qtable[prevrow * columns + prevcolumn][move])+ learningrate*(position[0][0]*rewards[position[0][0]][position[0][1]]+ discount*qtable[(position[0][0] * columns) + position[0][1]][max]);


	//Check if Game Over
	if (boardstate[position[0][0]][position[0][1]] == 0||position[0][0]==rows-1)
	{
		gameover = true;
		return;
	}
}

int main()
{
	srand(time(0));

	while (true)
	{
		epsilon = 1;
		initiateBoard();
		initiateRewardMatrix();
		initiateqtable();


		for (int x = 1; x <= epochs; x++)
		{
			resetEnvironment();
			for (int y = 0; y < maxmoves; y++)
			{

				gameover = false;
				int move = makeMove();
				updateMove(move);


				if (x == 49999)
				{
					printboard();
					Sleep(500);
					system("cls");
				}

				if (gameover == true)
					break;
			}

			epsilon -= epsilondecay;
		}
	}
	printqtable();
}
