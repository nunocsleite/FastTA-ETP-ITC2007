#ifndef EOSCEA_H
#define EOSCEA_H


#include "eoEvolutionOperator.h"
#include "algorithms/mo/moGDA.h"
#include <eoAlgo.h>
#include <eoContinue.h>
#include "algorithms/mo/moSA.h"

// Using boost accumulators framework for computing the variance
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

//#include "neighbourhood/statistics/ETTPneighborEvalWithStatistics.h"
//#include "statistics/ExamMoveStatistics.h"
#include "algorithms/mo/statistics/moTAWithStatistics.h"
//#include "neighbourhood/statistics/ETTPneighborhoodWithStatistics.h"
//#include "neighbourhood/statistics/ETTPneighborWithStatistics.h"
#include "neighbourhood/statistics/ETTPneighborEvalWithStatistics.h"


// I'm importing the accumulators namespace as "a" to make clear which
// elements belong to it
namespace a = boost::accumulators;


using namespace std;
using namespace boost;

extern const std::string currentDateTime();



/**
 * Memetic Algorithm based on Shuffled Complex Evolution Algorithm (SCEA) and
 * Shuffled Frog-Leaping Algorithm (SFLA)
 *
 * Shuffled Frog-Leaping Algorithm
 *
 * Global exploration
 *
 *  Step 0 Initialize. Select m and n, where m is the number of memeplexes and n is the number of frogs in each
 *  memeplex. Therefore, the total sample size, F, in the swamp is given by F = mn.
 *
 *  Step 1 Generate a virtual population. Sample F virtual frogs, U(1),...,U(F).
 *  Compute the performance value f(i) for each frog U.
 *
 *  Step 2 Rank frogs. Sort the F frogs in order of decreasing performance value. Store them in an array X={U(i),
 *  f(i), i=1,...,F} so that i=1 represents the frog with the best performance value. Record the best frog’s
 *  position, PX, in the entire population (F frogs; where PX =U(1)).
 *
 *  Step 3 Partition frogs into memeplexes. Partition array X into m memeplexes Y1, Y2,...,Ym, each containing
 *  n frogs. E.g., for m=3, rank 1 goes to memeplex 1, rank 2 goes to memeplex 2, rank 3 goes to memeplex 3,
 *  rank 4 goes to memeplex 1, and so on (Fig. 2).
 *
 *  Step 4 Memetic evolutions within each memeplex. Evolve each memeplex Y(l), l=1,..., m.
 *
 *  After partitioning frogs to m memeplexes, evolve each memeplex and each of them should iterate N times. After
 *  the memeplexes have been evolved, the algorithm returns to the global exploration for shuffling.
 *
 *  Local exploration: frog-leaping algorithm
 *  Step 4-0 Set im=0, where im counts the number of memeplexes and will be compared with
 *  the total number m of memeplexes. Set iN=0, where iN counts the number of evolutionary
 *  steps and will be compared with the maximum number N of steps to be completed within each
 *  memeplex. Within each memeplex (Fig. 3b), the frogs with the best and the worst fitness are
 *  identified as Pb and Pw, respectively. Also, the frog with the global best fitness is identified as
 *  Pg. Then, an evolution process is applied to improve only the frog with the worst fitness (i.e.,
 *  not all frogs) in each cycle.
 *  Step 4-1 Set im=im+1.
 *  Step 4-2 Set iN=iN+1.
 *  Step 4-3 Improve the worst frog’s position.
 *  The position of the frog with the worst fitness is adjusted as follows:
 *     Change in frog position (Di) = rand() x (Pb - Pw)     (5)
 *
 *     New position Pw = current position Pw + Di;
 *                          (Dmax geq Di geq Dmax)           (6)
 *
 *  where rand() is a random number between 0 and 1 and Dmax is the maximum allowed change in a frog’s position.
 *  Step 4-4 If this process produces a better frog (solution), it replaces the worst frog. Otherwise, the
 *  calculations in Eqs. 5 and 6 are repeated with respect to the global best frog (i.e., Pg replaces Pb).
 *  Step 4-5 If no improvement becomes possible in this latter case, then a new solution is
 *  randomly generated to replace the worst frog with another frog having any arbitrary fitness (as shown in Fig. 3b).
 *  Step 4-6 If iN < N, go to step 4-2.
 *  Step 4-7 If im<m, go to step 4-1. Otherwise, return to the global search to shuffle memeplexes.
 *
 *  Step 5 Shuffle memeplexes. After a defined number of memetic evolutionary steps within each memeplex,
 *  replace Y1,...,Ym into X such that X={Yk, k = 1..., m}. Sort X in order of decreasing performance value.
 *  Update the population the best frog’s position PX.
 *
 *  Step 6 Check convergences. If the convergence criteria are satisfied, stop. Otherwise, return to step 3.
 *  Typically, the decision on when to stop is made by a prespecified number of consecutive time loops when
 *  at least one frog carries the “best memetic pattern” without change. Alternatively, a maximum total number of
 *  function evaluations can be defined (Fig. 4).
 */




/**
    This is a generic class for Shuffled Complex Evolution algorithms. There
    is only one operator defined, which takes a population and does stuff to
    it. It needn't be a complete algorithm, can be also a step of an
    algorithm.

    @ingroup Algorithms
*/
template <class EOT>
class eoSCEA : public eoAlgo<EOT>
{
public:

    /** Constructor
     * @param n - number of memeplexes
     * @param N - number of frogs in each memeplex
     * @param F - total sample size in the swamp and given by F = mN
     * @param _init - An eoInit that initializes each frog (solution)
     * @param _continuator - An eoContinue that manages the stopping criterion and the checkpointing system
     * @param _eval - An eoEvalFunc: the evaluation performer
     */
    eoSCEA<EOT>(TestSet const& _testSet, std::string const& _outputDir, int _numBins,
                ofstream& _outFile, string const& _filename,
                int _m, int _N, int _q, int _F, long _L, double _up,
                double _cp, double _mp, double _ip,
                eoInit<EOT>& _init, eoContinue<EOT>& _continuator,
                eoEvalFunc<EOT>& _eval, // Evaluation function
                eoQuadOp <EOT> & _cross, // Crossover operator
                eoMonOp<EOT> & _mut // Mutation operator
                /*eoBinOp<EOT>& _chromEvolOperator*/) :
        testSet(_testSet),
        outputDir(_outputDir),
        numBins(_numBins),
        outFile(_outFile), filename(_filename), m(_m), N(_N), q(_q), F(_F), L(_L), up(_up),
        cp(_cp), mp(_mp), ip(_ip), init(_init),
        continuator(_continuator), eval(_eval),
        cross(_cross),
        mut(_mut),
//        chromEvolOperator(_chromEvolOperator),
        bestSolution(0), // null
        popVariance(0)
    { }

    // Apply SCEA to the population
    virtual void operator()(eoPop<EOT> &_pop)
    {
        try
        {
            // Step 0 and 1 - Initialization and generation of virtual population of frogs
            //   The population is already initialized by the 'init' object
            //
            // Step 2 - Frog ranking
            rankFrogs(_pop);
            // Create memeplex structure
            vector<vector<EOT> > memplexes = createMemeplexes();

            do
            {
                // Step 3 Partition frogs into memeplex
                partitionFrogsIntoMemplexes(_pop, memplexes);
                // Step 4 Frog-leaping algorithm for local search
                frogLeapingLocalSearch(memplexes);
                // Step 5 Shuffle memeplexes. After a defined number of memetic evolutionary steps within each memeplex,
                // replace Y1,...,Ym into X such that X={Yk, k = 1..., m}. Sort X in order of decreasing performance value.
                // Update the population the best frog’s position PX.
                shuffleMemplexes(memplexes, _pop);
                // Step 6 Check convergences. If the convergence criteria are satisfied, stop. Otherwise, return to step 3.
                // Typically, the decision on when to stop is made by a prespecified number of consecutive time loops when
                // at least one frog carries the “best memetic pattern” without change. Alternatively, a maximum total number of
                // function evaluations can be defined (Fig. 4).

                // Compute best solution and statistics
                computeVariance(_pop);
                // Print memeplex info and save best solution on file
                printMemplexes(memplexes);
            }
            while (continuator(_pop));
        }
        catch (std::exception& _e)
        {
            std::string s = _e.what();
            s.append (" in eoSCEA operator()");
            throw std::runtime_error(s);
        }
        cout << "Best solution overall fitness = " << Pg.fitness() << endl;
    }


    EOT getBestFrog() {
        return Pg;
    }

protected:

    //
    // Auxiliary methods
    //
    void printMemplexes(vector<vector<EOT> >& _memeplexes) {
        // Iterate over the memeplexes
        for (int im = 0; im < m; ++im) {
            cout << "Memplex #" << im << ":" << endl;
            // Iterate memeplex
            for (int iN = 0; iN < N; ++iN) {
                cout << '\t' << _memeplexes[im][iN].fitness() << endl;
            }
        }
        cout << endl;
        cout << endl << "Date/Time = " << currentDateTime() << ", popVariance = " << popVariance
             << ", best sol = " << Pg.fitness() << endl;

/*
        // Save population information into file
        // Iterate over the memeplexes
        for (int im = 0; im < m; ++im) {
            outFile << "Memplex #" << im << ":" << endl;
            // Iterate memeplex
            for (int iN = 0; iN < N; ++iN) {
                outFile << '\t' << _memeplexes[im][iN].fitness() << endl;
            }
        }
        outFile << endl;
*/
        outFile << endl << "Date/Time = " << currentDateTime() << ", popVariance = " << popVariance
                << ", best sol = " << Pg.fitness() << endl;

        // Write best solution to file
        ofstream solutionFile;
        solutionFile.open(filename + ".sol", ios::out | ios::trunc);
        solutionFile << "==============================================================" << endl;
        solutionFile << "Date/Time = " << currentDateTime() << endl;
        solutionFile << Pg << endl;
        solutionFile << "==============================================================" << endl;
        solutionFile.close();
    }

    void computeVariance(eoPop<EOT> const& _pop) {
        ///
        /// TODO - Compute Memeplex variance
        ///

        // The accumulator set which will calculate the properties for us:
        a::accumulator_set< double, a::stats<a::tag::mean, a::tag::variance> > acc_variance;
        // Compute population variance
        for (unsigned i = 0; i < _pop.size(); ++i)
            acc_variance(_pop[i].fitness());
        // Set variance value
        popVariance = a::variance(acc_variance);
    }

    /// TODO: Optimize copies

    void updateGlobalBestFrog(EOT _chrom) {
        // The best frog is located at index 0 in the ranked population
        Pg = _chrom;
    }
    EOT getMemeplexBestFrog(vector<EOT>& _memeplex) {
        // Pre-condition: vector _memeplex is sorted by fitness
        // Memeplex best frog is located at index 0 in the memeplex vector
        return _memeplex[0];
    }
    EOT getMemeplexWorstFrog(vector<EOT>& _memeplex) {
        // Pre-condition: vector _memeplex is sorted by fitness
        // Memeplex worst frog is located at the last index in the memeplex vector
        return _memeplex[_memeplex.size()-1];
    }

    struct FitnessCmp {
        bool operator()(EOT const& _chrom1, EOT const& _chrom2) {
            return _chrom1.fitness() < _chrom2.fitness(); // Ascending order because we're minimizing
        }
    };

    void rankFrogs(eoPop<EOT>& _pop) {
        // Step 2 Rank frogs. Sort the F frogs in order of decreasing performance value. Store them in an array
        // X={U(i), f(i), i=1,...,F} so that i=1 represents the frog with the best performance value.
        // Record the best frog’s position, PX, in the entire population (F frogs; where PX = U(1)).
        sort(_pop.begin(), _pop.end(), FitnessCmp());
        // Record the best frog’s position, PX, in the entire population (F frogs; where PX = U(1)).
        // The best frog is just the 0 index frog
        updateGlobalBestFrog(_pop[0]);
    }

    /// TODO: Optimizar copia do vector no retorno

    vector<vector<EOT> > createMemeplexes() {
        vector<vector<EOT> > memeplexes;
        for (int i = 0; i < m; ++i)
            memeplexes.push_back(vector<EOT>(N));
        return memeplexes;
    }

    void partitionFrogsIntoMemplexes(eoPop<EOT>& _pop, vector<vector<EOT> >& _memeplexes) {
        // Step 3 Partition frogs into memeplex. Partition array X into m memeplexes Y1, Y2,...,Ym, each containing
        // n frogs. E.g., for m=3, rank 1 goes to memeplex 1, rank 2 goes to memeplex 2, rank 3 goes to memeplex 3,
        // rank 4 goes to memeplex 1, and so on (Fig. 2).
        vector<int> memeplexIndexes(m);
        for (int i = 0; i < _pop.size(); ++i) {
            // Each entry in each memeplex references a solution contained in population vector
            _memeplexes[i%m][memeplexIndexes[i%m]] = _pop[i];
            // Increment memeplex solution index
            ++memeplexIndexes[i%m];
        }
    }

    void shuffleMemplexes(vector<vector<EOT> >& _memeplexes, eoPop<EOT>& _pop) {
        // Step 5 Shuffle memeplexes. After a defined number of memetic evolutionary steps within each memeplex,
        // replace Y1,...,Ym into X such that X={Yk, k = 1..., m}. Sort X in order of decreasing performance value.
        // Update the population the best frog’s position PX.

        ///
        /// TODO: Eliminate copies...
        ///
        /// Memeplexes should have references to population vector
        ///

        int k = 0;
        // Iterate over the memeplexes
        for (int im = 0; im < m; ++im) {
            // Perform N evolutionary steps
            for (int iN = 0; iN < N; ++iN) {
                _pop[k++] = _memeplexes[im][iN];
            }
        }

        // Sort the population vector.
        sort(_pop.begin(), _pop.end(), FitnessCmp());

        // Record the best frog’s position, PX, in the entire population (F frogs; where PX =U(1)).
        // The best frog is just the 0 index frog
        updateGlobalBestFrog(_pop[0]);
    }


    void frogLeapingLocalSearch(vector<vector<EOT> >& _memeplexes) {
        // Iterate over the memeplexes
        for (int im = 0; im < m; ++im) {

            // Use Sub-memeplexes
//            vector<bool> marked(N);
//            auto resultPair = createSubMemeplex(_memeplexes[im], q, marked);
//            auto subMemeplex = resultPair.first;
//            auto randomIdxs = resultPair.second;
//            sort(subMemeplex.begin(), subMemeplex.end(), FitnessCmp());

            // Do not use Sub-memeplexes
            auto& subMemeplex = _memeplexes[im];

            // Perform N evolutionary steps
            for (int iN = 0; iN < N; ++iN) {
                // Identify as Pb and Pw, respectively, the frogs with the best and the worst fitness.
                // Also, the frog with the global best fitness is identified as Pg.
                // Then, an evolution process is applied to improve only the frog with
                // the worst fitness (i.e., not all frogs) in each cycle.

/*
                // Use Sub-memeplexes
                // Optimize submemeplex
//                POT Pb = getMemeplexBestFrog(subMemeplex);
                POT Pw = getMemeplexWorstFrog(subMemeplex);
                int numSelected = 0;
                int k = 0;
//                while (numSelected < 1) {
                    int idx = (int)triangular(1, q, 1);
                    // cout << "idx = " << idx << endl;
//                    if (!marked[idx]) {
//                        marked[idx] = true;
//                        ++numSelected;
//                        randomIdxs.push_back(idx);
//        //                submemeplex[k++] = _memeplex[idx-1]; // humm
//                        submemeplex[k++] = _memeplex[idx];
//                    }
//                }
                POT Pb = subMemeplex[idx];
*/

                // Select a random frog except the memeplex's best frog
//                EOT& newPw = subMemeplex[rng.random(subMemeplex.size()-1)+1];

                // LAST
//                int indexNewPw = rng.random(subMemeplex.size()-1)+1;


                // Change best solution eventually
                int indexNewPw = rng.random(subMemeplex.size());


                EOT& newPw = subMemeplex[indexNewPw];

                // Select a random frog
                EOT _Pw = subMemeplex[rng.random(subMemeplex.size())];

/// TODO - Create custom cross and mut operators
//                // To perform cross-over
//                if (rng.uniform() < cp) {
//                    cross(newPw, _Pw);
//                }
//                // To perform mutation
//                if (rng.uniform() < mp) {
//                    mut(newPw);
////                    mut(_Pw);
//                }

//                newPw.invalidate();
//                fullEval(newPw);


                // To perform mutation
                if (rng.uniform() < mp) {
                    // Change the solutions directly
                    mut(newPw);
                    // # evals statistics computation. Add 2 to # evals
//                    numEvalsCounter.addNumEvalsToGenerationTotal(2);
                }


/// COMMENTED 4-MARCH-2016
///
//                // Random-parent crossover
//                if (rng.uniform() < cp) {
////                if (rng.uniform() < 0) {
////                if (rng.uniform() < 0.1) {
////                if (rng.uniform() < 0.2) { // LAST
//                    vector<unordered_set<int> >& pwPeriods = _Pw.getPeriods();
//                    // Get newPw periods
//                    vector<unordered_set<int> >& newPwPeriods = newPw.getPeriods();
//                    // Generate random number of periods to insert from Pw
////                    int randNumPeriods = 3;
////                    int randNumPeriods = 2;
//                    int randNumPeriods = 1;
//                    for (int i = 1; i <= randNumPeriods; ++i) {
//                        // Get a random period from Pw
//                        int randPwPeriod = rng.random(_Pw.getNumPeriods());
//                        // Insert Pw random period exams (those that are feasible) into the new frog
//                        // and remove duplicated exams

//                        // Generate a random index in the new frog
//                        int randNewPwPeriod = rng.random(newPw.getNumPeriods());
//                        // Pw exams from random period
//                        unordered_set<int>& pwExams = pwPeriods[randPwPeriod];

//                        // newPw exams from rand period
//                        unordered_set<int>& newPwExams = newPwPeriods[randNewPwPeriod];

//                        // Insert Pw period into newPw into that random position
//                        for (auto it = pwExams.begin(); it != pwExams.end(); ++it) {
//                            // If the exam does not exist and is feasible then insert it
//                            if (newPwExams.find(*it) == newPwExams.end() &&
//                                newPw.isFeasiblePeriodExam(randNewPwPeriod, *it)) {
//        //                        if (newPwExams.find(*it) == newPwExams.end() && newPw.isFeasiblePeriodExam(bestDayIdx, *it)) {

//                                newPwExams.insert(*it);
//                //                ++numInsertedExams;
//        //                        cout << "Insert: " << *it << endl;
//        //                        cin.get();

//                                /// TODO / SEE POSSIBLE OPTIMIZATIONS

//                                // Remove duplicated exams
//                                for (int i = 0; i < newPwPeriods.size(); ++i) {
//                                    if (i != randNewPwPeriod) {
//                //                        if (i != bestDayIdx) {
//                                        unordered_set<int>& exams = newPwPeriods[i];
//                                        if (exams.find(*it) != exams.end()) {
//                                            exams.erase(*it);
//                //                            ++numRemovedExams;
//                //                            cout << "Remove: " << *it << endl;
//                                            break;
//                                        }
//                                    }
//                                }
//                            }
//                //            else {
//                //                notInserted.insert(*it);
//                //                ++numNotInsertedExams;
//                //            }

//                        }
//                    }
//                    newPw.computeProximityCosts();
//                }

//                // Added 12/may
////                if (rng.uniform() < 0.05) {
//                if (rng.uniform() < mp) {
//                    EOT& _chromosome = newPw;
//                    //
//                    // Swap two periods
//                    //
//                    // Get Frog1 periods
//                    vector<unordered_set<int> >& chromPeriods = _chromosome.getPeriods();
//                    // Generate a random index in frogs
//                    int randPeriod1 = rng.random(_chromosome.getNumPeriods());
//                    int randPeriod2 = rng.random(_chromosome.getNumPeriods());
//                    // Swap period exams
//                    auto aux = chromPeriods[randPeriod1];
//                    chromPeriods[randPeriod1] = chromPeriods[randPeriod2];
//                    chromPeriods[randPeriod2] = aux;

//                    _chromosome.computeProximityCosts();
//                }

                newPw.invalidate();
                eval(newPw);

                sort(_memeplexes[im].begin(), _memeplexes[im].end(), FitnessCmp());

                //POT newFrog = improveWorstFrogPosition(Pb, Pw);

//                int randomIdx = rng.random(subMemeplex.size()/2);
//                int randomIdx = rng.random(subMemeplex.size()/4); // LAST
                int randomIdx = rng.random(subMemeplex.size()); // NOT GOOD


                EOT improvedFrog = subMemeplex[randomIdx];

//                if (rng.uniform() < 0) {
//                if (rng.uniform() < 0.1) { // NOT GOOD
//                if (rng.uniform() < 1) {
                if (rng.uniform() < ip) {
//                    /////////////////////////////////////////////////////////////
////                    moSimpleCoolingSchedule<EOT> coolSchedule(0.01, 0.00001, 5, 1e-7); // SLOW
////                    moSimpleCoolingSchedule<EOT> coolSchedule(0.001, 0.00001, 5, 1e-4); // Set 2: 440
////                    moSimpleCoolingSchedule<EOT> coolSchedule(0.01, 0.0001, 5, 1e-7); // REASONABLE
////                    moSimpleCoolingSchedule<EOT> coolSchedule(0.01, 0.001, 5, 1e-7);
////                    moSimpleCoolingSchedule<EOT> coolSchedule(0.01, 0.1, 5, 1e-7);

//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.0001, 5, 1e-7);
                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.00001, 5, 1e-6); // VERY GOOD, BUT SLOW


                    // Copy of cool schedule to use in TA solver
                    auto cool = coolSchedule;

//                    ExamMoveStatistics examMoveStatistics(testSet, outputDir,
//                                                          numBins, cool);
                    ExamMoveStatistics *examMoveStatistics = new ExamMoveStatistics(testSet, outputDir,
                                                          numBins, cool);
//                    // Generate initial solution
//                    examMoveStatistics.setPtrInitialSolution(&subMemeplex[randomIdx]);
//                    // Determine exams color degree necessary for sorting ExamInfo array by color degree
//                    examMoveStatistics.determineExamsColorDegree();
//                    // Determine thresholds based on the max number of evaluations
//                    examMoveStatistics.generateThresholds();

                    // Generate initial solution
                    examMoveStatistics->setPtrInitialSolution(&subMemeplex[randomIdx]);
                    // Determine exams color degree necessary for sorting ExamInfo array by color degree
                    examMoveStatistics->determineExamsColorDegree();
                    // Determine thresholds based on the max number of evaluations
                    examMoveStatistics->generateThresholds();


                    //
                    // Local search used: Threshold Accepting algorithm
                    //

                    //
                    // moTA with statistics
                    //
                    // moTA parameters
                    boost::shared_ptr<ETTPKempeChainHeuristicWithStatistics<EOT> > kempeChainHeuristic(
                                new ETTPKempeChainHeuristicWithStatistics<EOT>());
//                    eoETTPEval<eoChromosome> fullEval; // eoEvalFunc used to evaluate the solutions
                    ETTPNeighborhoodWithStatistics<EOT> neighborhood(kempeChainHeuristic);
                    ETTPneighborEvalWithStatistics<EOT> neighEval;
                    moTAWithStatistics<ETTPneighborWithStatistics<EOT> > ta(
                                *examMoveStatistics, neighborhood, eval, neighEval, cool);
//                    moTA<ETTPneighbor<EOT> > ta(neighborhood, eval, neighEval, cool);
                    ta(improvedFrog);

                    delete examMoveStatistics;
                    /////////////////////////////////////////////////////////////


                    //
                    // Improvement by Local search.
                    // Local search used: Great Deluge algorithm
                    //
                    // moGDA parameters
//                    boost::shared_ptr<ETTPKempeChainHeuristic<EOT> > kempeChainHeuristic(new ETTPKempeChainHeuristic<EOT>());
//                    ETTPneighborhood<EOT> neighborhood(kempeChainHeuristic);
//                    ETTPneighborEval<EOT> neighEval;



                    // ETTPneighborEvalWithStatistics which receives as argument an
                    // eoNumberEvalsCounter for counting neigbour # evaluations
//                    ETTPneighborEvalNumEvalsCounter<EOT> neighEval(this->numEvalsCounter);


//                    moGDA<ETTPneighbor<EOT> > gda(neighborhood, eval, neighEval, up);
//                    gda(improvedFrog);


//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.001, 5, 2e-4); // FAST cGA
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.0001, 5, 2e-4); //
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.01, 5, 2e-4); // NOT BAD - FAST
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(2, 0.001, 5, 2e-4); // REASONABLE
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.0001, 5, 2e-4); // GOOD BUT SLOW
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(20, 0.0001, 5, 2e-4); // GOOD BUT SLOW
////                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.005, 10, 2e-4);
////                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.0001, 10, 2e-4); // GOOD BUT SLOW


//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.0001, 5, 1e-7);

//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.001, 5, 1e-7);

//                     moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.00001, 5, 1e-7); // SLOW
                                                                                                // Dataset 1: 5400

//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.00005, 5, 1e-6);
                                                                                                // Dataset 1: 6318
                                                                                                // Dataset 2: 471
                                                                                                // Dataset 3: 12893
                                                                                                // Dataset 4: 13154
                                                                                                // Dataset 5: 3403
                                                                                                // Dataset 6: 26860
                                                                                                // Dataset 7: 5525
                                                                                                // Dataset 8: 8601
                                                                                                // Dataset 9: 1085
                                                                                                // Dataset 10: 14006
                                                                                                // Dataset 11: 36841
                                                                                                // Dataset 12: 5167

                    /// NOT VERIFIED RESULTS....


                    /// WITH CHANGED ACCEPTANCE CRITERION SA - NOT GOOD
                    ///
                    // WITH SHIFT MOVE + ROOM MOVE
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(1000, 0.01, 5, 0.01);  // Dataset 1: 13910
                                                                                                // Dataset 2: 3000
                                                                                                // Dataset 4: 15300
                                                                                                // Dataset 6: 28180

//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(1000, 0.001, 5, 0.001); // Dataset 1: 8314
                    //////////////////////////////////////////////////////////////////////////////////////////                                                                                               // Dataset 2: 831

                    // WITH SHIFT MOVE + ROOM MOVE
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.00001, 5, 1e-6); // VERY GOOD, BUT SLOW
                                                                                                // Dataset 1: 5466
                                                                                                // Dataset 2: 440
                                                                                                // Dataset 3: 10731
                                                                                                // Dataset 4: 13910
                                                                                                // Dataset 5: 3641
                                                                                                // Dataset 6: 26190
                                                                                                // Dataset 7: 4601
                                                                                                // Dataset 8: 8141
                                                                                                // Dataset 9: 1014
                                                                                                // Dataset 10: 13805
                                                                                                // Dataset 11: 31461
                                                                                                // Dataset 12: 5223
                    //////////////////////////////////////////////////////////////////////////////////
                    /// SCEA+TA + SHIFT MOVE + ROOM MOVE
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.0001, 5, 2e-6); // FAST
                                                                                            // Dataset 1: 5935
                                                                                            // Dataset 2: 475
                                                                                            // Dataset 3: 11067
                                                                                            // Dataset 4: 15542
                                                                                            // Dataset 5: 3569
                                                                                            // Dataset 6: 26390
                                                                                            // Dataset 7: 5243
                                                                                            // Dataset 8: 8662
                                                                                            // Dataset 9: 1064
                                                                                            // Dataset 10: 13958
                                                                                            // Dataset 11: 36604
                                                                                            // Dataset 12: 5208

                    /// SCEA+TA + SHIFT MOVE ONLY
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(100, 0.0001, 5, 2e-6); // FAST
                                                                                            // Dataset 1: 6118
                                                                                            // Dataset 2: 450
                                                                                            // Dataset 3: 12062
                                                                                            // Dataset 4: 15437
                                                                                            // Dataset 5: 3649
                                                                                            // Dataset 6: 26995
                                                                                            // Dataset 7: 5146
                                                                                            // Dataset 8: 8596
                                                                                            // Dataset 9: 1099
                                                                                            // Dataset 10: 14421
                                                                                            // Dataset 11: 38899
                                                                                            // Dataset 12: 5192


                    //////////////////////////////////////////////////////////////////////////////////

                    // WITH SHIFT MOVE ONLY
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.00001, 5, 1e-6); // VERY GOOD, BUT SLOW
                    moSimpleCoolingSchedule<eoChromosome> coolSchedule1(0.01, 0.00001, 5, 1e-6); // VERY GOOD, BUT SLOW
                                                                                                // Dataset 1: 5376
                                                                                                // Dataset 2: 420
                                                                                                // Dataset 3: 10307
                                                                                                // Dataset 4: 12454
                                                                                                // Dataset 5: 3232
                                                                                                // Dataset 6: 26315
                                                                                                // Dataset 7: 4713
                                                                                                // Dataset 8: 8103
                                                                                                // Dataset 9: 1012
                                                                                                // Dataset 10: 14365
                                                                                                // Dataset 11: 31747
                                                                                                // Dataset 12: 5178


                    // TA
                    moSimpleCoolingSchedule<eoChromosome> coolSchedule2(100, 0.00001, 5, 2e-6);


//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule2(100, 0.0001, 5, 2e-6);


//                    /*moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.0001, 5, 1e-7); // GOOD*/
                                                                                              // Dataset 1: 6358
                                                                                              // Dataset 2: 495, 470
                                                                                              // Dataset 3: 12553, 12032
                                                                                              // Dataset 4: 13678, 12976
                                                                                              // Dataset 5: 3688
                                                                                              // Dataset 9: 1132
                                                                                              // Dataset 12: 5162, 5175


//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.0001, 5, 1e-6); // GOOD
                                                                                            // Dataset 1:
                                                                                            // Dataset 2:
                                                                                            // Dataset 3:
                                                                                            // Dataset 4:
                                                                                            // Dataset 5:
                                                                                            // Dataset 9:
                                                                                            // Dataset 12:


/////////////////////////////////////
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.000001, 5, 1e-7);
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.00001, 5, 1e-7);
//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.01, 0.0001, 5, 1e-7);
////                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.00005, 5, 1e-7); // LAST
////                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.1, 5, 1e-7);

//                    if (rng.uniform() <= 0.5) {

//                        // Copy of cool schedule to use in TA solver
//                        auto cool = coolSchedule2;

//                        moTA<ETTPneighbor<EOT> > ta(neighborhood, eval, neighEval, cool);
//                        ta(improvedFrog);
//                    }
//                    else {

//                    moSimpleCoolingSchedule<eoChromosome> coolSchedule(0.1, 0.0001, 5, 1e-7);

//                    // Copy of cool schedule to use in SA solver
//                        auto cool = coolSchedule1;

//                        moSA<ETTPneighbor<EOT> > sa(neighborhood, eval, neighEval, cool);
//                        sa(improvedFrog);
//                    }


                    // APPROACH 1

//                    // LAST - JUSTIFY UNDERSTAND REASONING... HAVE MORE DIVERSITY THAN APPROACH 2
//                    if (improvedFrog.fitness() < subMemeplex[randomIdx].fitness() &&
//                        improvedFrog.fitness() < subMemeplex[q-1].fitness()) {
//                       //  If improves the worst frog then replace it
////                        subMemeplex[q-1] = improvedFrog;
//                        replaceWorstFrog(improvedFrog, subMemeplex);
//                    }
                    // ADDED 10-jun - WORSE RESULTS
//                    else {
//                        // Randomly generate a solution to replace the worst frog with another frog having any arbitrary fitness
//                        EOT newFrogRandom = generateRandomFrogPosition();
//                        replaceWorstFrog(newFrogRandom, subMemeplex);
//                    }



                    // APPROACH 2 - USED IN RUN 1

                    if (improvedFrog.fitness() < subMemeplex[q-1].fitness()) {
                       //  If improves the worst frog then replace it
//                        subMemeplex[q-1] = improvedFrog;
                        replaceWorstFrog(improvedFrog, subMemeplex); // Maintains memeplex order
                    }

//                    else {
//                        // Randomly generate a solution to replace the worst frog with another frog having any arbitrary fitness
//                        EOT newFrogRandom = generateRandomFrogPosition();
//                        replaceWorstFrog(newFrogRandom, subMemeplex);
//                    }

//                    subMemeplex[randomIdx] = improvedFrog;
//                    sort(_memeplexes[im].begin(), _memeplexes[im].end(), FitnessCmp());

                    // Just replace worst frog
//                    subMemeplex[q-1] = improvedFrog;// All solutions tend to be equal, Uta 3.58 Up = 0.00001


                }
            }


/*
                // Step 4-3 Improve the worst frog’s position.
                POT newFrog = improveWorstFrogPosition(Pb, Pw);

                if (newFrog.fitness() < Pw.fitness()) {
                    replaceWorstFrog(newFrog, subMemeplex);
                }
                else {
                    // Randomly generate a solution to replace the worst frog with another frog having any arbitrary fitness
                    POT newFrogRandom = generateRandomFrogPosition();
                    replaceWorstFrog(newFrogRandom, subMemeplex);
                }

                // Step 4-4 If this process produces a better frog (solution), it replaces the worst frog.
                // Otherwise, the calculations in Eqs. 5 and 6 are repeated with respect to the global best frog (i.e., Pg replaces Pb).
                // Step 4-5 If no improvement becomes possible in this latter case, then a new solution is
                // randomly generated to replace the worst frog with another frog having any arbitrary fitness (as shown in Fig. 3b).
                if (newFrog.fitness() < Pw.fitness()) {
                    replaceWorstFrog(newFrog, subMemeplex);
                }
                else {
                    // Repeat computations with respect to the global best frog (i.e., Pg replaces Pb).
                    POT newFrog = improveWorstFrogPosition(Pg, Pw);
                    if (newFrog.fitness() < Pw.fitness()) {
                        replaceWorstFrog(newFrog, subMemeplex);
                    }
                    else {
                        // Randomly generate a solution to replace the worst frog with another frog having any arbitrary fitness
                        POT newFrogRandom = generateRandomFrogPosition();
                        replaceWorstFrog(newFrogRandom, subMemeplex);
                    }
                }

                // Update Global frog
//                POT bestMemeplexFrog = getMemeplexBestFrog(subMemeplex);
//                if (bestMemeplexFrog.fitness() < Pg.fitness())
//                    updateGlobalBestFrog(bestMemeplexFrog);

//                listMemeplex(subMemeplex);


//            sort(_memeplexes[im].begin(), _memeplexes[im].end(), FitnessCmp()); // don't know if necessary....
            // Update Global frog
//            POT bestMemeplexFrog = getMemeplexBestFrog(subMemeplex);
//            if (bestMemeplexFrog.fitness() < Pg.fitness())
//                updateGlobalBestFrog(bestMemeplexFrog);

*/
        }

/*
//            cout << "[After improvement] Sorted submemeplex fitnesses" << endl;

//            for (int i = 0; i < subMemeplex.size(); ++i) {
//                cout << subMemeplex[i].fitness() << endl;
//            }

//            cout << "After iterating submemeplex" << endl;
//            copy(marked.begin(), marked.end(), ostream_iterator<bool>(cout, " "));
//            cout << endl;
//            copy(randomIdxs.begin(), randomIdxs.end(), ostream_iterator<int>(cout, " "));
//            cout << endl;

             // REMOVED 31-Out-2013

            // Upgrade the memeplex
//            int n = N, k = 0;
////            for (int i = 1; i <= n; ++i) {
//            for (int i = 1; i <= q; ++i) {
////                if (marked[i-1]) {
//                    int frogIdx = randomIdxs[i-1];
////                    cout << "frogIdx = " << frogIdx << endl;
//                    _memeplexes[im][frogIdx] = subMemeplex[k++];
////                    _memeplexes[im][frogIdx-1] = subMemeplex[k++];
////                }
//            }
*/
    }


    EOT generateRandomFrogPosition() {
//        boost::shared_ptr<POT> ptrNewFrogRandom(new POT());
        EOT newFrogRandom = EOT();
        init(newFrogRandom);
        eval(newFrogRandom);

        return newFrogRandom;
    }


    void replaceWorstFrog(EOT& newFrog, vector<EOT>& _memeplex) {
        // Replace worst frog by the new one and then orderly relocate new frog in the memeplex vector
        _memeplex[_memeplex.size()-1] = newFrog;
        int i;
        for (i = _memeplex.size()-2; i >= 0; --i) {
            if (_memeplex[i].fitness() > _memeplex[i+1].fitness()) {
                // Swap entries
                EOT aux = _memeplex[i];
                _memeplex[i] = _memeplex[i+1];
                _memeplex[i+1] = aux;
            }
            else // Stop. The memeplex is ordered.
                break;
        }
        // Update the global frog
        if (_memeplex[0].fitness() < Pg.fitness())
            updateGlobalBestFrog(_memeplex[0]);
    }


    EOT improveWorstFrogPosition(EOT const& _Pb, EOT const& _Pw) {
        EOT improvedFrog = _Pw;
        chromEvolOperator(improvedFrog, _Pb);
        // Evaluate new frog
        eval(improvedFrog);
        return improvedFrog;
    }



/*
    // Used in submemplex
    double triangular(double a,double b,double c) {
//       double U = rand() / (double) RAND_MAX;
        double U = rng.uniform();
        double F = (c - a) / (b - a);

        // cout << "U = " << U << "F = " << F << endl;

        if (U <= F)
          return a + sqrt(U * (b - a) * (c - a));
        else
          return b - sqrt((1 - U) * (b - a) * (b - c));
    }

    pair<vector<EOT>, vector<int> > createSubMemeplex(vector<EOT>& _memeplex, int _q, vector<bool>& marked) {
        int n = _memeplex.size();
        // Create a vector of n random indexes
        vector<int> randomIdxs;
//        for (int i = 1; i <= n; ++i)
//            randomIdxs[i-1] = i;
//        random_shuffle(randomIdxs.begin(), randomIdxs.end());

////        copy(randomIdxs.begin(), randomIdxs.end(), ostream_iterator<int>(cout, " "));


//        // Create a boolean vector in order to mark chosen positions
////        vector<bool> marked(n);
//        // Create submemeplex
        vector<EOT> submemeplex(_q);
//        int k = 0;
//        double sum = 0;
//        // Select q random frogs to form the submemeplex
//        for (int i = 1; i <= n; ++i) {
////            int idx = randomIdxs[i-1];
////            // Index of frog in original memeplex
////            int j = idx;
//////            cout << "j = " << j << ", n = " << n << endl;


////            // Individual probability of being chosen
////            double pj = (double)(2*(n+1-j)) / (n*(n+1));
////            sum += pj;
////            cout << "pj = " << pj << endl;
////            double r = rng.uniform();
////            cout << "r = " << r << endl;


//            if (r < pj) {
//                // Select individual
//                submemeplex[k++] = _memeplex[j-1];
//                marked[i-1] = true;
//            }
//            if (k == _q)
//                break; // We have selected q frogs
//        }

////        cout << "sum = " << sum << endl;
////        cout << "triangular = " << triangular(1, n, (n-10)) << endl;


//        // If there weren't chosen q frogs select the remainder manually
////        if (k < _q) {
////            for (int i = 1; i <= n; ++i) {
////                if (marked[i-1]) {
////                    int idx = randomIdxs[i-1];
////                    int j = idx;
////                    // Select individual
////                    submemeplex[k++] = _memeplex[j-1];
////                    marked[i-1] = true;
////                }
////                if (k == _q)
////                    break; // We have selected q frogs
////            }
////        }

////        cout << "k = " << k << ", q = " << _q << endl;

////        cout << "k == q: " << (k == _q) << endl;


        int numSelected = 0;
        int k = 0;
        while (numSelected < _q) {
            int idx = (int)triangular(1, n, 1);

            // cout << "idx = " << idx << endl;

            if (!marked[idx]) {
                marked[idx] = true;
                ++numSelected;
                randomIdxs.push_back(idx);
//                submemeplex[k++] = _memeplex[idx-1]; // humm
                submemeplex[k++] = _memeplex[idx];
            }
        }
//        cout << "done " << endl;
//        copy(marked.begin(), marked.end(), ostream_iterator<bool>(cout, " "));
//        cout << endl;

        sort(submemeplex.begin(), submemeplex.end(), FitnessCmp());

        return pair<vector<EOT>, vector<int> >(submemeplex, randomIdxs);
    }
*/

protected:
    /**
     * @brief _testSet
     */
    TestSet const& testSet;
    /**
     * @brief _outputDir
     */
    std::string const& outputDir;
    /**
     * @brief _numBins
     */
    int numBins;
    /**
     * @brief outFile
     */
    ofstream & outFile;
    /**
     * @brief filename
     */
    string const& filename;
    // m is the number of memeplexes
    int m;
    // N is the number of frogs in each memeplex
    int N;
    // q is the number of frogs in each submemeplex (q < N)
    int q;
    // The total sample size, F, in the swamp is given by F = mN
    int F;
    // Number of time loops
    long L;
    // Great Deluge Algorithm parameter
    double up;
    // Crossover, mutation and improve probabilities
    double cp, mp, ip;
    // SFLA Initializer
    //eoSFLAInitializer<POT>& init;
    eoInit<EOT>& init;
    // Continuator
    eoContinue<EOT>& continuator;
    // Evaluation function
    eoEvalFunc<EOT>& eval;
    eoBF<EOT &, EOT &, bool> & cross;
    eoMonOp<EOT> & mut;
    // Chromosome evolution operator
//    eoBinOp<EOT>& chromEvolOperator;
    // The frog with the global best fitness is identified as Pg.
    EOT Pg;
    // Reference to the best solution
    EOT* bestSolution;
    // Population variance
    double popVariance;
};



#endif // EOSCEA_H
