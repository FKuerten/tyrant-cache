#include "memoryBackedCache.h++"
#include <functional>
#include <iostream>

namespace Tyrant {
    namespace Cache {

        bool
        MemoryBackedCache::Eq::operator()
            (Core::SimulationTask const & a
            ,Core::SimulationTask const & b
            ) const
        {
            return a.surge == b.surge
                && a.delayFirstAttacker == b.delayFirstAttacker
                && a.battleGround == b.battleGround
                && a.achievement == b.achievement
                && std::string(*a.attacker) == std::string(*b.attacker)
                && std::string(*a.defender) == std::string(*b.defender)
                && a.numberOfRounds == b.numberOfRounds;
        }

        size_t
        MemoryBackedCache::Hash::operator()
            (Core::SimulationTask const & t
            ) const
        {
            std::hash<bool> bh;
            std::hash<signed int> sih;
            std::hash<std::string> sh;
            size_t hash = 0;
            hash *= 3u + bh(t.surge);
            hash *= 3u + bh(t.delayFirstAttacker);
            hash *= 3u + sih(t.battleGround);
            hash *= 3u + sih(t.achievement);
            hash *= 3u + sh(std::string(*t.attacker));
            hash *= 3u + sh(std::string(*t.defender));
            hash *= 3u + sih(t.numberOfRounds);
            return hash;
        }

        MemoryBackedCache::MemoryBackedCache(SimulatorCache::Ptr delegate)
        : SimulatorCache(delegate)
        , delegate(delegate)
        {}

        MemoryBackedCache::~MemoryBackedCache()
        {}

        Core::SimulationResult
        MemoryBackedCache::simulate(Core::SimulationTask const & task)
        {
            Core::SimulationResult cached = this->cache[task];
            if (cached.numberOfGames >= task.minimalNumberOfGames) {
                //std::clog << "cache hit" << std::endl;
                return cached;
            } else {
                //std::clog << "cache miss" << std::endl;
                cached = this->delegate->simulate(task);
                this->cache[task] = cached;
                return cached;
            }
        }

        Core::SimulationResult
        MemoryBackedCache::simulate
            (Core::SimulationTask const & task
            ,unsigned long numberOfNewSamples)
        {
            Core::SimulationResult result = this->delegate->simulate(task, numberOfNewSamples);
            this->cache[task] = result;
            return result;
        }

        std::map<std::string, Core::SimulationResult>
        MemoryBackedCache::getDecks
            (Core::SimulationTask const task
            ,bool wildCardAttacker
            ) const
        {
            return this->delegate->getDecks(task, wildCardAttacker);
        }

        void
        MemoryBackedCache::abort()
        {
            this->delegate->abort();
        }


    }
}
