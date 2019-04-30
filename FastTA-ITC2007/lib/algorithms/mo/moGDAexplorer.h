#ifndef MOGDAEXPLORER_H
#define MOGDAEXPLORER_H

#include <cstdlib>

#include <explorer/moNeighborhoodExplorer.h>
#include <comparator/moSolNeighborComparator.h>

#include <neighborhood/moNeighborhood.h>


/**
 * Explorer for the Great Deluge algorithm
 * Fitness must be > 0
 *
 */
template <class Neighbor>
class moGDAexplorer : public moNeighborhoodExplorer<Neighbor>
{
public:
    typedef typename Neighbor::EOT EOT ;
    typedef moNeighborhood<Neighbor> Neighborhood ;

    using moNeighborhoodExplorer<Neighbor>::neighborhood;
    using moNeighborhoodExplorer<Neighbor>::eval;
    using moNeighborhoodExplorer<Neighbor>::selectedNeighbor;

    /**
     * Constructor
     * @param _neighborhood the neighborhood
     * @param _eval the evaluation function
//     * @param _solNeighborComparator a solution vs neighbor comparator
//     * @param _coolingSchedule the cooling schedule
     */
  moGDAexplorer(Neighborhood& _neighborhood, moEval<Neighbor>& _eval,
               moSolNeighborComparator<Neighbor>& _solNeighborComparator, double _up)
      : moNeighborhoodExplorer<Neighbor>(_neighborhood, _eval), solNeighborComparator(_solNeighborComparator), up(_up) {

        isAccept = false;

        if (!neighborhood.isRandom()) {
            std::cout << "moGDAexplorer::Warning -> the neighborhood used is not random" << std::endl;
        }
    }

    /**
     * Destructor
     */
    ~moGDAexplorer() { }


    /**
     * initialization of the initial temperature
     * @param _solution the solution
     */
    virtual void initParam(EOT & _solution) {
//        temperature = coolingSchedule.init(_solution);

        // Choose the initial water level LEVEL
        initLevel = _solution.fitness();

//        level = initLevel;

        level = initLevel + 10;

        isAccept = false;

//        cout << "Initial solution: " << initLevel << endl;


    };

    /**
     * decrease the temperature if necessary
     * @param _solution unused solution
     */
    virtual void updateParam(EOT & _solution) {
//        cout << "Before Temperature = " << temperature << endl;
//        coolingSchedule.update(temperature, this->moveApplied());
//        cout << "After Temperature = " << temperature << endl;

        // Update the water level
//        level = level - up;

        level = level * up;

    };

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
//      cout << "moGDAexplorer operator()(sol)" << endl;

        // Test if _solution has a Neighbor
        if (neighborhood.hasNeighbor(_solution)) {
//            cout << "hasNeighbor" << endl;

            // Init on the first neighbor: supposed to be random solution in the neighborhood
            neighborhood.init(_solution, selectedNeighbor);
            // Eval the _solution moved with the neighbor and stock the result in the neighbor
            eval(_solution, selectedNeighbor);
        }
        else {
            // If _solution hasn't neighbor,
            isAccept = false;
        }
//        cout << "ok" << endl;
    }



    /**
     * continue if the temperature is not too low
     * @param _solution the solution
     * @return true if the criteria from the cooling schedule is true
     */
    virtual bool isContinue(EOT & _solution) {
//        return coolingSchedule(temperature);

        // The search stops when the water level reaches the solution fitness
        return _solution.fitness() < level;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Template of the great deluge algorithm.
    //
    //  Input: Level L.
    //  s = s0 ; /∗ Generation of the initial solution ∗/
    //  Choose the rain speed UP ; /∗ UP > 0 ∗/
    //  Choose the initial water level LEVEL ;
    //  Repeat
    //      Generate a random neighbor s′ ;
    //      If f (s′) < LEVEL Then s = s′ /∗ Accept the neighbor solution ∗/
    //      LEVEL = LEVEL − UP ; /∗ update the water level ∗/
    //  Until Stopping criteria satisfied
    //  Output: Best solution found.
    //
    ///////////////////////////////////////////////////////////////////////////

    /**
     * acceptance criterion according to the boltzmann criterion
     * @param _solution the solution
     * @return true if better neighbor or rnd < exp(delta f / T)
     */
    virtual bool accept(EOT & _solution) {
//        cout << "accept method" << endl;

        if (neighborhood.hasNeighbor(_solution)) {
//            if (solNeighborComparator(_solution, selectedNeighbor)) { // accept if the current neighbor is better than the solution


            // Downcast selectedNeighbor to ETTPneigbor
            Neighbor *selectedNeighborPtr = &selectedNeighbor;
            ETTPneighbor<EOT> *neighbourPtr = (ETTPneighbor<EOT> *)selectedNeighborPtr;
            if (neighbourPtr != nullptr && !neighbourPtr->isFeasible()) {
                isAccept = false;
#ifdef MOTAEXPLORER_DEBUG
            std::cout << "In [moTAexplorer::accept(sol)] method:" << std::endl;
            std::cout << "Infeasible solution, it will not be accepted. Generating a new one..." << std::endl;
#endif
                return isAccept;
            }

            if (selectedNeighbor.fitness() < level) { // Minimization problem
                isAccept = true;
            }
            else {
                isAccept = false;
            }

//            cout << "solution: " << _solution.fitness() << " neighbour: " << selectedNeighbor.fitness() << " level = " << level << endl;

            //            cout << "solution: " << _solution.fitness() << " level = " << level << endl;


        }
        return isAccept;
    };


private:
    // comparator betwenn solution and neighbor
    moSolNeighborComparator<Neighbor>& solNeighborComparator;
    // true if the move is accepted
    bool isAccept;
    // GDA parameters
    double level;
    double initLevel;
    double up;
};


#endif // MOGDAEXPLORER_H
