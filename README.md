# ase-player

[![Layer](https://img.shields.io/badge/Layer-3%20Modules-green.svg)]()
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)]()
[![ECS](https://img.shields.io/badge/Pattern-ECS-orange.svg)]()

> Player entity management, physics simulation, input handling, and network synchronization

Part of [ASE - Antares Simulation Engine](../../..)

## Overview

The **ase-player** module manages all aspects of player entities in the ASE engine, from initial spawn through active gameplay to graceful disconnect handling. It handles spawning (creating player entities with initial position, orientation, and component composition via request/response pattern), despawning (cleanup on disconnect including inventory persistence and territory release), input-to-movement translation (reading InputStateComponent and applying movement with collision detection against terrain), physics integration (gravity, jumping, falling, swimming state detection), and network broadcasting (replicating player state to nearby clients at 20Hz via the replication module). The player movement system implements server-authoritative movement with client-side prediction — the server processes the canonical movement simulation while clients predict locally, with reconciliation when states diverge. Player entities serve as the anchor point for all player-facing systems: camera tracks the player, terrain streams chunks around the player position, and combat reads player state for engagement resolution. Key features:

- **Lifecycle Management**: Spawn and despawn players with request/response pattern
- **Input Processing**: Translates network input into movement vectors
- **Movement Simulation**: First-person character controller with ground detection
- **Physics Integration**: AABB collision, gravity, ground snapping, velocity damping
- **Spatial Tracking**: Monitors which terrain chunk player occupies
- **Network Broadcasting**: Marks dirty players for state replication
- **Status Management**: Health, alive/dead state, respawn logic

## Architecture

### System Execution Order (FixedUpdate - 30Hz)

1. **PlayerLifeSpawnSystem** - Process spawn/despawn requests
2. **PlayerCtrlInputSystem** - Convert input to movement intent (runs after TerrainChunkSystem)
3. **PlayerCtrlMoveSystem** - Apply movement intent to velocity
4. **PlayerSimPhysSystem** - Physics simulation (gravity, collision, ground detection)
5. **PlayerStateStatusSystem** - Update player status (health, alive state)
6. **PlayerSpatialChunkSystem** - Track current chunk, detect chunk changes
7. **PlayerNetBroadcastSystem** - Mark dirty players for network sync

### Last (Debug)
8. **PlayerLogCausalitySystem** - Causality logging and debug visualization

### Player Entity Lifecycle

```
Spawn Request → Create Entity → Apply Components → Active Player
     ↓                                                    ↓
NetworkPlrReqSpawn ────────────────────────────────→ PlayerStaIdntComponent
                                                        PlayerStaPosComponent
                                                        PlayerStaVelComponent
                                                        PlayerStaPhysComponent
                                                        PlayerStMovComponent
                                                        PlayerStaStsComponent
                                                        PlayerStaChkComponent
                                                        PlayerSpndTag
                                                        ↓
                                            Movement/Physics Loop (30Hz)
                                                        ↓
                                            Despawn Request → Destroy Entity
```

## Components

### State Components

#### PlayerStaIdntComponent
```cpp
struct PlayerStaIdntComponent {
    uint32_t client_id = 0;      // Network client ID
    uint64_t player_id = 0;      // Unique player ID
    char name[32] = {};          // Player name
};
```

#### PlayerStaPosComponent
```cpp
struct PlayerStaPosComponent {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};
```

#### PlayerStaVelComponent
```cpp
struct PlayerStaVelComponent {
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
};
```

#### PlayerStMovComponent
```cpp
struct PlayerStMovComponent {
    float move_forward = 0.0f;   // -1.0 to 1.0
    float move_right = 0.0f;     // -1.0 to 1.0
    float move_speed = 5.0f;     // m/s
    float sprint_multiplier = 1.5f;
    bool is_sprinting = false;
    bool wants_jump = false;
};
```

#### PlayerStaPhysComponent
```cpp
struct PlayerStaPhysComponent {
    bool on_ground = false;
    float ground_height = 0.0f;
    float gravity = -9.81f;      // m/s²
    float jump_velocity = 5.0f;  // m/s
    float damping = 0.9f;        // velocity damping
};
```

#### PlayerStaStsComponent
```cpp
struct PlayerStaStsComponent {
    float health = 100.0f;
    float max_health = 100.0f;
    bool is_alive = true;
    float respawn_timer = 0.0f;
};
```

#### PlayerStaChkComponent
```cpp
struct PlayerStaChkComponent {
    int32_t chunk_x = 0;
    int32_t chunk_y = 0;
    int32_t chunk_z = 0;
    int32_t prev_chunk_x = 0;
    int32_t prev_chunk_y = 0;
    int32_t prev_chunk_z = 0;
};
```

### Request Components

#### PlayerReqSpwnComponent
```cpp
struct PlayerReqSpwnComponent {
    uint32_t client_id = 0;
    float spawn_x = 0.0f;
    float spawn_y = 0.0f;
    float spawn_z = 0.0f;
    char player_name[32] = {};
};
```

#### PlayerReqSpwnResComponent
Result of spawn request (entity ID, success flag).

#### PlayerReqDespComponent
Despawn request (player entity to destroy).

#### PlayerReqDespResComponent
Result of despawn request.

### Tag Components

#### PlayerManagerTag
Marks the player manager singleton entity.

#### PlayerSpndTag
Marks player as successfully spawned and active.

#### PlayerLoclTag
Marks player as locally controlled (client-side authority).

#### PlayerInitTag
Marks player as initialized.

#### PlayerDrtyTag
Marks player state as dirty (needs network broadcast).

#### PlayerChnkChgdTag
Marks player as having changed chunks (triggers observer update).

## Systems

### PlayerLifeSpawnSystem
Processes spawn and despawn requests:
- **Spawn**: Creates player entity with all required components, initializes position/velocity
- **Despawn**: Destroys player entity, cleans up references
- **Response**: Generates response components with success/failure status

### PlayerCtrlInputSystem
Converts raw network input into movement intent:
- Reads input state from network component
- Normalizes movement vectors
- Sets jump flag
- Runs after TerrainChunkSystem to ensure terrain data is available

### PlayerCtrlMoveSystem
Applies movement intent to velocity:
- Calculates movement vector from input
- Applies speed multipliers (sprint, crouch)
- Respects physics constraints (no air control)
- Updates velocity component

### PlayerSimPhysSystem
Physics simulation and collision detection:
- **Gravity**: Applies constant downward acceleration
- **Ground Detection**: Raycasts to terrain height map
- **Collision**: AABB collision with terrain
- **Ground Snapping**: Snaps player to ground surface when close
- **Velocity Damping**: Applies friction/air resistance
- **Jump**: Impulse velocity when grounded and jump requested

### PlayerStateStatusSystem
Updates player status:
- Health regeneration/damage
- Alive/dead state transitions
- Respawn timer countdown
- Death handling (velocity zero, trigger respawn)

### PlayerSpatialChunkSystem
Tracks which terrain chunk player occupies:
- Converts world position to chunk coordinates
- Detects chunk boundary crossings
- Sets `PlayerChnkChgdTag` on chunk change
- Updates previous chunk tracking

### PlayerNetBroadcastSystem
Marks dirty players for network synchronization:
- Detects state changes (position, velocity, health)
- Sets `PlayerDrtyTag` for replication system
- Throttles updates to 20Hz (replication schedule)

### PlayerLogCausalitySystem
Debug logging and causality tracking:
- Logs player spawns/despawns
- Tracks movement events
- Performance profiling
- Causality chain visualization

## Usage

### Module Registration

```cpp
#include <ase/player/player_module.hpp>

ase::ecs::App()
    .add_kernel<ase::kernel::Kernel>()
    .add_module<ase::player::PlayerModule>()
    .run();
```

### Spawning a Player

```cpp
// Create spawn request
auto request = registry.create();
registry.emplace<PlayerReqSpwnComponent>(request,
    client_id,
    spawn_x, spawn_y, spawn_z,
    "PlayerName"
);

// PlayerLifeSpawnSystem processes on next FixedUpdate
// Check for response
auto* response = registry.try_get<PlayerReqSpwnResComponent>(request);
if (response && response->success) {
    Entity player_entity = response->player_entity;
    // Player spawned successfully
}
```

### Despawning a Player

```cpp
// Create despawn request
auto request = registry.create();
registry.emplace<PlayerReqDespComponent>(request, player_entity);

// PlayerLifeSpawnSystem processes on next FixedUpdate
```

### Updating Player Input

```cpp
// Input arrives from network, PlayerCtrlInputSystem reads it
auto view = registry.view<PlayerStMovComponent>();
for (auto [entity, mov] : view.each()) {
    // Input system has already set mov.move_forward, mov.move_right
    // PlayerCtrlMoveSystem will apply on next tick
}
```

### Accessing Player Position

```cpp
auto view = registry.view<PlayerStaPosComponent, PlayerStaIdntComponent>();
for (auto [entity, pos, id] : view.each()) {
    spdlog::info("Player {} at ({:.2f}, {:.2f}, {:.2f})",
                 id.name, pos.x, pos.y, pos.z);
}
```

## API Reference

### Player Configuration

Default values are typically set in component initializers:

```cpp
constexpr float DefaultMoveSpeed = 5.0f;        // m/s
constexpr float DefaultSprintMultiplier = 1.5f;
constexpr float DefaultGravity = -9.81f;        // m/s²
constexpr float DefaultJumpVelocity = 5.0f;     // m/s
constexpr float DefaultDamping = 0.9f;
constexpr float DefaultMaxHealth = 100.0f;
```

### Helper Functions

```cpp
// Convert world position to chunk coordinates
int32_t chunk_x = static_cast<int32_t>(std::floor(world_x / 32.0f));
int32_t chunk_y = static_cast<int32_t>(std::floor(world_y / 32.0f));
int32_t chunk_z = static_cast<int32_t>(std::floor(world_z / 32.0f));
```

## Dependencies

### Layer 3 (Modules)
- **ase-terrain**: Ground height queries, chunk tracking

### Layer 2 (Kernel)
- **ase-kernel**: Game loop and schedule management

### Layer 1 (Core)
- **ase-ecs**: Entity Component System
- **ase-log**: Logging

### Layer 0 (Foundation)
- **ase-math**: Vector operations, collision detection
- **ase-types**: Common type definitions

## Physics Characteristics

- **Gravity**: -9.81 m/s² (Earth-like)
- **Move Speed**: 5 m/s walk, 7.5 m/s sprint
- **Jump Height**: ~1.3 meters (5 m/s initial velocity)
- **Ground Snap Distance**: 0.1 meters
- **AABB Size**: Configurable per player (typically 0.5m x 1.8m x 0.5m)
- **Velocity Damping**: 0.9 (10% energy loss per tick)
- **Update Rate**: 30Hz (FixedUpdate schedule)

## Network Protocol

Player state is replicated at 20Hz via the replication schedule. Only dirty components are sent:

- **Position**: Always replicated
- **Velocity**: Only if changed > threshold
- **Health**: Only on damage/heal events
- **Chunk**: Only on chunk boundary crossing

## Thread Safety

The player module is **single-threaded** and runs on the main ECS thread. All player state is accessed from the FixedUpdate schedule.

## License

MIT
