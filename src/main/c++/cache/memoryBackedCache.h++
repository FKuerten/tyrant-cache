#ifndef TYRANT_CACHE_CACHE_MEMORYBACKEDCACHE_HPP
#define TYRANT_CACHE_CACHE_MEMORYBACKEDCACHE_HPP

    #include "simulatorCache.h++"
    #include <core/simulationTask.h++>
    #include <core/simulationResult.h++>
    #include <unordered_map>

    namespace Tyrant {
        namespace Cache {

            class MemoryBackedCache : public SimulatorCache {
                public:
                    class Eq {
                        public:
                            bool operator() (Core::SimulationTask const & a, Core::SimulationTask const & b) const;
                    };

                    class Hash {
                        public:
                            size_t operator() (Core::SimulationTask const & a) const;
                    };

                private:
                    std::unordered_map<Core::SimulationTask, Core::SimulationResult, Hash, Eq> cache;
                    SimulatorCache::Ptr delegate;

                public:
                    MemoryBackedCache(SimulatorCache::Ptr delegate);
                    ~MemoryBackedCache();

                    virtual Core::SimulationResult simulate(Core::SimulationTask const &);
                    virtual Core::SimulationResult simulate(Core::SimulationTask const &, unsigned long numberOfNewSamples);

                    virtual std::map<std::string, Core::SimulationResult>
                    getDecks(Core::SimulationTask const task, bool wildCardAttacker) const;

                    virtual void abort();
            };
        }
    }

#endif
