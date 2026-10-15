# Weapon data placeholder

The current prototype uses one hard-coded generic hitscan rule. Extract weapon data only after magazine/reload and hit feedback are implemented.

Suggested future files: weapon_definition.hpp, weapon_catalog.hpp/.cpp, weapon_runtime.hpp/.cpp, tests/weapon_rules_tests.cpp, data/weapons/placeholder_rifle.json.

Data should include ID/version, fire mode, damage, cadence, effective range, spread, recoil, magazine/reserve count and reload time. Never let a client choose arbitrary damage or bypass server cooldown/ammo validation.
