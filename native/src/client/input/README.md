# Input layer placeholder

Input is currently sampled directly in the SDL3 client entrypoint. When the UI and controls grow, move input mapping into a separate layer.

Suggested future files: input_state.hpp, input_bindings.hpp/.cpp, input_sampler.hpp/.cpp, input_settings.hpp/.cpp. Support keyboard/mouse first, then gamepad. Convert device state into a small hng::ClientInput model; do not let platform input objects leak into the shared simulation or server.
