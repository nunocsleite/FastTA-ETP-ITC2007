#ifndef EXAMINFOOPT_H
#define EXAMINFOOPT_H


#include <ostream>
#include <boost/shared_array.hpp>
#include <boost/range/algorithm.hpp>

using namespace std;


class ExamInfoOpt {

public:
    // Ctor
    ExamInfoOpt(int _examIndex);
    inline int getExamColorDegree() const;
    inline void setExamColorDegree(int value);
    // Update exam move count
    inline void updateMove();
    // Get exam index
    inline int getExamIndex() const;

    friend ostream& operator<<(ostream& _os, const ExamInfoOpt& _examInfo);

    inline int getSequentialExamIndex() const;
    inline long getExamMoveCount() const;

private:
    int examIndex;
    int examColorDegree; // Exam color degree
    long moveCounts; // This exam move counts
};




int ExamInfoOpt::getExamColorDegree() const
{
    return examColorDegree;
}

void ExamInfoOpt::setExamColorDegree(int value)
{
    examColorDegree = value;
}

// Update exam move count
void ExamInfoOpt::updateMove() {
    ++moveCounts;
}

int ExamInfoOpt::getExamIndex() const
{
    return examIndex;
}

long ExamInfoOpt::getExamMoveCount() const {
    return moveCounts;
}

#endif // EXAMINFOOPT_H

