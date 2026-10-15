# Asset loader placeholder

There is no asset loader yet; the current client uses native drawing primitives. Add asset loading only when the generic greybox becomes insufficient. Keep decoding/loading off the authoritative server simulation path. Document file format, coordinate scale, license, attribution and hash.

Potential future split: asset_manifest.json, texture_registry.hpp/.cpp, model_registry.hpp/.cpp, audio_registry.hpp/.cpp. Keep dependencies small and avoid adding an engine or browser runtime simply to load sprites and generic test meshes.
