# Mechanics Catalog — Historical Reference and Native Simplification

Status: design inventory for a clean-room reimplementation, not a promise of exact patch-for-patch compatibility. Historical references below are community-maintained and may describe different game versions. Where exact values are uncertain, the native game should define and test its own parameters rather than present guesses as original values.

## 1. Core identity to preserve

The strongest identity of Heroes & Generals was the connection between two scales: a tactical real-time combined-arms battle and a campaign where player-controlled assault teams moved over a strategic map, connected battles along supply lines, and changed who controlled cities and capitals.

Preserve this loop: **campaign creates battles and allocates resources; battle results feed back into the campaign**. It does not require two different UI runtimes or the old private wire protocol. The strategic map can be a native screen in the same executable.

Historical references: [Strategy Game](https://heroesandgenerals.fandom.com/wiki/Strategy_Game), [Supply lines](https://heroesandgenerals.fandom.com/wiki/Supply_lines), [Action Rules](https://heroesandgenerals.fandom.com/wiki/Action_Rules).

## 2. Historical systems inventory

### 2.1 Factions and persistent war

The historical campaign used three factions: United States, Germany and Soviet Union. The campaign map connected cities and capital cities. Territorial ownership and supply connections influenced where assault teams could move and where a battle could be initiated. One documented ruleset used control of 15 of 23 capitals as a victory condition; exact victory rules varied across eras.

**Simplify first:** use two factions and a graph of 8–12 fictional territories; later add a third faction and grow the graph from data. Implement territory ownership, capital flag, edges, movement rules and one campaign victory condition. Avoid hardcoding the original European map, real capital names or the historical win threshold as required launch content.

Reference: [Strategy Game](https://heroesandgenerals.fandom.com/wiki/Strategy_Game).

### 2.2 Supply lines and assault teams

Assault teams were strategic units controlled by players in the campaign layer. They travelled along connected supply lines; contact with an opposing team could create a battle. Supply lines controlled access to connected territories, and the types and quantities of assault teams helped determine resources available in the tactical match. Guard/infantry, recon, airborne/pathfinder and vehicle-oriented resources existed in different forms.

**Simplify first:** represent the campaign as a graph with Territory nodes (owner, capital flag, neighbours, battle lock), SupplyEdge objects (connection and movement cost), AssaultTeam records (faction, archetype, resource budget, current node, order and readiness), and a BattleSeed (map, mode, teams, resource caps and deterministic seed).

Do not begin with every historical assault-team type, individual unit counts, resupply formulas, command ranks or war currency. A handful of tokens with a simple cooldown is enough to prove the campaign-to-battle loop.

References: [Strategy Game](https://heroesandgenerals.fandom.com/wiki/Strategy_Game), [Assault Team](https://heroesandgenerals.fandom.com/wiki/Assault_Team).

### 2.3 Tactical modes and objective flow

Historical guides describe:
- **Encounter:** compact battle around one main objective.
- **Skirmish:** generally three capture points; the team holding a majority for long enough wins. Points can usually be captured in either direction.
- **Assault:** attacker/defender roles. Attackers take a connected sequence or group of primary objectives; defenders delay them until time expires or otherwise fulfil the mode's win condition.
- Additional specialist modes and map variants existed in some versions.

Objective progress, control state, match timer, spawn access and battle result mattered beyond kill count.

**Simplify first:** implement a mode data structure with spawn areas, one-to-three objectives, attacker/defender roles, capture radius, capture rate and match clock. Keep capture and victory rules on the authoritative server. MVP uses one neutral point; next version uses three independently capturable points; only then add an ordered assault line.

References: [Action Rules](https://heroesandgenerals.fandom.com/wiki/Action_Rules), [Skirmish](https://heroesandgenerals.fandom.com/wiki/Skirmish).

### 2.4 Infantry combat

The essential loop includes movement, aiming, firing, hit validation, health/damage, death, respawn, ammunition, reload, cover/line-of-sight and objectives. Weapon variety supported multiple ranges and tactical roles. Exact weapon parameters changed between historical balance patches.

**Simplify first:** one generic rifle; fixed magazine size; one reload action; server-authoritative hitscan with modest spread, cooldown and visible damage feedback. Add collision and line-of-sight before complex projectiles. Tune damage/time-to-kill through reproducible tests instead of copying unverified numerical values.

Future weapon definitions should be data-driven: identifier, fire mode, damage, effective range, spread, recoil, magazine capacity, reload duration, projectile/hitscan type and allowed archetypes. Avoid a C++ subclass per gun.

### 2.5 Soldier archetypes and squads

Historical roles included:
- **Infantry:** flexible frontline and objective role.
- **Recon:** long-range observation and precision fire.
- **Tank crew:** operate armoured vehicles.
- **Pilot:** aircraft and air combat.
- **Paratrooper/pathfinder:** airborne insertion and objective pressure.
- **General / strategy player:** operates campaign-level resources and assault teams.
- Other motorized or mechanized loadouts connected infantry with transport.

Roles differ by movement, equipment budget, available vehicle/assault-team resources and the objective they support.

**Simplify first:** one infantry archetype represented by data, not a class hierarchy. Then add recon/spotting, tanker and one ground vehicle. Pilot and parachute insertion should come much later because flight, airspace, spawn timing, large map scale and extra replication increase scope significantly.

References: [Getting Started](https://heroesandgenerals.fandom.com/wiki/Getting_Started), [Classes](https://heroesandgenerals.fandom.com/wiki/Classes).

### 2.6 Vehicles and combined arms

The historic game included motorcycles and transports, armoured cars, tanks and aircraft. Vehicles provided mobility, protection and firepower; their availability could depend on strategic resources and valid spawn locations. Seats/crew and vehicle-specific roles connected them to soldier class and loadouts.

**Simplify first:** add one unarmed transport with driver seat, server-side movement and enter/exit; then one slow armoured vehicle with durability and a single weapon. Reuse shared damage/event interfaces where it makes sense, but do not force infantry and vehicles to share every movement or collision rule. Skip suspension simulation, track links, drivetrain, aerodynamic detail, component damage and advanced ballistics early.

### 2.7 Spawn rules, reinforcements and resupply

Spawning historically depended on map mode, team spawn zones, objective ownership, role, vehicle access points and available strategic resources. Some modes allowed access to captured objectives. Resupply points and resource budgets constrained extended combat.

**Simplify first:** fixed team spawn locations and respawn delay. Then add spawn selection at captured objectives, short spawn protection and finite team tickets. Add ammunition/health crates only after reload and damage rules are stable. Resource/ticket state is authoritative; never trust a client to claim unlimited ammunition or a valid spawn.

### 2.8 Loadouts, career, unlocks and economy

The full game included multiple soldiers, loadouts, equipment limits, progression, ranks/ribbons/badges, weapon unlocks, currencies, repair/resupply costs and a separate war economy for campaign units. These systems added long-term goals but also enlarged the backend, balancing surface and persistence workload.

**Simplify first:** every tester gets the same rifle and all test options unlocked. Later add profile/loadout persistence and a small unlock tree. Reward objective contribution and combat performance. Keep tactical soldier progression separate from campaign resource budgets. Add SQLite only when profile or campaign persistence is needed; do not access the database in the simulation tick.

References: [Getting Started](https://heroesandgenerals.fandom.com/wiki/Getting_Started), [Warfunds](https://heroesandgenerals.fandom.com/wiki/Warfunds).

### 2.9 Maps, visuals and HUD

Tactical maps had spawn zones, roads, cover, objective layouts, combat lanes and vehicle access. The separate campaign map exposed the wider war state. That interface layering made strategic play depend on a different presentation/runtime path.

**Simplify first:** one greybox arena with an objective. Add a collision grid or simple collision volumes and generic cover. A native screen stack will contain main menu, server address entry, soldier selection, battle HUD, scoreboard, loadout and eventually campaign map. No browser overlay, Flash replacement or browser process is needed.

### 2.10 Sound, animation, feedback and feel

Visual/audio feedback is gameplay, not polish alone. Muzzle flash, hit confirmation, directional audio, reload timing, footsteps, damage cues, spawn feedback and objective-state changes tell the player what happened.

**Simplify first:** placeholders expose state changes clearly. Add lightweight sound and animation after server results are stable. Effects should be driven by game events; the client may render them but must not invent authoritative damage or capture results.

## 3. Native architecture and simplification boundaries

- **Shared C++20 core:** simulation state, packet types, weapon/objective rules and deterministic helpers.
- **Native SDL3 client:** input, window, rendering, native menus and local audio.
- **Headless server:** authoritative world, damage, spawn, objectives, match result and eventually campaign/persistence services.
- **Explicit protocol:** small versioned UDP messages for frequent inputs/snapshots. Use reliability only for events that need it rather than implementing every old service protocol.
- **Local campaign screen:** draw territory nodes and links inside the standalone client; campaign service supplies data.
- **Data-driven maps and weapons:** keep content separate from simulation logic so tuning does not require rewiring systems.
- **One deployment model:** Windows client connects to the same headless server implementation compiled for Linux or Windows.

## 4. Latency and performance strategy

The user experienced substantial lag in the historical product. The external Flash/browser surface is an obvious layer to remove, but removing that surface alone does **not** prove the cause of historical network/server lag is fixed. Measure each latency segment independently.

1. **Remove separate runtime boundaries:** one native executable for all player-facing screens; no browser/Flash dependency.
2. **Use fixed-step authoritative simulation:** start at 30 ticks/s, profile tick duration and overruns before considering higher rates.
3. **Separate render and simulation:** render at display cadence; do not make gameplay timing depend on UI frame rate.
4. **Bound packets and parsing:** versioned binary format, explicit lengths, reject malformed/oversized payloads and cap datagrams. Current v1 is LAN-only.
5. **Keep hot paths simple:** no JSON, disk access, database operations or verbose per-input logging in the tick loop.
6. **Interpolate first, predict later:** snapshot interpolation often smooths remote movement; add prediction/reconciliation only after tests show where needed.
7. **Avoid premature architecture:** one process and one authoritative match initially. No microservices, Redis, Kubernetes or replicated campaign shards until measurements require them.
8. **Profile before micro-optimizing:** record CPU time, memory allocation, tick overrun, packets per second, snapshot bytes, packet loss and RTT. Fix measured bottlenecks first. Avoid SIMD/assembly unless profiling proves a hot loop needs it.
9. **Test conditions, not just localhost:** loopback, trusted LAN, injected delay/loss, reconnect and malformed packets on the designated nodes. Keep tests local.
10. **Harden before public hosting:** the current prototype has no authentication, encryption, anti-cheat or robust rate-limit hardening. Do not expose it publicly.

## 5. Milestone definitions

### MVP-0 — five-to-ten-minute playable slice
Two factions, one infantry archetype, one rifle, movement, aim, shooting, health, respawn, one capture point and match-state display. One Windows client connects to a Linux headless server over LAN. No accounts or persistence.

### MVP-1 — recognizable tactical battle
Native menu, weapon magazine/reload, multiple objectives, attack/defence/encounter rules, tickets/timer, objective-aware spawn, collision/cover, scoreboard and stable replication.

### MVP-2 — combined arms
One transport, then one armoured vehicle. Add enter/exit seats, vehicle damage, spawn/resource constraints and balancing tests.

### MVP-3 — strategic identity
Native campaign map, territory ownership, supply graph, assault-team tokens and deterministic battle creation/result transfer. Campaign simulation runs separately from the 30 Hz tactical loop.

### MVP-4 — long-term systems
SQLite persistence, profile/loadout, progression, economy, more classes/vehicles, content pipeline, audio/animation, moderation and security review. Public operation is a separate release gate.

## 6. What remains unknown

The audited public repositories do not establish complete authoritative specifications for historical weapon numbers, vehicle physics, each map's collision, progression/economy formulas, all tactical rules across versions, or the current private restoration backend. Use historical references to identify mechanic families; use explicit parameters and playtests to determine native balance.
