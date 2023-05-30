#ifndef __user
#define __user
#include <iostream>
#include <string>
#include "sha256.h"

using namespace std;

struct User
{
    int id;
    string login;
    string password;
    bool isAdmin;

    User(int _id, string log, string pass, bool isadmin)
    {
        id = _id;
        login = log;
        password = pass;
        isAdmin = isadmin;
    }

    static User fromCredentials(string log, string pass)
    {
        SHA256 sha256;
        return User(0, log, sha256(log) + sha256(pass), false);
    }

    void print()
    {
        cout << id << ' ' << login << " " << password << ' ' << isAdmin << endl;
    }

    bool equalData(const User &inpUser)
    {
        return (inpUser.login == login) && (inpUser.password == password);
    }

    void save(ofstream &file)
    {
        file << id << ',' << login << ',' << password << ',' << isAdmin;
    }

    static vector<string> read(ifstream &file)
    {
        vector<string> data;
        string temp;

        while (getline(file, temp, ','))
        {
            data.push_back(temp);
        }
        return data;
    }
};
#endif