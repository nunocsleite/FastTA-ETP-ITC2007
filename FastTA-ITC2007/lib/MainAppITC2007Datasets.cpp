#include <eo>

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "validator/validator.h"

#include "testset/TestSetDescription.h"
#include "testset/ITC2007TestSet.h"
#include "init/ETTPInit.h"


// For counting the # evaluations
#include "eval/eoNumberEvalsCounter.h"
#include "eval/statistics/eoETTPEvalWithStatistics.h"
#include "neighbourhood/statistics/ETTPneighborEvalWithStatistics.h"

#include "neighbourhood/ETTPneighborEval.h"
#include "eval/eoETTPEval.h"
#include "eval/eoNumberEvalsCounter.h"
#include "eval/eoETTPEvalNumberEvalsCounter.h"
#include "neighbourhood/ETTPneighbor.h"
#include "neighbourhood/ETTPneighborhood.h"

#include "algorithms/eo/eoCellularEARing.h"
#include "algorithms/eo/eoCellularEAMatrix.h"
#include "algorithms/eo/Mutation.h"
#include "algorithms/eo/Crossover.h"
#include "algorithms/eo/eoSelectBestOne.h"
#include "algorithms/eo/eoGenerationContinue.h"
#include "algorithms/mo/moSA.h"
#include "algorithms/mo/moSAexplorer.h"

#include "algorithms/eo/eoGenerationContinuePopVector.h"
#include "eoSelectOne.h"
#include "algorithms/eo/eoDeterministicTournamentSelectorPointer.h" // eoDeterministicTournamentSelector using boost::shared_ptr


#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "statistics/ExamMoveStatistics.h"

#include <chrono>
#include <thread>

#include "containers/ConflictBasedStatistics.h"

#include "statistics/optimised/ExamMoveStatisticsOpt.h"
#include "utils/DateTime.h"


using namespace std;


#define MAINAPP_DEBUG


extern int getSANumberEvaluations(double tmax, double r, double k, double tmin);


void generateExamMoveStatistics(const string &_outputDir, const TestSet &_testSet);

void runTA(TestSet const& _testSet, string const& _outputDir,
           moSimpleCoolingSchedule<eoChromosome> &_coolSchedule);

void runSA(TestSet const& _testSet, string const& _outputDir,
           moSimpleCoolingSchedule<eoChromosome> &_coolSchedule);



////////////////////////////////////////////////////////////////////////////////////////////////
//
// Examination Timetabling
//
//
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Based on Miguel Nunes MSc. work
//
////////////////////////////////////////////////////////////////////////////////////////////////
long estimateTotalNumberOfNeighbors(int average_reps, long total_time, TestSet const& _testSet)
{
    const int total_neighbor_operators = 1;
    long computed_neighbors;
    int total_operators = total_neighbor_operators;
    ///
    // Solution initializer
    ETTPInit<eoChromosome> init(_testSet.getTimetableProblemData().get());
    // Generate initial solution
    eoChromosome initialSolution;
    init(initialSolution);
    // # evaluations counter
    eoNumberEvalsCounter numEvalsCounter;
    // eoETTPEval used to evaluate the solutions; receives as argument an
    // eoNumberEvalsCounter for counting neigbour # evaluations
    eoETTPEvalNumberEvalsCounter<eoChromosome> fullEval(numEvalsCounter);
    // Evaluate solution
    fullEval(initialSolution);
    // Simulated Annealing parameters
    boost::shared_ptr<ETTPKempeChainHeuristic<eoChromosome> > kempeChainHeuristic(
                new ETTPKempeChainHeuristic<eoChromosome>());
    ETTPneighborhood<eoChromosome>neighborhood(kempeChainHeuristic);
    // ETTPneighborEval which receives as argument an
    // eoNumberEvalsCounter for counting neigbour # evaluations
    ETTPneighborEvalNumEvalsCounter<eoChromosome> neighEval(numEvalsCounter);
//    moSA<ETTPneighbor<eoChromosome> > sa(neighborhood, fullEval, neighEval, _coolSchedule); //
    ///

    // the selected neighbor after the exploration of the neighborhood
    ETTPneighbor<eoChromosome> selectedNeighbor;
    ///
    // Calibration
    for (int i = 0; i < 10; i++)
    {
        // Init on the first neighbor: supposed to be random solution in the neighborhood
        neighborhood.init(initialSolution, selectedNeighbor);
        // Eval the _solution moved with the neighbor and stock the result in the neighbor
        neighEval(initialSolution, selectedNeighbor);
        //std::cout << "selectedNeighbor.fitness() = " << selectedNeighbor.fitness() << std::endl;
    }
    ///

    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < average_reps; i++)
    {
        // Init on the first neighbor: supposed to be random solution in the neighborhood
        neighborhood.init(initialSolution, selectedNeighbor);
        // Eval the _solution moved with the neighbor and stock the result in the neighbor
        neighEval(initialSolution, selectedNeighbor);
        //std::cout << "selectedNeighbor.fitness() = " << selectedNeighbor.fitness() << std::endl;
    }
    // Stop time
    auto stop = std::chrono::high_resolution_clock::now();

    int computed_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    std::cout << "computed_time: " << computed_time << " ms\n";

    computed_neighbors = (total_time * average_reps * total_operators) / computed_time;
    std::cout << "computed_neighbors: " << computed_neighbors << std::endl;

    return computed_neighbors;
}


double computeRate(double TMax, double TMin, int reps, int exec_time, TestSet const& _testSet) {
    const int Num_Runs_SA_Rate = 10000;
    long comp_neighbours = estimateTotalNumberOfNeighbors(Num_Runs_SA_Rate, exec_time, _testSet);
    double offset = 0.8;

    /////////////////////////////////////
    /// DEBUG
    /////////////////////////////////////
    cout << "TMax = " << TMax << ", TMin = " << TMin << ", reps = " << reps << ", exec_time = " << exec_time << endl;
    cout << "Num_Runs_SA_Rate = " << Num_Runs_SA_Rate << endl;
    cout << "comp_neighbours = " << comp_neighbours << endl;
    cout << "offset = " << offset << endl;
    /////////////////////////////////////

    //////////////////////////////
    // DO NOT CONSIDER
//    /// Dataset 4 - multiply by 2.3
//    /// Dataset 9 - multiply by 1.2
//    offset *= 2.3;
//    offset *= 1.2;
//    //
    //////////////////////////////
//    total_neighbors = (long)(total_neighbors * (1 - offset));
    comp_neighbours = (long)(comp_neighbours * offset);

//    double rate = 1.50e-02;
    double rate = 1e-03;

    double rate_to_sub = 1e-3;
//    double rate_to_sub = 1e-5;

    int depth = 3;
    int total_neighbours;

    /////////////////////////////////////
    /// DEBUG
    /////////////////////////////////////
    cout << "comp_neighbours x offset = " << comp_neighbours << endl;
    cout << "initial rate = " << rate << endl;
    cout << "rate_to_sub = " << rate_to_sub << endl;
    cout << "depth = " << depth << endl;
    /////////////////////////////////////

    while (depth > 0)
    {
        total_neighbours = getSANumberEvaluations(TMax, rate, reps, TMin);
        if (total_neighbours < comp_neighbours)
        {
            if (rate <= rate_to_sub)
            {
                rate_to_sub /= 10;
            }
            rate -= rate_to_sub;
        }
        else
        {
            rate += rate_to_sub;
            --depth;
            rate_to_sub /= 10;
        }
        /////////////////////////////////////
        /// DEBUG
        /////////////////////////////////////
        cout << "total_neighbours = " << total_neighbours << endl;
        cout << "comp_neighbours = " << comp_neighbours << endl;
        cout << "updated rate = " << rate << endl;
        cout << "updated rate_to_sub = " << rate_to_sub << endl;
        cout << "updated depth = " << depth << endl;
        /////////////////////////////////////
    }
    /////////////////////////////////////
    /// DEBUG
    /////////////////////////////////////
    cout << "Final rate = " << rate << endl;
    /////////////////////////////////////
    return rate;
}
////////////////////////////////////////////////////////////////////////////////////////////////



// Generate exam move statistics
void generateExamMoveStatistics(string const& _outputDir, TestSet const& _testSet) {
    ///////////////////////////////////////////////////
    //
    // Generate exam move statistics
    //
    ///////////////////////////////////////////////////
    cout << "///////////////////////////////////////////////////" << endl;
    cout << "//" << endl;
    cout << "// Generate exam move statistics" << endl;
    cout << "//" << endl;
    cout << "///////////////////////////////////////////////////" << endl;
    // Number of thresholds
    int numBins = 10;
    // TA cooling schedule
    //    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.99, 3, 2e-5); // Sch #0
    //      moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.5, 0.001, 5, 2e-5); // Sch #11
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.001, 5, 2e-5); // Sch #1, 40 seg
    //      moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.0001, 5, 2e-5); // Sch #2, 6m30s
    //    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.00001, 5, 2e-5); // Uta 3.13, Sch #3
    //    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.5, 0.00001, 5, 2e-5); // Uta 3..., Sch #4
//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000001, 5, 2e-5); // Uta 3.03, Sch #5
    //    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.0000001, 5, 2e-5); // Sch #6

    // SA cooling schedule
//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.01, 5, 1e-6);

//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.0001, 5, 1e-6);

//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.00001, 5, 1e-6);
/*
    /////////////////////////////////////////////////////////////////////////////////
    ///
    /// Dinamically computed rate
    ///
    //////////////////////////////
    // SA
    //const double TMax = 0.1;
//    const double TMax = 0.01;
//    const double TMin = 1e-6;
    ///////////////////////////////
    //////////////////////////////
    // TA
    const double TMax = 350;
    const double TMin = 1e-6;
    ///////////////////////////////
    const int reps = 5;
    const double exec_time = 276000; // 276 s
//    const double exec_time = 50000; // 50 s
    // Dinamically computed rate
    double rate = computeRate(TMax, TMin, reps, exec_time, _testSet);
    std::cout << "Computed rate = " << rate << std::endl;
    moSimpleCoolingSchedule<eoChromosome> coolSchedule(TMax, rate, reps, TMin);
    /////////////////////////////////////////////////////////////////////////////////
*/

/*
    /////////////////////////////////////////////////////////////////////////////////
    /// SET 1
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6); // SET 1 - 5318 (159 sec.)
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000004, 5, 1e-6); // SET 1 - 5261, 5251, 5251 (205 sec.) 11512931 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000003, 5, 1e-6); //*SET 1 - 4998 (295 sec.) 15350571 evals.
//                                                                                    # evaluations performed = 10287760

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 2
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6); // SET 2  - 415 (3:25) 9210346 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000004, 5, 1e-6); // *SET 2  - 405, 400, 415 (4:30) 11512931 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000003, 5, 1e-6); // SET 2 - 410, 395, 420 (6:20) 15350571 evals.

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 3
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.00001, 5, 1e-6); // SET 3 - 10094 (2:25) 4605176 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6); // SET 3 - 9982, 10080, 9534, 9743 (5:40) 9210346 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000006, 5, 1e-6); // SET 3 - 10359 (3:40) 7675286 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 4, 1e-6); // *SET 3 - 9746, 9838,
                                                                                 // 10436, 10296 (4:15) 7368277 evals.
                                                                                 // Fast SA: 7007457 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.001, 0.000004, 5, 1e-6); // SET 3 - 9996, 10516, 10737 (5:15) 8634696 evals.

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 4
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6); // SET 4 - 13233 (3:06) 9210346 evals.
                                                                                 // SET 4 - 12944 (2:24) 9210346 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000005, 5, 1e-6); // SET 4 - 12506 (3:50) 11512931 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000005, 6, 1e-6); //*SET 4 - 12266, 12950,
                                                                                // 12729 (~4:30) 13815517 evals.
                                                                                // 288 sec.
                                                                                // # evaluations performed = 7904327 (-43% evals)
                                                                                // **SA full eval**
                                                                                // 11822, 12690, 11758 (13:40)
                                                                                // # evaluations performed = 13815517


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 5
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6); // SET 5 - 3504 (6:25) 9210346 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000007, 5, 1e-6); // SET 5 - 3271, 3454 (4:00) 6578816 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000007, 6, 1e-6); // *SET 5 - 3202, 3342,
                                                                                 // 3216, 3629, 3080, 3655 (4:30)
                                                                                 // Max # evals. = 7894579
                                                                                 // # evaluations performed = 7350655, 7421219,
                                                                                 // 7482551, 7362476


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 6
//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6); // SET 6 - 26010  (6:00) 9210346 evals.
//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000007, 5, 1e-6); // SET 6 - 26300 (2:15) 6578816 evals.
                                                                                     // SET 6 - 26050 (2:30) 6578816 evals.
                                                                                     // SET 6 - 26125 (2:25) 6578816 evals.
                                                                                     // SET 6 - 26380 (2:30) 6578816 evals.
//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.005, 0.000005, 5, 1e-6); // SET 6 - 26335 (2:25) 8517196 evals.
                                                                                      // SET 6 - 26085 (2:30) 8517196 evals.

//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.005, 0.000003, 5, 1e-6); // SET 6 - 25805 (4:00-6:00) 14195326 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.002, 0.000003, 5, 1e-6); // * SET 6 - 25720 (5:00) 12668176 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.005, 0.000003, 5, 1e-4); // SET 6 - 26125 (2:50) 6520041 evals.


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 7
//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6); // SET 7 - 4732 (4:15) 9210346 evals.
                                                                                       // SET 7 - 4730 (3:55) 9210346 evals.
                                                                                       // SET 7 - 4596 (4:00) 9210346 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.005, 0.000003, 5, 1e-6); // SET 7 - 4385 (9:00) 14195326 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.005, 0.000004, 5, 1e-6); // SET 7 - 4517, 4327 (5:00) 10646496 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.001, 0.000001, 5, 1e-6); // SET 7 - 4060 (15:00) 34538781 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.001, 0.000001, 2, 1e-6); // SET 7 - 4199, 4529, 4398, 4313 (6:00) 13815513 evals.
                                                                                  // 4285, 4511, 4497
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.001, 0.0000015, 2, 1e-6); // SET 7 - 4439, 4530, 4715 (4:00) 9210343 evals.
                                                                                   // 4385, 4535, 4494, 4523, 4599, 4649
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.001, 0.000003, 5, 1e-6); //* SET 7 - 4354, 4317, 4422,
                                                                                    // 4462 (5:00) 11512931 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.0005, 0.000002, 5, 1e-6); // SET 7 - 4692 (6:40) 15536526 evals.

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 8
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000002, 5, 1e-6);  // SET 8 - 7681 (5:27), 7505 (7:30)
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000003, 5, 1e-6);  // SET 8 - 7808 (6:50)
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000003, 5, 1e-6);  // SET 8 - 7753 (3:00) 15350571 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.001, 0.0000015, 5, 1e-6);  // *SET 8 - 7390, 7473, 7432 (4:40) 23025856 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000002, 5, 1e-6);  // SET 8 - 7735 (6:00) 23025856 evals.

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 9
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000002, 5, 1e-6);  // SET 9 - 1040 (3:00) 28782316 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 5, 1e-6);  //* SET 9 - 985, 994, 1008 (4:15) 46051706 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 6, 1e-6);  // SET 9 - 981, 1002, 989 (5:20) 55262047 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 5, 1e-6);  // SET 9  - 1068 (4:00) 46051706 evals.
                                                                                  // SET 9 - 1025 (5:30) 46051706 evals.
                                                                                  // SET 9 - 1039 (4:00) 46051706 evals.
                                                                                  // SET 9 - 1013 (6:00) 46051706 evals.
    /////////////////////////////////////////////////////////////////////////////////
    /// SET 10
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000002, 5, 1e-6);  // SET 10 - 14023 (3:00) 28782316 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 5, 1e-6);  // *SET 10 - 13838, 13486, 13595 (4:50) 46051706 evals.


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 11
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6);  // SET 11 - 29426 (9:15) 9210346 evals.
                                                                                    // SET 11 - 30382 (9:15) 9210346 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000008, 5, 1e-6);  // SET 11 - 29421 (5:40) 5756466 evals.
                                                                                    // SET 11 - 30659 (5:20) 5756466 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000009, 5, 1e-6);  // *SET 11 - 31265 (4:15) 5116861 evals.
                                                                                    //          30711 (4:30) 5116861 evals.
                                                                                    //          30308 (231 sec.) 5116861 evals.

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000009, 6, 1e-6);  // SET 11 - 31563, 30323 (280 sec.) 6140233 evals.


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 12
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000002, 5, 1e-6);  // SET 12 - 5209 (3:30) 28782316 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 5, 1e-6);  // SET 12 - 5156, 5272, 5149 (5:40) 46051706 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.0000015, 5, 1e-6);  // SET 12 - 5229, 5175, 5187 (3:40) 30701136 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.0000013, 5, 1e-6);  //*SET 12 - 5172, 5227, 5276 (4:15) 35424391 evals.


//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 5, 1e-6); // VERY GOOD, SLOW FOR SETS 3, 5, AND 11

    /////////////////////////////////////////////////////////////////////////////////

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 5, 1e-6); // TOO SLOW

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.00001, 5, 1e-7);
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.0001, 5, 1e-7);
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.00001, 5, 1e-7);

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.00001, 5, 1e-6);

    ///////////
    /// FAST SA Optimised
    ///
//    ExamMoveStatistics examMoveStats(_testSet, _outputDir, numBins, coolSchedule);

    ExamMoveStatisticsOpt examMoveStats(_testSet, _outputDir, numBins, coolSchedule);
    examMoveStats.run();
    ///////////


    ///////////
    /// SA
    ///
//    runSA(_testSet, _outputDir, coolSchedule);

    ///////////
*/

    ///////////
    /// FAST TA
    ///
    /// SEE ExamMoveStatisticsOpt's defines:
    ///
    /// #define HIGH_DEGREE_EXAM_INDEX_PERCENTAGE 0.80 // FastTA80
    /// #define HIGH_DEGREE_EXAM_INDEX_PERCENTAGE 1  // FastTA100
    ///
    ///

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 1
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000003, 5, 1e-6); //SA *SET 1 - 4998 (295 sec.) 15350571 evals.
//                                                                                    # evaluations performed = 10287760

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.000004, 5, 2e-4); //SET 1 - 5083 (211 sec.) 16402956 evals.
                                                                                // performed: 9348790 (-43%)

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.000004, 5, 1e-6); //*SET 1 - 5087 (279 sec.) 23025856 evals.
                                                                                // performed: 12190657 (-43%)
                                                                                // 4919 (268 sec.), 4836 (271)


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 2
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000004, 5, 1e-6); // SA *SET 2  - 405, 400, 415 (4:30) 11512931 evals.

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.000004, 5, 1e-6); // SET 2 - 390 (464 sec.)
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.000006, 5, 1e-6); // SET 2 - 390 (310 sec.)
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.000007, 5, 1e-6); // *SET 2 - 400, 395 (274 sec., 262 sec.)


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 3
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000005, 4, 1e-6); // SA *SET 3 - 9746, 9838,
                                                                                 // 10436, 10296 (4:15) 7368277 evals.

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.000006, 5, 1e-6); // SET 3 - 9475 (395 sec.)
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.000008, 5, 1e-6); // SET 3 - 9988 (300 sec.)
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(70, 0.000008, 5, 3e-6); // SET 3 - 9734, 9855 (274, 289 sec.)
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.000009, 5, 1e-6); // *SET 3 - 10112, 9691  (266, 272 sec.)


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 4
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000005, 6, 1e-6); // SA *SET 4 - 12266, 12950,

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(350, 0.0000045, 6, 1e-6); //*SET 4 - 12501, 11971 (261, 252 sec.) 26231263 evals.
//                                                                                 -> performed: 8623269 (-67%), 8419631

    /////////////////////////////////////////////////////////////////////////////////
    /// Generate paper figures
    /// TA with statistics
    /// SET 4
    // Light cool schedule
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(1000, 0.00001, 5, 1e-5); //+SET 4 - 11653, 12898 (312, 316 sec.) 9210346 evals.

    // Intensive cool schedule
    moSimpleCoolingSchedule<eoChromosome> coolSchedule(1000, 0.0000045, 5, 1e-5); //+SET 4 - 13056, 12356 (720, 708 sec.) 20467426 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(350, 0.0000045, 6, 1e-6); //*SET 4 - 12501, 11971 (261, 252 sec.) 26231263 evals.


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 5
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000007, 6, 1e-6); // SA *SET 5 - 3202, 3342,
                                                                                 // 3216, 3629, 3080, 3655 (4:30)

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.000011, 5, 1e-6); // *SET 5 - 3402, 3626, 3563 (263, 270, 263  sec.)  8872411 evals.
                                                                                 // -> performed: 8170261 (-%),

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 6
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.002, 0.000003, 5, 1e-6); // SA *SET 6 - 25720 (5:00) 12668176 evals.
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.000006, 5, 1e-6); //  *SET 6 - 26160, 26010, 25985, 26020 (257, 248, 232, 272 sec.) 16266081 evals.
                                                                                // -> performed: 13463673 (-17%),

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(30000, 0.000006, 5, 1e-6); //  SET 6 - 26280 (296 sec.)  evals.
                                                                                /// DETERMINE OPTIMAL INITIAL THRESH BASED ON THE SOLUTIONS
                                                                                /// FITNESS DIFFERENCE

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 7
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.001, 0.000003, 5, 1e-6); // SA *SET 7 - 4354, 4317, 4422,
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.000008, 5, 2e-6); // *SET 7 - 4683, 4343, 4653 (274, 273, 267 sec.) 11766346 evals.
                                                                                // -> performed: 11066997 (-6%)

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 8
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.001, 0.0000015, 5, 1e-6);  // SA *SET 8 - 7390, 7473, 7432 (4:40) 23025856 evals.

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.000003, 5, 3e-4); // *SET 8 - 7491, 7692, 7710 (261, 268, 266 sec.) 23025856 evals.
                                                                                    // -> performed: 17914906 (-22%)

    /////////////////////////////////////////////////////////////////////////////////
    /// SET 9
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 5, 1e-6);  // SA *SET 9 - 985, 994, 1008 (4:15) 46051706 evals.

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.0000015, 5, 3e-4); // *SET 9 - 1076, 1003, 939 (258, 252, 264 sec.) 46051706 evals.
                                                                                        // -> performed: 34910030 (-24%)



    /////////////////////////////////////////////////////////////////////////////////
    /// SET 10
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 5, 1e-6);  // SA *SET 10 - 13838, 13486, 13595 (4:50) CHANGED -> 46051706 evals.


//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.0000018, 5, 3e-4); // SET 10 - 13639 (237 sec.) 38376421 evals.
//                                                                                        // -> performed: 33592358 (-12%)

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.0000016, 5, 3e-4); // *SET 10 - 13647, 14012 (268, 266 sec.) 43173476 evals.
                                                                                        // -> performed: 38527959 (-11%)


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 11
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000009, 5, 1e-6);  // SA *SET 11 - 31265 (4:15) 5116861 evals.

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.000012, 5, 3e-4); // *SET 11 - 31061, 30865 (241, 234 sec.) 5756466 evals.
//                                                                                             -> performed: 4981578 (-13%)


    /////////////////////////////////////////////////////////////////////////////////
    /// SET 12
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.0000013, 5, 1e-6);  // SA *SET 12 - 5172, 5227, 5276 (4:15) 35424391 evals.

//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(300, 0.0000018, 5, 3e-4); // *SET 12 - 5256, (242, sec.) 38376421 evals.
                                                                                                // -> performed: 32056577 (-16%)



    ///////////
    /// FAST TA with statistics
    ///
    ExamMoveStatistics examMoveStats(_testSet, _outputDir, numBins, coolSchedule);
    examMoveStats.run();

    ///////////
    /// FAST TA with statistics - Optimised
    ///
//    ExamMoveStatisticsOpt examMoveStats(_testSet, _outputDir, numBins, coolSchedule);
//    examMoveStats.run();
    ///////////

    ///////////
    /// TA
    ///
//    runTA(_testSet, _outputDir, coolSchedule);

    ///////////



    ///////////
    /// SA
    ///
//    runSA(_testSet, _outputDir, coolSchedule);

    ///////////





//    // Creating the output file containing threshold information
//    stringstream sstream;
//    sstream << _outputDir << "/ExamMoveStatistics_" << _testSet.getName() << "_cool_"
//            << coolSchedule.initT << "_" << coolSchedule.alpha << "_"
//            << coolSchedule.span << "_" << coolSchedule.finalT << "_Thresholds.txt";
//    string filename;
//    sstream >> filename;
//    ofstream thresholdFile(filename);
//    // Export to file
//    thresholdFile << examMoveStats;

//    // Creating the output file containing information of move counts in each threshold per exam
//    stringstream sstream2;
//    sstream2 << _outputDir << "/ExamMoveStatistics_" << _testSet.getName() << "_cool_"
//            << coolSchedule.initT << "_" << coolSchedule.alpha << "_"
//            << coolSchedule.span << "_" << coolSchedule.finalT << "_MoveCountsPerExam.txt";
//    string filename2;
//    sstream2 >> filename2;
//    ofstream moveCountsPerExamFile(filename2);
//    // Export to file
//    examMoveStats.printExamMoveCountInfo(moveCountsPerExamFile);
//    //    moveCountsPerExamFile << examMoveStats;

    cout << "END" << endl;
}




void runTA(TestSet const& _testSet, string const& _outputDir,
           moSimpleCoolingSchedule<eoChromosome> &_coolSchedule) {

    // Creating the output filename
    stringstream sstream;
    sstream << _outputDir << "/TA_" << _testSet.getName() << "_cool_"
            << _coolSchedule.initT << "_" << _coolSchedule.alpha << "_"
            << _coolSchedule.span << "_" << _coolSchedule.finalT << ".txt";
    string outFilename;
    sstream >> outFilename;
    std::cout << outFilename << std::endl;
    // Output file
    std::ofstream outFile(outFilename);
    ///////////////////////////////////////////////////////////
    long maxNumEval;
    double tmax = _coolSchedule.initT, r = _coolSchedule.alpha,
           k = _coolSchedule.span, tmin = _coolSchedule.finalT;
    // max # evaluations
    maxNumEval = getSANumberEvaluations(tmax, r, k, tmin);
    std::cout << "numberEvaluations = " << maxNumEval << std::endl;
    // Print max # evaluations to file
    outFile << "numberEvaluations = " << maxNumEval << std::endl;
    ///////////////////////////////////////////////////////////
    // Solution initializer
    ETTPInit<eoChromosome> init(_testSet.getTimetableProblemData().get());
    // Generate initial solution
    eoChromosome initialSolution;
    init(initialSolution);
    // # evaluations counter
    eoNumberEvalsCounter numEvalsCounter;
    // eoETTPEval used to evaluate the solutions; receives as argument an
    // eoNumberEvalsCounter for counting neigbour # evaluations
    eoETTPEvalNumberEvalsCounter<eoChromosome> fullEval(numEvalsCounter);
    // Evaluate solution
    fullEval(initialSolution);

    //
    // Local search used: Threshold Accepting algorithm
    //
    // moTA parameters
    boost::shared_ptr<ETTPKempeChainHeuristic<eoChromosome> > kempeChainHeuristic(
                new ETTPKempeChainHeuristic<eoChromosome>());
    ETTPneighborhood<eoChromosome>neighborhood(kempeChainHeuristic);
    // ETTPneighborEval which receives as argument an
    // eoNumberEvalsCounter for counting neigbour # evaluations
    ETTPneighborEvalNumEvalsCounter<eoChromosome> neighEval(numEvalsCounter);

    moTA<ETTPneighbor<eoChromosome> > ta(neighborhood, fullEval, neighEval, _coolSchedule);

    /////// Write to output File ///////////////////////////////////////////
    cout << "Start Date/Time = " << currentDateTime() << endl;
    // Write Start time and algorithm parameters to file
    outFile << "Start Date/Time = " << currentDateTime() << endl;
    outFile << "SA parameters:" << endl;
    outFile << "cooling schedule: " << _coolSchedule.initT << ", " << _coolSchedule.alpha << ", "
            << _coolSchedule.span << ", " << _coolSchedule.finalT << endl;
    outFile << _testSet << std::endl;

    /////////////////////////////////////////
    // Get current time
    time_t now;
    double seconds = 0.0;
    time(&now);  // get current time; same as: now = time(NULL)
    /////////////////////////////////////////

    cout << "Before TA - initialSolution.fitness() = " << initialSolution.fitness() << endl;

    // Apply TA to the solution
    ta(initialSolution);

    // Validate solution
//    initialSolution.validate();

    cout << "After TA - initialSolution.fitness() = " << initialSolution.fitness() << endl;

    // Write best solution to file
    outFile << "==============================================================" << endl;
    outFile << "Date/Time = " << currentDateTime() << endl;
    // Print solution fitness
    outFile << "Solution fitness = " << initialSolution.fitness() << endl;
    // Print real # evaluations performed
    std::cout << "# evaluations performed = " << numEvalsCounter.getTotalNumEvals() << std::endl;
    outFile << "# evaluations performed = " << numEvalsCounter.getTotalNumEvals() << endl;
    // Print solution timetable to file
    outFile << initialSolution << endl;
    outFile << "==============================================================" << endl;
    /////////////////////////////////////////
    // Get current time
    time_t final;
    time(&final);  // get current time
    // Get difference in seconds
    seconds = difftime(final, now);
    /////////////////////////////////////////
    cout << "End Date/Time = " << currentDateTime() << endl;
    cout << "Seconds elapsed = " << seconds << endl;
    // Write to file
    outFile << "End Date/Time = " << currentDateTime() << endl;
    outFile << "Seconds elapsed = " << seconds << endl;
}
////////////////////////////////////////////////////////////////////////////////////////////////




void runSA(TestSet const& _testSet, string const& _outputDir,
           moSimpleCoolingSchedule<eoChromosome> &_coolSchedule) {

    // Creating the output filename
    stringstream sstream;
    sstream << _outputDir << "/SA_" << _testSet.getName() << "_cool_"
            << _coolSchedule.initT << "_" << _coolSchedule.alpha << "_"
            << _coolSchedule.span << "_" << _coolSchedule.finalT << ".txt";
    string outFilename;
    sstream >> outFilename;
    std::cout << outFilename << std::endl;
    // Output file
    std::ofstream outFile(outFilename);
    ///////////////////////////////////////////////////////////
    long maxNumEval;
    double tmax = _coolSchedule.initT, r = _coolSchedule.alpha,
           k = _coolSchedule.span, tmin = _coolSchedule.finalT;
    // max # evaluations
    maxNumEval = getSANumberEvaluations(tmax, r, k, tmin);
    std::cout << "numberEvaluations = " << maxNumEval << std::endl;
    // Print max # evaluations to file
    outFile << "numberEvaluations = " << maxNumEval << std::endl;
    ///////////////////////////////////////////////////////////
    // Solution initializer
    ETTPInit<eoChromosome> init(_testSet.getTimetableProblemData().get());
    // Generate initial solution
    eoChromosome initialSolution;
    init(initialSolution);
    // # evaluations counter
    eoNumberEvalsCounter numEvalsCounter;
    // eoETTPEval used to evaluate the solutions; receives as argument an
    // eoNumberEvalsCounter for counting neigbour # evaluations
    eoETTPEvalNumberEvalsCounter<eoChromosome> fullEval(numEvalsCounter);
    // Evaluate solution
    fullEval(initialSolution);

    //
    // Local search used: Threshold Accepting algorithm
    //
    // moSA parameters
    boost::shared_ptr<ETTPKempeChainHeuristic<eoChromosome> > kempeChainHeuristic(
                new ETTPKempeChainHeuristic<eoChromosome>());
    ETTPneighborhood<eoChromosome>neighborhood(kempeChainHeuristic);
    // ETTPneighborEval which receives as argument an
    // eoNumberEvalsCounter for counting neigbour # evaluations
    ETTPneighborEvalNumEvalsCounter<eoChromosome> neighEval(numEvalsCounter);

    moSA<ETTPneighbor<eoChromosome> > sa(neighborhood, fullEval, neighEval, _coolSchedule);

    /////// Write to output File ///////////////////////////////////////////
    cout << "Start Date/Time = " << currentDateTime() << endl;
    // Write Start time and algorithm parameters to file
    outFile << "Start Date/Time = " << currentDateTime() << endl;
    outFile << "SA parameters:" << endl;
    outFile << "cooling schedule: " << _coolSchedule.initT << ", " << _coolSchedule.alpha << ", "
            << _coolSchedule.span << ", " << _coolSchedule.finalT << endl;
    outFile << _testSet << std::endl;

    /////////////////////////////////////////
    // Get current time
    time_t now;
    double seconds = 0.0;
    time(&now);  // get current time; same as: now = time(NULL)
    /////////////////////////////////////////

    cout << "Before SA - initialSolution.fitness() = " << initialSolution.fitness() << endl;

    // Apply TA to the solution
    sa(initialSolution);

    // Validate solution
//    initialSolution.validate();

    cout << "After SA - initialSolution.fitness() = " << initialSolution.fitness() << endl;

    // Write best solution to file
    outFile << "==============================================================" << endl;
    outFile << "Date/Time = " << currentDateTime() << endl;
    // Print solution fitness
    outFile << "Solution fitness = " << initialSolution.fitness() << endl;
    // Print real # evaluations performed
    std::cout << "# evaluations performed = " << numEvalsCounter.getTotalNumEvals() << std::endl;
    outFile << "# evaluations performed = " << numEvalsCounter.getTotalNumEvals() << endl;
    // Print solution timetable to file
    outFile << initialSolution << endl;
    outFile << "==============================================================" << endl;
    /////////////////////////////////////////
    // Get current time
    time_t final;
    time(&final);  // get current time
    // Get difference in seconds
    seconds = difftime(final, now);
    /////////////////////////////////////////
    cout << "End Date/Time = " << currentDateTime() << endl;
    cout << "Seconds elapsed = " << seconds << endl;
    // Write to file
    outFile << "End Date/Time = " << currentDateTime() << endl;
    outFile << "Seconds elapsed = " << seconds << endl;
}
////////////////////////////////////////////////////////////////////////////////////////////////








////////////////////////////////////////////////////////////////////////////////////////////////
//
// Curriculum Based Course Timetabling
//
//
////////////////////////////////////////////////////////////////////////////////////////////////

void runCBTT(int _datasetIndex, string const& _testBenchmarksDir, string const& _outputDir) {

    std::srand(std::time(0)); // Seed random generator used in random_shuffle

//    cout << endl << "Start Date/Time = " << currentDateTime() << endl;
    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    vector<TestSetDescription> itc2007Benchmarks;
    itc2007Benchmarks.push_back(TestSetDescription("comp01.ctt", "ITC2007 Curriculum based TTP Track Dataset 1"));
    itc2007Benchmarks.push_back(TestSetDescription("comp02.ctt", "ITC2007 Curriculum based TTP Track Dataset 2"));
    itc2007Benchmarks.push_back(TestSetDescription("comp03.ctt", "ITC2007 Curriculum based TTP Track Dataset 3"));
    itc2007Benchmarks.push_back(TestSetDescription("comp04.ctt", "ITC2007 Curriculum based TTP Track Dataset 4"));
    itc2007Benchmarks.push_back(TestSetDescription("comp05.ctt", "ITC2007 Curriculum based TTP Track Dataset 5"));
    itc2007Benchmarks.push_back(TestSetDescription("comp06.ctt", "ITC2007 Curriculum based TTP Track Dataset 6"));
    itc2007Benchmarks.push_back(TestSetDescription("comp07.ctt", "ITC2007 Curriculum based TTP Track Dataset 7"));
    itc2007Benchmarks.push_back(TestSetDescription("comp08.ctt", "ITC2007 Curriculum based TTP Track Dataset 8"));
    itc2007Benchmarks.push_back(TestSetDescription("comp09.ctt", "ITC2007 Curriculum based TTP Track Dataset 9"));
    itc2007Benchmarks.push_back(TestSetDescription("comp10.ctt", "ITC2007 Curriculum based TTP Track Dataset 10"));
    itc2007Benchmarks.push_back(TestSetDescription("comp11.ctt", "ITC2007 Curriculum based TTP Track Dataset 11"));
    itc2007Benchmarks.push_back(TestSetDescription("comp12.ctt", "ITC2007 Curriculum based TTP Track Dataset 12"));
    itc2007Benchmarks.push_back(TestSetDescription("comp13.ctt", "ITC2007 Curriculum based TTP Track Dataset 13"));
    itc2007Benchmarks.push_back(TestSetDescription("comp14.ctt", "ITC2007 Curriculum based TTP Track Dataset 14"));
    itc2007Benchmarks.push_back(TestSetDescription("comp15.ctt", "ITC2007 Curriculum based TTP Track Dataset 15"));
    itc2007Benchmarks.push_back(TestSetDescription("comp16.ctt", "ITC2007 Curriculum based TTP Track Dataset 16"));
    itc2007Benchmarks.push_back(TestSetDescription("comp17.ctt", "ITC2007 Curriculum based TTP Track Dataset 17"));
    itc2007Benchmarks.push_back(TestSetDescription("comp18.ctt", "ITC2007 Curriculum based TTP Track Dataset 18"));
    itc2007Benchmarks.push_back(TestSetDescription("comp19.ctt", "ITC2007 Curriculum based TTP Track Dataset 19"));
    itc2007Benchmarks.push_back(TestSetDescription("comp20.ctt", "ITC2007 Curriculum based TTP Track Dataset 20"));
    itc2007Benchmarks.push_back(TestSetDescription("comp21.ctt", "ITC2007 Curriculum based TTP Track Dataset 21"));
    /////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef MAINAPP_DEBUG
//    copy(itc2007Benchmarks.begin(), itc2007Benchmarks.end(), ostream_iterator<TestSetDescription>(cout, "\n"));
#endif
    // Iterator to test set
    vector<TestSetDescription>::iterator it = itc2007Benchmarks.begin() + _datasetIndex;
//    // Create TestSet instance
//    ITC2007TestSet testSet((*it).getName(), (*it).getDescription(), _testBenchmarksDir);
//    // Load dataset
//    ITC2007TestSet* ptr = &testSet;
//    ptr->load();
//#ifdef MAINAPP_DEBUG
//    // Print testset info
//    cout << testSet << endl;
//    // Print timetable problem data
//    cout << *(ptr->getTimetableProblemData().get()) << endl;
//#endif

    std::cout << "Instance name: " << (*it).getName() << std::endl;

    validator::Faculty input(_testBenchmarksDir + "/" + (*it).getName());

    std::cout << "Name: "          << input.Name()          << std::endl;
    std::cout << "Courses: "       << input.Courses()       << std::endl;
    std::cout << "Rooms: "         << input.Rooms()         << std::endl;
    std::cout << "Curricula: "     << input.Curricula()     << std::endl;
    std::cout << "Periods: "       << input.Periods()       << std::endl;
    std::cout << "PeriodsPerDay: " << input.PeriodsPerDay() << std::endl;
    std::cout << "Days: "          << input.Days()          << std::endl;


//      Faculty input(argv[1]);
//      Timetable output(input, argv[2]);
//      Validator validator(input, output);

//      validator.PrintViolations(std::cout);
//      std::cout << std::endl;
//      validator.PrintCosts(std::cout);
//      std::cout << std::endl;
//      if (output.Warnings() > 0)
//        std::cout << "There are " << output.Warnings() << " warnings!" << std::endl;
//      std::cout << "Summary: ";
//      validator.PrintTotalCost(std::cout);

}




// Fast TA
void fastTA(string const& _outputDir, TestSet const& _testSet) {

    // Number of threshold bins
    int numBins = 10;


}

////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////
/// OLD CODE - TO REMOVE




////////////////////////////////////////////////////////
// ITC 2007 benchmarks
//
//
void runITC2007Datasets(int _datasetIndex, string const& _testBenchmarksDir, string const& _outputDir) {

    std::srand(std::time(0)); // Seed random generator used in random_shuffle

//    cout << endl << "Start Date/Time = " << currentDateTime() << endl;
    // Start time
    auto start = std::chrono::high_resolution_clock::now();

//    vector<TestSetDescription> itc2007Benchmarks;
//    itc2007Benchmarks.push_back(TestSetDescription("comp01.ctt", "ITC2007 Curriculum based TTP Track Dataset 1"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp02.ctt", "ITC2007 Curriculum based TTP Track Dataset 2"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp03.ctt", "ITC2007 Curriculum based TTP Track Dataset 3"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp04.ctt", "ITC2007 Curriculum based TTP Track Dataset 4"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp05.ctt", "ITC2007 Curriculum based TTP Track Dataset 5"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp06.ctt", "ITC2007 Curriculum based TTP Track Dataset 6"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp07.ctt", "ITC2007 Curriculum based TTP Track Dataset 7"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp08.ctt", "ITC2007 Curriculum based TTP Track Dataset 8"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp09.ctt", "ITC2007 Curriculum based TTP Track Dataset 9"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp10.ctt", "ITC2007 Curriculum based TTP Track Dataset 10"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp11.ctt", "ITC2007 Curriculum based TTP Track Dataset 11"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp12.ctt", "ITC2007 Curriculum based TTP Track Dataset 12"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp13.ctt", "ITC2007 Curriculum based TTP Track Dataset 13"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp14.ctt", "ITC2007 Curriculum based TTP Track Dataset 14"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp15.ctt", "ITC2007 Curriculum based TTP Track Dataset 15"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp16.ctt", "ITC2007 Curriculum based TTP Track Dataset 16"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp17.ctt", "ITC2007 Curriculum based TTP Track Dataset 17"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp18.ctt", "ITC2007 Curriculum based TTP Track Dataset 18"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp19.ctt", "ITC2007 Curriculum based TTP Track Dataset 19"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp20.ctt", "ITC2007 Curriculum based TTP Track Dataset 20"));
//    itc2007Benchmarks.push_back(TestSetDescription("comp21.ctt", "ITC2007 Curriculum based TTP Track Dataset 21"));
    /////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef MAINAPP_DEBUG
//    copy(itc2007Benchmarks.begin(), itc2007Benchmarks.end(), ostream_iterator<TestSetDescription>(cout, "\n"));
#endif
    // Iterator to test set
//    vector<TestSetDescription>::iterator it = itc2007Benchmarks.begin() + _datasetIndex;
//    // Create TestSet instance
//    ITC2007TestSet testSet((*it).getName(), (*it).getDescription(), _testBenchmarksDir);
//    // Load dataset
//    ITC2007TestSet* ptr = &testSet;
//    ptr->load();
//#ifdef MAINAPP_DEBUG
//    // Print testset info
//    cout << testSet << endl;
//    // Print timetable problem data
//    cout << *(ptr->getTimetableProblemData().get()) << endl;
//#endif

//    std::cout << "Instance name: " << (*it).getName() << std::endl;

//    validator::Faculty input(_testBenchmarksDir + "/" + (*it).getName());

//    std::cout << "Name: "          << input.Name()          << std::endl;
//    std::cout << "Courses: "       << input.Courses()       << std::endl;
//    std::cout << "Rooms: "         << input.Rooms()         << std::endl;
//    std::cout << "Curricula: "     << input.Curricula()     << std::endl;
//    std::cout << "Periods: "       << input.Periods()       << std::endl;
//    std::cout << "PeriodsPerDay: " << input.PeriodsPerDay() << std::endl;
//    std::cout << "Days: "          << input.Days()          << std::endl;


//      Faculty input(argv[1]);
//      Timetable output(input, argv[2]);
//      Validator validator(input, output);

//      validator.PrintViolations(std::cout);
//      std::cout << std::endl;
//      validator.PrintCosts(std::cout);
//      std::cout << std::endl;
//      if (output.Warnings() > 0)
//        std::cout << "There are " << output.Warnings() << " warnings!" << std::endl;
//      std::cout << "Summary: ";
//      validator.PrintTotalCost(std::cout);

//      return 0;
//    }


//    std::srand(std::time(0)); // Seed random generator used in random_shuffle


////    cout << endl << "Start Date/Time = " << currentDateTime() << endl;
//    // Start time
//    auto start = std::chrono::high_resolution_clock::now();

    vector<TestSetDescription> itc2007Benchmarks;
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set1.exam", "ITC2007 Examination Track Dataset 1"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set2.exam", "ITC2007 Examination Track Dataset 2"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set3.exam", "ITC2007 Examination Track Dataset 3"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set4.exam", "ITC2007 Examination Track Dataset 4"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set5.exam", "ITC2007 Examination Track Dataset 5"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set6.exam", "ITC2007 Examination Track Dataset 6"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set7.exam", "ITC2007 Examination Track Dataset 7"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set8.exam", "ITC2007 Examination Track Dataset 8"));
    // Hidden datasets
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set9.exam",  "ITC2007 Examination Track Dataset 9"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set10.exam", "ITC2007 Examination Track Dataset 10"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set11.exam", "ITC2007 Examination Track Dataset 11"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set12.exam", "ITC2007 Examination Track Dataset 12"));
    /////////////////////////////////////////////////////////////////////////////////////////////////
//#ifdef MAINAPP_DEBUG
////    copy(itc2007Benchmarks.begin(), itc2007Benchmarks.end(), ostream_iterator<TestSetDescription>(cout, "\n"));
//#endif
    // Iterator to test set
    vector<TestSetDescription>::iterator it = itc2007Benchmarks.begin() + _datasetIndex;
    // Create TestSet instance
    ITC2007TestSet testSet((*it).getName(), (*it).getDescription(), _testBenchmarksDir);
    // Load dataset
    ITC2007TestSet* ptr = &testSet;
    ptr->load();
//#ifdef MAINAPP_DEBUG
//    // Print testset info
//    cout << testSet << endl;
//    // Print timetable problem data
//    cout << *(ptr->getTimetableProblemData().get()) << endl;
//#endif


////    ConflictBasedStatistics::test1();




///*
// *
//    // Objective function evaluation
////    eoETTPEvalWithStatistics<eoChromosome> eval(numEvalCounter);
//    eoETTPEval<eoChromosome> fullEval;

//    eoChromosome chrom(testSet.getTimetableProblemData().get());
//    // Solution initialiser
//    ETTPInit<eoChromosome> init(testSet.getTimetableProblemData().get());
//    init(chrom);
//    // Evaluate solution
//    fullEval(chrom);



////    GCHeuristics<eoChromosome>::saturationDegree(*testSet.getTimetableProblemData().get(), chrom);

////    // Count # evaluations in 1 sec
////    int numberEvals = 0;
////    auto start1 = std::chrono::high_resolution_clock::now();
////    for (;;) {
////        auto now = std::chrono::high_resolution_clock::now();
////        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start1).count() >= 1000)
////            break;

////        // Compute solution fitness
////        chrom.computeFitness();
////        ++numberEvals;
////    }
////    cout << "Performed " << numberEvals << " in 1 sec." << endl;

////    std::cout << "After invoking [GCHeuristics<EOT>::saturationDegree]: chromosome feasible? "
////              << chrom.isFeasible() << std::endl;

//    std::cout << "solution fitness = " << chrom.fitness() << std::endl;

//    // Validate solution
////    chrom.validate();

//    if (chrom.isFeasible()) {
//        cout << "Found feasible solution" << endl;
//    }
//    else {
//        cout << "> No feasible solution was found" << endl;
//        exit(-1);
//    }

//    //
//    // Improvement by Local search.
//    // Local search used: Threshold Accepting algorithm
//    //
//    // moTA parameters
//    boost::shared_ptr<ETTPKempeChainHeuristic<eoChromosome> > kempeChainHeuristic(new ETTPKempeChainHeuristic<eoChromosome>());
//    ETTPneighborhood<eoChromosome> neighborhood(kempeChainHeuristic);
////                ETTPneighborEval<EOT> neighEval;

//    // For counting # evaluations
//    eoNumberEvalsCounter numEvalsCounter;
//    // ETTPneighborEvalNumEvalsCounter which receives as argument an
//    // eoNumberEvalsCounter for counting neigbour # evaluations
//    ETTPneighborEvalNumEvalsCounter<eoChromosome> neighEval(numEvalsCounter);
//    // Cooling schedule
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(1000, 0.1, 1, 910); // CoolSch #0
////        moSimpleCoolingSchedule<eoChromosome> coolSchedule(1000, 0.1, 5, 100); // CoolSch #1
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.1, 5, 2e-5); // 1333
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.01, 5, 2e-5); // CoolSch #2 Found sol for set #12
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.01, 5, 0.4); // CoolSch #2
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.0001, 5, 2e-5); // Set 1 (8 unscheduled) 11312, 104692 ms, 656,120  eval
//                                                                             // Set 2 - 1690 - 201 sec.  656,120  eval
//                                                                             // Set 9 - 1333, 1190 - 17535 ms 656,120  eval
//                                                                             // Set 10  23546, 22664 ms, 656,120  eval
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.0001, 5, 2e-5); // Set 2 - 1950 - 263 sec.  771,250 eval


//    /// FIRST TEST - 21-Jan
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.0001, 10, 2e-5); // Set 9 - 1160 - 50 sec. 1,542,500  eval
//        moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.00001, 5, 2e-5); // Set 9 -  - sec.  6,561,185 eval

//    //    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.00001, 5, 2e-5); // Set 9 - 1169 (1 unschedule exam), 189169 ms  7,712,475 eval
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.00001, 5, 2e-5); // Set 2 - -- 2-- sec. 7,712,475
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000001, 5, 2e-5); // CoolSch #3 Uta 3.03   // TEST

//#ifdef MAINAPP_DEBUG
//    // getSANumberEvaluations(double tmax, double r, double k, double tmin)
//    long numEvalsTA = getSANumberEvaluations(coolSchedule.initT, coolSchedule.alpha,
//                                             coolSchedule.span, coolSchedule.finalT);
//    /////////////////////////// Writing the TA parameters ////////////////////////////////////////////////////////
//    cout << "TA parameters:" << endl;
//    cout << "cooling schedule: " << coolSchedule.initT << ", " << coolSchedule.alpha << ", "
//            << coolSchedule.span << ", " << coolSchedule.finalT << endl;
//    cout << "# evals per TA local search: " << numEvalsTA << endl;
//#endif
//    moTA<ETTPneighbor<eoChromosome> > ta(neighborhood, fullEval, neighEval, coolSchedule);

//    // Apply TA to the solution
//    ta(chrom);
//    std::cout << "After TA" << std::endl;

//    // For debug, compute the cost
////    chrom.computeCost();
////    cout << "chrom cost (after computing it again) = " << chrom.getSolutionCost() << endl;

//    std::cout << "fitness = " << chrom.fitness() << ", Total # evaluations: "
//              << numEvalsCounter.getTotalNumEvals() << std::endl;


////    cout << endl << "END Date/Time = " << currentDateTime() << endl;

//    // Stop time
//    auto stop = std::chrono::high_resolution_clock::now();
//    std::cout << "duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms\n";


//    // Write solution to .sln file
//    ofstream slnFile(testSet.getRootDirectory() + "/" + testSet.getName() + ".sln");
//    slnFile << chrom;

//    // Validate solution
////    chrom.validate();

//*/



    // Generate exam move statistics
    generateExamMoveStatistics(_outputDir, testSet);


//    // Run test set
////    runSCEA(_outputDir, testSet);




//    // Run test set
////    runCellularEA(_outputDir, testSet);



}






///**
// * @brief runSCEA
// * @param _outputDir
// * @param _testSet
// */
//void runSCEA(const string &_outputDir, TestSet const& _testSet) {
//    //
//    // SCEA parameters
//    //
//    const int M = 1;  // m is the number of memeplexes
////    const int M = 3;  // m is the number of memeplexes
////    const int M = 5;  // m is the number of memeplexes
////    const int N = 8;  // N is the number of frogs in each memeplex
////    const int Q = 8;  // q is the number of frogs in each submemeplex

////    const int N = 3;  // N is the number of frogs in each memeplex
////    const int Q = 3;  // q is the number of frogs in each submemeplex

////    const int N = 10;  // N is the number of frogs in each memeplex
////    const int Q = 10;  // q is the number of frogs in each submemeplex

////    const int N = 2;  // N is the number of frogs in each memeplex
////    const int Q = 2;  // q is the number of frogs in each submemeplex

//    const int N = 1;  // N is the number of frogs in each memeplex
//    const int Q = 1;  // q is the number of frogs in each submemeplex


////    const double UP = 0.0000001; // ECTA 2014

//// const double UP = 0.0000001; // Uta 3.05 m = 1, n = 4, q = 4
//// const double UP = 0.000001; // Uta 3.23 3.34
//// LAST
//// const double UP = 0.00001; // Yor 35.05, Uta 3.52, Hec 10.46
//// const double UP = 0.0001; // Yor 36.68, Uta 3.61,
//// const double UP = 0.001; // Uta 3.86
//// const double UP = 0.01; // Uta 3.93 with m = 1, n = 4, q = 4
//                        // Uta 3.76 with m = 3, n = 10, q = 10
//                        // Uta 3.75 with m = 5, n = 25, q = 20
//// const double UP = 0.01; // Hec 10.56
////    const double UP = 1; // Hec 10.56


//    const double UP = 1 - (1 / 9e6);
////    const double UP = 0.1;

//    const int F = M*N; // The total sample size, F, in the swamp is given by F = mN.


//    // Number of consecutive time loops
////    int L = 1;
////    int L = 15;
////    int L = 50;
//    int L = 100000000;

//    // Crossover, mutation and improve probabilities
////    const double cp = 0.6;
////    const double mp = 0.1;

//    const double cp = 0;
////    const double cp = 0.2;

////    const double mp = 1;
//    const double mp = 0;
////    const double mp = 0.2;
////    const double mp = 0.1;//LAST
////        const double mp = 0.01;
////    const double mp = 0.5;

//    const double ip = 1;
////    const double ip = 0.1;
////    const double ip = 0;

//    // Creating the output file
//    stringstream filenameStr;
//    filenameStr << _testSet.getName() << "_m_" << M << "_N_" << N << "_q_" << Q
//            << "_L_" << L << "_cp_" << cp << "_mp_" << mp << "_ip_" << ip
//            << "_up_" << UP;

//    string filename;
//    filenameStr >> filename;
//    ofstream outFile(filename + ".txt");

//    cout << "Start Date/Time = " << currentDateTime() << endl;
//    // Write Start time and algorithm parameters to file
//    outFile << "Start Date/Time = " << currentDateTime() << endl;
//    outFile << "SCEA parameters:" << endl;
//    outFile << "m = " << M << ", N = " << N << ", q = " << Q << ", L = " << L << endl;
//    outFile << "cp = " << cp << ", mp = " << mp << ", ip = " << ip << endl;
//    outFile << "GDA parameters:" << endl;
//    outFile << "up: " << UP << endl;

//    cout << "Run SCEA" << endl;

//    cout << _testSet << endl;
//    outFile << _testSet << endl;

//    // Solution initializerb
////    ETTPInit<eoChromosome> init(testSet.getData(),
////                                *(testSet.getConflictMatrix()), *(testSet.getGraph()));
//    // Solution initializer
//    ETTPInit<eoChromosome> init(_testSet.getTimetableProblemData().get());
//    // Generate initial population
//    eoPop<eoChromosome> pop(F, init);
//    // Objective function evaluation
//    eoETTPEval<eoChromosome> eval;
//    // Evaluate population
//    for (int i = 0; i < pop.size(); ++i) {
//        eval(pop[i]);
//    }


//    // Terminate after concluding L time loops or 'Ctrl+C' signal is received
//    eoGenerationContinue<eoChromosome> terminator(L);
//    eoCheckPoint<eoChromosome> checkpoint(terminator);

//    // Crossover and mutation
//    Mutation<eoChromosome> mutation;
//    Crossover<eoChromosome> crossover;

//    // Chromosome evolution operator
//////    eoSFLAEvolOperator_3<eoChromosome> evolutionOperator; // generic hum...
////    eoSFLAEvolOperator_3 evolutionOperator; // generic hum...

//    const int NUM_BINS = 10;
//    // Build SCEA
//    eoSCEA<eoChromosome> scea(_testSet, _outputDir, NUM_BINS,
//                              outFile, filename, M, N, Q, F, L, UP,
//                              cp, mp, ip,
//                              init, checkpoint, eval,
//                              crossover, mutation
//                              /*, evolutionOperator*/);

//    // Run the algorithm
//    scea(pop);

//    cout << "End of evolution cycle" << endl
//         << "Writing best solution to file..." << endl;

//    // Write best solution to file
//    ofstream solutionFile;
//    solutionFile.open(filename + ".sol", ios::out | ios::trunc);
////    solutionFile << "==============================================================" << endl;
////    solutionFile << "Date/Time = " << currentDateTime() << endl;
//    solutionFile << scea.getBestFrog() << endl;
////    solutionFile << "==============================================================" << endl;
//    solutionFile.close();

//    cout << "End Date/Time = " << currentDateTime() << endl;

//    // Write to file
//    outFile << "End Date/Time = " << currentDateTime() << endl;
//}







///**
// * @brief runCellularEA
// * @param _outputDir
// * @param _testSet
// */
//void runCellularEA(const string &_outputDir, TestSet const& _testSet) {
//    //
//    // cEA parameters
//    //


//    const int NLINES = 2;
//    const int NCOLS = 2;

//    ////// Pop size = 16  ////////
//    // Rect
////    const int NLINES = 2;
////    const int NCOLS = 8;


///// COR2015 Paper
////    // Matrix
////    const int NLINES = 4;
////    const int NCOLS = 4;

//    // Matrix
////    const int NLINES = 10;
////    const int NCOLS = 10;



////    // Ring
////    const int NLINES = 1;
////    const int NCOLS = 16;
//    //////////////////////////////

//    ////// Pop size = 36  ////////
//    // Rect
////    const int NLINES = 3;
////    const int NCOLS = 12;
//    // Matrix
////    const int NLINES = 6;
////    const int NCOLS = 6;
//    // Ring
////    const int NLINES = 1;
////    const int NCOLS = 36;
//    //////////////////////////////

//    ////// Pop size = 64  ////////
//    // Rect
////    const int NLINES = 2;
////    const int NCOLS = 32;
//    // Matrix
////    const int NLINES = 8;
////    const int NCOLS = 8;
//    // Ring
////    const int NLINES = 1;
////    const int NCOLS = 64;
//    //////////////////////////////

//    ////// Pop size = 100  ////////
////    // Rect
////    const int NLINES = 5;
////    const int NCOLS = 20;
////    // Matrix
////    const int NLINES = 10;
////    const int NCOLS = 10;
////    // Ring
////    const int NLINES = 1;
////    const int NCOLS = 100;
//    //////////////////////////////

//    ////// Pop size = 225  ////////
//    // Rect
////    const int NLINES = 9;
////    const int NCOLS = 25;
////    // Matrix
////    const int NLINES = 15;
////    const int NCOLS = 15;

//    // Ring
////    const int NLINES = 1;
////    const int NCOLS = 225;
//    //////////////////////////////

////    const int NLINES = 10;
////    const int NCOLS = 100;

////    const int NLINES = 50;
////    const int NCOLS = 200;


//    const int POP_SIZE = NLINES*NCOLS;  // Population size
//    const int L = 5000000; // Number of generations

//    // Crossover probability
////        const double cp = 0.7;
////    const double cp = 0.6;
////    const double cp = 0.4; // TEST
////    const double cp = 0.15;
//    const double cp = 0;

//    // Mutation probability
////    const double mp = 1;
////        const double mp = 0.5;
//    const double mp = 0.1;
////    const double mp = 0; // TEST

//    // Improve probability
//    const double ip = 0.1; // TEST
////    const double ip = 0.5;
////      const double ip = 1;
////      const double ip = 0;

//    // TA parameters
////      moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.0001, 5, 2e-5); // CoolSch #1 < 30 min

////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.99, 3, 2e-5);
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.1, 5, 2e-5);
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.01, 5, 2e-5);
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.001, 5, 2e-5);//  Uta 3.68 1h
////      moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.0001, 5, 2e-5);//
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.00001, 5, 2e-5); // CoolSch #2 Uta 3.13,
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000001, 5, 2e-5); // CoolSch #3 Uta 3.03   // TEST

//    //////// ITC2007 parameters /////////////////////////

////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.0001, 10, 2e-5); // Set 9 - 1160 - 50 sec. 1,542,500  eval

//    /// LAST
//    // cool schedule 2
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.0001, 5, 2e-5); // Set 4 - 15426 - 771250 eval  2 gen


////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(30, 0.0001, 5, 2e-5);  // Set 1 - 6187 - 711050 evals   SLOW
//                                                                                // Set 2 - 440 - 711050 evals


////      moSimpleCoolingSchedule<eoChromosome> coolSchedule(30, 0.0001, 5, 2e-3);  // Set 1 -  -  evals
//                                                                                    // Set 2 -  -

//    // cool schedule 1
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.001, 5, 2e-5);  // Set 4 - 13882 - 77125 eval  ?
//                                                                                // Set 4 - 15422 - 77125 eval
//                                                                                // Set 1 - 7470 - 711050 evals   SLOW
//                                                                                // Set 2 - 638 - 711050 evals

//    // cool schedule 4
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.01, 10, 2e-5); // Set 1 - 8946 - 15430 evals
//                                                                               // Set 4 - 15365
//    // FAST
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.1, 5, 2e-5); // Set 1 - 9660 - 775 evals.
//                                                                             // Set 9 - 1500 - Best literature: 1047
//                                                                             // Set 4 - 15789 - Best literature: 15663
//                                                                             // Set 2 - 1062 - Best literature: 385


//    // cool schedule 3
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.001, 5, 0.1); // Set 1 - 6783 - 23030 evals - Best literature: 4370
//                                                                           // Set 2 - 605 - 23030 evals - Best literature: 400
//                                                                           // Set 3 -  - 23030 evals - Best literature: 8996
//                                                                           // Set 4 - 16002 - 23030 evals - Best literature: 16589


////        moSimpleCoolingSchedule<eoChromosome> coolSchedule(30, 0.0001, 5, 0.1); // Set 1 - 6480 - 285190 evals SLOW
////                                                                                // Set 2 - 507 -  evals
////                                                                                // Set 9 - 1065

////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(5, 0.0001, 5, 0.1); // Set 1 -  - 285190 evals
////                                                                            // Set 2 -  -  evals
////                                                                            // Set 9 -

////        moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.0001, 5, 0.1); // Set 1 -  -  evals


////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(20, 0.0001, 5, 0.01); // Set 1 -  -  evals SLOW
////                                                                            // Set 2 -  -  evals
//                                                                            // Set 9 -

//    // FAST
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(5, 0.01, 5, 0.001); // FAST cGA

////    // CONFIG2/Run4 / CONFIG4
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(50, 0.01, 5, 2e-4); // FAST cGA

//    // CONFIG2/Run5 - CONFIG5
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.01, 5, 2e-4); // FAST cGA

//    // CONFIG2/Run6 - CONFIG6
//    // COR2015 Paper
//    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.001, 5, 2e-4); // FAST cGA





//    //CONFIG3
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(15, 0.001, 5, 2e-6); // FAST cGA

////        moSimpleCoolingSchedule<eoChromosome> coolSchedule(5, 0.001, 5, 2e-5); //
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(25, 0.001, 5, 2e-5); //

////moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.001, 5, 0.001); //  cGA


//    // Creating the output file in the specified output directory
//    stringstream sstream;
//    sstream << _outputDir << _testSet.getName() << "_NLINES_" << NLINES << "_NCOLS_" << NCOLS
//            << "_cp_" << cp << "_mp_" << mp << "_ip_" << ip
//            << "_cool_" << coolSchedule.initT << "_" << coolSchedule.alpha << "_"
//            << coolSchedule.span << "_" << coolSchedule.finalT << ".txt";

//    string filename;
//    sstream >> filename;
//    ofstream outFile(filename);

//    cout << "Start Date/Time = " << currentDateTime() << endl;
//    // Write Start time and algorithm parameters to file
//    outFile << "Start Date/Time = " << currentDateTime() << endl;
//    // Print Test set info
//    cout << _testSet << endl;
//    // Write testset info to file
//    outFile << _testSet << endl;
//    // getSANumberEvaluations(double tmax, double r, double k, double tmin)
//    long numEvalsTA = getSANumberEvaluations(coolSchedule.initT, coolSchedule.alpha,
//                                             coolSchedule.span, coolSchedule.finalT);
//    /////////////////////////// Writing the cGA parameters ////////////////////////////////////////////////////////
//    cout << "cGA parameters:" << endl;
//    cout << "NLINES = " << NLINES << ", NCOLS = " << NCOLS << endl;
//    cout << "cp = " << cp << ", mp = " << mp << ", ip = " << ip << endl;
//    cout << "TA parameters:" << endl;
//    cout << "cooling schedule: " << coolSchedule.initT << ", " << coolSchedule.alpha << ", "
//            << coolSchedule.span << ", " << coolSchedule.finalT << endl;
//    cout << "# evals per TA local search: " << numEvalsTA << endl;
//    ///
//    outFile << "cGA parameters:" << endl;
//    outFile << "NLINES = " << NLINES << ", NCOLS = " << NCOLS << endl;
//    outFile << "cp = " << cp << ", mp = " << mp << ", ip = " << ip << endl;
//    outFile << "TA parameters:" << endl;
//    outFile << "cooling schedule: " << coolSchedule.initT << ", " << coolSchedule.alpha << ", "
//            << coolSchedule.span << ", " << coolSchedule.finalT << endl;
//    outFile << "# evals per TA local search: " << numEvalsTA << endl;
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    // Solution initializer
//    ETTPInit<eoChromosome> init(_testSet.getTimetableProblemData().get());
//    // Generate initial population
////    eoPop<eoChromosome> pop(POP_SIZE, init);
//      // We can't work with eoPop of shared_ptr because shared_ptr is not an EO
////    boost::shared_ptr<eoPop<boost::shared_ptr<eoChromosome> > > pop(new eoPop<boost::shared_ptr<eoChromosome> >());
//    // Solution: Work with vector<shared_ptr<EOT> > directly
//    boost::shared_ptr<vector<boost::shared_ptr<eoChromosome> > > pop(new vector<boost::shared_ptr<eoChromosome> >());

//    auto &solutionPop = *pop.get();
//    for (int i = 0; i < POP_SIZE; ++i) {
//        // Create solution object and insert it in the vector
////        solutionPop.push_back(boost::shared_ptr<eoChromosome>(new eoChromosome()));
////        // Initialize chromosome
////        init(*solutionPop.back().get());
////        solutionPop[i] = boost::shared_ptr<eoChromosome>(new eoChromosome(_testSet.getTimetableProblemData().get()));
//        solutionPop.push_back(boost::shared_ptr<eoChromosome>(new eoChromosome(_testSet.getTimetableProblemData().get())));
//        // Initialize chromosome
//        init(*solutionPop[i].get());
//    }

//    // # evaluations counter
//    eoNumberEvalsCounter numEvalCounter;
//    // Objective function evaluation
//    eoETTPEval<eoChromosome> eval;
//    // Objective function evaluation
////    eoETTPEvalWithStatistics<eoChromosome> eval(numEvalCounter);
//    // Evaluate population
////    for (int i = 0; i < pop.size(); ++i) {
////        eval(pop[i]);
////    }
//    for (int i = 0; i < solutionPop.size(); ++i) {
//        eval(*solutionPop[i].get());
//    }


//    /////////////////////////////////////////////////////////////////////////////////
//    // Print population information to output and
//    // save population information into file
//    std::cout << std::endl << "Initial population" << std::endl;
//    outFile << std::endl << "Initial population" << std::endl;
//    int k = 0;
//    for (int i = 0; i < NLINES; ++i) {
//        for (int j = 0; j < NCOLS; ++j, ++k) {
//            std::cout << (*solutionPop[k].get()).fitness() << "\t";
//            outFile << (*solutionPop[k].get()).fitness() << "\t";
//        }
//        std::cout << std::endl;
//        outFile << std::endl;
//    }
//    /////////////////////////////////////////////////////////////////////////////////


// /*   ////// DEBUG ///////////////

//    //
//    // Improvement by Local search.
//    // Local search used: Threshold Accepting algorithm
//    //
//    // moTA parameters
//    boost::shared_ptr<ETTPKempeChainHeuristic<eoChromosome> > kempeChainHeuristic(new ETTPKempeChainHeuristic<eoChromosome>());
//    ETTPneighborhood<eoChromosome> neighborhood(kempeChainHeuristic);
////                ETTPneighborEval<EOT> neighEval;

//    // For counting # evaluations
//    eoNumberEvalsCounter numEvalsCounter;
//    // ETTPneighborEvalNumEvalsCounter which receives as argument an
//    // eoNumberEvalsCounter for counting neigbour # evaluations
//    ETTPneighborEvalNumEvalsCounter<eoChromosome> neighEval(numEvalsCounter);
//    // Cooling schedule
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(1000, 0.1, 1, 910); // CoolSch #0
////        moSimpleCoolingSchedule<eoChromosome> coolSchedule(1000, 0.1, 5, 100); // CoolSch #1
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.1, 5, 2e-5); // 1333
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.01, 5, 2e-5); // CoolSch #2 Found sol for set #12
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.01, 5, 0.4); // CoolSch #2
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.0001, 5, 2e-5); // Set 1 (8 unscheduled) 11312, 104692 ms, 656,120  eval
//                                                                             // Set 2 - 1690 - 201 sec.  656,120  eval
//                                                                             // Set 9 - 1333, 1190 - 17535 ms 656,120  eval
//                                                                             // Set 10  23546, 22664 ms, 656,120  eval
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.0001, 5, 2e-5); // Set 2 - 1950 - 263 sec.  771,250 eval


//    /// FIRST TEST - 21-Jan
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.0001, 10, 2e-5); // Set 9 - 1160 - 50 sec. 1,542,500  eval
////        moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.00001, 5, 2e-5); // Set 9 -  - sec.  6,561,185 eval

//    //    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.00001, 5, 2e-5); // Set 9 - 1169 (1 unschedule exam), 189169 ms  7,712,475 eval
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.00001, 5, 2e-5); // Set 2 - -- 2-- sec. 7,712,475
////    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.000001, 5, 2e-5); // CoolSch #3 Uta 3.03   // TEST

////#ifdef MAINAPP_DEBUG
////    // getSANumberEvaluations(double tmax, double r, double k, double tmin)
////    long numEvalsTA = getSANumberEvaluations(coolSchedule.initT, coolSchedule.alpha,
////                                             coolSchedule.span, coolSchedule.finalT);
////    /////////////////////////// Writing the TA parameters ////////////////////////////////////////////////////////
////    cout << "TA parameters:" << endl;
////    cout << "cooling schedule: " << coolSchedule.initT << ", " << coolSchedule.alpha << ", "
////            << coolSchedule.span << ", " << coolSchedule.finalT << endl;
////    cout << "# evals per TA local search: " << numEvalsTA << endl;
////#endif
//    moTA<ETTPneighbor<eoChromosome> > ta(neighborhood, eval, neighEval, coolSchedule);

//    eoChromosome &chrom = *solutionPop[0].get();

////    eoChromosome chrom;

//    std::cout << "Before TA" << std::endl;
//    std::cout << "fitness = " << chrom.fitness() << std::endl;

//    chrom.validate();

//    // Apply TA to the solution
//    ta(chrom);

//    std::cout << "After TA" << std::endl;
//    std::cout << "fitness = " << chrom.fitness() << std::endl;

//    chrom.validate();


//    // Write solution to .sln file
//    ofstream slnFile(_testSet.getRootDirectory() + "/" + _testSet.getName() + ".sln");
//    slnFile << chrom;

//    ///////////////////////////

//return;
//*/

//    // Write solution to .sln file
//    ofstream slnFile(_testSet.getRootDirectory() + "/" + _testSet.getName() + ".sln");
//    slnFile << *solutionPop[0].get();

////    return;

//    //
//    // Build CellularGA
//    //
//    // Terminate after concluding L time loops or 'Ctrl+C' signal is received
////    eoGenerationContinue<eoChromosome> terminator(L);
////    eoCheckPoint<eoChromosome> checkpoint(terminator);
//    // The eoGenerationContinuePopVector object, instead of using an eoPop to represent the population,
//    // uses a vector. A vector is used in order to swap offspring and population efficiently using pointers
//    eoGenerationContinuePopVector<eoChromosome> terminator(L);

//    // Declare 1-selectors
//    //
//    // Binary deterministic tournament selector used in neighbour selection
////    eoDetTournamentSelect<eoChromosome> detSelectNeighbour;
//    // Work with pointers for efficiency
//    eoDetTournamentSelectSharedPtr<eoChromosome> detSelectNeighbourPtr;

//    // Crossover and mutation
//    Mutation<eoChromosome> mutation;
//    Crossover<eoChromosome> crossover;
//    eoSelectBestOne<eoChromosome> selectBestOne;

//    boost::shared_ptr<eoCellularEA<eoChromosome> > cGA;
//    // Build the corresponding type of cGA object depending on the layout (Ring or Matrix)
//    if (NLINES == 1) { // Ring cGA
////    eoCellularEARing<eoChromosome> cEA(checkpoint, eval, detSelectNeighbour,
////                      crossover, mutation,
////                     selectBestOne,
////                     selectBestOne
////                    );
//    }
//    else {
//        cGA = boost::make_shared<eoCellularEAMatrix<eoChromosome> >(
//                    outFile, NLINES, NCOLS,
//                    cp, mp, ip, coolSchedule,
//                    terminator,
////                    checkpoint,
//                    eval,
////                    detSelectNeighbour,
//                    detSelectNeighbourPtr, // Work with pointers for efficiency
//                    crossover, mutation,
//                    selectBestOne, // To choose one from the both children
//                    selectBestOne,  // Which to keep between the new child and the old individual?
//                    numEvalCounter
//        );

//    }

//    // Run the algorithm
//    (*cGA.get())(pop);


//    //
//    // Create solution file and run Rong Qu's exam evaluator
//    //
//    string solutionFilename = _testSet.getName() + ".sol";
//    ofstream solutionFile(_outputDir + solutionFilename);
//    // Write best solution to file
//    solutionFile << (*(*cGA.get()).getBestSolution());
//    // Close solution file
//    solutionFile.close();

//    // Write end Date/Time
//    cout << "End Date/Time = " << currentDateTime() << endl;
//    // Write to file
//    outFile << "End Date/Time = " << currentDateTime() << endl;
//    // Close output file
//    outFile.close();


//}





