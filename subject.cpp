#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct Subject
{
    int id;
    string name;

    Subject(int _id, string _name)
    {
        id = _id;
        name = _name;
    }

    void print()
    {
        cout << id << ' ' << name << endl;
    }

    void save(ostream &file)
    {
        file << id << ',' << name;     
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