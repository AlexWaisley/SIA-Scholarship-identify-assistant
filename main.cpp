#include <iostream>
#include <string>
#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <algorithm>
#include "sha256.h"
#include <vector>
#include <filesystem>
#include <fstream>
#include "user.cpp"
#include "database.cpp"

using namespace std;

void enableEcho(bool enable) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);

    if (enable) {
        tty.c_lflag |= ECHO;
    } else {
        tty.c_lflag &= ~ECHO;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

bool login(DataBase &db)
{
    string login, password;
    cout << "Enter login: ";
    cin >> login;
    enableEcho(false);
    cout << "Enter password: ";
    cin >> password;
    cout << endl;
    enableEcho(true);

    auto user = User::fromCredentials(login, password);
    if (db.findUser(user))
    {
        cout << "Welcome, " << login << endl;
        db.currentUserId = user.id;
        db.setGroupId(user.id);
        return true;
    }
    else
        cout << "The username or password is incorrect.\n";

    return false;
}
//---------------------------------------------------------------------------
enum Menu
{
    Main,
    ManageSubject,
    ManageUser,
    ManageGroup,
    ManageStudent,
    ShowTables,
    ExitMode,
};

void mainMenu(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        if (db.getUser().isAdmin)
        {
            cout << "1. Manage user\n";
            cout << "2. Manage student\n";
            cout << "3. Manage group\n";
            cout << "4. Manage subject\n";
            cout << "5. Display table\n";
            cout << "6. Exit\n";
        }
        else
        {
            cout << "1. Manage user\n";
            cout << "2. Manage student\n";
            cout << "3. Display table\n";
            cout << "4. Exit\n";
        }

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case 1:
        case -1:
            currMenu = Menu::ManageUser;
            return;
        case 2:
        case -2:
            currMenu = Menu::ManageStudent;
            return;
        case -3:
            currMenu = Menu::ManageGroup;
            return;
        case -4:
            currMenu = Menu::ManageSubject;
            return;
        case 3:
        case -5:
            currMenu = Menu::ShowTables;
            return;
        case 4:
        case -6:
            currMenu = Menu::ExitMode;
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

//-----------------------------------------------------------------------

enum userManage
{
    CreateUser,
    ChangeLogin,
    ChangePassword,
    RemoveUser
};



void createUser(Menu &currMenu, DataBase &db)
{
    string login, password;
    bool isadmin;
    cout << "Enter user login(whithout ',' or ' '): ";
    cin >> login;
    cout << "Enter user password: ";
    cin >> password;
    cout << "Is user admin(0,1): ";
    cin >> isadmin;

    if (login.find(',') != string::npos)
    {
        cout << "Reenter login without ','\n";
        return;
    }

    if (db.findUserByLogin(login))
    {
        cout << "User with this login is already exists.\n";
        return;
    }

    db.createUser(login, password, isadmin);
    cout << "User created.\n";
    db.save();
}
void changeLogin(Menu &currMenu, DataBase &db)
{
    string login, passwd;
    cout << "Enter new login(without ','): ";
    cin >> login;
    enableEcho(false);
    cout << "Enter password: ";
    cin >> passwd;
    cout << endl;
    enableEcho(true);

    SHA256 sha256;

    if ((sha256(db.getUser().login) + sha256(passwd)) != db.getUser().password)
    {
        cout << "Password is incorrect\n";
        return;
    }

    if (login.find(',') != string::npos)
    {
        cout << "Reenter login without ','\n";
        return;
    }

    if (db.findUserByLogin(login))
    {
        cout << "User with this login is already exists.\n";
        return;
    }

    db.changeLogin(login);
    db.changePassword(passwd);
    cout << "Login changed successfully\n";
    db.save();
}
void changePassword(Menu &currMenu, DataBase &db)
{
    string oldpasswd, newpasswd;
    enableEcho(false);
    cout << "Enter old password: ";
    cin >> oldpasswd;
    cout << endl;
    enableEcho(true);

    SHA256 sha256;

    if ((sha256(db.getUser().login) + sha256(oldpasswd)) != db.getUser().password)
    {
        cout << "Password is incorrect\n";
        return;
    }

    enableEcho(false);
    cout << "Enter new password: ";
    cin >> newpasswd;
    cout << endl;
    enableEcho(true);

    db.changePassword(newpasswd);
    cout << "Password changed successfully\n";

    db.save();
}
void removeUser(Menu &currMenu, DataBase &db)
{
    db.showAllUsers();
    int userid;
    cout << "Enter user id: ";
    cin >> userid;
    db.removeUser(userid);
    cout << "User removed successfully\n";
    db.save();
}


void manageUser(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        if (db.getUser().isAdmin)
        {
            cout << "1. Create new user\n";
            cout << "2. Change your login\n";
            cout << "3. Change your password\n";
            cout << "4. Remove user\n";
            cout << "5. Back\n";
        }
        else
        {
            cout << "1. Change your login\n";
            cout << "2. Change your password\n";
            cout << "3. Back\n";
        }

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case -1:
            createUser(currMenu, db);
            return;
        case 1:
        case -2:
            changeLogin(currMenu, db);
            return;
        case 2:
        case -3:
            changePassword(currMenu, db);
            return;
        case -4:
            removeUser(currMenu, db);
            return;
        case 3:
        case -5:
            currMenu = Menu::Main;
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

//---------------------------------------------------------------------------

enum studentManage
{
    CreateStudent,
    ChangeName,
    ChangeSurname,
    RemoveStudent
};

void createStudent(Menu &currMenu, DataBase &db)
{
    string name, surname;
    cout << "Enter student name(whithout ','): ";
    cin >> name;
    cout << "Enter student surname(whithout ','): ";
    cin >> surname;

    if (name.find(',') != string::npos || surname.find(',') != string::npos)
    {
        cout << "Reenter name/surname without ','\n";
        return;
    }

    db.createStudent(name, surname);
    cout << "Student created successfully\n";
    db.save();
}
void changeStudentName(Menu &currMenu, DataBase &db)
{
    int id;
    string name;
    if (db.getUser().isAdmin)
    {
        cout << "Enter student id: ";
        cin >> id;
    }
    else
    {
        id = db.getStudentByUserId(db.currentUserId).id;
    }
    cout << "Enter new name(whithout ','): ";
    cin >> name;

    if (name.find(',') != string::npos)
    {
        cout << "Reenter name without ','\n";
        return;
    }

    db.changeStudentName(id, name);
    cout << "Student name changed successfully\n";
    db.save();
}
void changeStudentSurname(Menu &currMenu, DataBase &db)
{
    string surname;
    int id;
    if (db.getUser().isAdmin)
    {
        cout << "Enter student id: ";
        cin >> id;
    }
    else
    {
        id = db.currentUserId;
        cout << id << endl;
    }
    cout << "Enter new surname(whithout ','): ";
    cin >> surname;

    if (surname.find(',') != string::npos)
    {
        cout << "Reenter surname without ','\n";
        return;
    }

    db.changeStudentSurname(id, surname);
    cout << "Student surname changed successfully\n";
    db.save();
}

void changeStudentAccount(Menu &currMenu, DataBase &db)
{
    string login;
    int id;
    cout << "Enter student id: ";
    cin >> id;
    db.showAllUsers();
    cout << "Enter account name(whithout ','): ";
    cin >> login;

    if (login.find(',') != string::npos)
    {
        cout << "Reenter login without ','\n";
        return;
    }
    if (!db.findUserByLogin(login))
    {
        cout << "User with this login not exists.\n";
        return;
    }
    

    db.changeStudentAccountId(id, login);
    cout << "Student user account changed successfully\n";
    db.save();
}
void removeStudent(Menu &currMenu, DataBase &db)
{
    int id;
    cout << "Enter student id: ";
    cin >> id;

    db.removeStudent(id);
    cout << "Student removed successfully\n";
    db.save();
}

void editStudent(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        db.showAllStudents();

        cout << "1. Change student name\n";
        cout << "2. Change student surname\n";
        cout << "3. Change student user account\n";
        cout << "4. Remove student\n";
        cout << "5. Back\n";

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case -1:
            changeStudentName(currMenu, db);
            return;
        case -2:
            changeStudentSurname(currMenu, db);
            return;
        case -3:
            changeStudentAccount(currMenu, db);
            return;
        case -4:
            removeStudent(currMenu, db);
            return;
        case -5:
            currMenu = Menu::ManageStudent;
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

void manageStudent(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        if (db.getUser().isAdmin)
        {
            cout << "1. Create new student\n";
            cout << "2. Edit student\n";
            cout << "3. Back\n";
        }
        else
        {
            db.getStudentByUserId(db.currentUserId).print();
            cout << "1. Change student name\n";
            cout << "2. Change student surname\n";
            cout << "3. Back\n";
        }

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case -1:
            createStudent(currMenu, db);
            return;
        case -2:
            editStudent(currMenu, db);
            return;
        case 1:
            changeStudentName(currMenu, db);
            return;
        case 2:
            changeStudentSurname(currMenu, db);
            return;
        case 3:
        case -3:
            currMenu = Menu::Main;
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void createGroup(Menu &currMenu, DataBase &db)
{
    string name;
    cout << "Enter group name(whithout ',' or ' '): ";
    cin >> name;

    if (db.findGroupByName(name))
    {
        cout << "Group with this name is already exists.\n";
        return;
    }

    db.createGroup(name);
    cout << "Group created successfully\n";
    currMenu = Menu::ManageGroup;
    db.save();
}

void addStudentToCurrGroup(Menu &currMenu, DataBase &db)
{
    db.showAllStudents();
    int id;
    cout << "Enter student id: ";
    cin >> id;

    db.addStudentToCurrGroup(id);
    cout << "Student successfully added to current group\n";
    db.save();
}
void removeStudentFromCurrGroup(Menu &currMenu, DataBase &db)
{
    int id;
    cout << "Enter student id: ";
    cin >> id;

    db.removeStudentFromCurrGroup(id);
    cout << "Student successfully removed from current group\n";
    db.save();
}

void editStudentInGroup(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        db.showGroupStudents(db.currentGroupId);
        cout << "1. Add new student\n";
        cout << "2. Remove student\n";
        cout << "3. Back\n";

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case -1:
            addStudentToCurrGroup(currMenu, db);
            editStudentInGroup(currMenu, db);
            return;
        case -2:
            removeStudentFromCurrGroup(currMenu, db);
            editStudentInGroup(currMenu, db);
            return;
        case -3:
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

void addSubjectToCurrGroup(Menu &currMenu, DataBase &db)
{
    db.showAllSubjects();
    int id;
    cout << "Enter subject id: ";
    cin >> id;

    db.addSubjectToCurrGroup(id);
    cout << "Subject successfully added to current group\n";
    db.save();
}
void setSubjectCoeffivient(Menu &currMenu, DataBase &db)
{
    db.showGroupSubjects(db.currentGroupId);
    int id;
    string coefficient;
    cout << "Enter subject id: ";
    cin >> id;
    cout << "Enter subject coefficient(use '.' to split): ";
    cin >> coefficient;

    try{
        double coef = stod(coefficient);
    }
    catch(exception ex){
        cout << "Wrong input.";
        return;
    }
    auto coeffici = stod(coefficient);

    db.setSubjectCoefficient(id, coeffici);
    cout << "Subject coefficient set successfully\n";
    db.save();
}
void removeSubjectFromCurrGroup(Menu &currMenu, DataBase &db)
{
    db.showGroupSubjects(db.currentGroupId);
    int id;
    cout << "Enter subject id: ";
    cin >> id;

    db.removeSubjectFromCurrGroup(id);
    cout << "Subject successfully removed from current group\n";
    db.save();
}

void editSubjectInGroup(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        db.showGroupSubjects(db.currentGroupId);
        cout << "1. Add new subject\n";
        cout << "2. Set subject coefficient\n";
        cout << "3. Remove subject\n";
        cout << "4. Back\n";

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case -1:
            addSubjectToCurrGroup(currMenu, db);
            editSubjectInGroup(currMenu, db);
            return;
        case -2:
            setSubjectCoeffivient(currMenu, db);
            editSubjectInGroup(currMenu, db);
            return;
        case -3:
            removeSubjectFromCurrGroup(currMenu, db);
            editSubjectInGroup(currMenu, db);
            return;
        case -4:
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}
void setStudentMarkInGroup(Menu &currMenu, DataBase &db)
{
    int studentId, subjectId, mark;
    db.showGroupStudents(db.currentGroupId);
    cout << "Enter student id: ";
    cin >> studentId;
    db.showGroupSubjects(db.currentGroupId);
    cout << "Enter subject id: ";
    cin >> subjectId;
    cout << "Enter student mark: ";
    cin >> mark;

    db.setStudentMark(studentId, subjectId, mark);
    cout << "Mark successfully set\n";
    db.save();
}
void changeGroupName(Menu &currMenu, DataBase &db)
{
    int id;
    string name;
    id = db.getGroupById(db.currentGroupId).id;

    cout << "Enter new name(whithout ','): ";
    cin >> name;

    if (name.find(',') != string::npos)
    {
        cout << "Reenter name without ','\n";
        return;
    }

    db.changeGroupName(id, name);
    cout << "Group name changed successfully\n";
    db.save();
}
void removeGroup(Menu &currMenu, DataBase &db)
{
    string answer;
    cout << "Are you sure you want remove this group?('Yes', 'No'): ";
    cin >> answer;

    for_each(
        answer.begin(),
        answer.end(),
        [](char &c)
        {
            c = ::tolower(c);
        });

    if (answer == "yes")
    {
        db.removeCurrGroup();
        cout << "Current group removed successfully\n";
    }
    else{
    cout << "Group not removed\n";
    }
    currMenu = Menu::ManageGroup;
    db.save();
}

void editGroup(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        string groupName;
        if(db.currentGroupId == -1){
        db.showAllGroups();
        cout << "Enter group name: ";
        cin >> groupName;
        db.currentGroupId = db.getGroupByName(groupName).id;
        }
        if (db.currentGroupId == -1)
        {
            cout << "Group with this name is not exists.\n";
            currMenu = Menu::Main;
            return;
        }

        cout << "1. Change group name\n";
        cout << "2. Edit group students\n";
        cout << "3. Edit group subjects\n";
        cout << "4. Set student mark\n";
        cout << "5. Remove group\n";
        cout << "6. Back\n";

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case -1:
            changeGroupName(currMenu, db);
            break;
        case -2:
            editStudentInGroup(currMenu, db);
            break;
        case -3:
            editSubjectInGroup(currMenu, db);
            break;
        case -4:
            setStudentMarkInGroup(currMenu, db);
            break;
        case -5:
            removeGroup(currMenu, db);
            break;
        case -6:
            currMenu = Menu::ManageGroup;
            db.currentGroupId = -1;
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

void manageGroup(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        cout << "1. Create new group\n";
        cout << "2. Edit group\n";
        cout << "3. Back\n";

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case -1:
            createGroup(currMenu, db);
            return;
        case -2:
            editGroup(currMenu, db);
            return;
        case -3:
            currMenu = Menu::Main;
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

//-----------------------------------------------------------------------

void createSubject(Menu &currMenu, DataBase &db)
{
    string name;
    cout << "Enter subject name(whithout ','): ";
    cin >> name;

    if (db.findSubjectByName(name))
    {
        cout << "Subject with this name is already exisits.\n";
        return;
    }

    db.createSubject(name);
    cout << "Subject created successfully\n";
    db.save();
}
void changeSubjectName(Menu &currMenu, DataBase &db)
{
    string oldname, newname;
    cout << "Enter subject old name: ";
    cin >> oldname;
    cout << "Enter subject new name: ";
    cin >> newname;

    if (!db.findSubjectByName(oldname))
    {
        cout << "Subject with this name is already exisits.\n";
        return;
    }

    db.changeSubjectName(oldname, newname);
    cout << "Subject name changed successfully\n";
    db.save();
}
void removeSubject(Menu &currMenu, DataBase &db)
{
    string name;
    cout << "Enter subject name: ";
    cin >> name;

    if (!db.findSubjectByName(name))
    {
        cout << "Subject with this name is not exisits.\n";
        return;
    }

    db.removeSubject(name);
    cout << "Subject removed successfully\n";
    db.save();
}

void manageSubject(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        db.showAllSubjects();
        cout << "1. Create new subject\n";
        cout << "2. Change subject name\n";
        cout << "3. Remove subject\n";
        cout << "4. Back\n";

        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
        case -1:
            createSubject(currMenu, db);
            return;
        case -2:
            changeSubjectName(currMenu, db);
            return;
        case -3:
            removeSubject(currMenu, db);
            return;
        case -4:
            currMenu = Menu::Main;
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

//---------------------------------------------------------------------------

void showAllStudents(Menu &currMenu, DataBase &db)
{
    db.showAllStudents();
}
void showAllUsers(Menu &currMenu, DataBase &db)
{
    db.showAllUsers();
}
void showAllSubjects(Menu &currMenu, DataBase &db)
{
    db.showAllSubjects();
}

void showGroupStudents(Menu &currMenu, DataBase &db)
{
    int groupid;
    if (db.getUser().isAdmin)
    {
        db.showAllGroups();
        cout << "Enter group id: ";
        cin >> groupid;
    }
    else
    {
        groupid = db.currentGroupId;
    }

    db.showGroupStudents(groupid);
}
void showGroupSubjects(Menu &currMenu, DataBase &db)
{
    int groupid;
    if (db.getUser().isAdmin)
    {
        db.showAllGroups();
        cout << "Enter group id: ";
        cin >> groupid;
    }
    else
    {
        groupid = db.currentGroupId;
    }

    db.showGroupSubjects(groupid);
}

void showGroupStudentsMarks(Menu &currMenu, DataBase &db)
{
    db.showAllGroups();
    int groupid;
    if (db.getUser().isAdmin)
    {
        cout << "Enter group id: ";
        cin >> groupid;
    }
    else
    {
        groupid = db.currentGroupId;
    }

    db.showGroupStudentsWithMarks(groupid);
}

void showTables(Menu &currMenu, DataBase &db)
{
    while (true)
    {
        if (db.getUser().isAdmin)
        {
            cout << "1. Show all users\n";
            cout << "2. Show all students\n";
            cout << "3. Show all subjects\n";
            cout << "4. Show group students\n";
            cout << "5. Show group subjects\n";
            cout << "6. Show group students marks\n";
            cout << "7. Back\n";
        }
        else
        {
            cout << "1. Show group students\n";
            cout << "2. Show group subjects\n";
            cout << "3. Show group students marks\n";
            cout << "4. Back\n";
        }
        int inp;
        cin >> inp;
        inp = abs(inp);
        if (db.getUser().isAdmin)
            inp *= -1;

        switch (inp)
        {
            case -1:
            showAllUsers(currMenu,db);
            return;
        case -2:
            showAllStudents(currMenu, db);
            return;
        case -3:
            showAllSubjects(currMenu, db);
            return;
        case 1:
        case -4:
            showGroupStudents(currMenu, db);
            return;
        case 2:
        case -5:
            showGroupSubjects(currMenu, db);
            return;
        case 3:
        case -6:
            showGroupStudentsMarks(currMenu, db);
            return;
        case 4:
        case -7:
            currMenu = Menu::Main;
            return;
        default:
            cout << "Incorrect.\n";
            break;
        }
    }
}

//--------------------------------------------------------------------------
int main()
{
    auto db = DataBase();
    db.load();

    if (login(db))
    {
        auto menu = Menu::Main;
        while (menu != Menu::ExitMode)
        {
            switch (menu)
            {
            case Menu::Main:
                mainMenu(menu, db);
                break;
            case Menu::ManageUser:
                manageUser(menu, db);
                break;
            case Menu::ManageGroup:
                manageGroup(menu, db);
                break;
            case Menu::ManageSubject:
                manageSubject(menu, db);
                break;
            case Menu::ManageStudent:
                manageStudent(menu, db);
                break;
            case Menu::ShowTables:
                showTables(menu, db);
                break;
            default:
                break;
            }
        }
    }
    db.save();
    return 0;
}