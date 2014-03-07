#include "simulatorCache.h++"

namespace TyrantCache {
    namespace Cache {

        SimulatorCache::SimulatorCache(SimulatorCore::Ptr & delegate)
        : delegate(delegate)
        {
        }

        SimulatorCache::~SimulatorCache()
        {
        }

        C::SimulatorCore::Ptr
        SimulatorCache::getDelegate() const {
            return this->delegate;
        }
        
        std::string
        SimulatorCache::getCoreName() const
        {
            return this->delegate->getCoreName();
        }
        
        std::string
        SimulatorCache::getCoreVersion() const
        {
            return this->delegate->getCoreVersion();
        }

                std::string
        SimulatorCache::getCoreVersionHumanReadable() const
        {
            return this->delegate->getCoreVersionHumanReadable();
        }

    }
}
