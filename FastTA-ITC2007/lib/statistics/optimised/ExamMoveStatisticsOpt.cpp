
#include "neighbourhood/statistics/ETTPneighborEvalWithStatistics.h"

#include "ExamMoveStatisticsOpt.h"

#include "testset/TestSetDescription.h"
#include "utils/CurrentDateTime.h"
#include "neighbourhood/ETTPneighborEval.h"
#include "neighbourhood/statistics/ETTPneighborWithStatistics.h"
//#include "neighbourhood/statistics/ETTPneighborEvalNumEvalsCounter.h"
#include "neighbourhood/statistics/ETTPneighborEvalWithStatisticsNumberEvalsCounter.h"

#include "neighbourhood/statistics/ETTPneighborhoodWithStatistics.h"
#include "algorithms/mo/statistics/moTAWithStatisticsOpt.h"
#include "kempeChain/statistics/ETTPKempeChainHeuristicWithStatistics.h"
#include "init/ETTPInit.h"
// Eval functions
#include "eval/eoNumberEvalsCounter.h"
#include "eval/eoETTPEvalNumberEvalsCounter.h"

#include <boost/range/algorithm.hpp>
#include <vector>

#include "algorithms/mo/moTA.h"


#define HIGH_DEGREE_EXAM_INDEX_PERCENTAGE 0.80 // FastTA80
//#define HIGH_DEGREE_EXAM_INDEX_PERCENTAGE 1  // FastTA100
//#define HIGH_DEGREE_EXAM_INDEX_PERCENTAGE 0 // FastTA (not freezing any exam, equivalent to original but using threshold bin structure)


//#define EXAMMOVESTATISTICS_DEBUG

//#define EXAMMOVESTATISTICS_DEBUG1


// Temperature actualization
extern double Temp(double t, double Tmax, double R);
// Determine # evaluations
extern int getSANumberEvaluations(double tmax, double r, double k, double tmin);

/////////////////////////////////////////////////////////////////////////////
//
// Private methods
//

boost::shared_ptr<string> ExamMoveStatisticsOpt::generateFilename() {
    // Creating the output filename
    stringstream sstream;
    sstream << outputDir << "/ExamMoveStatisticsOpt_" << testSet.getName() << "_cool_"
            << coolSchedule.initT << "_" << coolSchedule.alpha << "_"
            << coolSchedule.span << "_" << coolSchedule.finalT << ".txt";
    boost::shared_ptr<string> filename(new string());
    sstream >> *filename.get();
    cout << *filename.get() << endl;
    return filename;
}

// Generate initial and optimized solutions
void ExamMoveStatisticsOpt::generateInitialSolution() {
    // Solution initializer
    ETTPInit<eoChromosome> init(testSet.getTimetableProblemData().get());
    // Generate initial solution
    init(initialSolution);
    // Evaluate solution
    eval(initialSolution);
}

/////////////////////////////////////////////////////////////////////////////

// Ctor
ExamMoveStatisticsOpt::ExamMoveStatisticsOpt(TestSet const& _testSet, string const& _outputDir,
                                       int _numBins,
                                       moSimpleCoolingSchedule<eoChromosome> &_coolSchedule)
    : testSet(_testSet),                                  // Test set
      outputDir(_outputDir),                              // Output directory
      numBins(_numBins),
      coolSchedule(_coolSchedule),                        // Cooling schedule
      thresholdArraySize(numBins+1),
      thresholdArray(new double[thresholdArraySize]()),   // Threshold array has # thresholds equal to # of bins + 1
      outFilename(*generateFilename().get()),
      outFile(outFilename),
      moveCountsPreviousThreshold(testSet.getTimetableProblemData()->getNumExams()),
      moveCountsCurrentThreshold(testSet.getTimetableProblemData()->getNumExams()),
      ptrPreviousCounts(nullptr),
      ptrCurrentCounts(&moveCountsCurrentThreshold),
      currentThresholdIndex(0),
      examDegree(testSet.getTimetableProblemData()->getNumExams()),
      examIndexByColorDegree(testSet.getTimetableProblemData()->getNumExams())
{ }


eoChromosome& ExamMoveStatisticsOpt::getInitialSolution() { return initialSolution; }

eoChromosome& ExamMoveStatisticsOpt::getOptimizedSolution() { return optimizedSolution; }


// Generate thresholds for the specified cooling schedule
void ExamMoveStatisticsOpt::generateThresholds() {
    long maxNumEval;
    double tmax = coolSchedule.initT, r = coolSchedule.alpha,
           k = coolSchedule.span, tmin = coolSchedule.finalT;

    maxNumEval = getSANumberEvaluations(tmax, r, k, tmin);
    // Print max # evaluations to file
    outFile << "numberEvaluations = " << maxNumEval << std::endl;

#ifdef EXAMMOVESTATISTICS_DEBUG
    cout << "tmax = " << tmax << ", r = " << r << ", k = " << k << ", tmin = " << tmin << endl;
    cout << maxNumEval << endl;
#endif
    int numEvalPerThreshold = maxNumEval / numBins;
    long lastBin = maxNumEval % numBins;
    long numEval;
    long totalNumEval = 0;

#ifdef EXAMMOVESTATISTICS_DEBUG
    cout << "maxNumEval = " << maxNumEval << ", numBins = " << numBins
         << ", numEvalPerThreshold = " << numEvalPerThreshold << ", lastBin = " << lastBin << endl;
#endif

    // Add lower bound threshold
    thresholdArray[0] = tmax;

#ifdef EXAMMOVESTATISTICS_DEBUG
    cout << "i = " << 0 << ", threshold = " << thresholdArray[0] << endl;
#endif

    double t = 0, temp = tmax;
    totalNumEval = 0;
    bool continueCycle = true;

    for (int i = 1; i < thresholdArraySize; ++i) {
        long numberEvaluations = 0;
        for (;;) {
            for (int j = 1; j <= k; ++j) {
                ++numberEvaluations;
                if (i == thresholdArraySize-1) { // Last bin
                    if (numberEvaluations >= numEvalPerThreshold + lastBin) {
                        continueCycle = false;
                        break;
                    }
                }
                else {
                    if (numberEvaluations >= numEvalPerThreshold) {
                        continueCycle = false;
                        break;
                    }
                }
            }
            if (continueCycle == false)
                break;

            // Actualize temperature
            ++t;
            temp = Temp(t, tmax, r);
        }
        totalNumEval += numberEvaluations;
#ifdef EXAMMOVESTATISTICS_DEBUG
        cout << "i = " << i << ", numberEvaluations = " << numberEvaluations << ", threshold = " << temp
             << ", totalNumEval = " << totalNumEval << endl;
#endif
        // Add threshold to array
        thresholdArray[i] = temp;
        continueCycle = true;
    }
}




//
// Compute exam move statistics and color map. The color map has on the yy axis
// the exams indexes sorted by conflict degree and on the xx axis the
// TA thresholds bins determined in order to have the same number of evaluations
// on each bin.
//
void ExamMoveStatisticsOpt::run() {
    // Generate initial solution
    generateInitialSolution();
    // Determine thresholds based on the max number of evaluations
    generateThresholds();
    // Determine exams color degree
    determineExamsColorDegree();

    //
    // Local search used: Threshold Accepting algorithm
    //

    //
    // moTA with statistics
    //
    // moTA parameters
    boost::shared_ptr<ETTPKempeChainHeuristicWithStatistics<eoChromosome> > kempeChainHeuristic(
                new ETTPKempeChainHeuristicWithStatistics<eoChromosome>());
    // eoEvalFunc used to evaluate the solutions
//    eoETTPEval<eoChromosome> fullEval;
    // # evaluations counter
    eoNumberEvalsCounter numEvalsCounter;
    // eoETTPEvalWithStatistics used to evaluate the solutions; receives as argument an
    // eoNumberEvalsCounter for counting neigbour # evaluations
    eoETTPEvalNumberEvalsCounter<eoChromosome> fullEval(numEvalsCounter);
    /// CHANGED: RECEIVE PTR
    ETTPNeighborhoodWithStatistics<eoChromosome>neighborhood(kempeChainHeuristic);
//    ETTPneighborEvalWithStatistics<eoChromosome> neighEval; // LAST
    // ETTPneighborEvalWithStatistics which receives as argument an
    // eoNumberEvalsCounter for counting neigbour # evaluations
    ETTPneighborEvalWithStatisticsNumberEvalsCounter<eoChromosome> neighEval(numEvalsCounter);

    moTAWithStatisticsOpt<ETTPneighborWithStatistics<eoChromosome> > ta(*this, neighborhood, fullEval, neighEval, coolSchedule);

    /////// Write to output File ///////////////////////////////////////////
    cout << "Start Date/Time = " << currentDateTime() << endl;
    // Write Start time and algorithm parameters to file
    outFile << "Start Date/Time = " << currentDateTime() << endl;
    outFile << "TA parameters:" << endl;
    outFile << "cooling schedule: " << coolSchedule.initT << ", " << coolSchedule.alpha << ", "
            << coolSchedule.span << ", " << coolSchedule.finalT << endl;

    outFile << testSet << endl;
    outFile << "Exam Move Statistics" << endl;

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


//// Get index in the threshold array given a threshold
//int ExamMoveStatisticsOpt::getThresholdIndex(double _threshold) const {
//    // Threshold array is sorted in descending order. Example: [0.1, 0.01, 0.001, 0.0001, ..., 2e-5]
////    if (_threshold > thresholdArray[0]) // If threshold is greater than the first threshold, print an error
////        cerr << "Error: threshold > lower threshold: " << _threshold << " > " << thresholdArray[0] << endl;

//    // Threshold index
//    int threshIndex;
//    for (threshIndex = 0; threshIndex < thresholdArraySize-1; ++threshIndex) {
//        double upperThresh = thresholdArray[threshIndex+1]; // Get upper threshold
////        cout << "_threshold = " << _threshold << ", upperThresh = " << upperThresh << endl;
//        if (_threshold > upperThresh)  // If threshold value is greater than the upper threshold stop
//            break;
//    }
//    return threshIndex;
//}


/// OPTIMIZED VERSION
// Get index in the threshold array given a threshold
int ExamMoveStatisticsOpt::getThresholdIndex(double _threshold) const {
    // Threshold index
    int threshIndex;
    for (threshIndex = /*0*/currentThresholdIndex; threshIndex < thresholdArraySize-1; ++threshIndex) {
        double upperThresh = thresholdArray[threshIndex+1]; // Get upper threshold
        if (_threshold > upperThresh)  // If threshold value is greater than the upper threshold stop
            break;
    }
    return threshIndex;
}




// Return true if exam is fixed for this threshold
bool ExamMoveStatisticsOpt::isExamFixed(int _exam, double _threshold) {
    /// _threshold parameter not used in this implementation
    ///
    updateThreshold(_threshold);

    if (ptrPreviousCounts == nullptr) {
        return false;
    }
    return (*ptrPreviousCounts)[_exam] == 0;

}


// Increment exame move count
void ExamMoveStatisticsOpt::updateExamMove(int _exam, double _threshold) {
    /// _threshold parameter not used in this implementation
    ///
    (*ptrCurrentCounts)[_exam]++;
}


// Update threshold
void ExamMoveStatisticsOpt::updateThreshold(double _threshold) {
    // Get threshold index
    int thresholdIndex = getThresholdIndex(_threshold);

#ifdef EXAMMOVESTATISTICS_DEBUG1
    std::cout << "updateThreshold" << std::endl;
    std::cout << "thresholdIndex = " << thresholdIndex << std::endl;
    std::cin.get();
#endif

    if (thresholdIndex > currentThresholdIndex) {
        currentThresholdIndex = thresholdIndex;
        // Check if ptrPreviousCounts is null and set it to previous vector
        if (ptrPreviousCounts == nullptr) {
            ptrPreviousCounts = &moveCountsPreviousThreshold;
#ifdef EXAMMOVESTATISTICS_DEBUG1
            std::cout << "ptrPreviousCounts == nullptr, now set to zero vector" << std::endl;
            std::cout << "ptrPreviousCounts: " << endl;
            std::copy((*ptrPreviousCounts).begin(), (*ptrPreviousCounts).end(), ostream_iterator<int>(cout, "\n"));
            std::cin.get();
#endif
        }
        // Swap pointers
        std::vector<int> *auxPtr = ptrPreviousCounts;
        ptrPreviousCounts = ptrCurrentCounts;
        ptrCurrentCounts = auxPtr;
        // Clear ptrCurrentCounts referred vector
        auto &vec = *ptrCurrentCounts;
        std::fill(vec.begin(), vec.end(), 0);

#ifdef EXAMMOVESTATISTICS_DEBUG1
        std::cout << "ptrPreviousCounts: " << endl;
        std::copy((*ptrPreviousCounts).begin(), (*ptrPreviousCounts).end(), ostream_iterator<int>(cout, "\n"));

        std::cout << "ptrCurrentCounts: " << endl;
        std::copy((*ptrCurrentCounts).begin(), (*ptrCurrentCounts).end(), ostream_iterator<int>(cout, "\n"));

        std::cin.get();
#endif
    }
}


// Return true if it is a large degree exam
bool ExamMoveStatisticsOpt::isLargestDegree(int _examToMove) {
    return examIndexByColorDegree[_examToMove] < testSet.getTimetableProblemData()->getNumExams() * HIGH_DEGREE_EXAM_INDEX_PERCENTAGE;
}


// Determine exams color degree necessary for sorting ExamInfo array by color degree
void ExamMoveStatisticsOpt::determineExamsColorDegree() {

    cout << "determineExamsColorDegree()" << endl;

    // Get exam graph
    AdjacencyList const& graph = getInitialSolution().getExamGraph();

    AdjacencyList::vertex_iterator vertexIt, vertexEnd;
//    AdjacencyList::adjacency_iterator neighbourIt, neighbourEnd;
    tie(vertexIt, vertexEnd) = vertices(graph);
    int count = 0, vertexDegree;
    int numVertices = num_vertices(graph);
//    cout << "numVertices = " << numVertices << endl;
//    cin.get();

    for (; vertexIt != vertexEnd; ++vertexIt)
    {
        vertexDegree = degree(*vertexIt, graph);
        // Set exam degree
        examDegree[*vertexIt] = std::make_pair(vertexDegree, *vertexIt);
    }

    // Sort
    sort();
}



class Criterium {
public:
    bool operator()(const std::pair<int,int> &_left, const std::pair<int,int> &_right) const {
        return _left.first > _right.first;
    }
};



// Sort ExamInfo array in descending order by exam conflict degree
void ExamMoveStatisticsOpt::sort() {
    // Create an auxiliary vector from the range (begin, end[ for sorting
    std::vector<std::pair<int,int>>  examInfoVec(examDegree.begin(), examDegree.end());
    // boost::sort
    // For versions of the sort function without a predicate, ascending order is defined by
    // operator<() such that for all adjacent elements [x,y], y < x == false.
    // For versions of the sort function with a predicate, ascending order is defined by pred
    // such that for all adjacent elements [x,y], pred(y, x) == false.
    // Complexity: O(N log(N))
//    boost::sort(examInfoVec, greater<boost::shared_ptr<ExamInfo> >()); // Sort in *descending* order by exam conflict degree
    // Sort in *descending* order by exam conflict degree
    Criterium criterium;
    boost::sort(examInfoVec, [&criterium] (const std::pair<int,int> &_l, const std::pair<int,int> &_r)
        {
            return criterium(_l, _r);
        }
    );

    // Print
//    copy(examInfoVec.begin(), examInfoVec.end(), ostream_iterator<boost::shared_ptr<ExamInfo>>(cout, "\n"));

    // Fill examIndexByColorDegree vector
    int idx = 0;
    for (auto elem : examInfoVec) {
        examIndexByColorDegree[elem.second] = idx;
        ++idx;
    }

}









