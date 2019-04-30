#ifndef EXAMMOVESTATISTICSOPT_H
#define EXAMMOVESTATISTICSOPT_H

#include <eo>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/make_shared.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "chromosome/eoChromosome.h"
#include "algorithms/mo/moSimpleCoolingSchedule.h"
#include "statistics/optimised/ExamInfoOpt.h"
#include "testset/TestSet.h"
#include "eval/eoETTPEval.h"

#include <boost/unordered_map.hpp>

class ExamMoveStatisticsOpt {

public:
    /**
     * @brief ExamMoveStatisticsOpt
     * @param _numThresholds
     * @param _coolSchedule
     * @return
     */
    ExamMoveStatisticsOpt(TestSet const &_testSet, std::string const& _outputDir,
                       int _numBins, moSimpleCoolingSchedule<eoChromosome> & _coolSchedule);

    //
    // Public interface
    //
    inline eoChromosome &getInitialSolution();
    inline eoChromosome &getOptimizedSolution();

    // Generate thresholds for the specified cooling schedule
    void generateThresholds();

    void run();

    // Get index in the threshold array given a threshold
    int getThresholdIndex(double _threshold) const;
    // Return true if exam is fixed for this threshold
    bool isExamFixed(int _exam, double _threshold);
    // Increment exame move count
    void updateExamMove(int _exam, double _threshold);
    // Update threshold
    void updateThreshold(double _threshold);
    void setPtrInitialSolution(eoChromosome *_ptrInitialSolution);
    // Return true if it is a large degree exam
    bool isLargestDegree(int _examToMove);
    // Determine exams color degree
    void determineExamsColorDegree();
    // Sort
    void sort();

private:
    boost::shared_ptr<std::string> generateFilename();
    // Generate initial and optimized solutions
    void generateInitialSolution();

    //
    // Fields
    //
    eoETTPEval<eoChromosome> eval;              // Objective function evaluation
    eoChromosome initialSolution;               // Initial solution
    eoChromosome *ptrInitialSolution;           // Initial solution
    eoChromosome optimizedSolution;             // Optimized solution
    TestSet const& testSet;                     // Test set
    std::string const& outputDir;               // Output directory

    // Colling schedule used to generate the statistics
    moSimpleCoolingSchedule<eoChromosome> &coolSchedule;
    int numBins;                                // Threshold array has # thresholds equal to # of bins + 1
    int thresholdArraySize;
    boost::shared_array<double> thresholdArray; // Threshold array sorted in descending order
    std::string outFilename;                    // Output filename
    std::ofstream outFile;                      // Output file

    std::vector<int> moveCountsPreviousThreshold; // Exam move counts for the previous threshold
    std::vector<int> moveCountsCurrentThreshold;  // Exam move counts for the current threshold

    std::vector<int> *ptrPreviousCounts;
    std::vector<int> *ptrCurrentCounts;

    int currentThresholdIndex;
    // Exam color degree
    std::vector<std::pair<int,int>> examDegree;
    // Exam index sorted by color degree
    std::vector<int> examIndexByColorDegree;
};



inline void ExamMoveStatisticsOpt::setPtrInitialSolution(eoChromosome *_ptrInitialSolution) {
    ptrInitialSolution = _ptrInitialSolution;
}



#endif // EXAMMOVESTATISTICSOPT_H
