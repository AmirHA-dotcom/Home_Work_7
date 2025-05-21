#include <iostream>
#include <stdexcept>
#include <regex>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// --------------- Regexes ---------------

regex add_doc_pattern(R"(^add doctor (\w+) (\w+) (\S+) (\S+)(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?\s*$)");
regex change_NO_of_patients_pattern(R"(^change the number of patients of doctor (\w+) to (\S+)\s*$)");
regex change_working_days_pattern(R"(^change the working days of doctor (\w+) to (\S+)(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?(?: (\S+))?\s*$)");
regex add_patient_pattern(R"(^add patient (\w+) (\w+)\s*$)");
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
class doc_doesnt_exist : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Error: no doctor with this name exist";
    }
};
class doc_with_spec_doesnt_exist : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Error: no doctor for this problem exist";
    }
};
class doc_busy : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Error: doctors with this specialty are busy";
    }
};



// --------------- Model ---------------

class Patient
{
private:
    string name;
    string specialty;
public:
    Patient(string n, string spec) : name(n), specialty(spec) {}
    string get_name() const{return name;}
    string get_specialty() const{return specialty;}
};

class Doc
{
private:
    string name;
    string specialty;
    int number_of_patients;
    vector<string> working_days;
    vector<pair<Patient*, string>> patients;
    void delete_that_day(string day)
    {
        patients.erase(remove_if(patients.begin(), patients.end(),[&day](const pair<Patient*, string>& entry)
        {
            return entry.second == day;
        }),patients.end());
    }
public:
    Doc(string n, string spec, int NO_of_patients, vector<string> W_D) : name(n), specialty(spec), number_of_patients(NO_of_patients), working_days(W_D) {}
    string get_name() const{return name;}
    string get_specialty() const{return specialty;}
    int get_number_of_patients() const{return number_of_patients;}
    vector<string> get_working_days() const{return working_days;}
    void change_NO_of_patients(int NO)
    {
        if (NO < number_of_patients)
        {
            int d = number_of_patients - NO;
            for (int i = 0; i < d; i++)
            {
                if (patients[number_of_patients - i].first != nullptr)
                    patients.erase(patients.begin() + number_of_patients - i);
            }
        }
        number_of_patients = NO;
    }
    void change_working_days(vector<string> new_days)
    {
        working_days.swap(new_days);
        if (find(working_days.begin(), working_days.end(), "saturday") == working_days.end())
        {
            delete_that_day("saturday");
        }
        if (find(working_days.begin(), working_days.end(), "sunday") == working_days.end())
        {
            delete_that_day("sunday");
        }
        if (find(working_days.begin(), working_days.end(), "monday") == working_days.end())
        {
            delete_that_day("monday");
        }
        if (find(working_days.begin(), working_days.end(), "tuesday") == working_days.end())
        {
            delete_that_day("tuesday");
        }
        if (find(working_days.begin(), working_days.end(), "wednesday") == working_days.end())
        {
            delete_that_day("wednesday");
        }
    }
    void add_patient(Patient* patient, string day)
    {
        patients.emplace_back(patient, day);
    }
    vector<pair<Patient*, string>> get_patients() {return patients;}
};

// ------------ Controller ------------

class Controller
{
private:
    vector<Doc*> doctors;
    vector<Patient*> patients;
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
    int patient_index_finder_by_name(string& name)
    {
        for (int i = 0; i < patients.size(); i++)
        {
            if (name == patients[i]->get_name())
                return i;
        }
        return -1;
    }
    vector<Doc*> find_docs_with_spec(string& speciality)
    {
        vector<Doc*> available_docs;
        for (auto & doctor : doctors)
        {
            if (speciality == doctor->get_specialty())
                available_docs.push_back(doctor);
        }
        return available_docs;
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
    void change_number_of_patients(string doc_name, int NO)
    {
        int doc_index = doc_index_finder_by_name(doc_name);
        if (doc_index == -1)
            throw doc_doesnt_exist();
        if (NO < 0)
            throw not_valid_number();
        doctors[doc_index]->change_NO_of_patients(NO);
        cout << "number of patients changed" << endl;
    }
    void change_working_days(string doc_name, vector<string> working_days)
    {
        int doc_index = doc_index_finder_by_name(doc_name);
        if (doc_index == -1)
            throw doc_doesnt_exist();
        if (!week_day_valider(working_days))
            throw not_valid_day();
        doctors[doc_index]->change_working_days(working_days);
        cout << "working days changed" << endl;
    }
    void add_patient(string name, string speciality)
    {
        vector<Doc*> available_docs = find_docs_with_spec(speciality);
        if (available_docs.empty())
            throw doc_with_spec_doesnt_exist();
        Doc* free_doc = nullptr;
        string free_day;
        for (auto& doc : available_docs)
        {
            int free_times = doc->get_number_of_patients() * doc->get_working_days().size() - doc->get_patients().size();
            if (free_times > 0)
            {
                free_doc = doc;
                int day_index = (doc->get_number_of_patients() * doc->get_working_days().size() - free_times)/doc->get_number_of_patients();
                free_day = doc->get_working_days()[day_index];
            }
        }
        if (free_doc == nullptr)
        {
            throw doc_busy();
        }
        Patient* new_p = new Patient(name, speciality);
        patients.push_back(new_p);
        free_doc->add_patient(new_p, free_day);
        cout << "appointment set on day " << free_day << " doctor " << free_doc->get_name() << endl;
    }
    ~Controller()
    {
        for (Doc* doc : doctors)
            delete doc;
        for (Patient* p : patients)
            delete p;
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
    else if (regex_match(line, match, change_NO_of_patients_pattern))
    {
        AHA.change_number_of_patients(match[1], stoi(match[2]));
        return true;
    }
    else if (regex_match(line, match, change_working_days_pattern))
    {
        vector<string> days;
        for (int i = 0; i < 7; i++)
        {
            if (match[2 + i].matched)
                days.push_back(match[2 + i]);
        }
        AHA.change_working_days(match[1], days);
        return true;
    }
    else if (regex_match(line, match, add_patient_pattern))
    {
        AHA.add_patient(match[1], match[2]);
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
