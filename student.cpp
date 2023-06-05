#include <string>
#include <vector>

using namespace std;

struct Student
{
    int id;
    string name;
    string surname;
    int accountId;

    Student(int _id, string _name, string _surname, int accountid)
    {
        id = _id;
        name = _name;
        surname = _surname;
        accountId = accountid;
    }

    void print()
    {
        cout << "Id: " << id << ".\tSurname: " << surname << ".\tName: " << name << ".\tUser id: " << accountId << endl;
    }

    void save(ofstream &file)
    {
        file << id << ',' << name << ',' << surname << ',' << accountId;
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