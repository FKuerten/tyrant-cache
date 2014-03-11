#ifndef TYRANT_CACHE_CACHE_SIMULATORCACHE_HPP
    #define TYRANT_CACHE_CACHE_SIMULATORCACHE_HPP

    #include <map>
    #include <string>
    #include <core/simulationResult.h++>
    #include <core/simulatorCore.h++>
    // For std::shared_ptr
    #include <memory>

    namespace C = Tyrant::Core;
    namespace Tyrant {
        namespace Cache {

            class SimulatorCache : public C::SimulatorCore {
                public:
                    typedef std::shared_ptr<SimulatorCache> Ptr;

                protected:
                    C::SimulatorCore::Ptr delegate;

                public:
                    SimulatorCache(SimulatorCore::Ptr delegate);
                    virtual ~SimulatorCache();

                    C::SimulatorCore::Ptr getDelegate() const;
                    std::string getCoreName() const;
                    std::string getCoreVersion() const;
                    std::string getCoreVersionHumanReadable() const;
                    virtual C::SimulationResult simulate(C::SimulationTask const &) = 0;
                    virtual C::SimulationResult simulate(C::SimulationTask const &, unsigned long numberOfNewSamples) = 0;

                    virtual std::map<std::string, C::SimulationResult>
                    getDecks(C::SimulationTask const task, bool wildCardAttacker) const = 0;

                    virtual void abort() = 0;
            };
        }
    }
#endif
