
#include <cmath>
#include <iostream>

#include "algorithms/mo/moSimpleCoolingSchedule.h"


using namespace std;


// Temperature actualization
double Temp(double t, double Tmax, double R) {
    double newtemp = Tmax*exp(-R*t);
    return newtemp;
}


// ORIGINAL CODE
//int getSANumberEvaluations(double tmax, double r, double k, double tmin) {
//    double t = 0, temp = tmax;
//    long numberEvaluations = 0;
//    do {
//        for (int i = 1; i <= k; ++i)
//            ++numberEvaluations;
//        // Actualize temperature
//        ++t;
//        temp = Temp(t, tmax, r);
//    } while (temp >= tmin);

//    return numberEvaluations;
//}


int getSANumberEvaluations(double tmax, double r, double k, double tmin) {
    cout << "[getSANumberEvaluations] tmax = " << tmax << ", r = "
         << r << ", k = " << k << ", tmin = " << tmin << endl;

    double t = 0, temp = tmax;
    long numberEvaluations = 0;
    do {
        for (int i = 1; i <= k; ++i)
            ++numberEvaluations;
        // Actualize temperature
        ++t;
        temp = Temp(t, tmax, r);
    } while (temp > tmin);

    cout << "last temp (> tmin) = " << temp << endl;

    /////////////////////////////////////////////////////////////
    moSimpleCoolingSchedule<int> coolingSchedule(tmax, r, k, tmin);
    int sol = 10; // dummy
    coolingSchedule.init(sol);
    temp = tmax;
    bool acceptedMove = true;
    long numberEvaluations1 = 0;
    while (coolingSchedule(temp)) {
        coolingSchedule.update(temp, acceptedMove);
        ++numberEvaluations1;
    }
    cout << "numberEvaluations = " << numberEvaluations << ", numberEvaluations1 = " << numberEvaluations1 << endl;
//    return numberEvaluations;
    return numberEvaluations1;
}








/*
int main(int argc, char* argv[])
{

//    cout << getSANumberEvaluations(0.0001, 0.001, 5, 0.0001) << endl;
//    cout << getSANumberEvaluations(1, 0.001, 1, 0.0001) << endl;
//    cout << getSANumberEvaluations(1, 0.001, 5, 0.0001) << endl;
//    cout << getSANumberEvaluations(3, 0.001, 1, 0.0001) << endl;
//    cout << getSANumberEvaluations(3, 0.001, 5, 0.0001) << endl;

    // moSimpleCoolingSchedule<eoChromosome> cool2(0.1, 0.00001, 5, 0.0000001);
    double tmax = 0.1, r = 0.00001, k = 5, tmin = 0.0000001;

//    cout << getSANumberEvaluations(tmax, r, k, tmin) << endl;
// number of evaluations: 6 907 760

    const long maxNumEval = 6907760;
    const int numThresholds = 10;
    int numEvalPerThreshold = maxNumEval / numThresholds;
    long lastBin = maxNumEval % numThresholds;
    long numEval;
    long totalNumEval = 0;
    double epsilon = 0.000001;

//    for ( ; tmax-epsilon > 0.01; tmax -= 0.01) {
//            cout << "tmax = " << tmax << " tmax-0.01 = " << tmax-0.01 << ", " <<
//                    (numEval = getSANumberEvaluations(tmax, r, k, tmax-0.01));
//            totalNumEval += numEval;
//            cout << ", totalNumEval = " << totalNumEval << endl;
//    }
//    tmax = 0.01;
//    cout << "tmax = " << tmax << " tmin = " << tmin << ", " << (numEval = getSANumberEvaluations(tmax, r, k, tmin));
//    totalNumEval += numEval;
//    cout << ", totalNumEval = " << totalNumEval << endl;

    cout << "maxNumEval = " << maxNumEval << ", numThresholds = " << numThresholds
         << ", numEvalPerThreshold = " << numEvalPerThreshold << ", lastBin = " << lastBin << endl;


    double thresholds[numThresholds];

    double t = 0, temp = tmax;
    totalNumEval = 0;
    bool continueCycle = true;

    for (int i = 0; i < numThresholds; ++i) {
        long numberEvaluations = 0;
        for (;;) {
            for (int j = 1; j <= k; ++j) {
                ++numberEvaluations;
                if (i == numThresholds-1) {
                    if (numberEvaluations >= numEvalPerThreshold+lastBin) {
                        continueCycle = false;
                        break;
                    }
                }
                else {
                    if (numberEvaluations >= numEvalPerThreshold) {
                        continueCycle = false;
                        break;
                    }
                }
            }
            if (continueCycle == false)
                break;

            // Actualize temperature
            ++t;
            temp = Temp(t, tmax, r);
        }
        totalNumEval += numberEvaluations;
        cout << "i = " << i << ", numberEvaluations = " << numberEvaluations << ", threshold = " << temp
             << ", totalNumEval = " << totalNumEval << endl;
        thresholds[i] = temp;
        continueCycle = true;
    }


}

*/

