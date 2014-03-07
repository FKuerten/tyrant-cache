#ifndef TYRANT_CACHE_MULTIDECKDECOMPOSITOR_HPP
    #define TYRANT_CACHE_MULTIDECKDECOMPOSITOR_HPP

    #include <core/simulatorCore.h++>
    #include <core/simulationTask.h++>
    #include <core/simulationResult.h++>

    namespace Tyrant {
        namespace Cache {


            class MultiDeckDecompositor : public ::Tyrant::Core::SimulatorCore {

                protected:
                    ::Tyrant::Core::SimulatorCore::Ptr delegate;

                public:
                    MultiDeckDecompositor(SimulatorCore::Ptr delegate);
                    virtual ~MultiDeckDecompositor();

                    ::Tyrant::Core::SimulatorCore::Ptr getDelegate() const;
                    std::string getCoreName() const;
                    std::string getCoreVersion() const;
                    std::string getCoreVersionHumanReadable() const;

                    virtual ::Tyrant::Core::SimulationResult simulate(::Tyrant::Core::SimulationTask const &);
                    virtual void abort();
            };
        }
    }
#endif

