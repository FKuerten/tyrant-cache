#ifndef TYRANT_CACHE_CACHE_DISKBACKEDCACHE_HPP
    #define TYRANT_CACHE_CACHE_DISKBACKEDCACHE_HPP

    #include <map>
    #include <string>
    #include <core/simulationResult.h++>
    #include "simulatorCache.h++"
    #include "../sql/sqliteWrapper.h++"

    namespace C = Tyrant::Core;
    namespace S = TyrantCache::SQL;
    namespace Tyrant {
        namespace Cache {

            class DiskBackedCache : public SimulatorCache {
                private:
                    bool ignoreCoreVersion;
                    bool ignoreXMLVersion;
                    bool readFromCache;
                    bool writeToCache;
                    S::SQLiteWrapper database;

                    S::PreparedStatement * insertStatement;
                    S::PreparedStatement * selectStatement;
                    S::PreparedStatement * selectAllAttackerStatement;
                    S::PreparedStatement * selectAllDefenderStatement;

                    unsigned int randomData;

                    C::SimulationResult loadCache(C::SimulationTask const & task);
                    void addToCache(C::SimulationTask const & task, C::SimulationResult const & result);
                public:
                    typedef std::shared_ptr<DiskBackedCache> Ptr;

                public:
                    DiskBackedCache(Core::SimulatorCore::Ptr delegate);
                    virtual ~DiskBackedCache();
                    void setReadFromCache(bool);
                    void setWriteToCache(bool);

                    virtual C::SimulationResult simulate(C::SimulationTask const &);
                    virtual C::SimulationResult simulate(C::SimulationTask const &, unsigned long numberOfNewSamples);

                    virtual std::map<std::string, C::SimulationResult>
                    getDecks(C::SimulationTask, bool wildCardAttacker) const;

                    virtual void abort();
            };
        }
    }

#endif

