#ifndef ETTPNEIGHBORWITHSTATISTICS_H
#define ETTPNEIGHBORWITHSTATISTICS_H


#include "neighbourhood/ETTPneighbor.h"
#include "statistics/MovedExam.h"

#include "kempeChain/statistics/ETTPKempeChainHeuristicWithStatistics.h"



/**
 * Neighbor computation for the ETTP problem (used in local search)
 *
 * This neighbor computes statistics about moved exams
 *
 */
template<class EOT, class Fitness = typename EOT::Fitness>
class ETTPneighborWithStatistics : virtual public moNeighbor<EOT, typename EOT::Fitness> {

public:
    using moNeighbor<EOT, typename EOT::Fitness>::fitness;

    /**
     * Return the class Name
     * @return the class name as a std::string
     */
    virtual std::string className() const override {
        return "ETTPneighborWithStatistics";
    }


    /**
     * +Overriden method
     *
     * Apply move on a solution
     * @param _sol the solution to move
     */
    virtual void move(EOT& _sol) override {
        //
        // Move current solution _sol to this neighbour using
        // information maintained in the KempeChain structure
        //
        (*kempeChainHeuristic.get())(_sol); // Polymorphic behaviour
    }


    /**
     * @brief build Build neighbour. This consists in using kempe chain heuristic
     * to create a kempe chain for a random move
     * @param _sol
     */
    virtual void build(EOT& _sol) {
        (*kempeChainHeuristic.get()).build(_sol);
    }

    /**
     * @brief getKempeChain
     * @return The Kempe chain object relating the neighbor and the current solution
     */
    ETTPKempeChain<EOT> const& getKempeChain() {
        return kempeChainHeuristic->getKempeChain();
    }

    /**
     * @brief setKempeChainHeuristic
     * @param _kempeChainHeuristic
     */
//    void setKempeChainHeuristic(boost::shared_ptr<ETTPKempeChainHeuristic<EOT> > const &_kempeChainHeuristic) {
//        kempeChainHeuristic = _kempeChainHeuristic;
//    }
    void setKempeChainHeuristic(boost::shared_ptr<ETTPKempeChainHeuristicWithStatistics<EOT> > const &_kempeChainHeuristic) {
        kempeChainHeuristic = _kempeChainHeuristic;
    }

    /**
     * @brief getSolutionCost
     * @return
     */
    long getSolutionCost() const {
        return kempeChainHeuristic->getNeighborSolutionCost();
    }

    /**
     * @brief isFeasible
     * @return
     */
    bool isFeasible() const {
        // Return true if the neighbour built in the Kempe chain heuristic is feasible and return false otherwise.
        return kempeChainHeuristic->isFeasibleNeighbour();
    }


    /**
     * @brief evaluateMove Evaluate move of solution to the neighbour.
     * This envolves doing:
     *   - move, temporarily, the solution to the neighbour
     *   - record ti and tj final time slots
     *   - evaluate neighbour and set neighbour fitness
     *   - undo solution move
     * @param _sol
     */
    virtual void evaluateMove(EOT &_sol) {
        double neighFitness;
        // Evaluate move of solution to the neighbour
        kempeChainHeuristic->evaluateSolutionMove(_sol);
        // Get neighbor fitness
        neighFitness = kempeChainHeuristic->getNeighborFitness();
        // Set neighbor fitness
        fitness(neighFitness);
        ////////////////////////////////////
        // Compute statistical info about move exams
        //
        // Get moved exam
        //
        ////////////////////////////////////

//        ETTPKempeChainHeuristic<EOT>* mykempeChainHeuristic = kempeChainHeuristic.get();

//        ETTPKempeChainHeuristicWithStatistics<EOT>* kempeChainHeuristicWithStat =
//            static_cast<ETTPKempeChainHeuristicWithStatistics<EOT>* >(mykempeChainHeuristic);

//        if (kempeChainHeuristicWithStat == nullptr) {
//            throw runtime_error("kempeChainHeuristicWithStat == nullptr");
//        }

        if (kempeChainHeuristic->isFeasibleNeighbour()) {
//            ETTPKempeChainHeuristicWithStatistics<EOT>* kempeChainHeuristicWithStat = kempeChainHeuristic.get();
//            movedExam = kempeChainHeuristicWithStat->getMoveExamInfo();
            movedExam.exam = kempeChainHeuristic->getKempeChain().getEi();
            movedExam.sourcePeriod = kempeChainHeuristic->getKempeChain().getTi();
            movedExam.destPeriod = kempeChainHeuristic->getKempeChain().getTj();

        }
    }


    /**
     * @brief getMovedExam
     * @return Returns the MovedExam object
     */
    MovedExam getMovedExam() const
    {
        return movedExam;
    }

protected:

    /**
     * @brief kempeChainHeuristic Kempe Chain neighbourhood
     */
//    boost::shared_ptr<ETTPKempeChainHeuristic<EOT> > kempeChainHeuristic;
    boost::shared_ptr<ETTPKempeChainHeuristicWithStatistics<EOT> > kempeChainHeuristic;
    /**
     * @brief movedExam The moved exam
     */
    MovedExam movedExam;
};







///**
// * Neighbor computation for the ETTP problem (used in local search)
// *
// * This neighbor computes statistics about moved exams
// *
// */
//template<class EOT, class Fitness = typename EOT::Fitness>
//class ETTPneighborWithStatistics : public ETTPneighbor<EOT> {

//public:

//    /**
//     * Return the class Name
//     * @return the class name as a std::string
//     */
//    virtual std::string className() const override {
//        return "ETTPneighborWithStatistics";
//    }


//    virtual void evaluateMove(EOT &_sol) override {
//        // Invoke base class method
//        this->evaluateMove(_sol);
//        ////////////////////////////////////
//        // Compute statistical info about move exams
//        //
//        // Get moved exam
//        //
//        ////////////////////////////////////

//        ETTPKempeChainHeuristicWithStatistics<EOT>* kempeChainHeuristicWithStat =
//            static_cast<ETTPKempeChainHeuristicWithStatistics<EOT>* >(this->kempeChainHeuristic.get());

////        movedExam = this->kempeChainHeuristic->getMoveExamInfo();
//        if (kempeChainHeuristicWithStat == nullptr) {
//            throw runtime_error("kempeChainHeuristicWithStat == nullptr");
//        }

//        movedExam = kempeChainHeuristicWithStat->getMoveExamInfo();
//    }

//    /**
//     * @brief getMovedExam
//     * @return Returns the MovedExam object
//     */
//    MovedExam getMovedExam() const
//    {
//        return movedExam;
//    }


//protected:
//    MovedExam movedExam;

//};



#endif // ETTPNEIGHBORWITHSTATISTICS_H

