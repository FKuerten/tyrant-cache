#include "memoryBackedCache.h++"
#include <functional>
#include <iostream>
#include <vector>
#include <set>
#include <boost/functional/hash.hpp>

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
                && (*a.attacker == *b.attacker)
                && (*a.defender == *b.defender)
                && a.numberOfRounds == b.numberOfRounds;
        }

        size_t
        MemoryBackedCache::Hash::operator()
            (Core::SimulationTask const & t
            ) const
        {
            //std::hash<bool> bh;
            //std::hash<signed int> sih;
            //std::hash<std::string> sh;
            std::size_t hash = 0;
            boost::hash_combine(hash, t.surge);
            boost::hash_combine(hash, t.delayFirstAttacker);
            boost::hash_combine(hash, t.battleGround);
            boost::hash_combine(hash, t.achievement);
            boost::hash_combine(hash, *t.attacker);
            boost::hash_combine(hash, *t.defender);
            boost::hash_combine(hash, t.numberOfRounds);
            return hash;
        }

        MemoryBackedCache::MemoryBackedCache(SimulatorCache::Ptr delegate)
        : SimulatorCache(delegate)
        , delegate(delegate)
        {}

        MemoryBackedCache::~MemoryBackedCache()
        {
            bool const DEBUG_CHECK_HASH = false;
            if (DEBUG_CHECK_HASH) {
                // Check the collisions for each hash
                std::vector<Core::SimulationTask> keys;
                for (auto entry : this->cache) {
                    keys.push_back(entry.first);
                }
                std::set<size_t> hashsDone;
                Hash hash;
                unsigned int const size = keys.size();
                for (size_t i = 0; i+1 < size; i++) {
                    bool found = false;
                    size_t const hashI = hash(keys[i]);
                    if (hashsDone.find(hashI) == hashsDone.end()) {
                        for (size_t j = i+1; j < size; j++) {
                            size_t const hashJ = hash(keys[j]);
                            if(hashI == hashJ) {
                                if (!found) {
                                    std::clog << "Same hash " << hashI << ": "
                                              << std::string(*keys[i].attacker);
                                    found = true;
                                }
                                std::clog << ", " << std::string(*keys[j].attacker);
                            }
                        }
                        if (found) {
                            std::clog << std::endl;
                            hashsDone.insert(hashI);
                        }
                    }
                }
            }
        }

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
