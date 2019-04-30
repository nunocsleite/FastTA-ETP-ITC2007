#ifndef EOEVOLUTIONOPERATOR_H
#define EOEVOLUTIONOPERATOR_H

#include <eoEvalFunc.h>
#include "chromosome/eoChromosome.h"
#include "kempeChain/ETTPKempeChainHeuristic.h"
#include "neighbourhood/ETTPneighborhood.h"
#include "neighbourhood/ETTPneighborEval.h"
//#include "ProximityCostEval.h"
//#include "moSA.h"



////////////////////////////////////////////
// Code Paper ECTA 2013 LNCS
//
///////////////////////////////////////////


////////////////////////////////////////////////////////////////
////
//// eoEvolutionOperator 2
////
////////////////////////////////////////////////////////////////
//class eoSFLAEvolOperator_2 : public eoBinOp<eoChromosome> {

//    typedef eoChromosome POT;

//public:
//    bool operator()(POT& _Pw, POT const& _Pb) {
//        double originalPbFitness = _Pb.fitness();
//        double originalPwFitness = _Pw.fitness();

//        auto newPw = _Pb;

//        cout << endl << endl << "Initial fitness = " << newPw.fitness() << endl;
////        cout << endl << endl << "Best  frog fitness = " << _Pb.fitness() << endl;
////        cout << "Worst frog fitness = " << _Pw.fitness() << endl;



////        if (false) {
//        if (rng.uniform() < 0.1) {
//            vector<unordered_set<int> >& pwPeriods = _Pw.getPeriods();
//            // Get newPw periods
//            vector<unordered_set<int> >& newPwPeriods = newPw.getPeriods();

//            // Generate random number of periods to insert from Pw
//            int randNumPeriods = 3;
////            int randNumPeriods = 1;
//            for (int i = 1; i <= randNumPeriods; ++i) {
//                // Get a random period from Pw
//                int randPwPeriod = rng.random(_Pw.getNumPeriods());
//                // Insert Pw random period exams (those that are feasible) into the new frog and remove duplicated exams

//                // Generate a random index in the new frog
//                int randNewPwPeriod = rng.random(newPw.getNumPeriods());
//                // Pw exams from best day
//                unordered_set<int>& pwExams = pwPeriods[randPwPeriod];

//                // newPw exams from rand period
//                unordered_set<int>& newPwExams = newPwPeriods[randNewPwPeriod];

//                // Insert Pw period into newPw into that random position
//                for (auto it = pwExams.begin(); it != pwExams.end(); ++it) {
//                    // If the exam does not exist and is feasible then insert it
//                    if (newPwExams.find(*it) == newPwExams.end() && newPw.isFeasiblePeriodExam(randNewPwPeriod, *it)) {
////                        if (newPwExams.find(*it) == newPwExams.end() && newPw.isFeasiblePeriodExam(bestDayIdx, *it)) {


//                    newPwExams.insert(*it);
//        //                ++numInsertedExams;
////                        cout << "Insert: " << *it << endl;
////                        cin.get();


//                        /// TODO / SEE POSSIBLE OPTIMIZATIONS

//                        // Remove duplicated exams
//                        for (int i = 0; i < newPwPeriods.size(); ++i) {
//                            if (i != randNewPwPeriod) {
//        //                        if (i != bestDayIdx) {
//                                unordered_set<int>& exams = newPwPeriods[i];
//                                if (exams.find(*it) != exams.end()) {
//                                    exams.erase(*it);
//        //                            ++numRemovedExams;
//        //                            cout << "Remove: " << *it << endl;
//                                    break;
//                                }
//                            }
//                        }

//                    }
//        //            else {
//        //                notInserted.insert(*it);
//        //                ++numNotInsertedExams;
//        //            }

//                }
//            }
//            newPw.computeProximityCosts();

//            cout << "After Pw merging = " << newPw.fitness() << endl;
//        }

////        if (rng.uniform() < 0) {
//        if (rng.uniform() < 0.1) { // LAST
////        if (rng.uniform() < 1) {

//           // Insert exams
//           int randNumPeriods = 1;

//           for (int i = 1; i <= randNumPeriods; ++i) {
//               // Get Frog1 periods
//               vector<unordered_set<int> >& frog1Periods = newPw.getPeriods();
//               // Generate a random index in frogs
//               int randPeriod1 = rng.random(newPw.getNumPeriods());
//               int randPeriod2 = rng.random(newPw.getNumPeriods());
//               // Swap period exams
//               auto aux = frog1Periods[randPeriod1];
//               frog1Periods[randPeriod1] = frog1Periods[randPeriod2];
//               frog1Periods[randPeriod2] = aux;
//           }

//           newPw.computeProximityCosts();

//           cout << "Swap periods - fitness = " << newPw.fitness() << endl;

//       }



////        if (rng.uniform() < 0) {
//        if (rng.uniform() < 0.1) {
////        if (rng.uniform() < 1) {

//            //
//            // Neighborhood
////            ETTPneighborhood neighborhood;

////            ETTPneighborhood<ETTPneighborProximityCost> neighborhood;

//            // Full evaluation function
////            ProximityCostEval<POT> fullEval;
//            // Neighbor evaluation function
////            ETTPneighborEval neighEval;
////            ETTPneighborEval<ETTPneighborProximityCost> neighEval;
//// LAST
//moSimpleCoolingSchedule<eoChromosome> cool2(0.1, 0.00001, 5, 0.0000001); // 35.09 from 51.916 + pw merging 51.78
////                                                                         // Yor83 - 34.79, with pr=0.1 Swap, m=10, N=3


//            //ETTPneighborhood<ETTPneighborPlacedExamsCost> neighborhood;
//            //ETTPneighborEval<ETTPneighborPlacedExamsCost> neighEval;
//            //PlacedExamsCostEval<eoChromosome> fullEval;
//            ETTPneighborhood<ETTPneighborProximityCost> neighborhood;
//            ETTPneighborEval<ETTPneighborProximityCost> neighEval;
//            ProximityCostEval<eoChromosome> fullEval;
//            moSA<ETTPneighborProximityCost> sa2(neighborhood, fullEval, neighEval, cool2);

////            moSA<ETTPneighbor> sa2(neighborhood, fullEval, neighEval, cool2);

//            cout << "Before SA2 - fitness = " << newPw.fitness() << endl;
//            cout << "SA2" << endl;
//            sa2(newPw);
//            cout << "After SA2 - fitness = " << newPw.fitness() << endl << endl << endl;

//        }


//        _Pw = newPw;
//        _Pw.computeProximityCosts();
//        _Pw.validate();

//        return true;
//    }


//};






////////////////////////////////////////////////////////////////
////
//// eoEvolutionOperator 3
////
////////////////////////////////////////////////////////////////
//class eoSFLAEvolOperator_3 : public eoBinOp<eoChromosome> {

//    typedef eoChromosome POT;

//public:
//    bool operator()(POT& _Pw, POT const& _Pb) {
//        double originalPbFitness = _Pb.fitness();
//        double originalPwFitness = _Pw.fitness();

//        auto newPw = _Pb;

//        cout << endl << endl << "Initial fitness = " << newPw.fitness() << endl;
////        cout << endl << endl << "Best  frog fitness = " << _Pb.fitness() << endl;
////        cout << "Worst frog fitness = " << _Pw.fitness() << endl;



//        if (false) {
////        if (rng.uniform() < 0.1) {
//            vector<unordered_set<int> >& pwPeriods = _Pw.getPeriods();
//            // Get newPw periods
//            vector<unordered_set<int> >& newPwPeriods = newPw.getPeriods();

//            // Generate random number of periods to insert from Pw
//            int randNumPeriods = 3;
////            int randNumPeriods = 1;
//            for (int i = 1; i <= randNumPeriods; ++i) {
//                // Get a random period from Pw
//                int randPwPeriod = rng.random(_Pw.getNumPeriods());
//                // Insert Pw random period exams (those that are feasible) into the new frog and remove duplicated exams

//                // Generate a random index in the new frog
//                int randNewPwPeriod = rng.random(newPw.getNumPeriods());
//                // Pw exams from best day
//                unordered_set<int>& pwExams = pwPeriods[randPwPeriod];

//                // newPw exams from rand period
//                unordered_set<int>& newPwExams = newPwPeriods[randNewPwPeriod];

//                // Insert Pw period into newPw into that random position
//                for (auto it = pwExams.begin(); it != pwExams.end(); ++it) {
//                    // If the exam does not exist and is feasible then insert it
//                    if (newPwExams.find(*it) == newPwExams.end() && newPw.isFeasiblePeriodExam(randNewPwPeriod, *it)) {
////                        if (newPwExams.find(*it) == newPwExams.end() && newPw.isFeasiblePeriodExam(bestDayIdx, *it)) {


//                    newPwExams.insert(*it);
//        //                ++numInsertedExams;
////                        cout << "Insert: " << *it << endl;
////                        cin.get();


//                        /// TODO / SEE POSSIBLE OPTIMIZATIONS

//                        // Remove duplicated exams
//                        for (int i = 0; i < newPwPeriods.size(); ++i) {
//                            if (i != randNewPwPeriod) {
//        //                        if (i != bestDayIdx) {
//                                unordered_set<int>& exams = newPwPeriods[i];
//                                if (exams.find(*it) != exams.end()) {
//                                    exams.erase(*it);
//        //                            ++numRemovedExams;
//        //                            cout << "Remove: " << *it << endl;
//                                    break;
//                                }
//                            }
//                        }

//                    }
//        //            else {
//        //                notInserted.insert(*it);
//        //                ++numNotInsertedExams;
//        //            }

//                }
//            }
//            newPw.computeProximityCosts();

//            cout << "After Pw merging = " << newPw.fitness() << endl;
//        }

////        if (rng.uniform() < 0) {
//        if (false) {
////        if (rng.uniform() < 0.1) { // LAST
////        if (rng.uniform() < 1) {

//           // Insert exams
//           int randNumPeriods = 1;

//           for (int i = 1; i <= randNumPeriods; ++i) {
//               // Get Frog1 periods
//               vector<unordered_set<int> >& frog1Periods = newPw.getPeriods();
//               // Generate a random index in frogs
//               int randPeriod1 = rng.random(newPw.getNumPeriods());
//               int randPeriod2 = rng.random(newPw.getNumPeriods());
//               // Swap period exams
//               auto aux = frog1Periods[randPeriod1];
//               frog1Periods[randPeriod1] = frog1Periods[randPeriod2];
//               frog1Periods[randPeriod2] = aux;
//           }

//           newPw.computeProximityCosts();

//           cout << "Swap periods - fitness = " << newPw.fitness() << endl;

//       }


///*
////        if (rng.uniform() < 0) {
////        if (rng.uniform() < 0.1) {
//        if (rng.uniform() < 1) {

//            //
//            // Neighborhood
//            ETTPneighborhood neighborhood;
//            // Full evaluation function
//            ProximityCostEval<POT> fullEval;
//            // Neighbor evaluation function
//            ETTPneighborEval neighEval;
//// LAST
////moSimpleCoolingSchedule<eoChromosome> cool2(0.1, 0.00001, 5, 0.0000001); // 35.09 from 51.916 + pw merging 51.78
////                                                                         // Yor83 - 34.79, with pr=0.1 Swap, m=10, N=3

////            moSimpleCoolingSchedule<eoChromosome> cool2(0.01, 0.01, 10, 0.0001); // Hec 10.05 without random Pb
//            //            moSimpleCoolingSchedule<eoChromosome> cool2(0.01, 0.01, 10, 0.0001); // Hec 10.13 with random Pb

////            moSimpleCoolingSchedule<eoChromosome> cool2(0.01, 0.00001, 1, 0.0001); //  Yor 35.42 with random Pb
////            moSimpleCoolingSchedule<eoChromosome> cool2(0.001, 0.000001, 1, 0.0001); //  Yor  with random Pb


//            moSA<ETTPneighbor> sa(neighborhood, fullEval, neighEval, cool);

//            cout << "Before SA - fitness = " << newPw.fitness() << endl;
//            cout << "SA" << endl;
//            sa(newPw);
//            cout << "After SA - fitness = " << newPw.fitness() << endl << endl << endl;


//        }
//*/

//        _Pw = newPw;
//        _Pw.computeProximityCosts();
////        _Pw.validate();

//        return true;
//    }


//};




#endif // EOEVOLUTIONOPERATOR_H
