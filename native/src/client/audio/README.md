# Audio layer placeholder

No audio library is added to the initial vertical slice. Once movement/combat feedback is solid, select a small native audio path and introduce generic, licensed sounds only.

Suggested future files: audio_events.hpp, audio_mixer.hpp/.cpp, sound_bank.hpp/.cpp, assets/audio/manifest.json. Trigger sounds from game events such as shot, impact, reload, death and objective status. Limit voice/ambient mixing and asset decode work outside the simulation hot path.
