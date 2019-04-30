#ifndef EOETTPEVALNUMBEREVALSCOUNTER_H
#define EOETTPEVALNUMBEREVALSCOUNTER_H


#include "eval/eoETTPEval.h"
#include "eval/eoNumberEvalsCounter.h"



/**
 * Evaluation of objective function and # evals statistics computation
 */
template <class EOT>
class eoETTPEvalNumberEvalsCounter : public eoETTPEval<EOT> {
public:

    eoETTPEvalNumberEvalsCounter(eoNumberEvalsCounter &_numberEvalsCounter)
        : numberEvalsCounter(_numberEvalsCounter) { }

    void operator()(EOT& _chrom) {
        // Invoke base class method to perform chromosome evaluation
        eoETTPEval<EOT>::operator ()(_chrom);
        // # evals statistics computation. Add 1 to # evals
        numberEvalsCounter.addNumEvalsToTotal(1);
    }


protected:

    eoNumberEvalsCounter &numberEvalsCounter;
};



#endif // EOETTPEVALNUMBEREVALSCOUNTER_H
