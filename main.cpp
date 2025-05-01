#include <iostream>
#include <queue>
#include <vector>
#include <string>
using namespace std;

// Forward declarations
class Doc;
class Appointment;

// Patient Details List
class Patient {
public:
    int ID;
    string Name;
    int age;
    string mobile_no;
    string consult_date;
    string department_to_visit_for;
    string consult_timeslot;

    // Constructor
    Patient(int ID, string Name, int age, string mobile_no,
            string department_to_visit_for, string consult_date,
            string consult_timeslot)
        : ID(ID), Name(Name), age(age), mobile_no(mobile_no),
          department_to_visit_for(department_to_visit_for),
          consult_date(consult_date), consult_timeslot(consult_timeslot) {}
};

// Doctor Details
class Doc {
public:
    int Doc_id;
    string Name;
    string specialisation;
    string type; // e.g., Physician_1, Physician_2, Specialist

    // Constructor
    Doc(int Doc_id, string Name, string specialisation, string type)
        : Doc_id(Doc_id), Name(Name), specialisation(specialisation), type(type) {}
};
