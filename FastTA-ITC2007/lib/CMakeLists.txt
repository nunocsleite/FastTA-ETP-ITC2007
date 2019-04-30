#
# project
#
project(SOlib)

# Include "/lib" directory
include_directories(${PROJECT_SOURCE_DIR})
# Include ParadisEO directories
include_directories(${PROJECT_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/eo/src/)
include_directories(${PROJECT_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/mo/src/)
include_directories(${PROJECT_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/moeo/src/)



add_subdirectory(algorithms)
add_subdirectory(chromosome)
add_subdirectory(containers)
add_subdirectory(data)
add_subdirectory(eval)
add_subdirectory(graphColouring)
add_subdirectory(init)
add_subdirectory(kempeChain)
add_subdirectory(neighbourhood)
add_subdirectory(statistics)
add_subdirectory(testset)
add_subdirectory(utils)
add_subdirectory(validator)


#
# Set public header list (add your headers and source files here))
#
set(${PROJECT_NAME}_headers
        # algorithms/eo
        algorithms/eo/Crossover.h
        algorithms/eo/eoAlgoPointer.h
        algorithms/eo/eoCellularEA.h
        algorithms/eo/eoCellularEAMatrix.h
        algorithms/eo/eoCellularEARing.h
        algorithms/eo/eoDeterministicTournamentSelectorPointer.h
        algorithms/eo/eoEvolutionOperator.h
        algorithms/eo/eoGenerationContinue.h
        algorithms/eo/eoGenerationContinuePopVector.h
        algorithms/eo/Mutation.h
#        algorithms/eo/eoSCEA.h
        algorithms/eo/eoSelectBestOne.h
        # algorithms/mo
        algorithms/mo/moSimpleCoolingSchedule.h
        algorithms/mo/moGDA.h
        algorithms/mo/moGDAexplorer.h
        algorithms/mo/moTA.h
        algorithms/mo/moTAexplorer.h
        algorithms/mo/moSA.h
        algorithms/mo/moSAexplorer.h
        # algorithms/mo/statistics
        algorithms/mo/statistics/moTAWithStatistics.h   # Used to generate paper figures
        algorithms/mo/statistics/moTAexplorerWithStatistics.h
        # Optimised version
#        algorithms/mo/statistics/moTAWithStatisticsOpt.h
#        algorithms/mo/statistics/moTAexplorerWithStatisticsOpt.h
        # chromosome
        chromosome/eoChromosome.h
        # containers
        containers/ColumnMatrix.h
        containers/ConflictBasedStatistics.h
        containers/IntMatrix.h
        containers/Matrix.h
        containers/TimetableContainer.h
        containers/TimetableContainerMatrix.h
        containers/VertexPriorityQueue.h
        # data
        data/Constraint.hpp
        data/ConstraintValidator.hpp
        data/Data.h
        data/Exam.h
        data/ITC2007Constraints.hpp
        data/ITC2007Period.h
        data/Period.h
        data/ScheduledExam.h
        data/ScheduledRoom.h
        data/Room.h
        data/TimetableProblemData.hpp
        # eval
        eval/eoETTPEval.h
        eval/eoETTPEvalNumberEvalsCounter.h
        eval/eoNumberEvalsCounter.h
        # eval/statistics
        eval/statistics/eoETTPEvalWithStatistics.h
        # graphColouring
        graphColouring/GraphColouringHeuristics.h
        # init
        init/ETTPInit.h
        # kempeChain
        kempeChain/ETTPKempeChain.h
        kempeChain/ETTPKempeChainHeuristic.h
        # kempeChain/statistics
        kempeChain/statistics/ETTPKempeChainHeuristicWithStatistics.h
        # neighbourhood
        neighbourhood/ETTPneighbor.h
        neighbourhood/ETTPneighborEval.h
        neighbourhood/ETTPneighborhood.h
        neighbourhood/ETTPNeighborhoodExplorer.h
        # neighbourhood/statistics
        neighbourhood/statistics/ETTPneighborEvalNumEvalsCounter.h
        neighbourhood/statistics/ETTPneighborEvalWithStatistics.h
        neighbourhood/statistics/ETTPneighborEvalWithStatisticsNumberEvalsCounter.h
        neighbourhood/statistics/ETTPneighborhoodWithStatistics.h
        neighbourhood/statistics/ETTPneighborWithStatistics.h
        # statistics
        statistics/ExamInfo.h
        statistics/ExamMoveStatistics.h # Used to generate paper figures
        statistics/MovedExam.h
        # statistics/optimised
        # Optimised version
        statistics/optimised/ExamInfoOpt.h
#        statistics/optimised/ExamMoveStatisticsOpt.h
        # testset
        testset/ITC2007TestSet.h
        testset/TestSet.h
        testset/TestSetDescription.h
        # utils
        utils/Common.h
        utils/CurrentDateTime.h
        utils/DateTime.h
        # validator
        validator/validator.h

)


set(${PROJECT_NAME}_sources
        # algorithms/eo
        algorithms/eo/Crossover.cpp
        algorithms/eo/Mutation.cpp
        # chromosome
        chromosome/eoChromosome.cpp
        # containers
        containers/ConflictBasedStatistics.cpp
        containers/TimetableContainerMatrix.cpp
        containers/VertexPriorityQueue.cpp
        # data
        data/TimetableProblemData.cpp
        # graphColouring
        # statistics
        statistics/ExamInfo.cpp
        statistics/ExamMoveStatistics.cpp # Used to generate paper figures
        # statistics/optimised
        # Optimised version
        statistics/optimised/ExamInfoOpt.cpp
#        statistics/optimised/ExamMoveStatisticsOpt.cpp # Comment if ExamMoveStatistics is used
        # testset
        testset/ITC2007TestSet.cpp
        testset/TestSet.cpp
        testset/TestSetDescription.cpp
        # utils
        utils/CurrentDateTime.cpp
        utils/Utils.cpp
        utils/Common.cpp
        # lib
        MainApp.cpp
        MainAppITC2007Datasets.cpp
        # validator
        validator/validator.cc
)



add_library(SOlib SHARED ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})


# Include ParadisEO, Boost Regex, Armadillo, ncurses5-dev libs
#target_link_libraries(${PROJECT_NAME} boost_regex eo es moeo cma eoutils ga armadillo ncurses)
#target_link_libraries(${PROJECT_NAME} boost_regex eo es moeo cma eoutils ga armadillo)
target_link_libraries(${PROJECT_NAME} boost_regex eo es moeo cma eoutils ga)

