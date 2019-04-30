#ifndef MOSA_H
#define MOSA_H

#include <algo/moLocalSearch.h>
#include "algorithms/mo/moSAexplorer.h"
#include <continuator/moTrueContinuator.h>
#include <eval/moEval.h>
#include <eoEvalFunc.h>
#include "algorithms/mo/moSimpleCoolingSchedule.h"


/**
 * Simulated Annealing algorithm
 */
template<class Neighbor>
class moSA: public moLocalSearch<Neighbor>
{
public:

    typedef typename Neighbor::EOT EOT;
    typedef moNeighborhood<Neighbor> Neighborhood ;


    /**
     * Simple constructor for a Simulated Annealing
     * @param _neighborhood the neighborhood
     * @param _fullEval the full evaluation function
     * @param _eval neighbor's evaluation function
     * @param _cool a cooling schedule
     */

    moSA(Neighborhood& _neighborhood, eoEvalFunc<EOT>& _fullEval, moEval<Neighbor>& _eval,
         moCoolingSchedule<EOT>& _cool):
            moLocalSearch<Neighbor>(explorer, trueCont, _fullEval),
            defaultCool(0, 0, 0, 0),
            explorer(_neighborhood, _eval, defaultSolNeighborComp, _cool)
    {}


private:
    moTrueContinuator<Neighbor> trueCont;
    moSimpleCoolingSchedule<EOT> defaultCool;
    moSolNeighborComparator<Neighbor> defaultSolNeighborComp;
    moSAexplorer<Neighbor> explorer;
};



#endif // MOSA_H
