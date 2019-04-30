
//#include "Mutation.h"


//#include "ETTPneighborhood.h"
//#include "moSA.h"
//#include "moGDA.h"
//#include "ETTPneighborEval.h"
//#include "ProximityCostEval.h"


//template <typename EOT>
//string Mutation<EOT>::className() const
//{
//    return "ETTP Mutation";
//}

//template <typename EOT>
//bool Mutation<EOT>::operator()(EOT& _chromosome)
//{
////    cout << "Mutation" << endl;

//    bool chromosomeIsModified = true;



////    if (rng.uniform() < 1) { // NOT GOOD

////    if (rng.uniform() < 0) {

////    if (rng.uniform() < 0.1) {

////        if (rng.uniform() < 0.1) {
//            //
//            // Swap two periods
//            //
//            // Get Frog1 periods
//            vector<unordered_set<int> >& chromPeriods = _chromosome.getPeriods();
//            // Generate a random index in frogs
//            int randPeriod1 = rng.random(_chromosome.getNumPeriods());
//            int randPeriod2 = rng.random(_chromosome.getNumPeriods());
//            // Swap period exams
//            auto aux = chromPeriods[randPeriod1];
//            chromPeriods[randPeriod1] = chromPeriods[randPeriod2];
//            chromPeriods[randPeriod2] = aux;

//            _chromosome.computeProximityCosts();
////        }

///*
//        if (rng.uniform() < 0.1) {
//            ETTPKempeChainHeuristic kempe;
//            kempe(_chromosome);
//        }
//*/

////    }
///*
//    // Variation operator using SA

//    ETTPneighborhood neighborhood;
//    // Full evaluation function
//    ProximityCostEval<eoChromosome> fullEval;
//    // Neighbor evaluation function
//    ETTPneighborEval neighEval;

//    moSimpleCoolingSchedule<eoChromosome> cool(0.01, 0.99, 5, 0.001);
//    moSA<ETTPneighbor> sa(neighborhood, fullEval, neighEval, cool);

//    const int UP = 0.01;
//    // GDA
//    moGDA<ETTPneighbor> gda(neighborhood, fullEval, neighEval, UP);

////                cout << "Before GDA - fitness = " << pop[i].fitness() << endl;
////                cout << "GDA" << endl;
////    sa(_chromosome);
////    gda(_chromosome);

//    ETTPKempeChainHeuristic kempe;
//    kempe(_chromosome);
//*/
//    // return 'true' if at least one genotype has been modified
//    return chromosomeIsModified;
//}


