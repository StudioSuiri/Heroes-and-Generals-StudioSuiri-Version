# Planned simulation systems

The prototype intentionally keeps core logic in simulation.cpp while the mechanics are small enough to reason about. Split it into independently tested systems when a second implementation or balancing need justifies it.

Planned candidates:
- movement_system.hpp/.cpp — fixed-step acceleration, stance, collisions and movement constraints.
- combat_system.hpp/.cpp — weapon definitions, recoil, spread, hit validation, damage and armor penetration.
- objective_system.hpp/.cpp — multi-point capture, contest rules, timers and battle victory.
- spawn_system.hpp/.cpp — team spawns, tickets, resource availability and spawn protection.
- vehicle_system.hpp/.cpp — vehicle state, seats, damage and destruction.
- replication_system.hpp/.cpp — input history, snapshot sequences, interpolation and later delta state.

These filenames are planned boundaries, not implemented features. Avoid creating abstraction-only classes before there is concrete code and tests for them.
