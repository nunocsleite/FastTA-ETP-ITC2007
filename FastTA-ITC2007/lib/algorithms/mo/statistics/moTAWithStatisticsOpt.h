#ifndef MOTA_WITH_STATISTICSOPT_H
#define MOTA_WITH_STATISTICSOPT_H

#include <algo/moLocalSearch.h>
#include "algorithms/mo/statistics/moTAexplorerWithStatisticsOpt.h"
#include <continuator/moTrueContinuator.h>
#include <eval/moEval.h>
#include <eoEvalFunc.h>
#include "algorithms/mo/moSimpleCoolingSchedule.h"
// In order to produce statistical info
//#include "statistics/ExamMoveStatistics.h"
#include "statistics/optimised/ExamMoveStatisticsOpt.h"


/**
 * Threshold Accepting algorithm - With statistics
 */
template<class Neighbor>
class moTAWithStatisticsOpt : public moLocalSearch<Neighbor>
{
public:

    typedef typename Neighbor::EOT EOT;
    typedef moNeighborhood<Neighbor> Neighborhood ;


    /**
     * Simple constructor for a threshold accepting
     * @param _neighborhood the neighborhood
     * @param _fullEval the full evaluation function
     * @param _eval neighbor's evaluation function
     * @param _cool a cooling schedule
     */

    moTAWithStatisticsOpt(ExamMoveStatisticsOpt &_examMoveStatistics,
         Neighborhood& _neighborhood, eoEvalFunc<EOT>& _fullEval, moEval<Neighbor>& _eval,
         moCoolingSchedule<EOT>& _cool):
            moLocalSearch<Neighbor>(explorer, trueCont, _fullEval),
            defaultCool(0, 0, 0, 0),
            explorer(_examMoveStatistics, _neighborhood, _eval, defaultSolNeighborComp, _cool)
    {}

private:
    moTrueContinuator<Neighbor> trueCont;
    moSimpleCoolingSchedule<EOT> defaultCool;
    moSolNeighborComparator<Neighbor> defaultSolNeighborComp;
    // Changed
    moTAexplorerWithStatisticsOpt<Neighbor> explorer;
};



#endif // MOTA_WITH_STATISTICSOPT_H
