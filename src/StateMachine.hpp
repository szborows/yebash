#pragma once

#include <list>
#include <functional>

template <typename State, typename Input>
class StateTransition {

    using TransitionAction = std::function<void()>;

    Input inputCondition_;
    State in_, out_;
    TransitionAction transitionAction_;

public:

    explicit inline StateTransition(Input inputCondition, State in, State out, TransitionAction transitionAction)
        : inputCondition_(inputCondition), in_(in), out_(out), transitionAction_(transitionAction) {}

    inline bool check(State currentState, Input input) const {
        return (inputCondition_ == input && in_ == currentState);
    }

    inline State run() const {
        if (transitionAction_) transitionAction_();
        return out_;
    }

};

template <typename State, typename Input>
class StateMachine {

    State currentState_;
    std::list<StateTransition<State, Input>> transitions_;

public:

    explicit inline StateMachine(const std::initializer_list<StateTransition<State, Input>> &list) : transitions_(list) {}

    inline bool trigger(Input input) {
        for (auto &s : transitions_) {
            if (s.check(currentState_, input)) {
                auto newState = s.run();
                currentState_ = newState;
                return true;
            }
        }
        return false;
    }

};

