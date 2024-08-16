#pragma once

#include <DGM/classes/Fsm.hpp>

namespace dgm
{
    namespace fsm
    {
        template<StateTypeConcept StateType, class... BlackboardTypes>
        class Builder;

        namespace builders
        {
            template<StateTypeConcept StateType, class... BlackboardTypes>
            class [[nodiscard]] BuilderContext final
            {
            public:
                void addState(
                    StateType code,
                    std::same_as<State<StateType, BlackboardTypes...>> auto&&
                        state)
                {
                    states.emplace(code, std::forward<decltype(state)>(state));
                }

            public:
                std::map<StateType, State<StateType, BlackboardTypes...>>
                    states;
                StateType currentlyAddedState;
            };

            template<StateTypeConcept StateType, class... BlackboardTypes>
            class [[nodiscard]] DefaultTransitionBuilder final
            {
            public:
                constexpr DefaultTransitionBuilder(
                    builders::BuilderContext<StateType, BlackboardTypes...>&&
                        context,
                    std::vector<Transition<StateType, BlackboardTypes...>>
                        transitions,
                    Logic<BlackboardTypes...> logic)
                    : context(context), transitions(transitions), logic(logic)
                {
                }

                [[nodiscard]] auto andGoTo(StateType state)
                {
                    context.addState(
                        context.currentlyAddedState,
                        State<StateType, BlackboardTypes...>(
                            { .transitions = transitions,
                              .logic = logic,
                              .targetState = state }));
                    return Builder(std::move(context));
                }

                [[nodiscard]] auto andLoop()
                {
                    context.addState(
                        context.currentlyAddedState,
                        State<StateType, BlackboardTypes...>(
                            { .transitions = transitions,
                              .logic = logic,
                              .targetState = context.currentlyAddedState }));
                    return Builder(std::move(context));
                }

            private:
                builders::BuilderContext<StateType, BlackboardTypes...> context;
                std::vector<Transition<StateType, BlackboardTypes...>>
                    transitions;
                Logic<BlackboardTypes...> logic;
            };

            // Forward declaring transition builder because there is cyclic
            // reference between it and StateBuilder
            template<StateTypeConcept StateType, class... BlackboardTypes>
            class TransitionBuilder;

            template<StateTypeConcept StateType, class... BlackboardTypes>
            class [[nodiscard]] StateBuilder final
            {
            public:
                constexpr StateBuilder(
                    builders::BuilderContext<StateType, BlackboardTypes...>&&
                        context,
                    std::vector<Transition<StateType, BlackboardTypes...>>
                        transitions)
                    : context(context), transitions(transitions)
                {
                }

                [[nodiscard]] auto
                orWhen(Condition<BlackboardTypes...> condition)
                {
                    return TransitionBuilder<StateType, BlackboardTypes...>(
                        std::move(context), transitions, condition);
                }

                [[nodiscard]] auto
                otherwiseExec(Logic<BlackboardTypes...> logic)
                {
                    return DefaultTransitionBuilder<
                        StateType,
                        BlackboardTypes...>(
                        std::move(context), transitions, logic);
                }

            private:
                builders::BuilderContext<StateType, BlackboardTypes...> context;
                std::vector<Transition<StateType, BlackboardTypes...>>
                    transitions;
            };

            template<StateTypeConcept StateType, class... BlackboardTypes>
            class [[nodiscard]] TransitionBuilder final
            {
            public:
                constexpr TransitionBuilder(
                    builders::BuilderContext<StateType, BlackboardTypes...>&&
                        context,
                    std::vector<Transition<StateType, BlackboardTypes...>>
                        transitions,
                    Condition<BlackboardTypes...> condition)
                    : context(context)
                    , transitions(transitions)
                    , condition(condition)
                {
                }

                [[nodiscard]] auto goTo(StateType state)
                {
                    transitions.push_back(
                        Transition<StateType, BlackboardTypes...>(
                            condition, state));
                    return StateBuilder<StateType, BlackboardTypes...>(
                        std::move(context), transitions);
                }

            private:
                builders::BuilderContext<StateType, BlackboardTypes...> context;
                std::vector<Transition<StateType, BlackboardTypes...>>
                    transitions;
                Condition<BlackboardTypes...> condition;
            };

            template<StateTypeConcept StateType, class... BlackboardTypes>
            class [[nodiscard]] EmptyStateBuilder final
            {
            public:
                constexpr EmptyStateBuilder(
                    builders::BuilderContext<StateType, BlackboardTypes...>&&
                        context)
                    : context(context)
                {
                }

                [[nodiscard]] auto when(Condition<BlackboardTypes...> condition)
                {
                    return TransitionBuilder<StateType, BlackboardTypes...>(
                        std::move(context), {}, condition);
                }

                [[nodiscard]] auto exec(Logic<BlackboardTypes...> logic)
                {
                    return DefaultTransitionBuilder<
                        StateType,
                        BlackboardTypes...>(std::move(context), {}, logic);
                }

            private:
                builders::BuilderContext<StateType, BlackboardTypes...> context;
            };

            template<StateTypeConcept StateType, class... BlackboardTypes>
            class [[nodiscard]] FinalBuilder final
            {
            public:
                FinalBuilder(
                    BuilderContext<StateType, BlackboardTypes...>&& context,
                    Transition<StateType, BlackboardTypes...>
                        globalErrorTransition)
                    : context(std::move(context))
                    , globalErrorTransition(globalErrorTransition)
                {
                }

                [[nodiscard]] auto build()
                {
                    return Fsm<StateType, BlackboardTypes...>(
                        std::move(context.states), globalErrorTransition);
                }

            private:
                BuilderContext<StateType, BlackboardTypes...> context;
                Transition<StateType, BlackboardTypes...> globalErrorTransition;
            };

            template<StateTypeConcept StateType, class... BlackboardTypes>
            class [[nodiscard]] GlobalErrorBuilder final
            {
            public:
                GlobalErrorBuilder(
                    BuilderContext<StateType, BlackboardTypes...>&& context,
                    Condition<BlackboardTypes...> condition)
                    : context(context), condition(condition)
                {
                }

                [[nodiscard]] auto goTo(StateType state)
                {
                    return FinalBuilder<StateType, BlackboardTypes...>(
                        std::move(context),
                        Transition<StateType, BlackboardTypes...>(
                            condition, state));
                }

            private:
                BuilderContext<StateType, BlackboardTypes...> context;
                Condition<BlackboardTypes...> condition;
            };
        } // namespace builders

        template<StateTypeConcept StateType, class... BlackboardTypes>
        class [[nodiscard]] Builder final
        {
        public:
            Builder() = default;

            Builder(builders::BuilderContext<StateType, BlackboardTypes...>&&
                        context)
                : context(std::move(context))
            {
            }

            Builder(Builder&&) = delete;
            Builder(const Builder&) = delete;

        public:
            [[nodiscard]] auto with(StateType state)
            {
                context.currentlyAddedState = state;
                return builders::
                    EmptyStateBuilder<StateType, BlackboardTypes...>(
                        std::move(context));
            }

            [[nodiscard]] auto
            withGlobalErrorCondition(Condition<BlackboardTypes...> condition)
            {
                return builders::GlobalErrorBuilder(
                    std::move(context), condition);
            }

            [[nodiscard]] auto build()
            {
                return Fsm<StateType, BlackboardTypes...>(
                    std::move(context.states));
            }

        private:
            builders::BuilderContext<StateType, BlackboardTypes...> context;
        };
    } // namespace fsm
} // namespace dgm
