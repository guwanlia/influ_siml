#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <map>

using namespace std;

struct User
{
    int id;
    int biFollowCount;
    int city;
    bool veriFlag;
    int followerCount;
    string location;
    int province;
    int followeeCount;
    string name;
    bool gender;
    string createTime;
    int veriType;
    int staCount;
    string descri;
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

User g_users[N];

vector<int> g_follower[N];
// vector<int> g_followee[N];

map<int, int> g_userMap;
map<int, int> g_activeCircle;
map<int, long long> g_noactiveCircle;

void LoadUserMap()
{
    printf("load map.\n");
    FILE* f = fopen("/home/guwanli/IJCAI-2013-retweet/Data/uidlist.txt", "r");
    int userId, n = 0;

    while (fscanf(f, "%d\n", &userId) > 0)
    {
	g_userMap[userId] = n;
	n++;
    }
    fclose(f);
    printf("load map done!\n");
}

void LoadNetwork()
{
    printf("load network.\n");
    FILE* f = fopen("/home/guwanli/IJCAI-2013-retweet/Data/weibo_network.txt", "r");
    int userId, followeeSize, recipFlag, n = 0;
    fscanf(f, "%d %d\n", &userId, &followeeSize);
    while (fscanf(f, "%d %d", &userId, &followeeSize) > 0)
    {
	int followerId = userId;
	// vector<int>& followeeVector = g_followee[followerId];
	for (int i = 0; i < followeeSize; ++i)
	{
	    if (fscanf(f, "%d %d", &userId, &recipFlag) > 0)
	    {
		g_follower[userId].push_back(followerId);
		// followeeVector.push_back(userId);
	    }
	}
	if (++n % 100000 == 0)
	{
	    printf("%d\n", n);
	}
    }
    fclose(f);
    printf("load network down!\n");
}

void LoadProfile()
{
    printf("load profile.\n");
    
    for (int i = 0; i < 2; ++i)
    {
	stringstream ioStr;
	ioStr << i+1;
	const string fileName = string("/home/guwanli/IJCAI-2013-retweet/Data/userProfile/utf8_user_profile")
	    + ioStr.str() + string(".txt");
	// cout << fileName << endl;

	string line;
	ifstream profile(fileName.c_str());
	for (int i = 0; i < 15; ++i)
	{
	    getline(profile, line);
	}
	
	while (getline(profile, line))
	{
	    User tmpUser;

	    int userId = atoi(line.c_str());
	    tmpUser.id = g_userMap[userId];

	    getline(profile, line);
	    tmpUser.biFollowCount = atoi(line.c_str());

	    getline(profile, line);
	    tmpUser.city = atoi(line.c_str());

	    getline(profile, line);
	    if (line[0] == 'F')
	    {
		tmpUser.veriFlag = false;
	    }
	    else
	    {
		tmpUser.veriFlag = true;
	    }

	    getline(profile, line);
	    tmpUser.followerCount = atoi(line.c_str());

	    getline(profile, line);
	    tmpUser.location = line.substr(0, line.size()-1);

	    getline(profile, line);
	    tmpUser.province = atoi(line.c_str());

	    getline(profile, line);
	    tmpUser.followeeCount = atoi(line.c_str());

	    getline(profile, line);
	    tmpUser.name = line.substr(0, line.size()-1);

	    getline(profile, line);
	    if (line[0] == 'm')
	    {
		tmpUser.gender = true;
	    }
	    else
	    {
		tmpUser.gender = false;
	    }

	    getline(profile, line);
	    tmpUser.createTime = line.substr(0, line.size()-1);

	    getline(profile, line);
	    tmpUser.veriType = atoi(line.c_str());

	    getline(profile, line);
	    tmpUser.staCount = atoi(line.c_str());

	    getline(profile, line);
	    tmpUser.descri = line.substr(0, line.size()-1);

	    getline(profile, line);
	    
	    g_users[tmpUser.id] = tmpUser;
	}
	profile.close();
    }
    printf("load profile done!\n");
}

void CalOneTweet(const map<int, DateTime>& retweetMap)
{
    map<int, DateTime>::const_iterator itor1, itor2, endItor;

    map<int, vector<int> > retweetRelation, noRetweetRelation;

    int userId1, userId2;

    itor1 = retweetMap.begin();
    endItor = retweetMap.end();
    while (itor1 != endItor)
    {
	userId1 = itor1->first;
	const DateTime& retweetDateTime = itor1->second;
	const vector<int>& followerVector = g_follower[userId1];
	for (int i = 0; i < followerVector.size(); ++i)
	{
	    userId2 = followerVector[i];
	    itor2 = retweetMap.find(userId2);

	    if (itor2 != endItor)
	    {
		if (retweetDateTime < itor2->second)
		{
		    retweetRelation[userId2].push_back(userId1);
		}
	    }
	    else
	    {
		noRetweetRelation[userId2].push_back(userId1);
	    }
	}
	itor1++;
    }

    map<int, vector<int> >::const_iterator itor = retweetRelation.begin();
    int activeNum;
    while (itor != retweetRelation.end())
    {
	activeNum = (itor->second).size();
	g_activeCircle[activeNum]++;

	itor++;
    }
    itor = noRetweetRelation.begin();
    while (itor != noRetweetRelation.end())
    {
	activeNum = (itor->second).size();
	g_noactiveCircle[activeNum]++;

	itor++;
    }
}

void CalProb()
{
    printf("calculate probability.\n");
    
    ifstream retweetFile("/home/guwanli/IJCAI-2013-retweet/Data/total.txt");
    string line, tweetTime, origTweetId;
    int n = 0, userId;

    while (getline(retweetFile, line))
    {
	istringstream iOrigLine(line);
	iOrigLine >> origTweetId >> tweetTime;
	getline(retweetFile, line);
	if (tweetTime >= "2012-00-00-00:00:00")
	{
	    istringstream iReLine(line);
	    map<int, DateTime> retweetMap;

	    while (iReLine >> userId >> tweetTime)
	    {
		if (retweetMap.find(userId) == retweetMap.end())
		{
		    retweetMap[userId] = ParseTime(tweetTime);
		}
	    }
	    //****//
	    CalOneTweet(retweetMap);
	}
	if (++n % 10000 == 0)
	{
	    printf("%d\n", n);
	}
    }

    retweetFile.close();
    printf("calculate probability done!\n");
}

void OutputResult()
{
    printf("output.\n");

    FILE* out = fopen("sample_test.txt", "w");

    for (int i = 1; i < 31; ++i)
    {
	int activeNum = g_activeCircle[i];
	long long noActiveNum = g_noactiveCircle[i];
	double prob = activeNum / double(activeNum+noActiveNum);
	fprintf(out, "%d\t%d\t%lld\t%f\n", i, activeNum, noActiveNum, prob); 
    }

    fclose(out);
    printf("output done!\n");
}

int main(int argc, char *argv[])
{
    LoadNetwork();
    // LoadUserMap();
    // LoadProfile();

    CalProb();

    OutputResult();
    
    return 0;
}
