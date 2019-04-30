#ifndef MOTAEXPLORER_WITH_STATISTICS_H
#define MOTAEXPLORER_WITH_STATISTICS_H

//#include "statistics/ExamMoveStatistics.h"
#include "statistics/optimised/ExamMoveStatisticsOpt.h"

#include <explorer/moNeighborhoodExplorer.h>
#include <comparator/moSolNeighborComparator.h>
#include <coolingSchedule/moCoolingSchedule.h>
#include <neighborhood/moNeighborhood.h>

// In order to produce statistical info
//#include "neighbourhood/ETTPneighbor.h"
//#include "neighbourhood/statistics/ETTPneighborWithStatistics.h"
#include "neighbourhood/ETTPneighborEval.h"
#include "chromosome/eoChromosome.h"
#include "algorithms/mo/moTAexplorer.h"

#include "neighbourhood/ETTPneighborhood.h"
#include "neighbourhood/statistics/ETTPneighborhoodWithStatistics.h"





/**
 * Explorer for the Threshold Accepting algorithm
 * Fitness must be > 0
 *
 */
template <class Neighbor>
class moTAexplorerWithStatistics : public moTAexplorer<Neighbor>
{

public:

    /**
     * Constructor
     * @param _examMoveStatistics object that keeps statistical information
     * @param _neighborhood the neighborhood
     * @param _eval the evaluation function
     * @param _solNeighborComparator a solution vs neighbor comparator
     * @param _coolingSchedule the cooling schedule
     * @param _qmax the starting threshold
     */
    moTAexplorerWithStatistics(ExamMoveStatistics &_examMoveStatistics,
//    moTAexplorerWithStatistics(ExamMoveStatisticsOpt &_examMoveStatistics,
                typename moTAexplorer<Neighbor>::Neighborhood &_neighborhood,
                moEval<Neighbor> &_eval,
                moSolNeighborComparator<Neighbor> &_solNeighborComparator,
                moCoolingSchedule<typename moTAexplorer<Neighbor>::EOT> &_coolingSchedule)
        : moTAexplorer<Neighbor>(_neighborhood, _eval, _solNeighborComparator, _coolingSchedule),
          examMoveStatistics(_examMoveStatistics)
    {
          this->isAccept = false;

          if (!this->neighborhood.isRandom()) {
              std::cout << "moTAexplorerWithStatistics::Warning -> the neighborhood used is not random" << std::endl;
          }
      }

    /**
     * Destructor
     */
    ~moTAexplorerWithStatistics() { }


//    /**
//     * Explore one random solution in the neighborhood
//     * @param _solution the solution
//     */
//    virtual void operator()(typename moTAexplorer<Neighbor>::EOT & _solution) {
//        // Test if _solution has a Neighbor
//        if (this->neighborhood.hasNeighbor(_solution)) {
//            // Init on the first neighbor: supposed to be random solution in the neighborhood
//            this->neighborhood.init(_solution, this->selectedNeighbor);
//            // Eval the _solution moved with the neighbor and stock the result in the neighbor
//            this->eval(_solution, this->selectedNeighbor);
//        }
//        else {
//            // If _solution hasn't neighbor,
//            this->isAccept = false;
//        }
//    }


    /**
      * Overriden method
      * Explore one random solution in the neighborhood
      * @param _solution the solution
      */
     virtual void operator()(typename moTAexplorer<Neighbor>::EOT & _solution) override {
         // Test if _solution has a Neighbor
         if (this->neighborhood.hasNeighbor(_solution)) {
             //
             // Optimization: Only build the complete Kempe chain and eval the neighbor solution
             // if the selected exam to move, in the neighbour solution,
             // is still moving. If its not moving, there's no need to eval the neighbor
             //

             // Init on the first neighbor: supposed to be random solution in the neighborhood
             this->neighborhood.init(_solution, this->selectedNeighbor);


             int thresholdindex = examMoveStatistics.getThresholdIndex(this->q);

             std::vector<boost::shared_ptr<ExamInfo> > examInfoVector = this->examMoveStatistics.examInfoVector;

             ETTPNeighborhoodWithStatistics<typename moTAexplorer<Neighbor>::EOT>* neighbourhood =
                     static_cast<ETTPNeighborhoodWithStatistics<typename moTAexplorer<Neighbor>::EOT>* >(&this->neighborhood);


             /// FAST TA - SEE WAYS TO OPTIMIZE IT
             ///

             // See index-1
//             int previousThresholdindex = (thresholdindex-1 >= 0) ? thresholdindex-1 : 0;
//             if (previousThresholdindex == 0 ||
//                     (*examInfoVector[neighbourhood->getExamToMove()].get()).getExamMoveCount(previousThresholdindex) > 0) {
//                 // Eval the _solution moved with the neighbor and stock the result in the neighbor
//                 this->eval(_solution, this->selectedNeighbor);
//             }
//             else {
//                 /// TODO - WORK AROUND?
//                 ///
//                 // Eval neighbour to have worst fitness than the current solution in order to no be accepted
//                 this->selectedNeighbor.fitness(_solution.fitness()+10000000);
//             }

            ///
            /// NORMAL TA - USED TO GENERATE PAPER FIGURES
            ///
            this->eval(_solution, this->selectedNeighbor);

         }
         else {
             // If _solution hasn't neighbor,
             this->isAccept = false;
         }
     }




    /**
     * acceptance criterion of Threshold Accepting algorithm
     * @param _solution the solution
     * @return true if f (s′ ) − f (s) ≤ Q
     */
    virtual bool accept(typename moTAexplorerWithStatistics<Neighbor>::EOT & _solution) override {
//        cout << "accept method" << endl;

        // Test if _solution has a Neighbor
        if (this->neighborhood.hasNeighbor(_solution)) {
//            if (solNeighborComparator(_solution, selectedNeighbor)) { // accept if the current neighbor is better than the solution

/// TODO - ADD isFeasible to Neighbor class
///
///
            // Downcast selectedNeighbor to ETTPneigbor
            Neighbor *selectedNeighborPtr = &this->selectedNeighbor;
            ETTPneighbor<typename moTAexplorer<Neighbor>::EOT> *neighbourPtr
                    = (ETTPneighbor<typename moTAexplorer<Neighbor>::EOT> *)selectedNeighborPtr;
            if (neighbourPtr != nullptr && !neighbourPtr->isFeasible()) {
                this->isAccept = false;
#ifdef MOTAEXPLORER_DEBUG
            std::cout << "In [moTAexplorer::accept(sol)] method:" << std::endl;
            std::cout << "Infeasible solution, it will not be accepted. Generating a new one..." << std::endl;
#endif
                return this->isAccept;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            /// TA
            ///
            double e = this->selectedNeighbor.fitness() - _solution.fitness();

            if (e <= this->q) { // Minimization problem
                this->isAccept = true;

                // Static cast
                ETTPneighborWithStatistics<eoChromosome>* neigh =
                        static_cast<ETTPneighborWithStatistics<eoChromosome>* >(&this->selectedNeighbor);


//                cout << "Ei = " << neigh->movedExam.exam << ", ti = " << neigh->movedExam.sourcePeriod <<
//                        ", tj = " << neigh->movedExam.destPeriod << endl;

                examMoveStatistics.updateExamMove(neigh->getMovedExam().exam, this->q);

            }
            else {
                this->isAccept = false;
            }

#ifdef MOTAEXPLORER_DEBUG
            std::cout << "In [moTAexplorer::accept(sol)] method:" << std::endl;
            std::cout << "solution: " << _solution.fitness() << " neighbour: " << selectedNeighbor.fitness()
                      << ", q = " << q << std::endl;
            std::cout << "Accept solution? " << isAccept << std::endl;
#endif


//            ////////////////////////////////////////////////////////////////////////////////////
//            /// SA
//            ///
//            double fit1, fit2;
//            fit1 = (double)this->selectedNeighbor.fitness();
//            fit2 = (double)_solution.fitness();

//            //                       cout << "fit1 (neighbour) = " << fit1 << " fit2 (current solution) = " << fit2
//            //                            << ", T = " << this->temperature << endl;


//            // Static cast
//            ETTPneighborWithStatistics<eoChromosome>* neigh =
//                    static_cast<ETTPneighborWithStatistics<eoChromosome>* >(&this->selectedNeighbor);

//            if (this->selectedNeighbor.fitness() < _solution.fitness()) { // Minimization problem
//                examMoveStatistics.updateExamMove(neigh->getMovedExam().exam, this->q);
//                this->isAccept = true;
//            //                cout << "accept because the current neighbor is better than the solution" << endl;
//            }
//            else {
//               double alpha = 0.0;
//            //                double fit1, fit2;
//            //                fit1 = (double)selectedNeighbor.fitness();
//            //                fit2 = (double)_solution.fitness();

//            //                cout << "temperature = " << temperature << endl;
//            //                cout << "fit1 (neighbour) = " << fit1 << endl;
//            //                cout << "fit2 (solution)  = " << fit2 << endl;

//            //                cin.get();

//            //                if (fit1 < fit2) // this is a maximization
//            //                    alpha = exp((fit1 - fit2) / temperature );
//            //                else // this is a minimization
//            //                    alpha = exp((fit2 - fit1) / temperature );
//            //                isAccept = (rng.uniform() < alpha);

//            //                alpha = exp((fit2 - fit1) / (fit2*0.01) ); // 40.56
//            //                alpha = exp((fit2 - fit1) / (fit2*2) ); //
//               double temperature = this->q;
//               alpha = exp((fit2 - fit1) / (fit2*temperature) );
//            //                alpha = exp((fit2 - fit1) / temperature );

//               double r = rng.uniform();

//            //                cout << "temperature = " << temperature << endl;
//            //                cout << "alpha = " << alpha << endl;
//            //                cout << "rand = " << r << endl;


//               this->isAccept = (r < alpha);
////               if (this->isAccept && fit1 < 10000000)
//               if (this->isAccept)
//                    examMoveStatistics.updateExamMove(neigh->getMovedExam().exam, this->q);

//            }


        }
        return this->isAccept;
    }




private:
    // Added in order to produce statistical info
    ExamMoveStatistics &examMoveStatistics;
};


#endif // MOTAEXPLORER_WITH_STATISTICS_H
