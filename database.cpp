#include "group.cpp"
#include "student.cpp"
#include "user.cpp"
#include <sstream>
#include <string>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;

struct DataBase
{
    int currentUserId = -1;
    int currentGroupId = -1;
    vector<User> users;
    vector<Group> groups;
    vector<Student> students;
    vector<Subject> subjects;
    const string dirPath = "./db";
    const string groupsPath = "./db/groups.dat";
    const string studentsPath = "./db/students.dat";
    const string usersPath = "./db/users.dat";
    const string marksPath = "./db/marks.dat";
    const string subjectsPath = "./db/subjects.dat";

    void showGroupStudentsWithMarks(int groupId)
    {
        auto group = getGroupById(groupId);
        auto mark = group.calculateTotalMarks();
        for (auto sudent : mark)
        {
            auto stud = getStudentById(sudent.first);
            cout << "Id: " << stud.id << ".\tName: " << stud.name << ".\tSurname: " << stud.surname << ".\tTotal Mark: " << sudent.second << endl;
        }
    }
    void showAllGroups()
    {
        if (groups.size() == 0)
        {
            cout << "Groups not added yet.\n";
            return;
        }
        for (int i = 0; i < groups.size(); i++)
        {
            groups[i].print();
        }
    }
    void showAllStudents()
    {
        sort(students.begin(),students.end(),compareBySurname);
        if (students.size() == 0)
        {
            cout << "Students not added yet.\n";
            return;
        }
        for (int i = 0; i < students.size(); i++)
        {
            students[i].print();
        }
    }
    void showAllUsers()
    {
        if (users.size() == 0)
        {
            cout << "Users not added yet.\n";
            return;
        }
        for (int i = 0; i < users.size(); i++)
        {
            users[i].print();
        }
    }
    void showAllSubjects()
    {
        if (subjects.size() == 0)
        {
            cout << "Subjects not added yet.\n";
            return;
        }
        
        for (int i = 0; i < subjects.size(); i++)
        {
            subjects[i].print();
        }
    }
    void showGroupStudents(int groupid)
    {
        auto group = getGroupById(groupid);
        if (group.students.size() == 0)
        {
            cout << "Group has no students\n";
            return;
        }
        for (int i = 0; i < group.students.size() - 1; ++i) {
        bool swapped = false;

        for (int j = 0; j < group.students.size() - i - 1; ++j) {
            if (getStudentById( group.students[j]).surname > getStudentById( group.students[j + 1]).surname) {
                swap(group.students[j], group.students[j + 1]);
                swapped = true;
            }
        }

        if (!swapped) {
            break;
        }
    }

        for (int i = 0; i < group.students.size(); i++)
        {
            auto student = getStudentById(group.students[i]);
            if (student.id != -1)
            {
                student.print();
            }
            else
            {
                cout << "Student with id: " << group.students[i] << " is not exists.\n";
            }
        }
    }
    void showGroupSubjects(int groupid)
    {
        auto group = getGroupById(groupid);
        if (group.subjects.size() == 0)
        {
            cout << "Group has no subjects\n";
            return;
        }
        for (int i = 0; i < group.subjects.size(); i++)
        {
            auto subject = getSubject(group.subjects[i]);
            if (subject.id != -1)
            {
                subject.print();
            }
            else
            {
                cout << "Subject with id: " << group.subjects[i] << " is not exists.\n";
            }
        }
    }
    static bool compareBySurname(const Student& a, const Student& b) {
    return a.surname < b.surname;
    }
    static bool comparewStudentIdBySurname(pair<int,Student> a,pair<int,Student> b) {
    return a.second.surname < b.second.surname;
    }

    void setGroupId(int userid)
    {
        for (int i = 0; i < students.size(); i++)
            if (students[i].accountId == userid)
                for (int j = 0; j < groups.size(); j++)
                    for (int k = 0; k < groups[j].students.size(); k++)
                        currentGroupId = groups[j].id;
    }
    void setSubjectCoefficient(int id, double coefficient)
    {
        bool isExists = false;
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id == currentGroupId)
            {
                for (int j = 0; j < groups[i].subjects.size(); j++)
                    if (groups[i].subjects[j] == id)
                    {
                        isExists = true;
                        groups[i].coefficients.insert({id, coefficient});
                    }

                if (!isExists)
                {
                    cout << "Subject is not exists in current group. \n";
                }
            }
    }
    void removeStudentFromCurrGroup(int id)
    {
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id == currentGroupId)
            {
                for (int j = 0; j < groups[i].students.size(); j++)
                    if (groups[i].students[j] == id)
                        groups[i].students.erase(groups[i].students.begin() + j);
                break;
            }
    }
    void addStudentToCurrGroup(int id)
    {
        bool isExists = false;
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id == currentGroupId)
            {
                for (int j = 0; j < groups[i].students.size(); j++)
                    if (groups[i].students[j] == id)
                    {
                        isExists = true;
                        cout << "Student is already in group. \n";
                    }
                if (!isExists)
                {
                    groups[i].students.push_back(id);
                }
            }
    }

    void removeSubjectFromCurrGroup(int id)
    {
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id == currentGroupId)
            {
                for (int j = 0; j < groups[i].subjects.size(); j++)
                    if (groups[i].subjects[j] == id)
                        groups[i].subjects.erase(groups[i].subjects.begin() + j);
                break;
            }
    }
    void addSubjectToCurrGroup(int id)
    {
        bool isExists = false;
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id == currentGroupId)
            {
                for (int j = 0; j < groups[i].subjects.size(); j++)
                    if (groups[i].subjects[j] == id)
                    {
                        isExists = true;
                        cout << "Subject is already in group. \n";
                    }

                if (!isExists)
                {
                    groups[i].subjects.push_back(id);
                }
            }
    }

    void setStudentMark(int studentId, int subjectId, int mark)
    {
        bool isExists = false;
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id == currentGroupId)
            {
                for (int j = 0; j < groups[i].students.size(); j++)
                    if (groups[i].students[j] == studentId)
                    {
                        isExists = true;
                        for (int k = 0; k < groups[i].subjects.size(); k++)
                        {
                            if (groups[i].subjects[k] == subjectId)
                            {
                                map<int, int> temp;
                                temp.insert({subjectId, mark});
                                groups[i].marks[studentId].insert({make_pair(subjectId, mark)});
                                break;
                            }
                        }
                    }
                if (!isExists)
                {
                    cout << "Group have no student with this id.\n";
                }
            }
    }

    void createSubject(string name)
    {
        int id = 1;
        for (int i = 0; i < subjects.size(); i++)
            if (subjects[i].id >= id)
                id = subjects[i].id + 1;
        subjects.push_back(Subject(id, name));
        save();
    }
    void createUser(string login, string password, bool isadmin)
    {
        auto user = User::fromCredentials(login, password);
        int id = 1;
        for (int i = 0; i < users.size(); i++)
            if (users[i].id >= id)
                id = users[i].id + 1;
        user.id = id;
        user.isAdmin = isadmin;
        users.push_back(user);
    }
    void createGroup(string name)
    {
        int id = 1;
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id >= id)
                id = groups[i].id + 1;

        groups.push_back(Group(name, id));
        save();
    }
    void createStudent(string name, string surname)
    {
        int id = 1;
        for (int i = 0; i < students.size(); i++)
            if (students[i].id >= id)
                id = students[i].id + 1;
        students.push_back(Student(id, name, surname, -1));
        save();
    }

    Subject getSubject(int id)
    {
        for (int i = 0; i < subjects.size(); i++)
            if (subjects[i].id == id)
                return subjects[i];
        return Subject(-1, "");
    }
    User getUser()
    {
        for (int i = 0; i < users.size(); i++)
            if (users[i].id == currentUserId)
                return users[i];
        return User(-1, "", "", false);
    }
    User getUserByLogin(string login)
    {
        for (int i = 0; i < users.size(); i++)
            if (users[i].login == login)
                return users[i];
        return User(-1, "", "", false);
    }

    User getUserById(int _id)
    {
        for (int i = 0; i < users.size(); i++)
            if (users[i].id == _id)
                return users[i];
        throw runtime_error("User not found with the given ID");
    }

    Student getStudentByUserId(int _id)
    {
        for (int i = 0; i < students.size(); i++)
            if (students[i].accountId == _id)
                return students[i];
        return Student(-1, "", "", -1);
    }
    Student getStudentById(int _id)
    {
        for (int i = 0; i < students.size(); i++)
            if (students[i].id == _id)
                return students[i];
        return Student(-1, "", "", -1);
    }
    Group getGroupById(int _id)
    {
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id == _id)
                return groups[i];
        return Group("", -1);
    }
    Group getGroupByName(string name)
    {
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].name == name)
                return groups[i];
        return Group("", -1);
    }

    void changeStudentName(int id, string name)
    {
        bool isFind = false;
        for (int i = 0; i < students.size(); i++)
            if (students[i].id == id)
            {
                students[i].name = name;
                isFind = true;
            }
        if (!isFind)
            cout << "User with this id is not exists\n";
    }
    void changeStudentSurname(int id, string surname)
    {
        bool isFind = false;

        for (int i = 0; i < students.size(); i++)
            if (students[i].id == id)
            {
                students[i].surname = surname;
                isFind = true;
            }
        if (!isFind)
            cout << "User with this id is not exists\n";
    }
    void changeStudentAccountId(int id, string login)
    {
        bool isFind = false;

        auto user = getUserByLogin(login);
        if (user.id == -1)
        {
            cout << "User with this id is not exists.\n";
            return;
        }

        for (int i = 0; i < students.size(); i++)
            if (students[i].id == id)
            {
                students[i].accountId = user.id;
                isFind = true;
            }
        if (!isFind)
            cout << "Student with this id is not exists\n";
    }
    void changeSubjectName(string oldname, string newname)
    {
        for (int i = 0; i < subjects.size(); i++)
        {
            if (subjects[i].name == oldname)
            {
                subjects[i].name = newname;
                break;
            }
        }
    }
    void changeGroupName(int id, string name)
    {
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].id == id)
                groups[i].name = name;
    }

    void changePassword(string passwd)
    {
        for (int i = 0; i < users.size(); i++)
            if (users[i].id == currentUserId)
            {
                SHA256 sha256;
                users[i].password = (sha256(users[i].login) + sha256(passwd));
            }
    }

    void changeLogin(string login)
    {
        for (int i = 0; i < users.size(); i++)
            if (users[i].id == currentUserId)
                users[i].login = login;
    }

    void removeUser(int _id)
    {
        for (auto it = users.begin(); it != users.end(); ++it)
        {
            if (it->id == _id)
            {
                users.erase(it);
                return;
            }
        }
        cout << "User with this id is not found\n";
    }
    void removeStudent(int id)
    {
        auto it = find_if(students.begin(), students.end(), [id](const Student &student)
                          { return student.id == id; });
        students.erase(it);
    }
    void removeCurrGroup()
    {
        for (int i = 0; i < groups.size(); i++)
        {
            if (groups[i].id == currentGroupId)
            {
                groups.erase(groups.begin() + i);
                break;
            }
        }
    }
    void removeSubject(string name)
    {
        for (int i = 0; i < subjects.size(); i++)
        {
            if (subjects[i].name == name)
            {
                subjects.erase(subjects.begin() + i);
                break;
            }
        }
    }

    bool findUserByLogin(string login)
    {
        for (int i = 0; i < users.size(); i++)
            if (users[i].login == login)
                return true;

        return false;
    }
    bool findGroupByName(string name)
    {
        for (int i = 0; i < groups.size(); i++)
            if (groups[i].name == name)
                return true;

        return false;
    }
    bool findSubjectByName(string name)
    {
        for (int i = 0; i < subjects.size(); i++)
            if (subjects[i].name == name)
                return true;

        return false;
    }
    bool findStudentByName(string name)
    {
        for (int i = 0; i < students.size(); i++)
            if (students[i].name == name)
                return true;

        return false;
    }
    bool findUser(User &user)
    {
        for (int i = 0; i < users.size(); i++)
        {
            if (user.equalData(users[i]))
            {
                user = users[i];
                return true;
            }
        }
        return false;
    }
    Student findStudent(int id)
    {
        for (int i = 0; i < students.size(); i++)
        {
            if (students[i].id == id)
            {
                return students[i];
            }
        }
        return Student(-1, "", "", -1);
    }

    void load()
    {
        if (!filesystem::exists(dirPath))
        {
            filesystem::create_directory(dirPath);
        }
        readUsers();
        readGroups();
        readStudents();
        readSubjects();
    }
    void readUsers()
    {
        ifstream file(usersPath);
        auto data = User::read(file);

        users.clear();
        for (int i = 0; i < data.size(); i += 4)
            users.push_back(User(stoi(data[i]), data[i + 1], data[i + 2], stoi(data[i + 3])));

        file.close();

        if (users.size() == 0)
            createUser("admin", "admin", true);
    }
    void readGroups()
    {
        ifstream file(groupsPath);
        auto data = Group::read(file);

        groups.clear();
        int n = 0;
        for (int i = 0; i < data.size(); i += n - 1)
        {
            vector<int> studentsid;
            istringstream iss(data[i + 2]);
            vector<int> subjectsid;
            istringstream issubj(data[i + 3]);
            copy(istream_iterator<int>(iss), istream_iterator<int>(), back_inserter(studentsid));
            if (studentsid.front() == -1)
                studentsid.clear();
            copy(istream_iterator<int>(issubj), istream_iterator<int>(), back_inserter(subjectsid));
            if (subjectsid.front() == -1)
                subjectsid.clear();
            n = 5;

            int coefficientsSize = stoi(data[i + 4]);
            map<int, double> coefficients;
            for (int j = 0; j < coefficientsSize; j++, n += 2)
            {
                coefficients.insert({stoi(data[i + n]), stod(data[i + n + 1])});
            }
            if (coefficients.count(-1) > 0)
            {
                coefficients.clear();
            }

            int marksSize = stoi(data[i + n]);

            map<int, map<int, int>> marks;
            for (int j = 0; j < marksSize; j++, n += 2)
            {
                int studentsMarksSize = stoi(data[i + n + 2]);

                map<int, int> studentsmarks;
                for (int k = 0; k < studentsMarksSize; k++)
                {
                    studentsmarks.insert({stoi(data[i + n + 3 + k*2]), stod(data[i + n + 4 + k*2])});
                }

                if (studentsMarksSize == 1 && studentsmarks.count(-1) > 0)
                {
                    studentsmarks.clear();
                }

                marks.insert({stoi(data[i + n + 1]), studentsmarks});

                n += studentsMarksSize * 2;
            }
            if (marks.count(-1) > 0)
            {
                marks.clear();
            }

            n += marksSize * 2;
            groups.push_back(Group(stoi(data[i]), data[i + 1], studentsid, subjectsid, coefficients, marks));
        }
        file.close();
    }
    void readStudents()
    {
        ifstream file(studentsPath);
        auto data = Student::read(file);

        students.clear();
        for (int i = 0; i < data.size(); i += 4)
        {
            students.push_back(Student(stoi(data[i]), data[i + 1], data[i + 2], stoi(data[i + 3])));
        }
        file.close();
    }
    void readSubjects()
    {
        ifstream file(subjectsPath);
        auto data = Subject::read(file);

        subjects.clear();
        for (int i = 0; i < data.size(); i += 2)
        {
            subjects.push_back(Subject(stoi(data[i]), data[i + 1]));
        }
        file.close();
    }

    void save()
    {
        saveUsers();
        saveGroups();
        saveStudents();
        saveSubjects();
    }
    void saveUsers()
    {
        ofstream file(usersPath, ios::trunc);
        vector<string> data;
        string temp;

        for (int i = 0; i < users.size(); i++)
        {
            if (i != 0)
                file << ',';

            users[i].save(file);
        }

        file.close();
    }
    void saveGroups()
    {
        ofstream file(groupsPath, ios::trunc);
        vector<string> data;
        string temp;

        for (int i = 0; i < groups.size(); i++)
        {
            if (i != 0)
                file << ',';

            groups[i].save(file);
        }

        file.close();
    }
    void saveStudents()
    {
        ofstream file(studentsPath, ios::trunc);
        vector<string> data;
        string temp;

        for (int i = 0; i < students.size(); i++)
        {
            if (i != 0)
                file << ',';

            students[i].save(file);
        }

        file.close();
    }
    void saveSubjects()
    {
        ofstream file(subjectsPath, ios::trunc);
        vector<string> data;
        string temp;

        for (int i = 0; i < subjects.size(); i++)
        {
            if (i != 0)
                file << ',';

            subjects[i].save(file);
        }

        file.close();
    }
};