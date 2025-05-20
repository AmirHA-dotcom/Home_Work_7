#include <iostream>
#include <stdexcept>
#include <regex>
#include <string>
#include <vector>

using namespace std;

// --------------- Regexes ---------------

regex add_doc_pattern(R"(^add doctor (\w+) (\w+) (\S+) (\S+)(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?$)");
smatch match;

// ------------- Exceptions ------------------

class invalid_command : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Error: invalid command";
    }
};
class duplicate_doc : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Error: doctor added twice";
    }
};
class not_valid_day : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Error: invalid weekday";
    }
};
class not_valid_number : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Error: number of patients can not be negative";
    }
};

// --------------- Model ---------------

class Doc
{
private:
    string name;
    string specialty;
    int number_of_patients;
    vector<string> working_days;
public:
    Doc(string n, string spec, int NO_of_patients, vector<string> W_D) : name(n), specialty(spec), number_of_patients(NO_of_patients), working_days(W_D) {}
    string get_name() const{return name;}
    string get_specialty() const{return specialty;}
    int get_number_of_patients() const{return number_of_patients;}
    vector<string> get_working_days() const{return working_days;}
};

// ------------ Controller ------------

class Controller
{
private:
    vector<Doc*> doctors;
    int doc_index_finder(Doc* temp_doc)
    {
        for (int i = 0; i < doctors.size(); i++)
        {
            if (temp_doc == doctors[i])
                return i;
        }
        return -1;
    }
    int doc_index_finder_by_name(string& name)
    {
        for (int i = 0; i < doctors.size(); i++)
        {
            if (name == doctors[i]->get_name())
                return i;
        }
        return -1;
    }
    bool week_day_valider(vector<string>& days)
    {
        for (const auto& day : days)
        {
            if (day != "monday" && day != "sunday" && day != "saturday" && day != "wednesday" && day != "tuesday")
                return false;
        }
        return true;
    }
public:
    void add_doc(string name, string specialty, int NO_of_patients, vector<string> working_days)
    {
        int doc_index = doc_index_finder_by_name(name);
        if (doc_index != -1)
            throw duplicate_doc();
        if (!week_day_valider(working_days))
            throw not_valid_day();
        if (NO_of_patients < 0)
            throw not_valid_number();
        doctors.push_back(new Doc(name, specialty, NO_of_patients, working_days));
        cout << "doctor " << name << " added with specialty " << specialty << " with " << NO_of_patients << " patients" << endl;
    }
    ~Controller()
    {
        for (Doc* doc : doctors)
            delete doc;
    }
};
Controller AHA;

// ------------- Handler ----------------

bool input_handler(string line)
{
    if (line == "exit")
        return false;
    else if (regex_match(line, match, add_doc_pattern))
    {
        vector<string> days;
        for (int i = 0; i < 7; i++)
        {
            if (match[4 + i].matched)
                days.push_back(match[4 + i]);
        }
        AHA.add_doc(match[1], match[2], stoi(match[3]), days);
        return true;
    }
    else
        throw invalid_command();
}

// ------ do not change main() function ------
int main() {
    string line;
    bool cond = true;
    while (cond) {
        getline(cin, line);
        try {
            cond = input_handler(line);
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
    return 0;
}
