#include <cstdio>
//#include <set>
#include <list>
#include <cstring>
#include <vector>

using namespace std;

class Vector:public vector<int>
{
public:
    bool find(int el) const{
	int low = 0;
	int high = size()-1;
	int index, tmpValue;
	while (low <= high)
	{
	    index = (low+high)/2;
	    tmpValue = (*this)[index];
	    if (el == tmpValue)
	    {
		return true;
	    }
	    else if (el < tmpValue)
	    {
		high = index-1;
	    }
	    else
	    {
		low = index+1;
	    }
	}
	return false;
    }
};

//num of all the users
const int N = 1787443;
//const int N = 500000;

//all followees of each user
//set<int> g_followee[N];
Vector g_followee[N];

//the file contains relationships of all users
const char* g_networkFile = "/home/guwanli/IJCAI-2013-retweet/Data/weibo_network.txt";

//store blocks
FILE* g_block;

void DpFirSearch(int mapedUserId, int blockNum, bool* markFollowee,
		 const vector<int>& userFollowee, const list<int>* neigh)
{
    if (markFollowee[mapedUserId] == false)
    {
	markFollowee[mapedUserId] = true;
	fprintf(g_block, "%d %d ", userFollowee[mapedUserId], blockNum);
	for (list<int>::const_iterator it = neigh[mapedUserId].begin();
	     it != neigh[mapedUserId].end(); ++it)
	{
	    if (markFollowee[*it] == false)
	    {
		DpFirSearch(*it, blockNum, markFollowee,
			    userFollowee, neigh);
	    }
	}
    }
}

void CalBlock(int userId)
{
    Vector& userFollowee = g_followee[userId];
    int followeeSize = userFollowee.size();
 
    bool* markFollowee = new bool[followeeSize];
    list<int>* neigh = new list<int>[followeeSize];

    memset(markFollowee, 0, followeeSize*sizeof(bool));
    
    for (int i = 0; i < followeeSize; ++i)
    {
	int followeeId1 = userFollowee[i];
	const Vector& id1Followee = g_followee[followeeId1];
	for (int j = i+1; j < followeeSize; ++j)
	{
	    int followeeId2 = userFollowee[j];
	    Vector& id2Followee = g_followee[followeeId2];
	    if (id1Followee.find(followeeId2) && id2Followee.find(followeeId1))
	    {
		neigh[i].push_back(j);
		neigh[j].push_back(i);
	    }
	}
    }

    fprintf(g_block, "%d ", userId);
    int blockNum = 0;
    for (int i = 0; i < followeeSize; ++i)
    {
	if (markFollowee[i] == false)
	{
	    DpFirSearch(i, blockNum, markFollowee, userFollowee, neigh);
	    blockNum++;
	}
    }
    fprintf(g_block, "\n");

    delete [] markFollowee;
    markFollowee = NULL;
    delete [] neigh;
    neigh = NULL;
}

void LoadNetwork()
{
    printf("load network\n");
    FILE* f = fopen(g_networkFile, "r");
    int userId, followeeSize, recipFlag, n = 0;
    fscanf(f, "%d %d\n", &userId, &followeeSize);
    while (fscanf(f, "%d %d", &userId, &followeeSize) > 0)
    {
	Vector& followeeSet = g_followee[userId];
	for (int i = 0; i < followeeSize; ++i)
	{
	    if (fscanf(f, "%d %d", &userId, &recipFlag) > 0)
	    {
		if (userId < N)
		{
		    followeeSet.push_back(userId);
		}
	    }
	}
	if (++n % 100000 == 0)
	{
	    printf("%d\n", n);
	}
	if (n > N-1)
	{
	    break;
	}
    }
    fclose(f);
    printf("load network done\n");
    printf("calculate blocks\n");
    g_block = fopen("block.txt", "w");
    for (int i = 0; i < N; ++i)
    {
	if ((i+1) % 100000 == 0)
	{
	    printf("%d\n", i+1);
	}
	if (g_followee[i].size() == 0)
	{
	    fprintf(g_block, "%d \n", i);
	    continue;
	}
	CalBlock(i);
    }
    fclose(g_block);
    printf("calculate blocks done\n");
}

int main(int argc, char *argv[])
{
    LoadNetwork();
    return 0;
}
