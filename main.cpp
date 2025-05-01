#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <map>
#include <algorithm> //find algorithm is used.
#include <regex> //to validate input formats(here, date string entry is checked/validated)
#include <ctime>
#include <cctype>
#include <memory>
using namespace std;

//Forward declaration
class Doc;
class Appointment;

//Doctor types as enum
enum class DoctorType
{
    PHYSICIAN1,
    PHYSICIAN2,
    FIELD_SPECIALIST
};

//Utility Functions (to ease our req.)
string toLower(const string& str) 
{
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}
bool isDateValid(const string& date) 
{
    // Checking if date is in YYYY-MM-DD format
    regex dateFormat("\\d{4}-\\d{2}-\\d{2}");
    if (!regex_match(date, dateFormat)) return false;

    // Checking if date is not in the past,i.e, completed dates!
    time_t now = time(0);
    tm* currentDate = localtime(&now);
    
    int currentYear = currentDate->tm_year + 1900;
    int currentMonth = currentDate->tm_mon + 1;
    int currentDay = currentDate->tm_mday;

    int year, month, day;
    sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);

    if (year < currentYear) return false;
    if (year == currentYear && month < currentMonth) return false;
    if (year == currentYear && month == currentMonth && day < currentDay) return false;

    return true;
}

//Patient Details List
class Patient
{
    public:
        int ID;
        string Name;
        int age;
        string mobile_no;
        string consult_date;
        string department_to_visit_for;
        string consult_timeslot;
        //constructor
        Patient(int ID, string Name, int age,string mobile_no,string department_to_visit_for, string consult_date, string consult_timeSlot)
        : ID(ID), Name(Name), age(age), mobile_no(mobile_no), department_to_visit_for(department_to_visit_for), consult_date(consult_date), consult_timeslot(consult_timeSlot) {}

};
class Appointment
{
    public:
    static int next_appointmentID;
    int appointment_ID;
    Patient* patient;
    Doc* doctor;
    //constructor
    Appointment(Patient* p,Doc* d)
    : patient(p),doctor(d)
    {
        appointment_ID = next_appointmentID++;
    }
};
int Appointment::next_appointmentID = 1;
 
//Doctor Details
class Doc
{
    public:
    int id;
    string name;
    string specialisation;
    DoctorType type; //here, Physician_1,Physician_2, Specialist :)
    vector<string> availableSlots;
    map<string,vector<Appointment*>>scheduleByDate;
    //constructor
    Doc(int id,string name,string specialisation,DoctorType type)
    : id(id), name(name), specialisation(specialisation), type(type)
    {
        generateTimeSlots(); 
    }

    //toTime(9,30)------> 09:30
    string toTime(int hour, int min)
    {
      char buffer[6];
      sprintf(buffer, "%02d:%02d", hour, min);
      return string(buffer);
    }

    void generateTimeSlots() 
    {
        vector<string> slots;
        int startHour = 0, endHour = 0, startMin = 0, endMin = 0;

        switch(type) 
        {
            case DoctorType::PHYSICIAN1:
                startHour = 9; startMin = 30;
                endHour = 15; endMin = 30;
                break;
            case DoctorType::PHYSICIAN2:
                startHour = 15; startMin = 30;
                endHour = 20; endMin = 30;
                break;
            case DoctorType::FIELD_SPECIALIST:
                startHour = 15; startMin = 30;
                endHour = 19; endMin = 30;
                break;
            default:
                cerr << "Unknown doctor type!"<<endl;
                break;
            
        }
    

    int hour = startHour;
    int min = startMin;

    while (hour < endHour || (hour == endHour && min <= endMin))
    {
        // Skip 12:00 to 13:00(lunch) for Physician1
        if (!(type == DoctorType::PHYSICIAN1 && hour == 12 && min >= 0)) 
        {
            slots.push_back(toTime(hour, min));
        }
        // Advance time by 30 minutes
        min += 30;
        if (min >= 60) 
        {
            min = 0;
            hour++;
        }
    }

    availableSlots = slots;
    }


    //Display Doctor Schedule For a day
    void displayScheduleForDate(const string& date)
    {
        cout << "Schedule for: " << name << " on " << date << endl;
        if (scheduleByDate[date].empty())
        {
            cout << "No appointments for this day."<<endl;
        }
        else
        {
            cout<<"Booked Appointments"<<endl;
            for (const auto& a : scheduleByDate[date]) 
            {
                cout << "  - " << a->patient->Name << " at " << a->patient->consult_timeslot << endl;
            }
        } 
        
        cout << "\t Available Slots \t"<<endl;
        vector<string> bookedSlots;
        cout << "Booked appointments for the day:\n";
        for (auto& a : scheduleByDate[date]) 
        {
            bookedSlots.push_back(a->patient->consult_timeslot);
        }
        bool hasAvailable = false;
        for (const auto& slot : availableSlots) 
        {
            if (find(bookedSlots.begin(), bookedSlots.end(), slot) == bookedSlots.end()) 
            {
                cout << "  - " << slot << endl;
                hasAvailable = true;
            }
        }

        if (!hasAvailable) 
        {
            cout << "No available slots left for this day.\n";
        }
    
        
    }

    //Function to automatically alot the next available slot
    string getNextAvailableSlot(const string& date)
    {
        vector<string> booked;
        for (auto& a : scheduleByDate[date])
        {
            booked.push_back(a->patient->consult_timeslot);
        }
        for (const auto& slot : availableSlots)
        {
            if (find(booked.begin(), booked.end(), slot) == booked.end())
                return slot;
        }
        return "";
    }
};

int main()
{
    //List of Sample Doctors[Improvisation: use files]
    vector<Doc> doctors = {
        Doc(1, "Dr. Venkateshwarulu", "General Medicine", DoctorType::PHYSICIAN1),  // Removed trailing space
        Doc(2, "Dr. S P Jaiswal", "General Medicine", DoctorType::PHYSICIAN2),
        Doc(3, "Dr. Dileep", "Gynecology", DoctorType::FIELD_SPECIALIST),
        Doc(4, "Dr. Sudha Vani", "Dermatology",DoctorType::FIELD_SPECIALIST)
    };

    //Patient appointment records handling them using a vector of pointers to Patient objects
    vector<Appointment*> allAppointments;
    int patientID = 1;

    while (true)
    {
        cout << "\t HOSPITAL APPOINTMENT SYSTEM \t" << endl;
        cout << "1. Admin" << endl;
        cout << "2. Doctor" << endl;
        cout << "3. Patient" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: "<<endl;
        int r;
        cin >> r;
        switch (r)
        {
            case 1:
            //Admin
            {
                cout << "\t Admin View \t" << endl;
                if(allAppointments.empty())
                {
                    cout<<"No appointments Scheduled."<<endl;
                }
                cout << "All Appointments:" << endl;
                for (auto* a : allAppointments)
                {
                    cout<< "Appointment ID: " << a->appointment_ID<<endl;
                    cout<< ", Name: " << a->patient->Name<<endl;
                    cout<< ", Age: " << a->patient->age<<endl;
                    cout<< " Department: " << a->patient->department_to_visit_for<< endl;
                    cout<< " Date: " << a->patient->consult_date<< endl;
                    cout<< " Time: " << a->patient->consult_timeslot<<endl;
                }
                break;
            }

            case 2:
            //Doctor
            {
                cout<<"\t Doctor View \t"<<endl;
                int docID;
                string date;
                cout<<"Enter your Doctor ID: ";
                cin >> docID;
                cout<<"Enter date to view (YYYY-MM-DD): ";
                cin >> date;
                if (!isDateValid(date)) 
                {
                    cout << "Invalid date format or past date. Please use YYYY-MM-DD format for future dates.\n";
                    break;
                }
                bool found=false;
                for (auto& d : doctors)
                {
                    if (d.id == docID)
                    {
                        d.displayScheduleForDate(date);
                        found=true;
                        break;
                    }
                }
                if (!found)
                {
                    cout<<"Invalid ID!Please try again"<<endl;
                }
                break;
            }

            case 3:
            //Patient
            {
                cout<< "\t Patient View \t" << endl;
                string name,phone,date,field;
                int age,docID;

                cout<<"Enter your Name: ";
                cin.ignore(); 
                getline(cin,name);
                if (name.empty()) 
                {
                    cout << "Name cannot be empty"<<endl;
                    break;
                }
                cout<<"Age: "; 
                cin >> age;
                if (age <= 0 || age > 120) 
                {
                    cout << "Invalid age. Please enter a valid age"<<endl;
                    break;
                }
                cout << "\t Available Departments \t "<<endl;
                cout << "1. General Medicine"<<endl;
                cout << "2. Gynecology"<<endl;
                cout << "3. Dermatology"<<endl;
                cout << "Choose department (enter name): ";
                cin.ignore();
                 getline(cin,field);
                 field=toLower(field);
                cout<<"Enter appointment date(YYYY-MM-DD): ";
                cin >> date;
                if (!isDateValid(date)) 
                {
                    cout << "Invalid date format or past date. Please use YYYY-MM-DD format"<<endl;
                    break;
                }
                cout<<"Mobile_Number: ";
                cin >> phone;
                //checking phone number format...
                regex phone_format("^\\d{10}$");
                if (!regex_match(phone, phone_format)) 
                {
                    cout << "Invalid phone number. Must be 10 digits." << endl;
                    break;
                }


                //Visibility of available doctors!
                vector<Doc*> matchingDocs;
                cout<< " \n Available Doctors in" << field <<endl;
                for (auto& d : doctors)
                {

                    if (toLower(d.specialisation) == field)
                    {
                        matchingDocs.push_back(&d);
                        cout<<d.id<<"\t"<<d.name<<endl;
                    }
                }
                    if (matchingDocs.empty()) 
                    {
                        cout << "No doctors available in this department. Try again.\n";
                        continue;
                  
                    }
                cout <<"Choose your Doctor(enter ID): ";
                cin >> docID;
                Doc* selectedDoc = nullptr;
                for (auto& d : matchingDocs)
                {
                    if (d->id == docID)
                    {
                        selectedDoc= d;
                        /*
                        if doc Id is found, then that doctor's pointer is stored in doc!
                        for further need,i.e., to check the availability of that doc
                        by showing available time slots!
                        */
                        break;
                    }
                }
                if (!selectedDoc)
                {
                    cout<<"Invalid Doctor ID.Please try again"<<endl;
                    break;
                }

                string slot = selectedDoc->getNextAvailableSlot(date);
                if (slot.empty())
                {
                    cout<<"No available slots on: "<<date<<" for " << selectedDoc->name<<endl;
                    cout<<"Please try anaother date/doctor"<<endl;
                    break;
                }

                //Creation of Patient object & Appointment
                Patient* p = new Patient(patientID++, name, age, phone, field, date, slot);
                Appointment* a = new Appointment(p, selectedDoc);
                selectedDoc->scheduleByDate[date].push_back(a);
                allAppointments.push_back(a);


                cout<<"\t Appointment Confirmed! \t"<<endl;
                cout<<"Appointment ID: "<<a->appointment_ID<<endl;
                cout<<"Patient Name: "<<p->Name<<endl;
                cout<<"Doctor: "<<selectedDoc->name<<endl;
                cout<<"Date: "<<date<<" Time_Slot: "<<slot<<endl;
                cout<<"Mobile Number: "<<phone<<endl;
                cout<<"---------------------------------------"<<endl;
                break;
            
            }

            case 4:
            //EXIT
            {    cout<<"\t Existing system \t"<<endl;
                //Deletion of Memory(since allocated dynamically!)
                for (auto* a : allAppointments)
                {   delete a->patient;
                    delete a;
                }
                
                allAppointments.clear();

                cout << "Thank you. GoodBye! " << endl;
                return 0;
            }

            default:
            {
                cout<<"Invalid choice.Please enter a number between 1-4"<<endl;
                cin.clear();
                cin.ignore();
                break;
            }
        }
    }
    return 0;
}