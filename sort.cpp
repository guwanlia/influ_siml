#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <vector>

using namespace std;

typedef struct qunima
{
    int userId;
    int blockNum;
}UserBlock;

int partition(vector<UserBlock>::iterator it, int p, int r)
{
    UserBlock key = *(it+r);
    UserBlock tmp;
    int i = p-1;
    for (int j = p; j < r; j++)
    {
	if ((*(it+j)).userId < key.userId)
	{
	    i++;
	    tmp = *(it+j);
	    *(it+j) = *(it+i);
	    *(it+i) = tmp;
	}
    }
    *(it+r) = *(it+i+1);
    *(it+i+1) = key;
    return i+1;
}
void quick_sort(vector<UserBlock>::iterator it, int p, int r)
{
    while (p < r)
    {
	int q;
	q = partition(it, p, r);
	quick_sort(it, p, q-1);
	p = q + 1;
    }
}

int main(int argc, char *argv[])
{
    ifstream fin("/home/guwanli/experiment/retweet/infl_siml/block.txt");
    FILE* fout = fopen("/home/guwanli/experiment/retweet/infl_siml/sorted_block.txt", "w");
    string line;
    int userId, followeeId, blockNum;
    int n = 0;
    while (getline(fin, line))
    {
	istringstream is(line);
	is >> userId;
	vector<UserBlock>  followeeVector;
	UserBlock tmp;
	while (is >> followeeId >> blockNum)
	{
	    tmp.userId = followeeId;
	    tmp.blockNum = blockNum;
	    followeeVector.push_back(tmp);
	}

	quick_sort(followeeVector.begin(), 0, followeeVector.size()-1);
	

	fprintf(fout, "%d ", userId);
	for (unsigned int i = 0; i < followeeVector.size(); ++i)
	{
	    fprintf(fout, "%d %d ", followeeVector[i].userId, followeeVector[i].blockNum);
	}
	fprintf(fout, "\n");
	if (++n % 100000 == 0)
	{
	    printf("%d\n", n);
	}
    }
    
    return 0;
}

