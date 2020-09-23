//ChatTracker.cpp

// This is a correct but inefficient implementation of
// the ChatTracker functionality.

#include "ChatTracker.h"
#include <string>
#include <vector>
#include <functional>
#include <list>
#include <stack>
#include <iostream>
using namespace std;

// This class does the real work of the implementation.

class ChatTrackerImpl
{
public:
	ChatTrackerImpl(int maxBuckets);
	void join(string user, string chat);
	int terminate(string chat);
	int contribute(string user);
	int leave(string user, string chat);
	int leave(string user);

	//hash function for strings
	unsigned int hashFunction(const string& hashMe)
	{
		hash<string>str_hash; //creates a string hasher
		unsigned int hashValue = str_hash(hashMe); //hash string

		unsigned int bucketNum = hashValue % m_maxBuckets;
		return bucketNum; //bucknum is index of where value is placed
	}

private:
	struct User
	{
		User(string name, string chat) : name(name)
		{
			chats.push_back(make_pair(0, chat));
		}
		string name;
		vector<pair<int, string>> chats;
	};
	struct Chat
	{
		Chat(string name) : name(name), count(0) {}
		string name;
		vector<string> users;
		int count;
	};

	int m_maxBuckets;
	vector<vector<User>> m_users;
	vector<vector<Chat>> m_chats;
};


ChatTrackerImpl::ChatTrackerImpl(int maxBuckets)
{
	m_maxBuckets = maxBuckets;
	m_users.resize(maxBuckets); //resize vector to size of maxBuckets
	m_chats.resize(maxBuckets);
}

void ChatTrackerImpl::join(string user, string chat)
{
	int index = hashFunction(user);
	int index2 = hashFunction(chat);
	bool isNew = true;
	//see if user already exists
	for (int i = 0; i < m_users[index].size(); ++i)
	{
		//if user already exists
		if (m_users[index][i].name == user)
		{
			int count = 0;
			//check if previously a part of that chat
			for (int j = 0; j < m_users[index][i].chats.size(); ++j)
			{
				//if previously part of this chat
				if (m_users[index][i].chats[j].second == chat)
				{
					//set count back to old count and erase pair
					count = m_users[index][i].chats[j].first;
					m_users[index][i].chats.erase(m_users[index][i].chats.begin() + j);
					break;
				}

			}
			//push back pair with count and chat
			m_users[index][i].chats.push_back(make_pair(count, chat));
			isNew = false;
			break;
		}
	}
	//if user is new create a new user
	if (isNew)
	{
		User u = User(user, chat);
		//place in m_users
		m_users[index].push_back(u);
	}

	// see if chat (parameter) is in m_chats by comparing names
	bool chatExists = false;
	bool userExists = false;
	for (int i = 0; i < m_chats[index2].size(); ++i)
	{
		if (m_chats[index2][i].name == chat)
		{
			chatExists = true;
			//does the user exist in this chat already
			for (int j = 0; j < m_chats[index2][i].users.size(); ++j)
			{
				if (m_chats[index2][i].users[j] == user)
				{
					userExists = true;
					break;
				}
			}
			//add user to chat's users
			if (!userExists)
			{
				m_chats[index2][i].users.push_back(user);
			}
			break;
		}
	}
	// if chat doesn't exist: create it and put in m_chats
	if (!chatExists)
	{
		Chat c = Chat(chat);
		m_chats[index2].push_back(c);
		// add user (parameter) to the new chat's users
		m_chats[index2][m_chats[index2].size() - 1].users.push_back(user);
		// c.users.push_back(user);
	}
}

int ChatTrackerImpl::terminate(string chat)
{
	int total = 0;
	//find the chat in m_chats
	int index = hashFunction(chat);
	for (int i = 0; i < m_chats[index].size(); ++i)
	{
		//if we find the chat
		if (m_chats[index][i].name == chat)
		{
			//all users associated with the chat are no longer associated
			while (m_chats[index][i].users.size() > 0) 
			{
				leave(m_chats[index][i].users[0], chat);
			}

			total = m_chats[index][i].count;
			//erase the chat from m_chats
			m_chats[index].erase(m_chats[index].begin() + i);
			break;
		}
	}
	return total;
}

int ChatTrackerImpl::contribute(string user)
{
	//contribute to the count of the user
	int index = hashFunction(user);
	//loop through bucket where our user is stored
	for (int i = 0; i < m_users[index].size(); ++i)
	{
		if (m_users[index][i].name == user)
		{
			//if the user has no current chat
			if (m_users[index][i].chats.size() == 0)
			{
				return 0;
			}
			//add one to this user's contributions
			m_users[index][i].chats[m_users[index][i].chats.size() - 1].first++;

			//add one to the current chat's contribution
			int index2 = hashFunction(m_users[index][i].chats[m_users[index][i].chats.size() - 1].second);
			//loop through the chat hash table
			for (int j = 0; j < m_chats[index2].size(); ++j)
			{
				//if we find our user's current chat
				if (m_users[index][i].chats[m_users[index][i].chats.size() - 1].second == m_chats[index2][j].name)
				{
					//add one to contribution
					m_chats[index2][j].count++;
					break;
				}
			}

			return m_users[index][i].chats[m_users[index][i].chats.size() - 1].first;
		}
	}
	return 0;
}

int ChatTrackerImpl::leave(string user, string chat)
{
	//find our user in hash table
	int index = hashFunction(user);
	int count = -1;
	for (int i = 0; i < m_users[index].size(); ++i)
	{
		if (m_users[index][i].name == user)
		{
			//find the chat
			for (int j = 0; j < m_users[index][i].chats.size(); ++j)
			{
				if (m_users[index][i].chats[j].second == chat)
				{
					//store the number of contributions
					count = m_users[index][i].chats[j].first;
					//erase chat in question
					m_users[index][i].chats.erase(m_users[index][i].chats.begin() + j);
					break;
				}
			}
			break;
		}
	}
	//erase user from the chat's vector of users
	int index2 = hashFunction(chat);
	for (int i = 0; i < m_chats[index2].size(); ++i)
	{
		if (m_chats[index2][i].name == chat)
		{
			//erase user from table of users
			for (int j = 0; j < m_chats[index2][i].users.size(); ++j)
			{
				//if we find our user
				if (m_chats[index2][i].users[j] == user)
				{
					m_chats[index2][i].users.erase(m_chats[index2][i].users.begin() + j);
					break;
				}
			}
			break;
		}
	}
	return count;
}

int ChatTrackerImpl::leave(string user)
{
	//find our user in hash table
	int index = hashFunction(user);
	int count = -1;
	string name;
	for (int i = 0; i < m_users[index].size(); ++i)
	{
		if (m_users[index][i].name == user)
		{
			//if user has no current chat
			if (m_users[index][i].chats.size() == 0)
			{
				return -1;
			}
			//store the number of contributions
			count = m_users[index][i].chats[m_users[index][i].chats.size() - 1].first;
			//store chat name
			name = m_users[index][i].chats[m_users[index][i].chats.size() - 1].second;
			//erase last chat
			m_users[index][i].chats.erase(m_users[index][i].chats.begin() + (m_users[index][i].chats.size() - 1));
			break;
		}
	}
	//erase user from the chat's vector of users
	int index2 = hashFunction(name);
	for (int i = 0; i < m_chats[index2].size(); ++i)
	{
		if (m_chats[index2][i].name == name)
		{
			//erase user from vector of users
			for (int j = 0; j < m_chats[index2][i].users.size(); ++j)
			{
				//if we find our user
				if (m_chats[index2][i].users[j] == user)
				{
					m_chats[index2][i].users.erase(m_chats[index2][i].users.begin() + j);
					break;
				}
			}
			break;
		}
	}
	return count;
}

//*********** ChatTracker functions **************

// These functions simply delegate to ChatTrackerImpl's functions.
// You probably don't want to change any of this code.

ChatTracker::ChatTracker(int maxBuckets)
{
	m_impl = new ChatTrackerImpl(maxBuckets);
}

ChatTracker::~ChatTracker()
{
	delete m_impl;
}

void ChatTracker::join(string user, string chat)
{
	m_impl->join(user, chat);
}

int ChatTracker::terminate(string chat)
{
	return m_impl->terminate(chat);
}

int ChatTracker::contribute(string user)
{
	return m_impl->contribute(user);
}

int ChatTracker::leave(string user, string chat)
{
	return m_impl->leave(user, chat);
}

int ChatTracker::leave(string user)
{
	return m_impl->leave(user);
}