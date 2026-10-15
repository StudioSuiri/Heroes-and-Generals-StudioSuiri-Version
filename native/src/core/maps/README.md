# Map definitions placeholder

No authored map format exists yet. Start with a data-only definition that contains map ID/version, world bounds, spawn anchors, objective anchors, static collision primitives and navigation hints. Keep positions in a single documented coordinate system.

Suggested files: map_schema.hpp, map_definition.hpp/.cpp, map_loader.hpp/.cpp, data/maps/greybox.json. Validate dimensions and resource limits before loading; the server must load/own the authoritative map state.
