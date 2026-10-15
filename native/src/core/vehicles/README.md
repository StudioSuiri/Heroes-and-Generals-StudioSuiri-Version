# Vehicle domain placeholder

No vehicles are implemented. Planned order: unarmed transport, generic armoured vehicle, vehicle-mounted weapon, then additional variants.

Suggested future files: vehicle_definition.hpp, vehicle_state.hpp, vehicle_system.hpp/.cpp, seat_system.hpp/.cpp, vehicle_damage.hpp/.cpp, tests/vehicle_tests.cpp, data/vehicles/.

Use a simpler server-authoritative movement model with distinct handling parameters. Do not start with track simulation, suspension, aerodynamics or component-level damage. Vehicle spawn/resource rules should be introduced only after objectives and tactical matches are stable.
