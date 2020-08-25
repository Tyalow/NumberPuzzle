#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Node
{
public:

	int puzzle[9] = {};
	int zeroLocation;
	int parentZeroLocation;
	int depth;
	bool isWin;
	bool hasParent;
	vector<int> moves;

	//Root
	Node(int puzzleContents[9])
	{
		int zeroVal = -1;
		for (int i = 0; i < 9; i++)
		{
			puzzle[i] = puzzleContents[i];
			if (puzzleContents[i] == 0)
				zeroVal = i;
		}
		isWin = checkWin();
		hasParent = false;
		zeroLocation = zeroVal;
		depth = 0;
	}
	//Leaf
	Node(int puzzleContents[9], Node parentNode, int zeroLoc)
	{
		for (int i = 0; i < 9; i++)
		{			
			puzzle[i] = puzzleContents[i];
		}
		isWin = checkWin();
		hasParent = true;
		zeroLocation = zeroLoc;
		parentZeroLocation = parentNode.zeroLocation;	
		depth = parentNode.depth+1;
		moves = parentNode.moves;
		moves.push_back(zeroLoc);
	}	

	bool checkWin()
	{
		bool didIWin = true;
		int winningSolution[9] = {1, 2, 3, 4, 0, 5, 6, 7, 8};
		for (int i = 0; i < 9; i++)
		{
			if (winningSolution[i] != puzzle[i])
				didIWin = false;
		}
		return didIWin;
	}
};
#endif
