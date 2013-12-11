#include <iostream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <fstream>

using namespace std;

typedef struct qunima
{
    int userId;
    int blockNum;
}UserBlock;

class Vector:public vector<UserBlock>
{
public:
    const_iterator find(int el) const
    {
	int low = 0;
	int high = size()-1;
	int index, tmpValue;
	const_iterator it = this->begin();
	while (low <= high)
	{
	    index = (low+high)/2;
	    tmpValue = (*(it+index)).userId;
	    if (el == tmpValue)
	    {
		return it+index;
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
	return this->end();
    }
};

class DateTime
{
public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    bool operator < (const DateTime& dt) const{
	if (year != dt.year)
	{
	    return year < dt.year ? true : false;
	}
	if (month != dt.month)
	{
	    return month < dt.month ? true : false;
	}
	if (day != dt.day)
	{
	    return day < dt.day ? true : false;
	}
	if (hour!= dt.month)
	{
	    return hour< dt.hour? true : false;
	}
	if (minute != dt.minute)
	{
	    return minute < dt.minute ? true : false;
	}
	if (second != dt.second)
	{
	    return second < dt.second ? true : false;
	}
	return false;
    }
};

DateTime ParseTime(const string& dt)
{
    DateTime d;
    sscanf(dt.c_str(), "%d-%d-%d-%d:%d:%d",
	   &d.year, &d.month, &d.day, &d.hour, &d.minute, &d.second);
    return d;
}

const int N = 1787443;
const char* g_networkFile = "/home/guwanli/IJCAI-2013-retweet/Data/weibo_network.txt";
const char* g_blockFile = "/home/guwanli/experiment/retweet/infl_siml/sorted_block.txt";
const char* g_tweetFile = "/home/guwanli/IJCAI-2013-retweet/Data/total.txt";
vector<int> g_follower[N];
Vector g_followeeCircle[N];
map<int, int> g_activeCircle[7];
map<int, int> g_noactiveCircle[7];
int numMap[31];
int g_maxBlock[N];

void LoadNumMap()
{
    for (int i = 2; i < 6; ++i)
	numMap[i] = i - 2;
    for (int i = 6; i <= 10; ++i)
	numMap[i] = 4;
    for (int i = 11; i <= 20; ++i)
	numMap[i] = 5;
    for (int i = 21; i <= 30; ++i)
	numMap[i] = 6;

    FILE* f = fopen("/home/guwanli/experiment/retweet/infl_siml/blocknum.txt", "r");
    int num;
    int i = 0;
    while (fscanf(f, "%d\n", &num) > 0)
    {
	g_maxBlock[i] = num;
	i++;
    }
}

void LoadNetwork()
{
    printf("load network\n");
    FILE* f = fopen(g_networkFile, "r");
    int userId, followeeSize, recipFlag, n = 0;
    fscanf(f, "%d %d\n", &userId, &followeeSize);
    while (fscanf(f, "%d %d", &userId, &followeeSize) > 0)
    {
	int followerId = userId;
	for (int i = 0; i < followeeSize; ++i)
	{
	    if (fscanf(f, "%d %d", &userId, &recipFlag) > 0)
	    {
		if (userId < N)
		{
		    g_follower[userId].push_back(followerId);
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
    printf("load network down\n");
}

void LoadCircle()
{
    printf("load circle\n");
    ifstream f(g_blockFile);
    string line;
    int userId, blockNum, n = 0;
    while (getline(f, line))
    {
	istringstream is(line);
	is >> userId;
	Vector& circleVector = g_followeeCircle[userId];
	UserBlock tmp;
	while (is >> userId >> blockNum)
	{
	    tmp.userId = userId;
	    tmp.blockNum = blockNum;
	    circleVector.push_back(tmp);
	}
	if (++n % 100000 == 0)
	{
	    printf("%d\n", n);
	}
    }
    f.close();
    printf("load circle done\n");
}

void OutputResult()
{
    printf("output\n");
    int idx[] = {2, 3, 4, 5, 10, 20, 30};
    for (int i = 0; i < 7; ++i)
    {
	char name[32];
	sprintf(name, "circle_prob_size%d.txt", idx[i]);
	FILE *f = fopen(name, "w");
	map<int, int>::const_iterator iter = g_activeCircle[i].begin();
	while (iter != g_activeCircle[i].end())
	{
	    int n = iter->first;
	    int r = iter->second;
	    int nr = g_noactiveCircle[i][n];
	    fprintf(f, "%d\t%d\t%d\t%f\n", n, r, nr, double(r) / (r + nr));
	    ++iter;
	}
	fclose(f);
    }
    printf("output done!\n");
}

int CountCircle(int userId, const vector<int>& activeNeigh)
{
    set<int> blockSet;
    int activeNum = activeNeigh.size();
    const Vector& circleVector = g_followeeCircle[userId];
    Vector::const_iterator it;
    for (int i = 0;i < activeNum; ++i)
    {
	it = circleVector.find(activeNeigh[i]);
	blockSet.insert(it->blockNum);
    }
    return blockSet.size();
}

void CalOneTweet(const map<int, DateTime>& retweetMap)
{
    map<int, DateTime>::const_iterator it1, it2, endIt;
    
    //first: store all the retweet or not relationship for one tweet
    map<int, vector<int> > retweetRelation, noRetweetRelation;
    
    int activeNum, circleNum, userId, userId1, userId2, followerSize, i;
    
    it1 = retweetMap.begin();
    endIt = retweetMap.end();
    while (it1 != endIt)
    {
	userId1 = it1->first;
	const DateTime& retweetDateTime = it1->second;
	const vector<int>& followerVector = g_follower[userId1];
	followerSize = followerVector.size();
	for (i = 0; i < followerSize; ++i)
	{
	    userId2 = followerVector[i];
	    it2 = retweetMap.find(userId2);
	    
	    //for each follower, record retweet or not
	    if (it2 != endIt)
	    {
		if (retweetDateTime < it2->second)
		{
		    retweetRelation[userId2].push_back(userId1);
		}
	    }
	    else
	    {
		noRetweetRelation[userId2].push_back(userId1);
	    }
	}
	it1++;
    }

    //second:
    map<int, vector<int> >::const_iterator it = retweetRelation.begin(), end = retweetRelation.end();

    while (it != end)
    {
	const vector<int>& activeNeigh = it->second;
	activeNum = activeNeigh.size();
	userId = it->first;
	if (activeNum > 1)
	{
	    if (activeNum <= 30)
	    {
		activeNum = numMap[activeNum];
	    }
	    else
	    {
		activeNum = 6;
	    }
	    //circleNum = CountCircle(userId, activeNeigh);
	    //circleNum = g_maxBlock[userId];
	    circleNum = g_followeeCircle[userId].size() / activeNeigh.size();
	    g_activeCircle[activeNum][circleNum]++;
	}
	it++;
    }
    it = noRetweetRelation.begin();
    end = noRetweetRelation.end();
    while (it != end)
    {
	const vector<int>& activeNeigh = it->second;
	activeNum = activeNeigh.size();
	userId = it->first;
	if (activeNum > 1)
	{
	    if (activeNum <= 30)
	    {
		activeNum = numMap[activeNum];
	    }
	    else
	    {
		activeNum = 6;
	    }
	    //circleNum = CountCircle(userId, activeNeigh);
	    //circleNum = g_maxBlock[userId];
	    circleNum = g_followeeCircle[userId].size() / activeNeigh.size();
	    g_noactiveCircle[activeNum][circleNum]++;
	}
	it++;
    }
}

void CalProb()
{
    printf("calculate probability\n");
    ifstream f(g_tweetFile);
    string line, tweetTime, origTweetId;
    int n = 0, userId;
    
    while (getline(f, line))
    {
	
	//each loop calculate one tweet
	istringstream is1(line);
	is1 >> origTweetId >> tweetTime;
	getline(f, line);
	if (tweetTime >= "2012-00-00-00:00:00")
	{
	    //read retweet information of one tweet
	    istringstream is2(line);
	    
	    //store retweet information of one tweet
	    map<int, DateTime> retweetMap;

	    while (is2 >> userId >> tweetTime)
	    {
		retweetMap[userId] =  ParseTime(tweetTime);
	    }
	    
	    CalOneTweet(retweetMap);
	}
	if (++n % 10000 == 0)
	{
	    printf("%d\n", n);
	    OutputResult();
	}
    }
    f.close();
    printf("calculate probability done\n");
}

int main(int argc, char *argv[])
{
    LoadNumMap();
    LoadNetwork();
    LoadCircle();
    CalProb();
    OutputResult();
    
    return 0;
}
