#ifndef ETTPNEIGHBOREVALWITHSTATISTICSNUMBEREVALSCOUNTER_H
#define ETTPNEIGHBOREVALWITHSTATISTICSNUMBEREVALSCOUNTER_H

//#include <eval/moEval.h>
//#include <eoEvalFunc.h>
//#include "neighbourhood/statistics/ETTPneighborWithStatistics.h"

#include "neighbourhood/statistics/ETTPneighborEvalWithStatistics.h"
#include "neighbourhood/ETTPneighborEval.h"
#include "eval/eoNumberEvalsCounter.h"


///**
// * @brief Evaluation function used to evaluate the neighbour solution
// */
//template <typename EOT>
//class ETTPneighborEvalWithStatisticsNumberEvalsCounter : public moEval<ETTPneighborWithStatistics<EOT> > {

//public:

//    /**
//     * @brief operator () Eval the _solution moved with the neighbor and stock the result in the neighbor
//     * @param _solution The current solution
//     * @param _neighbor The neighbour solution. The neigbour doesn't contain the timetable data, it only
//     *                  contains the Kempe chain information from which the neighbour solution can be built.
//     */
//    virtual void operator()(typename ETTPneighborWithStatistics<EOT>::EOT &_solution, ETTPneighborWithStatistics<EOT> &_neighbor) {
//        //====================================================
//        //
//        // Incremental evaluation of the neighbour solution
//        //
//        //====================================================

//        // Obtain the Kempe chain object
//        ETTPKempeChain<EOT> const& kempeChain = _neighbor.getKempeChain();

//        // Evaluate move of solution to the neighbour.
//        // This envolves doing:
//        //   - move, temporarily, the solution to the neighbour
//        //   - record ti and tj final time slots
//        //   - evaluate neighbour (incrementally) and set neighbour fitness
//        //   - undo solution move
//        _neighbor.evaluateMove(_solution);

//     }

//};




/**
 * @brief Evaluation function used to evaluate the neighbour solution (with statistics, used in Fast SA)
 */
template <typename EOT>
class ETTPneighborEvalWithStatisticsNumberEvalsCounter : public ETTPneighborEvalWithStatistics<EOT> {

public:

    ETTPneighborEvalWithStatisticsNumberEvalsCounter(eoNumberEvalsCounter &_numberEvalsCounter)
        : numberEvalsCounter(_numberEvalsCounter) { }

    /**
     * @brief operator () Eval the _solution moved with the neighbor and stock the result in the neighbor
     * @param _solution The current solution
     * @param _neighbor The neighbour solution. The neigbour doesn't contain the timetable data, it only
     *                  contains the Kempe chain information from which the neighbour solution can be built.
     */
    virtual void operator()(typename ETTPneighborWithStatistics<EOT>::EOT &_solution, ETTPneighborWithStatistics<EOT> &_neighbor) {
        // Invoke base class method to perform neighbour (incremental) evaluation
        ETTPneighborEvalWithStatistics<EOT>::operator ()(_solution, _neighbor);
        // # evals statistics computation. Add 1 to # evals
        numberEvalsCounter.addNumEvalsToTotal(1);
     }


protected:
    /**
     * @brief numberEvalsCounter
     */
    eoNumberEvalsCounter &numberEvalsCounter;

};




#endif // ETTPNEIGHBOREVALWITHSTATISTICSNUMBEREVALSCOUNTER_H
