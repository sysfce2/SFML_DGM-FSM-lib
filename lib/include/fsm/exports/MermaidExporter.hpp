#pragma once

#include <filesystem>
#include <fsm/Types.hpp>
#include <fsm/detail/BuilderContext.hpp>
#include <fsm/detail/Constants.hpp>
#include <fsm/detail/Helper.hpp>
#include <fstream>
#include <print>

namespace fsm
{
    /**
     *  Exports the diagram in plaintext format worded as Mermaid Flowchart
     * diagram You can paste the data here: https://mermaid.live/edit and get a
     * nice looking diagram for your machine.
     *
     *  It does not contain self loops nor names transitions.
     */
    class [[nodiscard]] MermaidExporter final
    {
    public:
        explicit MermaidExporter(const std::filesystem::path& outFilePath)
            : fileStream(outFilePath), save(fileStream)
        {
        }

        explicit MermaidExporter(std::ostream& stream) : save(stream) {}

        MermaidExporter(const MermaidExporter&) = delete;
        MermaidExporter(MermaidExporter&&) = default;

    public:
        template<BlackboardTypeConcept BbT>
        void exportDiagram(const detail::BuilderContext<BbT>& context)
        {
            printHeader(context);
            printErrorTransition(context);
            printAllIntraStateTransitions(context);
        }

    private:
        template<BlackboardTypeConcept BbT>
        [[nodiscard]] std::string getFullEntryName(
            const std::string& machineName,
            const detail::BuilderContext<BbT>& context) const
        {
            return detail::createFullStateName(
                machineName, context.machines.at(machineName).entryState);
        }

        template<BlackboardTypeConcept BbT>
        void printHeader(const detail::BuilderContext<BbT>& context)
        {
            std::println(save, "flowchart TD");
            std::println(
                save,
                "  _( ) -->|entry| {}",
                getFullEntryName(detail::MAIN_MACHINE_NAME, context));
        }

        template<BlackboardTypeConcept BbT>
        void printErrorTransition(const detail::BuilderContext<BbT>& context)
        {
            if (context.useGlobalError)
            {
                std::println(
                    save,
                    "  __( ) -->|error: with global entry condition| {}",
                    getFullEntryName(detail::ERROR_MACHINE_NAME, context));
            }
            else if (context.machines.contains(detail::ERROR_MACHINE_NAME))
            {
                std::println(
                    save,
                    "  __( ) -->|error: no global entry condition| {}",
                    getFullEntryName(detail::ERROR_MACHINE_NAME, context));
            }
        }

        template<BlackboardTypeConcept BbT>
        void printAllIntraStateTransitions(
            const fsm::detail::BuilderContext<BbT>& context)
        {
            for (auto&& [machineName, machineContext] : context.machines)
            {
                for (auto&& [stateName, stateContext] : machineContext.states)
                {
                    const auto currentState =
                        detail::createFullStateName(machineName, stateName);

                    for (auto&& condition : stateContext.conditions)
                    {
                        printTransition(currentState, condition.destination);
                    }

                    printTransition(currentState, stateContext.destination);
                }
            }
        }

        auto printTransition(
            const std::string& source,
            const detail::TransitionContext& destination) -> void
        {

            if (destination.primary.empty())
            {
                if (destination.secondary.empty())
                {
                    std::println(
                        save, "  {0} -->|finish| __finish__{0}( )", source);
                }
                else
                    assert(false);
            }
            else
            {
                const auto&& currentMachineName =
                    detail::getMachineAndStateNameFromFullName(source).first;
                const auto&& targetMachineName =
                    detail::getMachineAndStateNameFromFullName(
                        destination.primary)
                        .first;

                const bool isTransitionToAnotherMachine =
                    currentMachineName != targetMachineName;

                if (isTransitionToAnotherMachine)
                {
                    if (destination.secondary
                            .empty()) // immediately finishing after return
                    {
                        std::println(
                            save,
                            "  {0} -->|submachine: {1}| __finish__{0}( )",
                            source,
                            targetMachineName);
                    }
                    else
                    {
                        std::println(
                            save,
                            "  {} -->|submachine: {}| {}",
                            source,
                            targetMachineName,
                            destination.secondary);
                    }
                }
                else if (source != destination.primary) // not yet logging
                                                        // looping states
                {
                    std::println(
                        save, "  {} --> {}", source, destination.primary);
                }
            }
        };

    private:
        std::ofstream fileStream;
        std::ostream& save;
    };
} // namespace fsm