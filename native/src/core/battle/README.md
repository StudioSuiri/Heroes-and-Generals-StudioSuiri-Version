# Tactical battle domain placeholder

The current simulation has one arena and one capture point, but no match lifecycle. Planned records/files:
- battle_definition.hpp/.cpp — map, mode, sides, time limit and resource caps.
- objective_definition.hpp/.cpp — capture radius, ownership, contest and attack ordering.
- battle_state.hpp/.cpp — lobby, live, overtime, complete.
- victory_rules.hpp/.cpp — mode-specific deterministic resolution.
- tests/battle_rules_tests.cpp — timer, simultaneous capture, overtime and result cases.

Do not bind campaign persistence directly into the 30 Hz loop. Publish a completed battle result for the campaign service to consume.
