#ifndef MOSAEXPLORER_H
#define MOSAEXPLORER_H


#include <comparator/moSolNeighborComparator.h>
#include <coolingSchedule/moCoolingSchedule.h>
#include <neighborhood/moNeighborhood.h>

#include "neighbourhood/ETTPNeighborhoodExplorer.h"


#include "neighbourhood/ETTPneighbor.h"


//#define MOSAEXPLORER_DEBUG


/**
 * Explorer for the Simulated Annealing algorithm
 * Fitness must be > 0
 *
 */
template <class Neighbor>
class moSAexplorer : public ETTPNeighborhoodExplorer<Neighbor>
{
public:
    typedef typename Neighbor::EOT EOT;
    typedef moNeighborhood<Neighbor> Neighborhood;

    using moNeighborhoodExplorer<Neighbor>::neighborhood;
    using moNeighborhoodExplorer<Neighbor>::eval;
    using moNeighborhoodExplorer<Neighbor>::selectedNeighbor;

    /**
     * Constructor
     * @param _neighborhood the neighborhood
     * @param _eval the evaluation function
     * @param _solNeighborComparator a solution vs neighbor comparator
     * @param _coolingSchedule the cooling schedule
     */

  moSAexplorer(Neighborhood& _neighborhood, moEval<Neighbor>& _eval,
               moSolNeighborComparator<Neighbor>& _solNeighborComparator, moCoolingSchedule<EOT>& _coolingSchedule)
      : ETTPNeighborhoodExplorer<Neighbor>(_neighborhood, _eval),
        solNeighborComparator(_solNeighborComparator), coolingSchedule(_coolingSchedule),
        outFile("sa_plot_data.txt", std::ofstream::out), acceptedEvals(1), evals(0)
  {
        isAccept = false;

        if (!neighborhood.isRandom()) {
            std::cout << "moSAexplorer::Warning -> the neighborhood used is not random" << std::endl;
        }
    }

    /**
     * Destructor
     */
    ~moSAexplorer() {
        outFile << "# evals = feasible neighbours = " << evals;
        outFile.close();
    }


    /**
     * initialization of the initial temperature
     * @param _solution the solution
     */
    virtual void initParam(EOT & _solution) {
        q = coolingSchedule.init(_solution);  // q = Qmax, the starting threshold

        isAccept = false;

        //        cout << "Initial solution: " << _solution.fitness() << endl;

    }

    /**
     * decrease the temperature if necessary
     * @param _solution unused solution
     */
    virtual void updateParam(EOT & _solution) {
        // q = g(q); // Threshold update
        coolingSchedule.update(q, this->moveApplied());
    }

    /**
     * terminate: NOTHING TO DO
     * @param _solution unused solution
     */
    virtual void terminate(EOT & _solution) { }

    /**
     * Explore one random solution in the neighborhood
     * @param _solution the solution
     */
    virtual void operator()(EOT & _solution) {
        // Test if _solution has a Neighbor
        if (neighborhood.hasNeighbor(_solution)) {
            // Init on the first neighbor: supposed to be random solution in the neighborhood
            neighborhood.init(_solution, selectedNeighbor);
            // Eval the _solution moved with the neighbor and stock the result in the neighbor
            eval(_solution, selectedNeighbor);
        }
        else {
            // If _solution hasn't neighbor,
            isAccept = false;
        }
    }

    /**
     * continue if the temperature is not too low
     * @param _solution the solution
     * @return true if the criteria from the cooling schedule is true
     */
    virtual bool isContinue(EOT & _solution) {
        /* e.g. a given number of iterations executed at each threshold Q */
        return coolingSchedule(q);
    }


    /**
     * acceptance criterion of Simulated Annealing algorithm
     * @param _solution the solution
     * @return
     */
    virtual bool accept(EOT & _solution) {
//        cout << "accept method" << endl;

        // Test if _solution has a Neighbor
        if (neighborhood.hasNeighbor(_solution)) {
//            if (solNeighborComparator(_solution, selectedNeighbor)) { // accept if the current neighbor is better than the solution

/// TODO - ADD isFeasible to Neighbor class
///
///
            // Downcast selectedNeighbor to ETTPneigbor
            Neighbor *selectedNeighborPtr = &selectedNeighbor;
            ETTPneighbor<EOT> *neighbourPtr = (ETTPneighbor<EOT> *)selectedNeighborPtr;
            if (neighbourPtr != nullptr && !neighbourPtr->isFeasible()) {
                isAccept = false;
#ifdef MOSAEXPLORER_DEBUG
            std::cout << "In [moSAexplorer::accept(sol)] method:" << std::endl;
            std::cout << "Infeasible solution, it will not be accepted. Generating a new one..." << std::endl;
#endif
                return isAccept;
            }

#ifdef MOSAEXPLORER_DEBUG
            std::cout << "In [moSAexplorer::accept(sol)] method:" << std::endl;
            std::cout << "solution: " << _solution.fitness() << " neighbour: " << selectedNeighbor.fitness()
                      << ", q = " << q << std::endl;
            std::cout << "Accept solution? " << isAccept << std::endl;
#endif

            ////////////////////////////////////////////////////////////////////////////////////
            /// SA
            ///
            double fit1, fit2;
            fit1 = (double)selectedNeighbor.fitness();
            fit2 = (double)_solution.fitness();

            //                       cout << "fit1 (neighbour) = " << fit1 << " fit2 (current solution) = " << fit2
            //                            << ", T = " << this->temperature << endl;


            if (selectedNeighbor.fitness() < _solution.fitness()) { // Minimization problem
               isAccept = true;
            //                cout << "accept because the current neighbor is better than the solution" << endl;
            }
            else {
               double alpha = 0.0;
            //                double fit1, fit2;
            //                fit1 = (double)selectedNeighbor.fitness();
            //                fit2 = (double)_solution.fitness();

            //                cout << "temperature = " << temperature << endl;
            //                cout << "fit1 (neighbour) = " << fit1 << endl;
            //                cout << "fit2 (solution)  = " << fit2 << endl;

            //                cin.get();

            //                if (fit1 < fit2) // this is a maximization
            //                    alpha = exp((fit1 - fit2) / temperature );
            //                else // this is a minimization
            //                    alpha = exp((fit2 - fit1) / temperature );
            //                isAccept = (rng.uniform() < alpha);

            //                alpha = exp((fit2 - fit1) / (fit2*0.01) ); // 40.56
            //                alpha = exp((fit2 - fit1) / (fit2*2) ); //
               double temperature = q;
               alpha = exp((fit2 - fit1) / (fit2*temperature) );
            //                alpha = exp((fit2 - fit1) / temperature );

               double r = rng.uniform();

            //                cout << "temperature = " << temperature << endl;
            //                cout << "alpha = " << alpha << endl;
            //                cout << "rand = " << r << endl;

               isAccept = (r < alpha);
               if (isAccept) {
                   //
                   // Update output file
                   //
                   outFile << acceptedEvals << " " << fit2 << std::endl;
                   ++acceptedEvals;
               }
               ++evals;

            }


        }
        return isAccept;
    }


protected:

    // comparator between solution and neighbor
    moSolNeighborComparator<Neighbor>& solNeighborComparator;
    // true if the move is accepted
    bool isAccept;
    // TA parameters
    double q; // Current threshold
    moCoolingSchedule<EOT> &coolingSchedule;
    // Output file
    std::ofstream outFile;
    // # evaluated accepted neighbours
    int acceptedEvals;
    // # evaluated neighbours
    int evals;
};


#endif // MOSAEXPLORER_H
