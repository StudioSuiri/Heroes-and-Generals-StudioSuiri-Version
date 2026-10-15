# Persistence placeholder

No database dependency is used in the first prototype; players live in memory. SQLite is the default candidate for local/self-hosted persistence after player profiles and campaign state become concrete.

Candidate files: schema.sql, migration_runner.hpp/.cpp, player_repository.hpp/.cpp, campaign_repository.hpp/.cpp, backup_restore.md.

Never write to a database in the 30 Hz simulation hot path. Persist completed battle results or queued events outside that loop. If accounts are added, use a modern password-hashing scheme; never store plaintext passwords or trust a password supplied as both identity and secret.
