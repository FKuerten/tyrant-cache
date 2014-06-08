#include "diskBackedCache.h++"
#include <sstream>
#include <errorHandling/assert.h++>
#include <iostream>

namespace Core = Tyrant::Core;
namespace Tyrant {
    namespace Cache {

        DiskBackedCache::DiskBackedCache(Core::SimulatorCore::Ptr delegate)
        : SimulatorCache(delegate)
        , ignoreCoreVersion(true)
        , ignoreXMLVersion(true)
        , readFromCache(true)
        , writeToCache(true)
        , database("cache.sqlite")
        {
            std::stringstream xmlVersion;
            std::map<std::string,std::string> xmlHashes = this->delegate->getXMLVersions();
            for(std::map<std::string,std::string>::const_iterator iter = xmlHashes.begin()
               ; iter != xmlHashes.end()
               ; iter++
               )
            {
                // TODO: Order is probably not guaranteed, need to address that
                xmlVersion << iter->first << "=" << iter->second << ";";
            }

            {
                std::stringstream ssCreateTable;
                ssCreateTable << "CREATE TABLE IF NOT EXISTS ";
                ssCreateTable << "TyrantCache";
                ssCreateTable << " (" << "coreName string NOT NULL";
                ssCreateTable << ", " << "coreVersion string NOT NULL";
                ssCreateTable << ", " << "xmlVersions string NOT NULL";
                ssCreateTable << ", " << "attacker string NOT NULL";
                ssCreateTable << ", " << "defender string NOT NULL";
                ssCreateTable << ", " << "surge bool NOT NULL";
                ssCreateTable << ", " << "delayFirstCard bool NOT NULL";
                ssCreateTable << ", " << "battleGroundId int DEFAULT 0";
                ssCreateTable << ", " << "achievementId int DEFAULT -1";
                ssCreateTable << ", " << "numberOfRounds int DEFAULT 50";
                ssCreateTable << ", " << "useRaidRules int NOT NULL";
                ssCreateTable << ", " << "numberOfGames int NOT NULL";
                ssCreateTable << ", " << "gamesWon int NOT NULL";
                ssCreateTable << ", " << "gamesStalled int NOT NULL";
                ssCreateTable << ", " << "gamesLost int NOT NULL";
                ssCreateTable << ", " << "pointsAttacker int NOT NULL";
                ssCreateTable << ", " << "pointsAttackerAuto int NOT NULL";
                ssCreateTable << ", " << "pointsDefender int NOT NULL";
                ssCreateTable << ", " << "pointsDefenderAuto int NOT NULL";
                ssCreateTable << ")";
                database.execute(ssCreateTable.str());
            }

            {
                std::stringstream ssCreateIndex;
                ssCreateIndex << "CREATE INDEX IF NOT EXISTS ";
                ssCreateIndex << "task";
                ssCreateIndex << " ON ";
                ssCreateIndex << "TyrantCache";
                ssCreateIndex << " (" << "attacker";
                ssCreateIndex << " ," << "defender";
                ssCreateIndex << " ," << "surge";
                ssCreateIndex << " ," << "delayFirstCard";
                ssCreateIndex << " ," << "battleGroundId";
                ssCreateIndex << " ," << "achievementId";
                ssCreateIndex << " ," << "numberOfRounds";
                ssCreateIndex << " ," << "useRaidRules";
                ssCreateIndex << ")";
                database.execute(ssCreateIndex.str());
            }

            {
                std::stringstream ssInsert;
                ssInsert << "INSERT INTO " << "TyrantCache ";
                ssInsert << "(coreName, coreVersion, xmlVersions";
                ssInsert << ", attacker, defender, surge, delayFirstCard";
                ssInsert << ", battleGroundId, achievementId";
                ssInsert << ", numberOfRounds, useRaidRules";
                ssInsert << ", numberOfGames, gamesWon, gamesStalled, gamesLost";
                ssInsert << ", pointsAttacker, pointsAttackerAuto, pointsDefender, pointsDefenderAuto";
                ssInsert << ") ";
                ssInsert << "VALUES ";
                ssInsert << "(?, ?, ?";
                ssInsert << ", ?, ?, ?, ?";
                ssInsert << ", ?, ?";
                ssInsert << ", ?, ?"; // numberOfRounds, useRaidRules
                ssInsert << ", ?, ?, ?, ?";
                ssInsert << ", ?, ?, ?, ?";
                ssInsert << ")";
                this->insertStatement = database.prepareStatement(ssInsert.str());

                this->insertStatement->bindText(1, this->delegate->getCoreName());
                this->insertStatement->bindText(2, this->delegate->getCoreVersion());
                this->insertStatement->bindText(3, xmlVersion.str());
            }

            {
                std::stringstream ssSelect;
                ssSelect << "SELECT " << std::endl;
                ssSelect << "\tnumberOfGames, gamesWon, gamesStalled, gamesLost";
                ssSelect << ", pointsAttacker, pointsAttackerAuto, pointsDefender, pointsDefenderAuto";
                ssSelect << " " << std::endl;
                ssSelect << "FROM " << "TyrantCache " << std::endl;
                ssSelect << "WHERE coreName = ? ";
                ssSelect << "AND (coreVersion = ? ";
                if(this->ignoreCoreVersion) {
                    ssSelect << "OR 1) ";
                }
                ssSelect << "AND (xmlVersions = ? ";
                if(this->ignoreXMLVersion) {
                    ssSelect << "OR 1) ";
                }
                ssSelect << "AND attacker = ? ";
                ssSelect << "AND defender = ? ";
                ssSelect << "AND surge = ? ";
                ssSelect << "AND delayFirstCard = ? ";
                ssSelect << "AND battleGroundId = ? ";
                ssSelect << "AND achievementId = ? ";
                ssSelect << "AND numberOfRounds = ? ";
                ssSelect << "AND useRaidRules = ? ";
                ssSelect << ";";
                this->selectStatement = database.prepareStatement(ssSelect.str());
                this->selectStatement->bindText(1, this->delegate->getCoreName());
                this->selectStatement->bindText(2, this->delegate->getCoreVersion());
                this->selectStatement->bindText(3, xmlVersion.str());
            }

            {
                std::stringstream ssAttacker;
                ssAttacker << "SELECT " << std::endl;
                ssAttacker << "\tSUM(numberOfGames) AS numberOfGames";
                ssAttacker << ", SUM(gamesWon) as gamesWon";
                ssAttacker << ", SUM(gamesStalled) as gamesStalled";
                ssAttacker << ", SUM(gamesLost) as gamesLost";
                ssAttacker << ", SUM(pointsAttacker) as pointsAttacker";
                ssAttacker << ", SUM(pointsAttackerAuto) as pointsAttackerAuto";
                ssAttacker << ", SUM(pointsDefender) as pointsDefender";
                ssAttacker << ", SUM(pointsDefenderAuto) as pointsDefenderAuto";
                ssAttacker << ", attacker";
                ssAttacker << " " << std::endl;
                ssAttacker << "FROM " << "TyrantCache " << std::endl;
                ssAttacker << "WHERE coreName = ? ";
                ssAttacker << "AND (coreVersion = ? ";
                if(this->ignoreCoreVersion) {
                    ssAttacker << "OR 1) ";
                }
                ssAttacker << "AND (xmlVersions = ? ";
                if(this->ignoreXMLVersion) {
                    ssAttacker << "OR 1) ";
                }
                ssAttacker << "AND defender = ? ";
                ssAttacker << "AND surge = ? ";
                ssAttacker << "AND delayFirstCard = ? ";
                ssAttacker << "AND battleGroundId = ? ";
                ssAttacker << "AND achievementId = ? ";
                ssAttacker << "AND numberOfRounds = ? ";
                ssAttacker << "AND useRaidRules = ? ";
                ssAttacker << "GROUP BY attacker, defender, surge, delayFirstCard, battleGroundId, achievementId, numberOfRounds, useRaidRules";
                ssAttacker << ";";
                this->selectAllAttackerStatement = database.prepareStatement(ssAttacker.str());
                this->selectAllAttackerStatement->bindText(1, this->delegate->getCoreName());
                this->selectAllAttackerStatement->bindText(2, this->delegate->getCoreVersion());
                this->selectAllAttackerStatement->bindText(3, xmlVersion.str());
            }

            {
                std::stringstream ssDefender;
                ssDefender << "SELECT " << std::endl;
                ssDefender << "\tSUM(numberOfGames) AS numberOfGames";
                ssDefender << ", SUM(gamesWon) as gamesWon";
                ssDefender << ", SUM(gamesStalled) as gamesStalled";
                ssDefender << ", SUM(gamesLost) as gamesLost";
                ssDefender << ", SUM(pointsAttacker) as pointsAttacker";
                ssDefender << ", SUM(pointsAttackerAuto) as pointsAttackerAuto";
                ssDefender << ", SUM(pointsDefender) as pointsDefender";
                ssDefender << ", SUM(pointsDefenderAuto) as pointsDefenderAuto";
                ssDefender << ", defender";
                ssDefender << " " << std::endl;
                ssDefender << "FROM " << "TyrantCache " << std::endl;
                ssDefender << "WHERE coreName = ? ";
                ssDefender << "AND (coreVersion = ? ";
                if(this->ignoreCoreVersion) {
                    ssDefender << "OR 1) ";
                }
                ssDefender << "AND (xmlVersions = ? ";
                if(this->ignoreXMLVersion) {
                    ssDefender << "OR 1) ";
                }
                ssDefender << "AND attacker = ? ";
                ssDefender << "AND surge = ? ";
                ssDefender << "AND delayFirstCard = ? ";
                ssDefender << "AND battleGroundId = ? ";
                ssDefender << "AND achievementId = ? ";
                ssDefender << "AND numberOfRounds = ? ";
                ssDefender << "AND useRaidRules = ? ";
                ssDefender << "GROUP BY attacker, defender, surge, delayFirstCard, battleGroundId, achievementId, numberOfRounds, useRaidRules";
                ssDefender << ";";
                this->selectAllDefenderStatement = database.prepareStatement(ssDefender.str());
                this->selectAllDefenderStatement->bindText(1, this->delegate->getCoreName());
                this->selectAllDefenderStatement->bindText(2, this->delegate->getCoreVersion());
                this->selectAllDefenderStatement->bindText(3, xmlVersion.str());
            }

            this->randomData = static_cast<unsigned int>(time(nullptr));
        }

        DiskBackedCache::~DiskBackedCache()
        {
        }

        C::SimulationResult
        DiskBackedCache::loadCache(Core::SimulationTask const & task
                                  )
        {
            assertX(task.attacker);
            assertX(task.defender);
            if(!this->readFromCache) {
                return Core::SimulationResult();
            }
            this->selectStatement->bindText(4, std::string(*task.attacker));
            this->selectStatement->bindText(5, std::string(*task.defender));
            this->selectStatement->bindBool(6, task.surge);
            this->selectStatement->bindBool(7, task.delayFirstAttacker);
            this->selectStatement->bindInt(8, task.battleGround);
            this->selectStatement->bindInt(9, task.achievement);
            this->selectStatement->bindInt(10, task.numberOfRounds);
            if (task.useRaidRules == Core::tristate::UNDEFINED) {
                //std::clog << "useRaidRules == UNDEFINED" << std::endl;
                this->selectStatement->bindInt(11, 2);
            } else if (task.useRaidRules == Core::tristate::TRUE) {
                this->selectStatement->bindInt(11, 1);
            } else {
                this->selectStatement->bindInt(11, 0);
            }
            S::SQLResults sqlResults = this->selectStatement->query();
            C::SimulationResult result;

            for(S::SQLResults::const_iterator iter = sqlResults.begin()
               ;iter != sqlResults.end()
               ;iter++
               )
            {
                S::SQLResult const & sqlResult = *iter;
                C::SimulationResult currentResult;
                currentResult.numberOfGames      = sqlResult.getULong("numberOfGames");
                currentResult.gamesWon           = sqlResult.getULong("gamesWon");
                currentResult.gamesStalled       = sqlResult.getULong("gamesStalled");
                currentResult.gamesLost          = sqlResult.getULong("gamesLost");
                currentResult.pointsAttacker     = sqlResult.getULong("pointsAttacker");
                currentResult.pointsAttackerAuto = sqlResult.getULong("pointsAttackerAuto");
                currentResult.pointsDefender     = sqlResult.getULong("pointsDefender");
                currentResult.pointsDefenderAuto = sqlResult.getULong("pointsDefenderAuto");
                result += currentResult;
                //std::clog << "loaded entry with " << currentResult.numberOfGames << " elements" << std::endl;
            }
            //std::clog << "loaded cache with " << result.numberOfGames << " elements" << std::endl;
            //if(result.gamesWon < result.numberOfGames) {
            //    throw Exception("Non perfect result (this is expected and only a debugging exception.)");
            //}
            return result;
        }

        void
        DiskBackedCache::addToCache(C::SimulationTask const & task
                                   ,C::SimulationResult const & freshResult
                                   )
        {
            if(this->writeToCache) {
                this->insertStatement->bindText(4, std::string(*task.attacker));
                this->insertStatement->bindText(5, std::string(*task.defender));
                this->insertStatement->bindBool(6, task.surge);
                this->insertStatement->bindBool(7, task.delayFirstAttacker);
                this->insertStatement->bindInt(8, task.battleGround);
                this->insertStatement->bindInt(9, task.achievement);
                this->insertStatement->bindInt(10, task.numberOfRounds);
                if (task.useRaidRules == Core::tristate::UNDEFINED) {
                    //std::clog << "Write: useRaidRules == UNDEFINED" << std::endl;
                    this->insertStatement->bindInt(11, 2);
                } else if (task.useRaidRules == Core::tristate::TRUE) {
                    this->insertStatement->bindInt(11, 1);
                } else {
                    this->insertStatement->bindInt(11, 0);
                }
                this->insertStatement->bindInt(12, static_cast<signed long>(freshResult.numberOfGames));
                this->insertStatement->bindInt(13, static_cast<signed long>(freshResult.gamesWon));
                this->insertStatement->bindInt(14, static_cast<signed long>(freshResult.gamesStalled));
                this->insertStatement->bindInt(15, static_cast<signed long>(freshResult.gamesLost));
                this->insertStatement->bindInt(16, static_cast<signed long>(freshResult.pointsAttacker));
                this->insertStatement->bindInt(17, static_cast<signed long>(freshResult.pointsAttackerAuto));
                this->insertStatement->bindInt(18, static_cast<signed long>(freshResult.pointsDefender));
                this->insertStatement->bindInt(19, static_cast<signed long>(freshResult.pointsDefenderAuto));
                this->insertStatement->execute();
                //std::clog << "Wrote " << freshResult.numberOfGames << " to cache" << std::endl;
            }
        }

        C::SimulationResult
        DiskBackedCache::simulate(C::SimulationTask const & task
                                 )
        {
            assertX(task.attacker);
            assertX(task.defender);
            C::SimulationResult cached = loadCache(task);
            //std::clog << "loaded " << cached.numberOfGames << " games" << std::endl;
            if (cached.numberOfGames < task.minimalNumberOfGames) {
                unsigned long missingGames = task.minimalNumberOfGames - cached.numberOfGames;
                C::SimulationTask remainderTask(task);
                remainderTask.minimalNumberOfGames = missingGames;
                C::SimulationResult fresh = this->delegate->simulate(remainderTask);
                this->addToCache(remainderTask, fresh);
                return cached + fresh;
            } else {
                return cached;
            }
        }


        C::SimulationResult
        DiskBackedCache::simulate(C::SimulationTask const & task
                                 ,unsigned long numberOfNewSamples
                                 )
        {
            C::SimulationResult cached = loadCache(task);
            if (cached.numberOfGames < task.minimalNumberOfGames) {
                unsigned long missingGames = task.minimalNumberOfGames - cached.numberOfGames;
                unsigned long newGames = std::max(missingGames, numberOfNewSamples);
                C::SimulationTask remainderTask(task);
                remainderTask.minimalNumberOfGames = newGames;
                C::SimulationResult fresh = this->delegate->simulate(remainderTask);
                this->addToCache(remainderTask, fresh);
                return cached + fresh;
            } else {
                return cached;
            }
        }

        void
        DiskBackedCache::abort()
        {
            this->delegate->abort();
        }

        void
        DiskBackedCache::setReadFromCache(bool readFromCache)
        {
            this->readFromCache = readFromCache;
        }

        void
        DiskBackedCache::setWriteToCache(bool writeToCache)
        {
            this->writeToCache = writeToCache;
        }

        std::map<std::string, C::SimulationResult>
        DiskBackedCache::getDecks
            (C::SimulationTask const task
            ,bool wildCardAttacker
            ) const
        {
            S::PreparedStatement * statement =  wildCardAttacker ? this->selectAllAttackerStatement : this->selectAllDefenderStatement;
            if (wildCardAttacker) {
                //assertX(task.defender);
                statement->bindText(4, std::string(*task.defender));
            } else {
                //assertX(task.attacker);
                statement->bindText(4, std::string(*task.attacker));
            }
            statement->bindBool(5, task.surge);
            statement->bindBool(6, task.delayFirstAttacker);
            statement->bindInt(7, task.battleGround);
            statement->bindInt(8, task.achievement);
            statement->bindInt(9, task.numberOfRounds);
            if (task.useRaidRules == Core::tristate::UNDEFINED) {
                //std::clog << "useRaidRules == UNDEFINED" << std::endl;
                this->selectStatement->bindInt(11, 2);
            } else if (task.useRaidRules == Core::tristate::TRUE) {
                this->selectStatement->bindInt(11, 1);
            } else {
                this->selectStatement->bindInt(11, 0);
            }

            S::SQLResults sqlResults = statement->query();
            std::map<std::string, C::SimulationResult> results;

            for(S::SQLResults::const_iterator iter = sqlResults.begin()
               ;iter != sqlResults.end()
               ;iter++
               )
            {
                S::SQLResult const & sqlResult = *iter;
                std::string deckStr = sqlResult.getString(wildCardAttacker ? "attacker" : "defender");
                C::SimulationResult currentResult;
                currentResult.numberOfGames      = sqlResult.getULong("numberOfGames");
                currentResult.gamesWon           = sqlResult.getULong("gamesWon");
                currentResult.gamesStalled       = sqlResult.getULong("gamesStalled");
                currentResult.gamesLost          = sqlResult.getULong("gamesLost");
                currentResult.pointsAttacker     = sqlResult.getULong("pointsAttacker");
                currentResult.pointsAttackerAuto = sqlResult.getULong("pointsAttackerAuto");
                currentResult.pointsDefender     = sqlResult.getULong("pointsDefender");
                currentResult.pointsDefenderAuto = sqlResult.getULong("pointsDefenderAuto");

                results[deckStr] = currentResult;
                //std::clog << "loaded entry with " << currentResult.numberOfGames << " elements" << std::endl;
            }
            return results;
        }

    }
}
