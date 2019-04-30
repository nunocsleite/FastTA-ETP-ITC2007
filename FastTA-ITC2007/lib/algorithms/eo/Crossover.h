#ifndef CROSSOVER_H
#define CROSSOVER_H

#include "chromosome/eoChromosome.h"
#include <eoOp.h>
#include "kempeChain/ETTPKempeChainHeuristic.h"
#include <utility>
#include <boost/unordered_map.hpp>
#include "neighbourhood/ETTPneighborhood.h"
#include "algorithms/mo/moTA.h"
#include "neighbourhood/ETTPneighborEval.h"
#include "algorithms/mo/moSimpleCoolingSchedule.h"
#include "testset/TestSetDescription.h"
#include <eoGenContinue.h>
#include <utils/eoCheckPoint.h>

// For debugging purposes
//#define _CROSSOVER_DEBUG


template <typename EOT>
class Crossover : public eoQuadOp<EOT>
{

public:

    /**
    * the class name (used to display statistics)
    */
    std::string className() const;

    /**
    * eoQuad crossover - _chromosome1 and _chromosome2 are the (future) offspring, i.e. _copies_ of the parents
    * @param _chromosome1 the first parent
    * @param _chromosome2 the second parent
    */
    bool operator()(EOT& _chromosome1, EOT& _chromosome2);

protected:

    /**
    * generation of an offspring
    * @param _parent1 the first parent
    * @param _parent2 the second parent
    */
    void generateOffspring(EOT &_parent1, EOT &_parent2);

    void insertExams(EOT &_offspring, const EOT &_parent);

    /**
     * @brief eoChromosome::removeExam Remove exam _ei from timetable ignoring period _randOffspringPeriod
     * @param _timetableOffspring
     * @param _ei
     * @param _randOffspringPeriod
     */
    int findExamPeriod(TimetableContainer &_timetable, int _ei, int _randOffspringPeriod) const;

//    void generateOffspring2(EOT &_parent1, EOT &_parent2);

//    void feasibleExamInsertion(EOT &_chrom, int exam);

//    boost::shared_ptr<int> selectDays(const EOT & _chrom);

////    void insertDays2(eoChromosome & _parent1, eoChromosome const& _parent2, boost::shared_ptr<int> _range2);

//    void insertDays(EOT &_parent1, const EOT &_parent2, boost::shared_ptr<int> _range2);

    /**
    * best day computation
    * @param chromosome
    */
//    int bestDay(eoChromosome const& chromosome);

    /**
    * insert day into chromosome
    * @param _parent1
    * @param _parent2
    * @param day2
    */
//    void insertDay(eoChromosome& _parent1, eoChromosome& _parent2, int day2);
};




//class IsInExamsToRemove {
//    std::vector<int> const& examsToRemove;
//public:
//    IsInExamsToRemove(vector<int> const& examsToRemove) : examsToRemove(examsToRemove) { }
//    bool operator()(int exam) {
//        return find_if(examsToRemove.begin(), examsToRemove.end(), bind2nd(equal_to<int>(), exam)) != examsToRemove.end();
//    }
//};


template <typename EOT>
std::string Crossover<EOT>::className() const
{
    return "ETTP Crossover";
}

/*
bool Crossover::operator()(eoChromosome& _chromosome1, eoChromosome& _chromosome2)
{
//    cout << "Crossover" << endl;

    bool oneAtLeastIsModified = true;
    //
    // Day-exchange crossover
    //
    // In day-exchange crossover, only the best days (excluding
    // Saturdays, since exams scheduled on Saturdays are always
    // clash-free) of chromosomes, selected based on the crossover
    // rate, are eligible for exchange. The best day consists of three
    // periods and is the day with the lowest number of clashes per student.

    // Computation of the offspring
    generateOffspring(_chromosome1, _chromosome2);
    generateOffspring(_chromosome2, _chromosome1);


//    // does at least one genotype has been modified ?
//    if ((_chromosome1 != offspring1) || (_chromosome2 != offspring2))
//    {
//        // update
//        _chromosome1.value(offspring1);
//        _chromosome2.value(offspring2);
//        // at least one genotype has been modified
//        oneAtLeastIsModified = true;
//    }
//    else
//    {
//        // no genotype has been modified
//        oneAtLeastIsModified = false;
//    }
    // return 'true' if at least one genotype has been modified
    return oneAtLeastIsModified;
}
*/

template <typename EOT>
bool Crossover<EOT>::operator()(EOT& _chromosome1, EOT& _chromosome2)
{
//    cout << "Crossover" << endl;

    bool oneAtLeastIsModified = true;
    // Computation of the offspring
    EOT chrom1 = _chromosome1; // Original chromosome 1
    EOT chrom2 = _chromosome2; // Original chromosome 2
    generateOffspring(_chromosome1, chrom2); // offspring 1 is the resulting chromosome 1
    generateOffspring(_chromosome2, chrom1); // offspring 2 is the resulting chromosome 2

//    generateOffspring2(_chromosome1, chrom2); // offspring 1 is the resulting chromosome 1
//    generateOffspring2(_chromosome2, chrom1); // offspring 2 is the resulting chromosome 2

    return oneAtLeastIsModified;
}


/*

int max_element_idx(vector<unordered_set<int> > const& solutionPeriods) {
    int indexLargest = 0;
    for (int i = 1; i < solutionPeriods.size(); ++i) {
        if (solutionPeriods[i].size() > solutionPeriods[indexLargest].size())
            indexLargest = i;
    }
    return indexLargest;
}


//
// GPX based crossover (for graph colouring)
// (see Memetic Algorithms in Discrete Optimization paper by J.-K. Hao)
//
void Crossover::generateOffspring(eoChromosome & _parent1, eoChromosome & _parent2)
{
    // Original Parent 1 (copy)
    eoChromosome s1 = _parent1;
    // Original Parent 2 (reference)
    eoChromosome& s2 = _parent2;
    // We change directly _parent1 to be the new offspring because it's already a copy
    // of the original parent.
    eoChromosome& offspring = _parent1;
    // Get parent 1 periods
    vector<unordered_set<int> >& s1Periods = s1.getPeriods();
    // Get parent 2 periods
    vector<unordered_set<int> >& s2Periods = s2.getPeriods();
    // Get offspring periods
    vector<unordered_set<int> >& offspringPeriods = offspring.getPeriods();
    // Empty periods in the offspring
    offspringPeriods.clear();


    cout << "Periods sizes:" << endl;
    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it)
        cout << (*it).size() << " ";
    cout << endl;

    bool firstParent = true;
    vector<unordered_set<int> > * s1PeriodsPtr = 0;
    vector<unordered_set<int> > * s2PeriodsPtr = 0;

    // At the first step, GPX create first offspring period by choosing the largest period
    // from s1 and removes its vertices from both the s1 and s2.
    // GPX repeats then these same operations for the next k-1 steps, but alternates each time
    // the parent considered. If some vertices remain unassigned at the end of these k steps,
    // they are randomly assigned to one of the k periods (classes).
    for (int i = 0; i < s1.getNumPeriods(); ++i) {
        if (firstParent) {
            s1PeriodsPtr = &s1Periods;
            s2PeriodsPtr = &s2Periods;
        }
        else {
            s1PeriodsPtr = &s2Periods;
            s2PeriodsPtr = &s1Periods;
        }

        // Choose the largest period from s1 and insert it in the offspring
        int indexLargest = max_element_idx(*s1PeriodsPtr);

//        cout << "largest period size = " << (*s1PeriodsPtr)[indexLargest].size() << endl;

        // Insert largest period exams in offspring
        offspringPeriods.push_back((*s1PeriodsPtr)[indexLargest]);
//        cout << "Offspring periods:" << endl;

//        for (int i = 0; i < offspringPeriods.size(); ++i)
//            cout << offspringPeriods[i].size() << " ";
//        cout << endl;

        // Removes largest period vertices from both the s1 and s2.
        // Remove from s1
        (*s1PeriodsPtr)[indexLargest].clear();
        // Remove from s2
        unordered_set<int> const& examsToRemove = offspringPeriods.back();

        IsInExamsToRemove pred(examsToRemove);
        // Remove each exam in examsToRemove vector
        for (unordered_set<int>::const_iterator itRem = examsToRemove.begin(); itRem != examsToRemove.end(); ++itRem) {
            int i;
            for (i = 0; i < (*s2PeriodsPtr).size(); ++i) {
                unordered_set<int>& exams = (*s2PeriodsPtr)[i];
                if (exams.find(*itRem) != exams.end()) {
                    exams.erase(*itRem);
                    break;
                }
            }
            if (i == (*s2PeriodsPtr).size()) {
                cout << endl << "numPeriods = " << (*s2PeriodsPtr).size() << endl;
                cout << endl << "Couldn't find exam " << *itRem << endl;
                cin.get();
            }
        }
//        s2.validate();

        // Alternate between parents
        firstParent = !firstParent;

    }

    cout << "s1 Periods sizes:" << endl;
    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it)
        cout << (*it).size() << " ";
    cout << endl;


    cout << "s2 Periods sizes:" << endl;
    for (auto it = s2Periods.begin(); it != s2Periods.end(); ++it)
        cout << (*it).size() << " ";
    cout << endl;


    cout << "offspring Periods sizes:" << endl;
    for (auto it = offspringPeriods.begin(); it != offspringPeriods.end(); ++it)
        cout << (*it).size() << " ";
    cout << endl;

    cin.get();

    // If some vertices remain unassigned at the end of these k steps,
    // they are randomly assigned to one of the k periods (classes).

/// TODO: Try to insert exams into feasible periods first before using Kempe chain operator

    // Insert remainder exams from s1
    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it) {
        if ((*it).size() > 0) {
            cout << (*it).size() << " exams to insert" << endl;

            // Insert all exams from this period at random periods in the offspring
            for (auto examIt = (*it).begin(); examIt != (*it).end(); ++examIt) {
                cout << endl << endl << "\tInserting exam " << *examIt << endl;

                cin.get();

                feasibleExamInsertion(offspring, *examIt);

            }
        }
    }

//    cin.get();

    offspring.validate();

    // Actualize proximity costs
    offspring.computeProximityCosts();
}



void Crossover::feasibleExamInsertion(eoChromosome & _chrom, int exam) {

    cout << "_chrom.getNumPeriods() = " << _chrom.getNumPeriods() << endl;
    cout << "Inserting exam " << exam << endl;


    ETTPKempeChainHeuristic kempeHeuristic;

    int exami;
    int ti, tj;

    unordered_set<int>* ti_period;
    unordered_set<int>* tj_period;

    // Select randomly two timeslots, ti and tj.
    do {
        ti = rng.random(_chrom.getNumPeriods());
        do {
            tj = rng.random(_chrom.getNumPeriods());
        }
        while (ti == tj);
    }
    while (_chrom.getPeriods()[ti].size() == 0);

    // Select randomly an exam ei from timeslot ti and move it
    // to timeslot tj. If the solution becomes infeasible,
    // because exam ei has students in common with exams ej, ek, ...,
    // that are assigned to time slot tj, one have to move exams
    // ej, ek, ..., to time slot ti. This process is repeated until all the
    // exams that have students in common are assigned to different time slots.


//    cout << "ti = " << ti << ", tj = " << tj << ", _chrom.getNumPeriods() = " << _chrom.getNumPeriods() << endl;

    vector<unordered_set<int> >& periods = _chrom.getPeriods();

    ti_period = &periods[ti];
    tj_period = &periods[tj];

//    // Generate random index
//    int idx = rng.random(ti_period->size());

////    cout << "idx = " << idx << ", ti_period.size() = " << ti_period.size() << endl;

//    /// TODO - NOT EFFICIENT
//    // Get exam id
//    unordered_set<int>::const_iterator it_i;
//    it_i = ti_period->begin();

//    for (int i = 0; i < idx; ++i, ++it_i)
//        ;

//    exami = *it_i;

    // Assume that exam to insert feasibly into a random time slot is initially at time slot ti
    // This is temporary because exam is going to be moved to time slot tj
    // Note: exam could be infeasible in time slot ti
    ti_period->insert(exam);
    exami = exam;

    // Kempe chain operator
    kempeHeuristic.kempeChainOperator(_chrom, *ti_period, *tj_period, exami);

}

*/

//===
// Abdullah et al., Sabar et. al feasible crossover
//===
template <typename EOT>
void Crossover<EOT>::generateOffspring(EOT &_parent1, EOT &_parent2)
{
    //
    // This crossover operator generates two offspring in the following way:
    // exams chosen from a random period in parent2 are inserted in a
    // random period in period1. Unfeasible exams are nor inserted and all
    // duplicated exams in the other periods in parent1 are deleted in order
    // to maintain feasibility.
    //
    // Instantiate a copy of parent1 to be used in the generation of offspring2
    EOT parent1Copy = _parent1;
    // We change directly _parent1 and _parent2 to be the new offsprings because
    // they're already a copy of the original parents.
    EOT& offspring1 = _parent1;
    EOT& offspring2 = _parent2;
    // Insert exams in offspring1 and compute offspring1 proximity cost incrementally
    insertExams(offspring1, _parent2);
    // Insert exams in offspring2 and compute offspring2 proximity cost incrementally
    insertExams(offspring2, parent1Copy);
}


template <typename EOT>
void Crossover<EOT>::insertExams(EOT &_offspring, const EOT &_parent)
{
//    long examProximityConflictsOfRemovedExams = 0;
//    long examProximityConflictsOfInsertedExams = 0;

//    // Offspring timetable
//    TimetableContainer &timetableOffspring = _offspring.getTimetableContainer();
//    // Parent timetable
//    TimetableContainer const &timetableParent = _parent.getTimetableContainer();
//    // # exams
//    const int numExams = _offspring.getNumExams();
//    // Insert 3 random periods
//    int randNumPeriods = 3; // TEST
////    int randNumPeriods = 1;
//    for (int i = 1; i <= randNumPeriods; ++i) {
//        // Get a random period from parent2
//        int randParentPeriod = rng.random(_parent.getNumPeriods());
//        // Generate a random index in the offspring
//        int randOffspringPeriod = rng.random(_offspring.getNumPeriods());
//        // Insert parent2 random period exams (those that are feasible)
//        // into the new offspring and remove duplicated exams
//        //
//        // For each exam of period pi do
//        for (int ei = 0; ei < numExams; ++ei) {
//            // If exam ei is scheduled in parent
//            if (timetableParent.isExamScheduled(ei, randParentPeriod)) {
//                // Insert exam in offspring if it's not already insert and is feasible in period
//                if (!timetableOffspring.isExamScheduled(ei, randOffspringPeriod) &&
//                     _offspring.isExamFeasibleInPeriod(ei, randOffspringPeriod)) {
//                    // Find exam ei period
//                    int fromPeriod = findExamPeriod(timetableOffspring, ei, randOffspringPeriod);
//                    if (fromPeriod == -1) {
//                        std::cout << std::endl <<
//                                     "In method [Crossover<EOT>::insertExams] - could not found duplicated exam" << std::endl;
//                        std::cin.get();
//                        std::cout << "Exiting..." << endl;
//                        exit(-1);
//                    }
//                    // For incremental evaluation, we need to sum proximity conflicts of this exam to be removed
//                    long sourceExamProximityConflictsRemoval = _offspring.getExamProximityConflicts(ei, fromPeriod);
//                    // Sum proximity conflicts of this exam
//                    examProximityConflictsOfRemovedExams += sourceExamProximityConflictsRemoval;
//                    // Remove original exam in timetable in period fromPeriod
//                    timetableOffspring.removeExam(ei, fromPeriod);

//                    // Now, insert the exam
//                    timetableOffspring.insertExam(ei, randOffspringPeriod);
//                    // For incremental evaluation, we need to sum proximity conflicts of this inserted exam
//                    // Determine the proximity conflicts
//                    long sourceExamProximityConflictsInsertion = _offspring.getExamProximityConflicts(ei, randOffspringPeriod);
//                    // Sum proximity conflicts of this exam
//                    examProximityConflictsOfInsertedExams += sourceExamProximityConflictsInsertion;
//                }
//            }
//        }
//    }

//    // Actualize proximity costs - Incremental evaluation
//    long offspringExamProximityConflicts = _offspring.getProximityConflicts();
//    // Now subtract to the solution proximity conflicts the conflicts of the removed exams
//    offspringExamProximityConflicts -= examProximityConflictsOfRemovedExams;
//    // ... and add the conflicts of the inserted exams
//    offspringExamProximityConflicts += examProximityConflictsOfInsertedExams;
//    // Actualize proximity costs
//    _offspring.setExamProximityConflicts(offspringExamProximityConflicts);


/// DEBUG
///
    // Validate
//    _offspring.validate();

}


/**
 * @brief eoChromosome::removeExam Remove exam _ei from timetable ignoring period _randOffspringPeriod
 * @param _timetableOffspring
 * @param _ei
 * @param _randOffspringPeriod
 */
template <typename EOT>
int Crossover<EOT>::findExamPeriod(TimetableContainer &_timetable, int _ei, int _randOffspringPeriod) const {
    // For each period (except _randOffspringPeriod) do
    for (int pi = 0; pi < _timetable.getNumPeriods(); ++pi) {
        if (pi != _randOffspringPeriod) {
            // If exam ei is scheduled in period pi
            if (_timetable.isExamScheduled(_ei, pi)) {
                return pi;
            }
        }
    }
    return -1; // Exam not found
}



/*
 *
 *ORIGINAL CROSSOVER TESTED ON CGA
 *
//===
// Abdullah feasible crossover
//===
template <typename EOT>
void Crossover<EOT>::generateOffspring(EOT &_parent1, EOT &_parent2)
{
    // We change directly _parent1 to be the new offspring because it's already a copy
    // of the original parent.
    EOT& offspring = _parent1;


    std::vector<unordered_set<int> >& p2Periods = _parent2.getPeriods();
    // Get offspring periods
    std::vector<unordered_set<int> >& offspringPeriods = offspring.getPeriods();

    // Generate random number of periods to insert from Pw
//    int randNumPeriods = rng.uniform(10);

//    int randNumPeriods = 100;
    int randNumPeriods = 3; // BETTER in cGA
//    int randNumPeriods = 1; //LAST used with GDA
    for (int i = 1; i <= randNumPeriods; ++i) {
        // Get a random period from Pw
        int randParent2Period = rng.random(_parent2.getNumPeriods());
        // Insert Pw random period exams (those that are feasible)
        // into the new frog and remove duplicated exams

        // Generate a random index in the new frog
        int randOffspringPeriod = rng.random(offspring.getNumPeriods());
        // Pw exams from best day
        unordered_set<int>& p2Exams = p2Periods[randParent2Period];

        // newPw exams from rand period
        unordered_set<int>& offspringExams = offspringPeriods[randOffspringPeriod];

        // Insert Pw period into newPw into that random position
        for (auto it = p2Exams.begin(); it != p2Exams.end(); ++it) {
            // If the exam does not exist and is feasible then insert it
            if (offspringExams.find(*it) == offspringExams.end()
                    && offspring.isFeasiblePeriodExam(randOffspringPeriod, *it)) {
//                        if (newPwExams.find(*it) == newPwExams.end() && newPw.isFeasiblePeriodExam(bestDayIdx, *it)) {


            offspringExams.insert(*it);
//                ++numInsertedExams;
//                        cout << "Insert: " << *it << endl;
//                        cin.get();


                /// TODO / SEE POSSIBLE OPTIMIZATIONS

                // Remove duplicated exams
                for (int i = 0; i < offspringPeriods.size(); ++i) {
                    if (i != randOffspringPeriod) {
//                        if (i != bestDayIdx) {
                        unordered_set<int>& exams = offspringPeriods[i];
                        if (exams.find(*it) != exams.end()) {
                            exams.erase(*it);
//                            ++numRemovedExams;
//                            cout << "Remove: " << *it << endl;
                            break;
                        }
                    }
                }

            }
//            else {
//                notInserted.insert(*it);
//                ++numNotInsertedExams;
//            }

        }
    }


    // Actualize proximity costs
    offspring.computeProximityCosts();



/// TODO - INCREMENTAL EVALUATION
///
///
///

}
*/


#endif // CROSSOVER_H
