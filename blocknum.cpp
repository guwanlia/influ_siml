#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    ifstream fin("/home/guwanli/experiment/retweet/infl_siml/block.txt");
    FILE* fout = fopen("/home/guwanli/experiment/retweet/infl_siml/blocknum.txt", "w");
    string line;
    int userId, followeeId, blockNum;
    int n = 0;
    while (getline(fin, line))
    {
	istringstream is(line);
	is >> userId;
	blockNum = 0;
	while (is >> followeeId >> blockNum)
	{
	    blockNum += 1;
	}
	fprintf(fout, "%d\n", blockNum);
	if (++n % 100000 == 0)
	{
	    printf("%d\n", n);
	}
    }
    
    return 0;
}

