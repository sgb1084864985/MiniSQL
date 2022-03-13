#include <iostream>
#include "API.h"
#include <vector>
using namespace std;

int main()
{
	Interpreter IP;
	int ret = 1;
	cout << "Welcome to our miniSQL!" << endl;

	while (ret)
	{
		try {
			cout << ">>>";
			IP.GetQS();
			ret = IP.Execute();
		}
		catch (TableException te)
		{
			cout << te.what() << endl;
		}
		catch (QueryException qe)
		{
			cout << qe.what() << endl;
		}
	}
	return 0;
}