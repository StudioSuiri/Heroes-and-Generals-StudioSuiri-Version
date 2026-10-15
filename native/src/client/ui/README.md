# Native UI plan

The current client uses SDL3 for its native window, input, placeholder rendering and a simple HUD. No web view, Flash runtime, browser process or embedded HTML exists.

Planned screens, all inside the native application:
- main_menu — host/join, settings and quit.
- server_browser — manual address entry first; LAN discovery later.
- soldier_select — choose an archetype after more than infantry exists.
- battle_hud — objectives, health, ammunition, team tickets, timer and squad state.
- scoreboard — combat and objective contribution.
- loadout — equipment slots and supply points after progression exists.
- campaign_map — locally rendered strategic graph/map served by the native backend, never a browser/Flash surface.
- options — resolution, fullscreen, sensitivity, binds and audio.

Keep rendering separate from replicated state. If a network thread is introduced, it must never mutate SDL objects directly.
