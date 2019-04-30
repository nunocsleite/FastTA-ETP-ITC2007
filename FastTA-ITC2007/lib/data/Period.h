#ifndef PERIOD_H
#define PERIOD_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include "Exam.h"
#include <ostream>





//class Period {

//public:
//    Period() : periodExams(0) { }
//    vector<boost::shared_ptr<Exam> > const& getExams() const { return periodExams; }

//    int getCapacity() const { return periodExams.size(); }

//    void addExam(boost::shared_ptr<Exam> const& exam) {
//        periodExams.push_back(exam);
//    }

////    friend ostream& operator<<(ostream& out, Period const& period) {
////        out << "Id = " << period.getId() << endl;
////        out << "capacity = " << period.getCapacity() << endl;
////        out << "Exams = ";
////        out << "Exams size = " << period.getExams().size() << endl;
////        for (vector<boost::shared_ptr<Exam> >::const_iterator it = period.getExams().begin(); it != period.getExams().end(); ++it) {
////            out << (*it).get()->getId() << endl;
////        }
////        return out;
////    }

//private:

//    std::vector<int> periodExams;

//};



/* USED IN ITC 2007?
 */
class Period {
    std::vector<boost::shared_ptr<Exam> > examsToSchedule;
    int id;

public:
    Period() : examsToSchedule(0), id(0) { }
    Period(int id) : examsToSchedule(0), id(id) { }
    int getId() const { return id; }
    void setId(int id) { this->id = id; }
    std::vector<boost::shared_ptr<Exam> > const& getExams() const { return examsToSchedule; }
    int getCapacity() const { return examsToSchedule.size(); }
    void addExam(boost::shared_ptr<Exam> const& exam) {
        examsToSchedule.push_back(exam);
    }
    bool operator<(Period const& period) const {
        return getCapacity() < period.getCapacity();
    }

    friend std::ostream& operator<<(std::ostream& out, Period const& period) {
        out << "Id = " << period.getId() << std::endl;
        out << "capacity = " << period.getCapacity() << std::endl;
        out << "Exams = ";
        out << "Exams size = " << period.getExams().size() << std::endl;
        for (std::vector<boost::shared_ptr<Exam> >::const_iterator it = period.getExams().begin(); it != period.getExams().end(); ++it) {
            out << (*it).get()->getId() << std::endl;
        }
        return out;
    }
};


#endif // PERIOD_H
