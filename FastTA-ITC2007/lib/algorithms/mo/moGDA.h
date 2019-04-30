#ifndef MOGDA_H
#define MOGDA_H


//Template of the great deluge algorithm.
//  Input: Level L.
//  s = s0 ; /∗ Generation of the initial solution ∗/
//  Choose the rain speed UP ; /∗ UP > 0 ∗/
//  Choose the initial water level LEVEL ;
//  Repeat
//      Generate a random neighbor s′ ;
//      If f (s′ ) < LEVEL Then s = s′ /∗ Accept the neighbor solution ∗/
//      LEVEL = LEVEL − UP ; /∗ update the water level ∗/
//  Until Stopping criteria satisfied
//  Output: Best solution found.


#include <algo/moLocalSearch.h>
#include "moGDAexplorer.h"
#include <continuator/moTrueContinuator.h>
#include <eval/moEval.h>
#include <eoEvalFunc.h>


/**
 * Great Deluge algorithm
 */
template<class Neighbor>
class moGDA: public moLocalSearch<Neighbor>
{
public:

    typedef typename Neighbor::EOT EOT;
    typedef moNeighborhood<Neighbor> Neighborhood ;


    /**
     * Basic constructor for Great Deluge algorithm
     * @param _neighborhood the neighborhood
     * @param _fullEval the full evaluation function
     * @param _eval neighbor's evaluation function
     * @param _initLevel initial level
     * @param _up rain speed
     */
    moGDA(Neighborhood& _neighborhood, eoEvalFunc<EOT>& _fullEval, moEval<Neighbor>& _eval, double _up):
            moLocalSearch<Neighbor>(explorer, trueCont, _fullEval),
//            defaultCool(_initT, _alpha, _span, _finalT),
//            explorer(_neighborhood, _eval, defaultSolNeighborComp, defaultCool)
            explorer(_neighborhood, _eval, defaultSolNeighborComp, _up)
    {}


private:
    moTrueContinuator<Neighbor> trueCont;
//    moSimpleCoolingSchedule<EOT> defaultCool;
    moSolNeighborComparator<Neighbor> defaultSolNeighborComp;
//    moSAexplorer<Neighbor> explorer;
    moGDAexplorer<Neighbor> explorer;
};




#endif // MOGDA_H
