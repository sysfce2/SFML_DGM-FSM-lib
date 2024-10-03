#include "Blackboard.hpp"
#include "CsvParser.hpp"
#include "catch_amalgamated.hpp"
#include <fsm/Builder.hpp>
#include <fsm/exports/MermaidExporter.hpp>

TEST_CASE("[MermaidExporter]")
{
    SECTION("Properly logs submachine call with immediate finish")
    {
        std::stringstream ss;

        // clang-format off
        std::ignore = fsm::Builder<Blackboard>()
            .withNoErrorMachine()
            .withSubmachine("2")
                .withEntryState("B")
                    .exec(nothing)
                        .andFinish()
                .done()
            .withSubmachine("1")
                .withEntryState("A")
                    .exec(nothing)
                        .andGoToMachine("2")
                            .thenFinish()
                .done()
            .withMainMachine()
                .withEntryState("Start")
                    .exec(nothing)
                        .andGoToMachine("1")
                            .thenGoToState("End")
                .withState("End")
                    .exec(nothing).andLoop()
            .done()
        .exportDiagram(fsm::MermaidExporter(ss))
        .build();
        // clang-format on

        const auto expectedLog =
            "flowchart TD\n"
            "  _( ) -->|entry| __main__:Start\n"
            "  1:A -->|submachine: 2| __finish__1:A( )\n"
            "  2:B -->|finish| __finish__2:B( )\n"
            "  __main__:Start -->|submachine: 1| __main__:End\n";

        REQUIRE(ss.str() == expectedLog);
    }
}