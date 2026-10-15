# Planned server service boundaries

The current server is a single UDP process with an in-memory simulation. The following services are not implemented yet:

- session_service.hpp/.cpp — join, leave, timeout, reconnect and session lifetime.
- account_service.hpp/.cpp — add only after the offline gameplay loop works; never store plaintext passwords.
- persistence_service.hpp/.cpp — versioned schema, migrations, durable profile/campaign state and restore.
- matchmaking_service.hpp/.cpp — battle allocation and capacity management.
- battle_service.hpp/.cpp — maps, rulesets, battle lifecycle and results.
- campaign_service.hpp/.cpp — strategic territories, supply graph, assault teams and battle creation.
- telemetry_service.hpp/.cpp — bounded local metrics; never log each frame or packet verbosely by default.
- admin_service.hpp/.cpp — controlled admin operations, moderation and audit log.

Until these are implemented and reviewed, use only loopback or a trusted private LAN. The server, not clients, must own damage, positions, capture, faction assignment, vehicle state and rewards.
