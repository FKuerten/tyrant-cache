#include "multiDeckDecompositor.h++"
#include <core/multiDeckTemplate.h++>
#include <memory>
#include <errorHandling/assert.h++>
#include <unordered_map>
#include <boost/foreach.hpp>
#include <boost/range/adaptor/map.hpp>
#include <iostream>

using namespace Tyrant::Core;
namespace Tyrant {
    namespace Cache {

        MultiDeckDecompositor::MultiDeckDecompositor(SimulatorCore::Ptr delegate)
        : delegate(delegate)
        {
        }

        MultiDeckDecompositor::~MultiDeckDecompositor()
        {
        }

        ::Tyrant::Core::SimulatorCore::Ptr
        MultiDeckDecompositor::getDelegate() const
        {
            return this->delegate;
        }

        std::string
        MultiDeckDecompositor::getCoreName() const
        {
            return this->delegate->getCoreName();
        }

        std::string
        MultiDeckDecompositor::getCoreVersion() const
        {
            return this->delegate->getCoreVersion();
        }

        std::string
        MultiDeckDecompositor::getCoreVersionHumanReadable() const
        {
            return this->delegate->getCoreVersionHumanReadable();
        }

        SimulationTask
        taskWithIterationsAndDeck
            (SimulationTask const & original
            ,unsigned long numberOfIterations
            ,DeckTemplate::Ptr const & deck
            )
        {
            SimulationTask copy(original);
            copy.minimalNumberOfGames = numberOfIterations;
            copy.defender = deck;
            return copy;
        }

        typedef std::multiset<DeckTemplate::Ptr> DeckMSet;
        typedef std::pair<SimulationResult, size_t> ResultCountPair;
        typedef std::map<DeckTemplate::Ptr, ResultCountPair> ResultMap;

        SimulationResult &
        operator*=
            (SimulationResult & result
            ,long double factor
            )
        {
            result.numberOfGames = static_cast<size_t>(result.numberOfGames * factor);
            result.gamesWon = static_cast<size_t>(result.gamesWon * factor);
            result.gamesStalled = static_cast<size_t>(result.gamesStalled * factor);
            result.gamesLost = static_cast<size_t>(result.gamesLost * factor);
            result.pointsAttacker = static_cast<size_t>(result.pointsAttacker * factor);
            result.pointsAttackerAuto = static_cast<size_t>(result.pointsAttackerAuto * factor);
            result.pointsDefender = static_cast<size_t>(result.pointsDefender * factor);
            result.pointsDefenderAuto = static_cast<size_t>(result.pointsDefenderAuto * factor);
            return result;
        }

        SimulationResult operator* (SimulationResult result, double factor) {
            return result *= factor;
        }

        SimulationResult
        MultiDeckDecompositor::simulate
            (SimulationTask const & task
            )
        {
            assertX(task.defender);
            if (MultiDeckTemplate::ConstPtr defender =
                std::dynamic_pointer_cast<MultiDeckTemplate const>(task.defender)
               )
            {
                assertX(defender);
                unsigned long const numberOfIterations = task.minimalNumberOfGames;
                DeckMSet const & decks = defender->getDecks();
                ResultMap results;
                size_t const totalCount = decks.size();
                DeckMSet::const_iterator iter = decks.begin();
                while(iter != decks.end()) {
                    DeckTemplate::Ptr const currentDeck = *iter;
                    size_t const currentDeckCount = decks.count(currentDeck);
                    //std::clog << "current deck: " << currentDeck->toString() << " cardinality " << currentDeckCount << std::endl;
                    unsigned long currentIterations = static_cast<unsigned int>(std::ceil(
                                                        static_cast<long double>(numberOfIterations)
                                                        * currentDeckCount
                                                        / totalCount
                                                      ));
                    SimulationTask currentTask = taskWithIterationsAndDeck(task, currentIterations, currentDeck);
                    SimulationResult currentResult = this->simulate(currentTask);
                    results[currentDeck] = ResultCountPair(currentResult, currentDeckCount);
                    iter = decks.upper_bound(currentDeck);
                    //throw LogicError("Not implemented: Next deck!");
                }
                // combine results
                // . find min number
                unsigned long minIterationsForSingleItem = numberOfIterations;
                BOOST_FOREACH(ResultCountPair & resultPair, results | boost::adaptors::map_values) {
                    unsigned long currentIterationsForSingleItem = resultPair.first.numberOfGames / resultPair.second;
                    if (currentIterationsForSingleItem < minIterationsForSingleItem) {
                        minIterationsForSingleItem = currentIterationsForSingleItem;
                    }
                }
                // . combine now
                SimulationResult combinedResult;
                BOOST_FOREACH(ResultCountPair & resultPair, results | boost::adaptors::map_values) {
                    SimulationResult const & currentResult = resultPair.first;
                    size_t const count = resultPair.second;
                    unsigned long const currentIterationsForSingleItem = currentResult.numberOfGames / count;
                    double const factor = static_cast<double>(minIterationsForSingleItem)
                                        / static_cast<double>(currentIterationsForSingleItem);
                    //std::clog << "scaling result with factor " << factor << std::endl;
                    SimulationResult scaled = currentResult * factor;
                    combinedResult += scaled;
                }
                return combinedResult;
            } else {
                return this->delegate->simulate(task);
            }
        }

        void
        MultiDeckDecompositor::abort() {
            this->delegate->abort();
        }


    }
}

