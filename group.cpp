#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "subject.cpp"

using namespace std;

struct Group
{
    int id;
    string name;
    vector<int> students;/*<student id>*/
    vector<int> subjects;/*<subject id>*/
    map<int, double> coefficients; /*<subject id, coefficient>*/
    map<int, map<int, int>> marks; /*<student id, <subject id, mark>>*/

    Group(string _name, int _id)
    {
        id = _id;
        name = _name;
    }

    Group(int _id, string _name, vector<int> _students, vector<int> _subjects, map<int, double> _coefficients, map<int, map<int, int>>& _marks)
    {
        id = _id;
        name = _name;
        students = _students;
        subjects = _subjects;
        coefficients = _coefficients;
        marks = (_marks);
    }

    void calculateTotalMarks()
{
    auto group = *this;
    for (int studentId : group.students)
    {
        int totalMarks = 0;

        for (int subjectId : group.subjects)
        {
            if (group.marks.find(studentId) != group.marks.end())
            {
                const auto& studentMarks = group.marks.at(studentId);
                if (studentMarks.find(subjectId) != studentMarks.end())
                {
                    int mark = studentMarks.at(subjectId);
                    double coefficient = group.coefficients.at(subjectId);
                    totalMarks += mark * coefficient;
                }
            }
        }

        cout << "Student ID: " << studentId << ", Total Marks: " << totalMarks << endl;
    }
}

    void print()
    {
        cout << id << ' ' << name << endl;
    }

    void save(ostream &file)
    {
        file << id << ',' << name << ',';
        for (int i = 0; i < students.size(); i++)
            file << students[i] << ' ';
        if (students.size() == 0)
        {
            file << -1;
        }
        file << ",";
        for (int i = 0; i < subjects.size(); i++)
            file << subjects[i] << ' ';
        if (subjects.size() == 0)
        {
            file << -1;
        }

        if (coefficients.size() == 0)
        {
            file << "," << 1 << "," << -1 << "," << -1;
        }
        else
        {
            file << "," << coefficients.size();

            for (auto &coefficient : coefficients)
            {
                file << "," << coefficient.first << "," << coefficient.second;
            }
        }

        if (marks.size() == 0)
        {
            file << "," << 1 << "," << 0 << "," << 1 << "," << 0 << "," << 0;
        }
        else
        {
            file << "," << marks.size();
            for (auto &studentmark : marks)
            {
                file << "," << studentmark.first << "," << studentmark.second.size();
                for (auto &mark : studentmark.second)
                {
                    file << "," << mark.first << "," << mark.second;
                }
            }
        }
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