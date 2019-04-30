
//#include "Crossover.h"

//#include "ETTPKempeChainHeuristic.h"

////#include "GraphColouringHeuristics.h"
//#include <utility>



//#include <boost/unordered_map.hpp>
//#include "ETTPneighborhood.h"
//#include "ProximityCostEval.h"
//#include "moTA.h"
//#include "ETTPneighborEval.h"
//#include "moSimpleCoolingSchedule.h"

//#include "moGDA.h"


/////////

////#include "eoSCEA.h"
////#include "ETTPInit.h"
//#include "TestSetDescription.h"
////#include "TestSet.h"
////#include "TorontoTestSet.h"


//////////

//#include <eoGenContinue.h>
//#include <utils/eoCheckPoint.h>


//// For debugging purposes
////#define _CROSSOVER_DEBUG


//template <typename EOT>
//class IsInExamsToRemove {
//    vector<int> const& examsToRemove;
//public:
//    IsInExamsToRemove(vector<int> const& examsToRemove) : examsToRemove(examsToRemove) { }
//    bool operator()(int exam) {
//        return find_if(examsToRemove.begin(), examsToRemove.end(), bind2nd(equal_to<int>(), exam)) != examsToRemove.end();
//    }
//};


//template <typename EOT>
//string Crossover<EOT>::className() const
//{
//    return "ETTP Crossover";
//}

///*
//bool Crossover::operator()(eoChromosome& _chromosome1, eoChromosome& _chromosome2)
//{
////    cout << "Crossover" << endl;

//    bool oneAtLeastIsModified = true;
//    //
//    // Day-exchange crossover
//    //
//    // In day-exchange crossover, only the best days (excluding
//    // Saturdays, since exams scheduled on Saturdays are always
//    // clash-free) of chromosomes, selected based on the crossover
//    // rate, are eligible for exchange. The best day consists of three
//    // periods and is the day with the lowest number of clashes per student.

//    // Computation of the offspring
//    generateOffspring(_chromosome1, _chromosome2);
//    generateOffspring(_chromosome2, _chromosome1);


////    // does at least one genotype has been modified ?
////    if ((_chromosome1 != offspring1) || (_chromosome2 != offspring2))
////    {
////        // update
////        _chromosome1.value(offspring1);
////        _chromosome2.value(offspring2);
////        // at least one genotype has been modified
////        oneAtLeastIsModified = true;
////    }
////    else
////    {
////        // no genotype has been modified
////        oneAtLeastIsModified = false;
////    }
//    // return 'true' if at least one genotype has been modified
//    return oneAtLeastIsModified;
//}
//*/

//bool Crossover<EOT>::operator()(EOT& _chromosome1, EOT& _chromosome2)
//{
////    cout << "Crossover" << endl;

//    bool oneAtLeastIsModified = true;
//    // Computation of the offspring
//    EOT chrom1 = _chromosome1; // Original chromosome 1
//    EOT chrom2 = _chromosome2; // Original chromosome 2
//    generateOffspring(_chromosome1, chrom2); // offspring 1 is the resulting chromosome 1
//    generateOffspring(_chromosome2, chrom1); // offspring 2 is the resulting chromosome 2

////    generateOffspring2(_chromosome1, chrom2); // offspring 1 is the resulting chromosome 1
////    generateOffspring2(_chromosome2, chrom1); // offspring 2 is the resulting chromosome 2

//    return oneAtLeastIsModified;
//}


///*

//int max_element_idx(vector<unordered_set<int> > const& solutionPeriods) {
//    int indexLargest = 0;
//    for (int i = 1; i < solutionPeriods.size(); ++i) {
//        if (solutionPeriods[i].size() > solutionPeriods[indexLargest].size())
//            indexLargest = i;
//    }
//    return indexLargest;
//}


////
//// GPX based crossover (for graph colouring)
//// (see Memetic Algorithms in Discrete Optimization paper by J.-K. Hao)
////
//void Crossover::generateOffspring(eoChromosome & _parent1, eoChromosome & _parent2)
//{
//    // Original Parent 1 (copy)
//    eoChromosome s1 = _parent1;
//    // Original Parent 2 (reference)
//    eoChromosome& s2 = _parent2;
//    // We change directly _parent1 to be the new offspring because it's already a copy
//    // of the original parent.
//    eoChromosome& offspring = _parent1;
//    // Get parent 1 periods
//    vector<unordered_set<int> >& s1Periods = s1.getPeriods();
//    // Get parent 2 periods
//    vector<unordered_set<int> >& s2Periods = s2.getPeriods();
//    // Get offspring periods
//    vector<unordered_set<int> >& offspringPeriods = offspring.getPeriods();
//    // Empty periods in the offspring
//    offspringPeriods.clear();


//    cout << "Periods sizes:" << endl;
//    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it)
//        cout << (*it).size() << " ";
//    cout << endl;

//    bool firstParent = true;
//    vector<unordered_set<int> > * s1PeriodsPtr = 0;
//    vector<unordered_set<int> > * s2PeriodsPtr = 0;

//    // At the first step, GPX create first offspring period by choosing the largest period
//    // from s1 and removes its vertices from both the s1 and s2.
//    // GPX repeats then these same operations for the next k-1 steps, but alternates each time
//    // the parent considered. If some vertices remain unassigned at the end of these k steps,
//    // they are randomly assigned to one of the k periods (classes).
//    for (int i = 0; i < s1.getNumPeriods(); ++i) {
//        if (firstParent) {
//            s1PeriodsPtr = &s1Periods;
//            s2PeriodsPtr = &s2Periods;
//        }
//        else {
//            s1PeriodsPtr = &s2Periods;
//            s2PeriodsPtr = &s1Periods;
//        }

//        // Choose the largest period from s1 and insert it in the offspring
//        int indexLargest = max_element_idx(*s1PeriodsPtr);

////        cout << "largest period size = " << (*s1PeriodsPtr)[indexLargest].size() << endl;

//        // Insert largest period exams in offspring
//        offspringPeriods.push_back((*s1PeriodsPtr)[indexLargest]);
////        cout << "Offspring periods:" << endl;

////        for (int i = 0; i < offspringPeriods.size(); ++i)
////            cout << offspringPeriods[i].size() << " ";
////        cout << endl;

//        // Removes largest period vertices from both the s1 and s2.
//        // Remove from s1
//        (*s1PeriodsPtr)[indexLargest].clear();
//        // Remove from s2
//        unordered_set<int> const& examsToRemove = offspringPeriods.back();

//        IsInExamsToRemove pred(examsToRemove);
//        // Remove each exam in examsToRemove vector
//        for (unordered_set<int>::const_iterator itRem = examsToRemove.begin(); itRem != examsToRemove.end(); ++itRem) {
//            int i;
//            for (i = 0; i < (*s2PeriodsPtr).size(); ++i) {
//                unordered_set<int>& exams = (*s2PeriodsPtr)[i];
//                if (exams.find(*itRem) != exams.end()) {
//                    exams.erase(*itRem);
//                    break;
//                }
//            }
//            if (i == (*s2PeriodsPtr).size()) {
//                cout << endl << "numPeriods = " << (*s2PeriodsPtr).size() << endl;
//                cout << endl << "Couldn't find exam " << *itRem << endl;
//                cin.get();
//            }
//        }
////        s2.validate();

//        // Alternate between parents
//        firstParent = !firstParent;

//    }

//    cout << "s1 Periods sizes:" << endl;
//    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it)
//        cout << (*it).size() << " ";
//    cout << endl;


//    cout << "s2 Periods sizes:" << endl;
//    for (auto it = s2Periods.begin(); it != s2Periods.end(); ++it)
//        cout << (*it).size() << " ";
//    cout << endl;


//    cout << "offspring Periods sizes:" << endl;
//    for (auto it = offspringPeriods.begin(); it != offspringPeriods.end(); ++it)
//        cout << (*it).size() << " ";
//    cout << endl;

//    cin.get();

//    // If some vertices remain unassigned at the end of these k steps,
//    // they are randomly assigned to one of the k periods (classes).

///// TODO: Try to insert exams into feasible periods first before using Kempe chain operator

//    // Insert remainder exams from s1
//    for (auto it = s1Periods.begin(); it != s1Periods.end(); ++it) {
//        if ((*it).size() > 0) {
//            cout << (*it).size() << " exams to insert" << endl;

//            // Insert all exams from this period at random periods in the offspring
//            for (auto examIt = (*it).begin(); examIt != (*it).end(); ++examIt) {
//                cout << endl << endl << "\tInserting exam " << *examIt << endl;

//                cin.get();

//                feasibleExamInsertion(offspring, *examIt);

//            }
//        }
//    }

////    cin.get();

//    offspring.validate();

//    // Actualize proximity costs
//    offspring.computeProximityCosts();
//}



//void Crossover::feasibleExamInsertion(eoChromosome & _chrom, int exam) {

//    cout << "_chrom.getNumPeriods() = " << _chrom.getNumPeriods() << endl;
//    cout << "Inserting exam " << exam << endl;


//    ETTPKempeChainHeuristic kempeHeuristic;

//    int exami;
//    int ti, tj;

//    unordered_set<int>* ti_period;
//    unordered_set<int>* tj_period;

//    // Select randomly two timeslots, ti and tj.
//    do {
//        ti = rng.random(_chrom.getNumPeriods());
//        do {
//            tj = rng.random(_chrom.getNumPeriods());
//        }
//        while (ti == tj);
//    }
//    while (_chrom.getPeriods()[ti].size() == 0);

//    // Select randomly an exam ei from timeslot ti and move it
//    // to timeslot tj. If the solution becomes infeasible,
//    // because exam ei has students in common with exams ej, ek, ...,
//    // that are assigned to time slot tj, one have to move exams
//    // ej, ek, ..., to time slot ti. This process is repeated until all the
//    // exams that have students in common are assigned to different time slots.


////    cout << "ti = " << ti << ", tj = " << tj << ", _chrom.getNumPeriods() = " << _chrom.getNumPeriods() << endl;

//    vector<unordered_set<int> >& periods = _chrom.getPeriods();

//    ti_period = &periods[ti];
//    tj_period = &periods[tj];

////    // Generate random index
////    int idx = rng.random(ti_period->size());

//////    cout << "idx = " << idx << ", ti_period.size() = " << ti_period.size() << endl;

////    /// TODO - NOT EFFICIENT
////    // Get exam id
////    unordered_set<int>::const_iterator it_i;
////    it_i = ti_period->begin();

////    for (int i = 0; i < idx; ++i, ++it_i)
////        ;

////    exami = *it_i;

//    // Assume that exam to insert feasibly into a random time slot is initially at time slot ti
//    // This is temporary because exam is going to be moved to time slot tj
//    // Note: exam could be infeasible in time slot ti
//    ti_period->insert(exam);
//    exami = exam;

//    // Kempe chain operator
//    kempeHeuristic.kempeChainOperator(_chrom, *ti_period, *tj_period, exami);

//}

//*/


////===
//// Abdullah feasible crossover
////===
//void Crossover::generateOffspring(eoChromosome &_parent1, eoChromosome &_parent2)
//{
//    // We change directly _parent1 to be the new offspring because it's already a copy
//    // of the original parent.
//    eoChromosome& offspring = _parent1;


//    vector<unordered_set<int> >& p2Periods = _parent2.getPeriods();
//    // Get offspring periods
//    vector<unordered_set<int> >& offspringPeriods = offspring.getPeriods();

//    // Generate random number of periods to insert from Pw
////    int randNumPeriods = rng.uniform(10);

////    int randNumPeriods = 100;
//    int randNumPeriods = 3; // BETTER in cGA
////    int randNumPeriods = 1; //LAST used with GDA
//    for (int i = 1; i <= randNumPeriods; ++i) {
//        // Get a random period from Pw
//        int randParent2Period = rng.random(_parent2.getNumPeriods());
//        // Insert Pw random period exams (those that are feasible)
//        // into the new frog and remove duplicated exams

//        // Generate a random index in the new frog
//        int randOffspringPeriod = rng.random(offspring.getNumPeriods());
//        // Pw exams from best day
//        unordered_set<int>& p2Exams = p2Periods[randParent2Period];

//        // newPw exams from rand period
//        unordered_set<int>& offspringExams = offspringPeriods[randOffspringPeriod];

//        // Insert Pw period into newPw into that random position
//        for (auto it = p2Exams.begin(); it != p2Exams.end(); ++it) {
//            // If the exam does not exist and is feasible then insert it
//            if (offspringExams.find(*it) == offspringExams.end()
//                    && offspring.isFeasiblePeriodExam(randOffspringPeriod, *it)) {
////                        if (newPwExams.find(*it) == newPwExams.end() && newPw.isFeasiblePeriodExam(bestDayIdx, *it)) {


//            offspringExams.insert(*it);
////                ++numInsertedExams;
////                        cout << "Insert: " << *it << endl;
////                        cin.get();


//                /// TODO / SEE POSSIBLE OPTIMIZATIONS

//                // Remove duplicated exams
//                for (int i = 0; i < offspringPeriods.size(); ++i) {
//                    if (i != randOffspringPeriod) {
////                        if (i != bestDayIdx) {
//                        unordered_set<int>& exams = offspringPeriods[i];
//                        if (exams.find(*it) != exams.end()) {
//                            exams.erase(*it);
////                            ++numRemovedExams;
////                            cout << "Remove: " << *it << endl;
//                            break;
//                        }
//                    }
//                }

//            }
////            else {
////                notInserted.insert(*it);
////                ++numNotInsertedExams;
////            }

//        }
//    }


////    ETTPKempeChainHeuristic kempe;
////    kempe(offspring);

//    // Actualize proximity costs
//    offspring.computeProximityCosts();
//}







////void Crossover::generateOffspring2(eoChromosome &_parent1, eoChromosome &_parent2)
////{
////    ////////////////////////////////////////////////////////////////////////////////////////
////    //  We change directly _parent1 to be the new offspring because it's already a copy
////    //  of the original parent.
////    eoChromosome& offspring = _parent1;
////    // Select a range of days in the second parent.
////    boost::shared_ptr<int> range2 = selectDays(_parent2);
////    // Insert range2 days in the first parent and remove afected days, that contain duplicated exams.
////    // Then reinsert unplaced exams (e.g. using first fit or graph colouring heuristic).
//////    insertDays(offspring, _parent2, range2);

//////    insertDays2(offspring, _parent2, range2);


////    // Actualize proximity costs
////    offspring.computeProximityCosts();

//////    cout << "offspring # periods = " << offspring.getNumPeriods() << endl;


//////    cout << "offspring" << endl;

//////    cout << offspring << endl;


//////    cin.get();

////}




///*

//// WORKING ON CODE

//// Substitute range2 days in the first parent and then reinsert unplaced exams in a feasible fashion.
//// Using fitting optimization.
//void Crossover::insertDays2(eoChromosome & _parent1, eoChromosome const& _parent2, boost::shared_ptr<int> _range2) {

//#ifdef _CROSSOVER_DEBUG
//    cout << endl << endl << "Before inserting days" << endl;
//    cout << endl << "# periods = " << _parent1.getNumPeriods() << endl;

//    _parent1.validate();
//#endif

//    // Select a range of days in the first parent.
//    boost::shared_ptr<int> range1 = selectDays(_parent1);
//    // Get periods
//    vector<unordered_set<int> > & periods1 = _parent1.getPeriods();
//    vector<unordered_set<int> > const& periods2 = _parent2.getConstPeriods();
//    int dayA1 = range1.get()[0];       // First day in periods1
//    int dayB1 = _range2.get()[0];      // First day in periods2
//    int dayB2 = _range2.get()[1];      // Last day in periods2
//    int dayA2 = dayA1 + (dayB2-dayB1); // Last day in periods1

//#ifdef _CROSSOVER_DEBUG
//    cout << "range1 = [" << *(range1.get()) << "," << *(range1.get()+1) << "]" << endl;
//    cout << "range2 = [" << *(_range2.get()) << "," << *(_range2.get()+1) << "]" << endl;

//    cout << "dayA1 = " << dayA1 << endl;
//    cout << "dayA2 = " << dayA2 << endl;
//    cout << "dayB1 = " << dayB1 << endl;
//    cout << "dayB2 = " << dayB2 << endl;

//    cout << "dayA2-dayA1+1 = " << (dayA2-dayA1+1) << endl;
//    cout << "dayB2-dayB1+1 = " << (dayB2-dayB1+1) << endl;
//#endif

//    auto it2Beg = periods2.cbegin()+dayB1;
//    auto it2End = periods2.cbegin()+dayB2+1; // past the end iterator

//    // Set fixed exams to be range2 exams in [dayA1, dayA2] periods
//    boost::shared_ptr<unordered_map<int, int> > ptrFixedExams(new unordered_map<int, int>());

//    int period = dayA1; // Insert exams in range [dayA1, dayA2]
//    for (; it2Beg != it2End; ++it2Beg, ++period) {
//        for (unordered_set<int>::const_iterator examIt = (*it2Beg).begin(); examIt != (*it2Beg).end(); ++examIt) {
//            ptrFixedExams.get()->insert(pair<int, int>(*examIt, period));
//        }
//    }

//#ifdef _CROSSOVER_DEBUG
////    cout << "period = " << period << endl;
//#endif


//    // Insert range2 days in the parent1 in a feasible manner, doing optimization... not very efficient
//    _parent1.setFixedExams(ptrFixedExams.get());

//    // Do optimization

////    ETTPneighborhood neighborhood;
////    // Full evaluation function
////    ProximityCostEval<eoChromosome> fullEval;
////    // Placed Exams eval
////    PlacedExamsCostEval<eoChromosome> placedExamsEval;
////    // Neighbor evaluation function
////    ETTPneighborEval neighEval;

//    ETTPneighborhood<ETTPneighborPlacedExamsCost> neighborhood;
//    // Full evaluation function
//    ProximityCostEval<eoChromosome> fullEval;
//    // Placed Exams eval
//    PlacedExamsCostEval<eoChromosome> placedExamsEval;
//    // Neighbor evaluation function
//    ETTPneighborEval<ETTPneighborPlacedExamsCost> neighEval;





////    moSimpleCoolingSchedule<eoChromosome> cool(50, 0.000001, 5, 1e-6);

//    //    moSimpleCoolingSchedule<eoChromosome> cool(0.5, 0.00001, 5, 1e-6);
////    moTA<ETTPneighbor> ta(neighborhood, placedExamsEval, neighEval, cool);
////    const double UP = 0.0001;
////    moGDA<ETTPneighbor> ta(neighborhood, placedExamsEval, neighEval, UP);


/////////////////////////// CSEA /////////////////////////////////////////////////

////    TestSetDescription hec("hec-s-92", "Ecole des Hautes Etudes Commerciales", 18);
//    TestSetDescription hec("uta-s-92", "University of Toronto, Arts & Science", 35);

////    cout << "Test set\t # students\t # exams\t # enrolments\t conflict density\t # time slots" << endl;

//    // Version I of Toronto benchmarks
//    string rootDir = "./../../../Toronto Testprob/Toronto";


//    // Periods range definition.
//    int periods = hec.getPeriods();

//    // Specify MinPeriods and MaxPeriods

////      LAST
//    Data data(periods, periods, periods); // Fixed length timetables

//    //        Data data(periods-1, periods+1, periods); // Variable length timetables
////        Data data(periods-4, periods+4, periods);
////    Data data(periods-4, periods, periods);


//    // LAST USED IN NSGA-II
//    //Data data(periods, periods+4, periods);


//    // Create TestSet instance
//    TorontoTestSet testSet(hec.getName(), hec.getDescription(), data, rootDir);

//    TorontoTestSet* ptr = &testSet;
//    ptr->load(hec.getName(), rootDir);

//    ofstream outFile(hec.getName() + ".txt");



////    const int m = 1;   // m is the number of memeplexes
////    const int N = 4;  // N is the number of frogs in each memeplex
////    const int q = 4;  // q is the number of frogs in each submemeplex
//        const int m = 1;   // m is the number of memeplexes
//        const int N = 4;  // N is the number of frogs in each memeplex
//        const int q = 4;  // q is the number of frogs in each submemeplex



////    const int m = 5;   // m is the number of memeplexes
////    const int N = 20;  // N is the number of frogs in each memeplex
////    const int q = 20;  // q is the number of frogs in each submemeplex


//    const int F = m*N; // The total sample size, F, in the swamp is given by F = mN.

//    // Solution initializer
//    ETTPInit<eoChromosome> init(testSet.getData(), *(testSet.getConflictMatrix()), *(testSet.getGraph()));
//    // Generate initial population
////    eoPop<eoChromosome> pop(F, init);

//    placedExamsEval(_parent1);

//    eoPop<eoChromosome> pop;
//    for (int i = 0; i < F; ++i) {
//        pop.push_back(_parent1);
//        placedExamsEval(pop[i]);
////        cout << pop[i].fitness() << endl;
//        pop[i].setFixedExams(ptrFixedExams.get());
//    }



//    // Objective function evaluation
////    eoETTPEval eval;

//    // Number of consecutive time loops
////    int L = 1;

////    int L = 3; // PAPER

////    int L = 15;
//    int L = 50;
////        int L = 200;

//    //    int L = 500;
////      int L = 1000000;
////      int L = 10000; // Uta 3.38
////int L = 1000000; // Uta 3...


//    eoGenContinue<eoChromosome> terminator(L); // Terminate after concluding L time loops
//    eoCheckPoint<eoChromosome> checkpoint(terminator);
////    checkpoint.add(repair);
//    // Chromosome evolution operator
////    eoSFLAEvolOperator_1<eoChromosome> evolutionOperator; // generic hum...
////    eoSFLAEvolOperator_1 evolutionOperator; // generic hum...

//    // The best
////    eoSFLAEvolOperator_2 evolutionOperator; // generic hum...
//    eoSFLAEvolOperator_3 evolutionOperator; // generic hum...


//    // Build CSEA
////    eoCSEA<eoChromosome> csea(m, N, q, F, L, init, checkpoint, eval, evolutionOperator, outFile);
//    eoCSEA<eoChromosome> csea(m, N, q, F, L, init, checkpoint, placedExamsEval, evolutionOperator, outFile);

//    cout << "Before placing exams: fitness = " << _parent1.fitness() << endl;

//    // Run the algorithm
////    csea(pop);

////    cout << "Fim" << endl;

////    _parent1 = csea.getBestFrog().getChromosome();


////    moSimpleCoolingSchedule<eoChromosome> cool(0.1, 0.001, 5, 0.000001);
////    moSA<ETTPneighborPlacedExamsCost> sa(neighborhood, fullEval, neighEval, cool);

////    moSimpleCoolingSchedule<eoChromosome> cool(0.5, 0.1, 10, 0.0001);
//    moSimpleCoolingSchedule<eoChromosome> cool(0.5, 0.1, 50, 0.0001);

//    moTA<ETTPneighborPlacedExamsCost> ta(neighborhood, fullEval, neighEval, cool);

//    ta(_parent1);

//    placedExamsEval(_parent1);

//    cout << "After placing exams: fitness = " << _parent1.fitness() << endl;

//    fullEval(_parent1);

//    cout << endl;

////    cout << endl << "After evaluating with ProximityCostEval: fitness = " << _parent1.fitness() << endl;

////    cin.get();

//}

//*/







/////////////////////////////////////////////////////////////

////boost::shared_ptr<int> Crossover::selectDays(const eoChromosome & _chrom) {
////    int ti, tj;
////    // Select randomly two distinct timeslots, ti and tj.
////    ti = rng.random(_chrom.getNumPeriods());
////    do {
////        tj = rng.random(_chrom.getNumPeriods());
////    }
////    while (ti == tj);
////    // Sort time slots
////    boost::shared_ptr<int> range( new int[2], []( int *p ) { delete[] p; } ); // We have to specify a custom deleter
//////    boost::shared_ptr<int> range(new int[2]);
////    if (ti < tj) {
////        *(range.get()) = ti;
////        *(range.get()+1) = tj;
////    }
////    else {
////        *(range.get()) = tj;
////        *(range.get()+1) = ti;
////    }

////#ifdef _CROSSOVER_DEBUG
////    cout << "range = [" << *(range.get()) << "," << *(range.get()+1) << "]" << endl;
////#endif

//////    cin.get();

////    return range;
////}



////// Insert range2 days in the first parent and remove afected days, that contain duplicated exams.
////// Then reinsert unplaced exams (e.g. using first fit or graph colouring heuristic).
////void Crossover::insertDays(eoChromosome & _parent1, eoChromosome const& _parent2, boost::shared_ptr<int> _range2) {

////#ifdef _CROSSOVER_DEBUG
////    cout << endl << endl << "Before inserting days" << endl;
////    cout << endl << "# periods = " << _parent1.getNumPeriods() << endl;

////    _parent1.validate();
////#endif

////    // Select a range of days in the first parent.
////    boost::shared_ptr<int> range1 = selectDays(_parent1);
////    // Get periods
////    vector<unordered_set<int> > & periods1 = _parent1.getPeriods();
////    vector<unordered_set<int> > const& periods2 = _parent2.getConstPeriods();
////    int dayA1 = range1.get()[0];       // First day in periods1
////    int dayB1 = _range2.get()[0];      // First day in periods2
////    int dayB2 = _range2.get()[1];      // Last day in periods2
////    int dayA2 = dayA1 + (dayB2-dayB1); // Last day in periods1

////#ifdef _CROSSOVER_DEBUG
////    cout << "range1 = [" << *(range1.get()) << "," << *(range1.get()+1) << "]" << endl;
////    cout << "range2 = [" << *(_range2.get()) << "," << *(_range2.get()+1) << "]" << endl;

////    cout << "dayA1 = " << dayA1 << endl;
////    cout << "dayA2 = " << dayA2 << endl;
////    cout << "dayB1 = " << dayB1 << endl;
////    cout << "dayB2 = " << dayB2 << endl;

////    cout << "dayA2-dayA1+1 = " << (dayA2-dayA1+1) << endl;
////    cout << "dayB2-dayB1+1 = " << (dayB2-dayB1+1) << endl;
////#endif

////    auto it2Beg = periods2.cbegin()+dayB1;
////    auto it2End = periods2.cbegin()+dayB2+1; // past the end iterator
////    // Insert range2 days in the parent1 *before* the selected range1.
////    periods1.insert(periods1.begin()+dayA1, it2Beg, it2End);

////#ifdef _CROSSOVER_DEBUG
////    cout << endl << endl << "After inserting range2 days" << endl;
////    cout << "# periods = " << _parent1.getNumPeriods() << endl;
////#endif

////    // Periods to remove
////    unordered_set<int> periodsToRemove;
////    // ...and remove the afected days, that contain duplicated exams,
////    // while the newly inserted periods are left intact.
////    for (int day2 = dayB1; day2 <= dayB2; ++day2) {
////        unordered_set<int> const& examsToRemove = periods2[day2];
////        // Remove each day from parent1 containing exams in examsToRemove vector
////        for (unordered_set<int>::const_iterator itRem = examsToRemove.begin(); itRem != examsToRemove.end(); ++itRem) {
////            int i;
////            for (i = 0; i < periods1.size(); ++i) {
////                if (i < dayA1 || i > dayA2) { // Newly inserted periods are left intact.
////                    unordered_set<int>& exams = periods1[i];
////                    if (exams.find(*itRem) != exams.end()) {
////                        exams.erase(*itRem); // Erase duplicated exam
////                        // Mark period for removal
////                        periodsToRemove.insert(i); // If the period already exists, don't insert
////                        break;

////                        /// TODO
////                        // exams.clear(); // Hipothese 1: leave period empty
//////                        periods1.erase(periods1.begin()+i); // Hipothese 2: remove period completely
////                    }
////                }
////            }
////            if (i == periods1.size()) {
//////                cout << endl << "numPeriods = " << periods1.size() << endl;
////                cout << endl << "While removing duplicated exams... " << endl;
////                cout << endl << "Couldn't find exam " << *itRem << endl;
////                cin.get();
////            }
////        }
////    }

////#ifdef _CROSSOVER_DEBUG
////    cout << endl << endl << "After marking affected days" << endl;
////    cout << "# periods = " << _parent1.getNumPeriods() << endl;
////    cout << "There are " << periodsToRemove.size() << " periods to remove: " << endl;
////    copy(periodsToRemove.begin(), periodsToRemove.end(), ostream_iterator<int>(cout, " "));
////    cout << endl;
////#endif


////    // Determine the exams to reinsert...
////    unordered_set<int> examsToReinsert;
////    for (unordered_set<int>::iterator itPeriod = periodsToRemove.begin(); itPeriod != periodsToRemove.end(); ++itPeriod) {
////        unordered_set<int> const& periodExams = periods1[*itPeriod];
////        for (unordered_set<int>::const_iterator itExam = periodExams.begin(); itExam != periodExams.end(); ++itExam) {
////            examsToReinsert.insert(*itExam);
////        }
////    }

////#ifdef _CROSSOVER_DEBUG
////    cout << endl << endl << "After determining the exams to reinsert" << endl;
////    cout << "There are " << examsToReinsert.size() << " exams to reinsert: " << endl;
//////    copy(examsToReinsert.begin(), examsToReinsert.end(), ostream_iterator<int>(cout, " "));
////    cout << endl;
////#endif

////    // ...and remove affected periods
//////DO NOT WORK....    for (unordered_set<int>::iterator itPeriod = periodsToRemove.begin(); itPeriod != periodsToRemove.end(); ++itPeriod)
//////        periods1.erase(periods1.begin()+(*itPeriod));

////    ///
////    /// TODO - OPTIMIZE
////    ///

////    vector<unordered_set<int> > newPeriods1;
////    for (int i = 0; i < periods1.size(); ++i) {
////        if (periodsToRemove.find(i) == periodsToRemove.end()) // If time slot 'i' is not to be removed then copy it
////            newPeriods1.push_back(periods1[i]);
////    }
////    periods1.clear();
////    for (int i = 0; i < newPeriods1.size(); ++i)
////        periods1.push_back(newPeriods1[i]);


////#ifdef _CROSSOVER_DEBUG
////    cout << endl << endl << "After removing affected periods" << endl;
////    cout << "# periods = " << _parent1.getNumPeriods() << endl;

////    vector<int> placedExams;
////    for (int i = 0; i < periods1.size(); ++i) {
////        for (unordered_set<int>::const_iterator itExam = periods1[i].begin(); itExam != periods1[i].end(); ++itExam)
////            placedExams.push_back(*itExam);
////    }

////    cout << "# Placed exams " << placedExams.size() << endl;
////    cout << "# Unplaced exams " << examsToReinsert.size() << endl;
////    cout << "# Total exams " << (placedExams.size() + examsToReinsert.size()) << endl;

//////    bool range2CopiedOk = false;

//////    for (int i = 0, day2 = dayB1; i < periods1.size() && day2 <= dayB2; ++i, ++day2) {
//////        unordered_set<int>::const_iterator itExam1 = periods1[i].begin();
//////        unordered_set<int>::const_iterator itExam2 = periods2[day2].begin();
//////        cout << "periods1[" << i << "].size = " << periods1[i].size() << endl;
//////        cout << "periods2[" << day2 << "].size = " << periods2[day2].size() << endl;
//////        copy(periods1[i].begin(), periods1[i].end(), ostream_iterator<int>(cout, " "));
//////        cout << endl;
//////        copy(periods2[day2].begin(), periods2[day2].end(), ostream_iterator<int>(cout, " "));
//////        cout << endl;
//////    }
//////    cout << "Verifying if parent1 exams are equal to range2 exams: " << range2CopiedOk << endl;
////#endif


//////    _parent1.validate();


////    // Then reinsert unplaced exams (e.g. using first fit or graph colouring heuristic).

////    /// TODO: OPTIMIZE, DON'T DO THE COPY TO THE VECTOR...

////    // Determine unplaced exams
////    vector<int> remainderExams;
////    for (unordered_set<int>::const_iterator itExam = examsToReinsert.begin(); itExam != examsToReinsert.end(); ++itExam) {
////        remainderExams.push_back(*itExam);
////    }
////    // Determine fixed exams and respective time slots
////    vector<int> fixedExams;
////    unordered_map<int, int> examsPeriods;
////    for (int p = 0; p < periods1.size(); ++p) {
////        unordered_set<int> const& periodExams = periods1[p];
////        for (unordered_set<int>::const_iterator itExam = periodExams.begin(); itExam != periodExams.end(); ++itExam) {
////            int ei = *itExam;
////            pair<int, int> pairValue(ei, p);
////            examsPeriods.insert(pairValue);
////            fixedExams.push_back(*itExam);
////        }
////    }

////#ifdef _CROSSOVER_DEBUG
////    cout << endl << "# Fixed exams = " << fixedExams.size() << endl;
////    cout << endl << "# examsPeriods = " << examsPeriods.size() << endl;
////#endif

////    Matrix const& conflictMatrix = *_parent1.getConflictMatrix();
////    AdjacencyList const& examsGraph = *_parent1.getGraph();

////    GCHeuristics::SD(conflictMatrix, examsGraph, _parent1, fixedExams, examsPeriods, remainderExams);

////#ifdef _CROSSOVER_DEBUG
////    cout << endl << "After reinserting exams using SD graph colouring heuristic" << endl;
////    cout << endl << "# periods = " << _parent1.getNumPeriods() << endl;

////    cout << endl << "Validating..." << endl;
////#endif

////    _parent1.validate();

////    _parent1.computeProximityCosts();


////#ifdef _CROSSOVER_DEBUG
////    _parent1.validate();

////    cout << endl << "After crossover" << endl;
////    cout << endl << "# periods = " << _parent1.getNumPeriods() << endl;

////    cin.get();
////#endif

////    /*
////    // OLD CODE

////    // The newly inserted day takes the place of a randomly chosen day
////    // which is pushed to the end of the timetable.
//////    int numPeriods = _parent1.getNumPeriods();
////    // Get periods
//////    vector<vector<int> >& periods1 = _parent1.getPeriods();
//////    vector<vector<int> >& periods2 = _parent2.getPeriods();
////    vector<unordered_set<int> >& periods1 = _parent1.getPeriods();
////    vector<unordered_set<int> >& periods2 = _parent2.getPeriods();

////    // Generate random period
//////    int randPeriod = rng.random(numPeriods);
////    int randPeriod = rng.random(periods1.size());


//////    cout << "randPeriod = " << randPeriod << endl;
//////    cout << "periods1 exams before inserting day from periods2" << endl;
//////    int i = 1;
//////    for (vector<vector<int> >::iterator it = periods1.begin(); it != periods1.end(); ++it) {
//////        cout << endl << "Period " << i << endl;
//////        for (vector<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
//////            cout << *examit << " ";
//////        }
//////        cout << endl;
//////        ++i;
//////    }

////    // Push existing day to the end of the timetable
////    periods1.push_back(periods1[randPeriod]);
////    // Insert new day from second parent
////    periods1[randPeriod] = periods2[day2];
////    // Increment number of periods
//////    ++numPeriods;
//////    _parent1.setNumPeriods(numPeriods);

//////    cout << "periods1 exams after inserting day from periods2" << endl;
//////    i = 1;
//////    for (vector<vector<int> >::iterator it = periods1.begin(); it != periods1.end(); ++it) {
//////        cout << endl << "Period " << i << endl;
//////        for (vector<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
//////            cout << *examit << " ";
//////        }
//////        cout << endl;
//////        ++i;
//////    }



////    // To ensure the feasibility of chromosomes after the
////    // crossover, duplicated exams are deleted. These exams are
////    // removed from the original periods, while the newly inserted
////    // periods are left intact.
//////    vector<int> const& examsToRemove = periods2[day2];
////    unordered_set<int> const& examsToRemove = periods2[day2];

//////    cout << "periods2 exams to remove:" << endl;
//////    for (vector<int>::const_iterator examit = examsToRemove.begin(); examit != examsToRemove.end(); ++examit) {
//////        cout << *examit << " ";
//////    }

//////    IsInExamsToRemove pred(examsToRemove);

////    int numRemovedExams = 0;

////    /// TODO: USAR unordored_map<int> PARA GUARDAR EXAMES

////    // Remove each exam in examsToRemove vector
//////    for (vector<int>::const_iterator itRem = examsToRemove.begin(); itRem != examsToRemove.end(); ++itRem) {
////    for (unordered_set<int>::const_iterator itRem = examsToRemove.begin(); itRem != examsToRemove.end(); ++itRem) {

////        int i;
//////        for (i = 0; i < numPeriods; ++i) {
////        for (i = 0; i < periods1.size(); ++i) {
////            if (i != randPeriod) {
//////                vector<int>& exams = periods1[i];
////                unordered_set<int>& exams = periods1[i];
////    //            vector<int>::iterator newEnd = remove_if(exams.begin(), exams.end(), pred);
////    //            numRemovedExams += (exams.end()-newEnd);
////    //            exams.erase(newEnd, exams.end());


////                if (exams.find(*itRem) != exams.end()) {
////                    exams.erase(*itRem);
////                    break;
////                }

//////                vector<int>::iterator newEnd = remove_if(exams.begin(), exams.end(), bind2nd(equal_to<int>(), *itRem));

////                // ESTA LINHA NAO FUNCIONA
//////                unordered_set<int>::iterator newEnd = remove_if(exams.begin(), exams.end(),
//////                                                                     bind2nd(equal_to<int>(), *itRem));

//////                if (newEnd != exams.end()) {
//////                    // Remove exam
//////                    exams.erase(newEnd, exams.end());
//////                    ++numRemovedExams;
//////                    break;
//////                }
//////                int numExamsToRemove = (exams.end()-newEnd);
//////                cout << endl << "numExamsToRemove = " << numExamsToRemove << endl;
//////                cout << endl << "exams before remove" << endl;
//////                copy(exams.begin(), exams.end(), ostream_iterator<int>(cout, " "));
//////                cout << endl;
//////                numRemovedExams += numExamsToRemove;
//////                exams.erase(newEnd, exams.end());
//////                cout << endl << "exams after remove" << endl;
//////                copy(exams.begin(), exams.end(), ostream_iterator<int>(cout, " "));
//////                cout << endl;
////            }
////        }
////        if (i == periods1.size()) {
////            cout << endl << "numPeriods = " << periods1.size() << endl;
////            cout << endl << "Couldn't find exam " << *itRem << endl;
////            cin.get();
////        }
////    }

//////    cout << endl << endl << "numRemovedExams = " << numRemovedExams << endl;
//////    cout << endl << endl << "examsToRemove.size() = " << examsToRemove.size() << endl;

//////    cout << endl << endl << "After inserting day and remove duplicated exams" << endl;

//////    cout << "periods1 exams" << endl;
//////    i = 1;
//////    for (vector<vector<int> >::iterator it = periods1.begin(); it != periods1.end(); ++it) {
//////        cout << endl << "Period " << i << endl;
//////        for (vector<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
//////            cout << *examit << " ";
//////        }
//////        cout << endl;
//////        ++i;
//////    }

////    _parent1.validate();
////*/

////}








////void Crossover::generateOffspring(eoChromosome &_parent1, eoChromosome &_parent2)
////{
////    ////////////////////////////////////////////////////////////////////////////////////////
////    //  We change directly _parent1 to be the new offspring because it's already a copy
////    //  of the original parent.
////    eoChromosome& offspring = _parent1;

////    // Compute the "Best day" of the second parent.
////    int day2 = bestDay(_parent2);
////    // Insert best day in the first parent and remove duplicated exams
////    insertDay(offspring, _parent2, day2);

////    // Actualize proximity costs
////    offspring.computeProximityCosts();

//////    cout << "offspring # periods = " << offspring.getNumPeriods() << endl;


//////    cout << "offspring" << endl;

//////    cout << offspring << endl;


//////    cin.get();

////}



////// Compute the "Best day" to exchange between chromosomes.
////// The best day consist of three periods (we exclude saturdays because
////// saturdays are always clash-free) and is the day with the lowest number
////// of clashes per student.
////int Crossover::bestDay(const eoChromosome &chromosome) {
////    int numPeriods = chromosome.getNumPeriods();

////    // Choose a random period
////    return rng.uniform(numPeriods);

/////*
////    int indexBestDay = 0, numClashesDay = 0, numStudentsDay = 0;
////    double numClashesPerStudent = 0.0, bestNumClashes = 0.0;

//////    for (int i = 0; i < numPeriods; ++i) {

////    // Except last day which has no conflicts
////    for (int i = 0; i < numPeriods-1; ++i) {


////        // Get number of clashes of the current "day" (or period)
////        numClashesDay = chromosome.getNumClashesPeriod(i);
////        // Get number of students of the current "day" (or period)
////        numStudentsDay = chromosome.getNumStudentsPeriod(i);
////        // Compute number of clashes per student
////        numClashesPerStudent = (double)numClashesDay / numStudentsDay;
//////        numClashesPerStudent = numStudentsDay;

//////        cout << "numClashesDay = " << numClashesDay << endl;
//////        cout << "numStudentsDay = " << numStudentsDay << endl;
//////        cout << "numClashesPerStudent = " << numClashesPerStudent << endl;
//////        cin.get();

////        // Actualize best day
////        if (i == 0 || numClashesPerStudent < bestNumClashes) {
////            bestNumClashes = numClashesPerStudent;
////            indexBestDay = i;
////        }
////    }
////    return indexBestDay;
////*/
////}


////void Crossover::insertDay(eoChromosome &_parent1, eoChromosome &_parent2, int day2) {

//////    cout << endl << endl << "Before inserting day" << endl;
////    _parent1.validate();

////    // The newly inserted day takes the place of a randomly chosen day
////    // which is pushed to the end of the timetable.
//////    int numPeriods = _parent1.getNumPeriods();
////    // Get periods
//////    vector<vector<int> >& periods1 = _parent1.getPeriods();
//////    vector<vector<int> >& periods2 = _parent2.getPeriods();
////    vector<unordered_set<int> >& periods1 = _parent1.getPeriods();
////    vector<unordered_set<int> >& periods2 = _parent2.getPeriods();

////    // Generate random period
//////    int randPeriod = rng.random(numPeriods);
////    int randPeriod = rng.random(periods1.size());


//////    cout << "randPeriod = " << randPeriod << endl;
//////    cout << "periods1 exams before inserting day from periods2" << endl;
//////    int i = 1;
//////    for (vector<vector<int> >::iterator it = periods1.begin(); it != periods1.end(); ++it) {
//////        cout << endl << "Period " << i << endl;
//////        for (vector<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
//////            cout << *examit << " ";
//////        }
//////        cout << endl;
//////        ++i;
//////    }

////    // Push existing day to the end of the timetable
////    periods1.push_back(periods1[randPeriod]);
////    // Insert new day from second parent
////    periods1[randPeriod] = periods2[day2];
////    // Increment number of periods
//////    ++numPeriods;
//////    _parent1.setNumPeriods(numPeriods);

//////    cout << "periods1 exams after inserting day from periods2" << endl;
//////    i = 1;
//////    for (vector<vector<int> >::iterator it = periods1.begin(); it != periods1.end(); ++it) {
//////        cout << endl << "Period " << i << endl;
//////        for (vector<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
//////            cout << *examit << " ";
//////        }
//////        cout << endl;
//////        ++i;
//////    }



////    // To ensure the feasibility of chromosomes after the
////    // crossover, duplicated exams are deleted. These exams are
////    // removed from the original periods, while the newly inserted
////    // periods are left intact.
//////    vector<int> const& examsToRemove = periods2[day2];
////    unordered_set<int> const& examsToRemove = periods2[day2];

//////    cout << "periods2 exams to remove:" << endl;
//////    for (vector<int>::const_iterator examit = examsToRemove.begin(); examit != examsToRemove.end(); ++examit) {
//////        cout << *examit << " ";
//////    }

//////    IsInExamsToRemove pred(examsToRemove);

////    int numRemovedExams = 0;

////    /// TODO: USAR unordored_map<INT> PARA GUARDAR EXAMES

////    // Remove each exam in examsToRemove vector
//////    for (vector<int>::const_iterator itRem = examsToRemove.begin(); itRem != examsToRemove.end(); ++itRem) {
////    for (unordered_set<int>::const_iterator itRem = examsToRemove.begin(); itRem != examsToRemove.end(); ++itRem) {

////        int i;
//////        for (i = 0; i < numPeriods; ++i) {
////        for (i = 0; i < periods1.size(); ++i) {
////            if (i != randPeriod) {
//////                vector<int>& exams = periods1[i];
////                unordered_set<int>& exams = periods1[i];
////    //            vector<int>::iterator newEnd = remove_if(exams.begin(), exams.end(), pred);
////    //            numRemovedExams += (exams.end()-newEnd);
////    //            exams.erase(newEnd, exams.end());


////                if (exams.find(*itRem) != exams.end()) {
////                    exams.erase(*itRem);
////                    break;
////                }



//////                vector<int>::iterator newEnd = remove_if(exams.begin(), exams.end(), bind2nd(equal_to<int>(), *itRem));

////                // ESTA LINHA NAO FUNCIONA
//////                unordered_set<int>::iterator newEnd = remove_if(exams.begin(), exams.end(),
//////                                                                     bind2nd(equal_to<int>(), *itRem));

//////                if (newEnd != exams.end()) {
//////                    // Remove exam
//////                    exams.erase(newEnd, exams.end());
//////                    ++numRemovedExams;
//////                    break;
//////                }
//////                int numExamsToRemove = (exams.end()-newEnd);
//////                cout << endl << "numExamsToRemove = " << numExamsToRemove << endl;
//////                cout << endl << "exams before remove" << endl;
//////                copy(exams.begin(), exams.end(), ostream_iterator<int>(cout, " "));
//////                cout << endl;
//////                numRemovedExams += numExamsToRemove;
//////                exams.erase(newEnd, exams.end());
//////                cout << endl << "exams after remove" << endl;
//////                copy(exams.begin(), exams.end(), ostream_iterator<int>(cout, " "));
//////                cout << endl;
////            }
////        }
////        if (i == periods1.size()) {
////            cout << endl << "numPeriods = " << periods1.size() << endl;
////            cout << endl << "Couldn't find exam " << *itRem << endl;
////            cin.get();
////        }
////    }

//////    cout << endl << endl << "numRemovedExams = " << numRemovedExams << endl;
//////    cout << endl << endl << "examsToRemove.size() = " << examsToRemove.size() << endl;

//////    cout << endl << endl << "After inserting day and remove duplicated exams" << endl;

//////    cout << "periods1 exams" << endl;
//////    i = 1;
//////    for (vector<vector<int> >::iterator it = periods1.begin(); it != periods1.end(); ++it) {
//////        cout << endl << "Period " << i << endl;
//////        for (vector<int>::const_iterator examit = (*it).begin(); examit != (*it).end(); ++examit) {
//////            cout << *examit << " ";
//////        }
//////        cout << endl;
//////        ++i;
//////    }

////    _parent1.validate();
////}

///////////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////////////////////////////

////% Day-exchange crossover
////function [timetable1, timetable2] = dayExchangeCrossover(Data, timetable1, timetable2)
////    % In day-exchange crossover, only the best days (excluding
////    % Saturdays, since exams scheduled on Saturdays are always
////    % clash-free) of chromosomes, selected based on the crossover
////    % rate, are eligible for exchange. The best day consists of three
////    % periods and is the day with the lowest number of clashes per student.
////    %
////    % Compute the "Best day" to exchange between chromosomes.
////    day1 = bestDay(Data, timetable1);
////    day2 = bestDay(Data, timetable2);

////    if (~verifyTimetable(Data, timetable1))
////        fprintf('[Before exchangeDays] Timetable 1 is not valid\n');
////        printTimetable(Data, timetable1);
////        pause
////    end
////    if (~verifyTimetable(Data, timetable2))
////        fprintf('[Before exchangeDays] Timetable 2 is not valid\n');
////        printTimetable(data, timetable2);
////        pause
////    end

////    % Exchange best days between the two chromossomes.
////    [timetable1, timetable2] = exchangeDays(Data, timetable1, day1, timetable2, day2);

////    if (~verifyTimetable(Data, timetable1))
////        fprintf('[After exchangeDays] Timetable 1 is not valid\n');
////        printTimetable(data, timetable1 );
////        pause
////    end
////    if (~verifyTimetable(Data, timetable2))
////        fprintf('[After exchangeDays] Timetable 2 is not valid\n');
////        printTimetable(data, timetable2);
////        pause
////    end

////    % Actualize number of clashes
////    timetable1 = computeNumClashes(Data, timetable1);
////    timetable2 = computeNumClashes(Data, timetable2);
////end


////% Compute the "Best day" to exchange between chromosomes.
////% The best day consist of three periods (we exclude saturdays because
////% saturdays are always clash-free) and is the day with the lowest number
////% of clashes per student.
////function bday = bestDay(Data, timetable)
////    NumPeriods = timetable.NumPeriods;
////    clashesPerStudent = zeros(1, NumPeriods);
////    % For each period do:
////    %
////    %   - Get number of clashes in each period divided by
////    %     the number of students in that period.
////    numClashes0 = 0;

////    for p = 1 : NumPeriods
////        numStudents = getNumberStudentsPeriod(Data, timetable, p);
////        if (p > 1)
////            numClashes0 = getNumberClashesPeriod(Data, timetable, p-1);
////        end
////        if (p == NumPeriods)
////            numClashes = numClashes0;
////        else
////            numClashes = numClashes0 + getNumberClashesPeriod(Data, timetable, p);
////        end
////%         examList = timetable.Periods{p};
////%         Data.Classes(examList)
////%         clashesPerStudent(p) = numClashes/numStudents;
////        clashesPerStudent(p) = numClashes;

////        % VER MELHOR MEDIDA

////    end

////    [V, I] = min(clashesPerStudent); % Considering just one period per day
////    bday = I(1);
////end


////% Exchange best days between the two chromossomes.
////function [timetable1, timetable2] = exchangeDays(Data, timetable1, day1, timetable2, day2)
////    % Exchange days between timetables.
////    % The best day and the day after are exchanged -> SEE THIS
////    %
////    % The newly inserted day takes the place of a randomly chosen day
////    % which is pushed to the end of the timetable.
////    periodsToInsertTimetable1Copy = cell(1, 1);
////    periodsToInsertTimetable1Copy{1} = timetable1.Periods{day1};
////    % Insert day2 in timetable1
////    idx1 = randi([1, int32(timetable1.NumPeriods)]);
////    fromPeriodIdx = idx1;
////    periodsToInsert = cell(1, 1);
////    periodsToInsert{1} = timetable2.Periods{day2};
////    timetable1 = insertDay(Data, timetable1, fromPeriodIdx, periodsToInsert);
////    % To ensure the feasibility of chromosomes after the
////    % crossover, duplicated exams are deleted. These exams are
////    % removed from the original periods, while the newly inserted
////    % periods are left intact.
////    timetable1 = removeDuplicatedExams(Data, timetable1, fromPeriodIdx, periodsToInsert);

////    numExams = 0;
////    for j = 1 : timetable1.NumPeriods
////        examList = timetable1.Periods{j};
////        numExams = numExams + length(examList);
////    end
////    if (numExams ~= 80)
////        pause
////    end
////    % Insert day1 in timetable2
////    idx2 = randi([1, int32(timetable2.NumPeriods)]);
////    fromPeriodIdx = idx2;
////    periodsToInsert{1} = periodsToInsertTimetable1Copy{1};
////    timetable2 = insertDay(Data, timetable2, fromPeriodIdx, periodsToInsert);
////    % To ensure the feasibility of chromosomes after the
////    % crossover, duplicated exams are deleted. These exams are
////    % removed from the original periods, while the newly inserted
////    % periods are left intact.
////    timetable2 = removeDuplicatedExams(Data, timetable2, fromPeriodIdx, periodsToInsert);
////    numExams = 0;
////    for j = 1 : timetable2.NumPeriods
////        examList = timetable2.Periods{j};
////        numExams = numExams + length(examList);
////    end

////    if (numExams ~= 80)
////        pause
////    end
////end

////function showClassNames(Data, periodExamList)
////    Data.Classes(periodExamList)
////end

////function timetable = insertDay(Data, timetable, fromPeriodIdx, periodsToInsert)
////    periodsToRemove = cell(1, 1);
////    periodsToRemove{1} = timetable.Periods{fromPeriodIdx};
////    timetable.Periods{fromPeriodIdx} = periodsToInsert{1};
////    % Add two periods to the timetable
////    timetable = addPeriodToTimetable(timetable);
////    timetable.Periods{timetable.NumPeriods} = periodsToRemove{1};
////end

////% To ensure the feasibility of chromosomes after the
////% crossover, duplicated exams are deleted. These exams are
////% removed from the original periods, while the newly inserted
////% periods are left intact.
////function timetable = removeDuplicatedExams(Data, timetable, fromPeriodIdx, periodsToInsert)
////    % Form an exam list with the inserted periods
////    insertedExams = [periodsToInsert{1}];
////    numRepeatedExams = 0;

////    for i = 1 : timetable.NumPeriods
////        % Except the periods just inserted
////         if (i ~= fromPeriodIdx)
////            % Remove duplicated exams
////            examList = timetable.Periods{i};
////            numExams = length(examList);
////            % Find exams from this list which are repeated and remove them
////            exams = [];
////            for j = 1 : numExams
////                I = find(insertedExams == examList(j));
////                if (~isempty(I))
////                    exams = [exams examList(j)];
////                    numRepeatedExams = numRepeatedExams+1;
////                end
////            end
////            % Remove exams
////            if (~isempty(exams))
////                for j = 1 : length(exams)
////                    examList = remove(exams(j), examList);
////                end
////                timetable.Periods{i} = examList;
////            end
////        end
////    end
////    if (numRepeatedExams ~= length(insertedExams))
////        disp('Bug...')
////        pause
////    end
////end

/////////////////////////////////////////////////////////////////////////////////////////////////////

////std::string FlowShopOpCrossoverQuad::className() const
////  {
////    return "FlowShopOpCrossoverQuad";
////  }


////bool FlowShopOpCrossoverQuad::operator()(FlowShop & _flowshop1, FlowShop & _flowshop2)
////{
////  bool oneAtLeastIsModified;
////  // computation of the 2 random points
////  unsigned int point1, point2;
////  do
////    {
////      point1 =  rng.random(std::min(_flowshop1.size(), _flowshop2.size()));
////      point2 =  rng.random(std::min(_flowshop1.size(), _flowshop2.size()));
////    }
////  while (fabs((double) point1-point2) <= 2);
////  // computation of the offspring
////  FlowShop offspring1 = generateOffspring(_flowshop1, _flowshop2, point1, point2);
////  FlowShop offspring2 = generateOffspring(_flowshop2, _flowshop1, point1, point2);
////  // does at least one genotype has been modified ?
////  if ((_flowshop1 != offspring1) || (_flowshop2 != offspring2))
////    {
////      // update
////      _flowshop1.value(offspring1);
////      _flowshop2.value(offspring2);
////      // at least one genotype has been modified
////      oneAtLeastIsModified = true;
////    }
////  else
////    {
////      // no genotype has been modified
////      oneAtLeastIsModified = false;
////    }
////  // return 'true' if at least one genotype has been modified
////  return oneAtLeastIsModified;
////}


////FlowShop FlowShopOpCrossoverQuad::generateOffspring(const FlowShop & _parent1, const FlowShop & _parent2, unsigned int _point1, unsigned int _point2)
////{
////  FlowShop result = _parent1;
////  std::vector<bool> taken_values(result.size(), false);
////  if (_point1 > _point2)
////    std::swap(_point1, _point2);
////  /* first parent */
////  for (unsigned int i=0 ; i<=_point1 ; i++)
////    {
////      // result[i] == _parent1[i]
////      taken_values[_parent1[i]] = true;
////    }
////  for (unsigned int i=_point2 ; i<result.size() ; i++)
////    {
////      // result[i] == _parent1[i]
////      taken_values[_parent1[i]] = true;
////    }
////  /* second parent */
////  unsigned int i = _point1+1;
////  unsigned int j = 0;
////  while (i<_point2 && j<_parent2.size())
////    {
////      if (! taken_values[_parent2[j]])
////        {
////          result[i] = _parent2[j];
////          i++;
////        }
////      j++;
////    }
////  return result;
////}

