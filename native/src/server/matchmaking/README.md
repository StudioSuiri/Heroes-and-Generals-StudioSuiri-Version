# Matchmaking placeholder

MVP-0 hosts one battle process; there is no matchmaking service. Later add a small battle directory that creates battle sessions by map, mode, faction slots and resource budget.

Suggested files: battle_directory.hpp/.cpp, matchmaking_rules.hpp/.cpp, server_registration.hpp/.cpp, tests/matchmaking_tests.cpp.

Prefer a single simple service before distributed match allocation. Verify capacity, reconnect, faction balance and timeout behavior. This folder does not contain an implemented matchmaking system.
