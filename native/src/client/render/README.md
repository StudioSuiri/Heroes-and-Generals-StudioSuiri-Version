# Native rendering placeholder

The current rendering code draws simple shapes directly with SDL3. The native game does not need a browser process, Flash compatibility, Electron or Tauri.

Suggested future files: render_world.hpp/.cpp, render_map.hpp/.cpp, render_player.hpp/.cpp, camera.hpp/.cpp, ui_draw.hpp/.cpp, placeholder_assets.hpp/.cpp.

Keep draw-only state separate from authoritative simulation snapshots. First add interpolation and a camera, then a lightweight asset pipeline and generic models. Do not add a large engine or renderer dependency until profiling and actual feature requirements justify it.
