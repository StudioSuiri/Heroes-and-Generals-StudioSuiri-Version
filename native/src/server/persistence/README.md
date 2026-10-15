# Server persistence service placeholder

The native prototype has in-memory player/session state only. SQLite is the initial candidate after durable profiles or campaign state become necessary.

Suggested files: schema.sql, migration_runner.hpp/.cpp, player_repository.hpp/.cpp, campaign_repository.hpp/.cpp, battle_result_repository.hpp/.cpp, backup_restore.md.

Keep SQL and disk writes outside the authoritative simulation hot path. Apply schema migrations explicitly, use transactions for battle results and provide a local backup/restore test before relying on persistent player progress.
