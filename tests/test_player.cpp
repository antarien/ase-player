// The doctest entry point is a compile definition of this target, set in tests/CMakeLists.txt —
// it reaches the translation unit before every #include, so doctest still generates exactly one
// main() here. A #define in the source would be a macro definition in C++, which is forbidden.
#include <doctest/doctest.h>

/**
 * ASE Player Module Tests (ECS)
 *
 * doctest CHECK/REQUIRE only - never assert() (Release/NDEBUG no-op).
 *
 * WHY THIS FILE WAS CONVERTED ON 2026-08-20, and what the conversion UNCOVERED:
 *
 *   All five compile_commands.json of the servers carry -O3 -DNDEBUG; 141 of 141 test
 *   translation units, not one without. Under NDEBUG the preprocessor replaces assert(expr)
 *   with ((void)0) - THE EXPRESSION IS NEVER COMPILED. So an assert may name things that no
 *   longer exist, and the build stays green.
 *
 *   This file did exactly that. Two lines checked `pos->x` and `pos->z`, but
 *   PlayerStaPosComponent has carried chunk_x / chunk_z / local_x / local_z / y since the S2b
 *   split of 2026-08-11 - the file even documents that split at the component test below. The
 *   dead assert hid a compile error for nine days.
 *
 *   The height claim was wrong too: `pos->y == 10.0f  // From terrain height` cannot hold.
 *   player_life_spwn_sys.cpp:334-337 reads the ground from the HUB
 *   (hub::get(registry, pos_hash, "TRN_HGT_AT_POS"_hs)), not from a terrain chunk. This test
 *   sets no Hub value, so ground_y stays 0.0f. See the two notes at the spawn test.
 *
 *   The target already linked doctest::doctest (tests/CMakeLists.txt:32-37); the file simply
 *   never used it. No build change was needed.
 *
 * IF A CHECK TURNS RED ON THE FIRST REAL RUN, THAT IS A FINDING, NOT A REGRESSION. The checks
 * were never executed; a failure appearing now was already failing, silently, for as long as
 * the build has carried -DNDEBUG. Do not "fix" it by weakening the check.
 */

#include <ase/player/player.hpp>
// The lifecycle chain (split 2026-08-30): birth, errand, death - three systems where one stood.
#include <ase/player/systems/lifecycle/player_life_spwn_sys.hpp>
#include <ase/player/systems/lifecycle/player_life_roam_ini_sys.hpp>
#include <ase/player/systems/lifecycle/player_life_desp_sys.hpp>
// The teleport lever and the session-start pulse (register D0703)
#include <ase/player/systems/sync/player_sync_tlpt_sys.hpp>
#include <ase/player/systems/simulation/player_sim_tlpt_sys.hpp>
#include <ase/player/systems/simulation/player_sim_phys_sys.hpp>
#include <ase/player/systems/anticheat/player_acc_mov_sys.hpp>
#include <ase/player/systems/hub/player_hub_sess_strt_sys.hpp>
#include <ase/player/components/input/player_inp_tlpt_comp.hpp>
#include <ase/player/components/input/player_inp_trn_comp.hpp>
#include <ase/player/components/tag/player_sess_strt_tag.hpp>
#include <ase/player/components/tag/player_sess_done_tag.hpp>
#include <ase/hub/api.hpp>
#include <entt/core/hashed_string.hpp>
// No <ase/terrain/...>: ase::terrain was removed from ase-player on 2026-08-18 under R15, and
// the only code here that used it was dead scaffolding - see the tombstone below.
#include <ase/ecs/system.hpp>
#include <ase/ecs/app.hpp>
#include <ase/ecs/internal/dependency_sorter.hpp>
#include <ase/containers/vector.hpp>
#include <ase/containers/hash_map.hpp>
#include <cstring>
// Names in INFO lines as text: doctest prints a raw `const char*` as an ADDRESS unless
// DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING is set, and the message would name no system.
#include <string>

using namespace ase;
using namespace ase::player;

namespace {

constexpr uint32_t LIFECYCLE_STAGE_COUNT = 3u;

/**
 * The lifecycle chain as player_module.hpp registers it in Dynamics: PlayerLifeSpwnSystem answers
 * spawn requests, PlayerLifeRoamIniSystem sends a newborn walker on its errand, and
 * PlayerLifeDespSystem answers despawn requests. Until 2026-08-30 the first one answered all
 * three; since the split a helper that ticks only it leaves every despawn request unanswered, and
 * the despawn case below was red for exactly that reason.
 *
 * The names of the stages it ticked land in `ran`, in tick order - the order case at the end of
 * this file reads them from there, so the list it checks is the list that runs.
 */
void run_lifecycle(ase::ecs::Registry& registry, const char* (&ran)[LIFECYCLE_STAGE_COUNT]) {
    PlayerLifeSpwnSystem spwn;
    PlayerLifeRoamIniSystem roam;
    PlayerLifeDespSystem desp;
    ase::ecs::System* const stages[LIFECYCLE_STAGE_COUNT] = {&spwn, &roam, &desp};
    for (uint32_t i = 0; i < LIFECYCLE_STAGE_COUNT; ++i) {
        ran[i] = stages[i]->name();
        stages[i]->tick(registry, 0.0f);
    }
}

void run_lifecycle(ase::ecs::Registry& registry) {
    const char* ran[LIFECYCLE_STAGE_COUNT] = {};
    run_lifecycle(registry, ran);
}

// Process spawn request synchronously by running the lifecycle chain
ase::ecs::Entity do_spawn_request(
    ase::ecs::Registry& registry,
    const char* player_id,
    float x, float z
) {
    auto request_entity = registry.create();
    auto& request = registry.emplace<PlayerReqSpwnComponent>(request_entity);
    std::strncpy(request.player_id, player_id, sizeof(request.player_id) - 1);
    request.player_id[sizeof(request.player_id) - 1] = '\0';
    request.x = x;
    request.z = z;

    run_lifecycle(registry);

    ase::ecs::Entity spawned_entity = ase::ecs::NullEntity;
    auto* result = registry.try_get<PlayerReqSpwnResComponent>(request_entity);
    if (result && result->success) {
        spawned_entity = result->spawned_entity;
    }

    registry.destroy(request_entity);
    return spawned_entity;
}

bool do_despawn_request(
    ase::ecs::Registry& registry,
    const char* player_id
) {
    auto request_entity = registry.create();
    auto& request = registry.emplace<PlayerReqDespComponent>(request_entity);
    std::strncpy(request.player_id, player_id, sizeof(request.player_id) - 1);
    request.player_id[sizeof(request.player_id) - 1] = '\0';

    run_lifecycle(registry);

    bool success = false;
    auto* result = registry.try_get<PlayerReqDespResComponent>(request_entity);
    if (result) {
        success = result->success;
    }

    registry.destroy(request_entity);
    return success;
}

ase::ecs::Entity do_find_player(ase::ecs::Registry& registry, const char* player_id) {
    auto view = registry.view<PlayerStaIdntComponent>();
    for (auto [entity, identity] : view.each()) {
        if (std::strcmp(identity.player_id, player_id) == 0) {
            return entity;
        }
    }
    return ase::ecs::NullEntity;
}

ase::containers::Vector<ase::containers::Pair<std::string, ase::ecs::Entity>> do_get_all_players(ase::ecs::Registry& registry) {
    ase::containers::Vector<ase::containers::Pair<std::string, ase::ecs::Entity>> result;
    auto view = registry.view<PlayerStaIdntComponent>();
    for (auto [entity, identity] : view.each()) {
        result.emplace_back(identity.player_id, entity);
    }
    return result;
}

// DELETED 2026-08-20: `setup_terrain_chunk(Registry&, int32_t, int32_t, float)` and its six
// call sites. It built terrain chunk entities and filled a TerrainChunkLayerResourceManager
// with a height - and nothing in the code under test ever read any of it.
//
// MEASURED, twice on the same day and from two directions:
//
//   THE PRODUCER DOES NOT READ IT. player_life_spwn_sys.cpp:334-337 takes the ground height
//   from the Hub key TRN_HGT_AT_POS and falls back to 0.0f when the key is absent. The
//   helper filled a ResourceManager the spawn path never touches, so the height path was
//   never covered by this file and could not be. That is why the spawn test below checks
//   `pos->y == 0.0f` and says so.
//
//   THE BUILD SAYS THE SAME THING. ase::terrain was removed from ase-player on 2026-08-18
//   under R15 (CMakeLists.txt:347 carries the entry), so `#include <ase/terrain/terrain.hpp>`
//   stopped resolving. The compile error was not a regression - it was the layer boundary
//   finally reporting scaffolding that had been unreachable all along. It only surfaced now
//   because the dead assert() around it used to hide the whole block from the compiler.
//
// Removing it changes nothing observable: every call passed a height that no assertion ever
// depended on. What it removes is a false suggestion - that this suite exercises the terrain
// seam. It does not, and now it no longer pretends to.
//
// THE HEIGHT PATH REMAINS UNTESTED, and that is the open item, not this helper. A real test
// would have to set the Hub key, which means recomputing the producer's pos_hash inside the
// test - the "logic duplicate" the house form of the sibling suites forbids by name. That is
// a test-design decision and is reported as such.

}  // anonymous namespace

// ============================================================================
// Constants - compile time
// ============================================================================
//
// Both blocks were runtime asserts. Every value on both sides is constexpr, so they are
// decidable while building: a changed constant now breaks the build at the line that names
// it, instead of at a run nobody may make.

static_assert(MOVEMENT_DEFAULT_WALK_SPEED > 0.0f);
static_assert(MOVEMENT_DEFAULT_RUN_SPEED > MOVEMENT_DEFAULT_WALK_SPEED);
static_assert(MOVEMENT_DEFAULT_GRAVITY > 0.0f);
static_assert(MOVEMENT_DEFAULT_JUMP_IMPULSE > 0.0f);

static_assert(PLAYER_STATE_IDLE == 0);
static_assert(PLAYER_STATE_WALKING == 1);
static_assert(PLAYER_STATE_RUNNING == 2);
static_assert(PLAYER_STATE_JUMPING == 3);
static_assert(PLAYER_STATE_FALLING == 4);

// ============================================================================
// Component Tests - runtime
// ============================================================================

TEST_CASE("player components hold the values written into them") {
    ase::ecs::Registry registry;
    auto entity = registry.create();

    // Add PlayerStaIdntComponent
    auto& identity = registry.emplace<PlayerStaIdntComponent>(entity);
    std::strncpy(identity.player_id, "test_player_1", sizeof(identity.player_id) - 1);
    identity.player_id[sizeof(identity.player_id) - 1] = '\0';
    identity.spawned_at_ms = 1000;
    identity.last_input_ms = identity.spawned_at_ms;
    CHECK(std::strcmp(identity.player_id, "test_player_1") == 0);

    // Add PlayerStaPosComponent (chunk-relativ seit S2b 2026-08-11: 10 m = Wabe 0, lokal 10)
    auto& pos = registry.emplace<PlayerStaPosComponent>(entity);
    pos.chunk_x = 0;
    pos.local_x = 10.0f;
    pos.y = 5.0f;
    pos.chunk_z = 0;
    pos.local_z = 20.0f;
    CHECK(pos.chunk_x == 0);
    CHECK(pos.local_x == 10.0f);
    CHECK(pos.y == 5.0f);
    CHECK(pos.local_z == 20.0f);

    // Der Blick wohnt seit S2b in der eigenen Komponente
    auto& yaw = registry.emplace<PlayerStaYawComponent>(entity);
    yaw.yaw = 1.57f;
    CHECK(yaw.yaw == 1.57f);

    // Add PlayerStaVelComponent
    auto& vel = registry.emplace<PlayerStaVelComponent>(entity);
    vel.vx = 1.0f;
    vel.vy = -9.8f;
    vel.vz = 0.5f;
    CHECK(vel.vx == 1.0f);
    CHECK(vel.vy == -9.8f);

    // Add PlayerStaPhysComponent
    auto& physics = registry.emplace<PlayerStaPhysComponent>(entity);
    physics.on_ground = false;
    physics.gravity_enabled = true;
    CHECK(!physics.on_ground);
    CHECK(physics.gravity_enabled);

    // Add PlayerStaStsComponent
    auto& state = registry.emplace<PlayerStaStsComponent>(entity);
    state.sts = PLAYER_STATE_RUNNING;
    CHECK(state.sts == PLAYER_STATE_RUNNING);

    // Add PlayerStaChkComponent
    auto& chunk = registry.emplace<PlayerStaChkComponent>(entity);
    chunk.chunk_x = 2;
    chunk.chunk_y = 3;
    CHECK(chunk.chunk_x == 2);
    CHECK(chunk.chunk_y == 3);
}

TEST_CASE("spawn via PlayerLifeSpwnSystem") {
    ase::ecs::Registry registry;

    // No terrain setup: the spawn path reads its ground height from the Hub, not from a
    // terrain chunk. The two setup_terrain_chunk calls that stood here were inert - see the
    // tombstone in the anonymous namespace. The second one also carried a stale chunk number
    // ("(100, 200) → chunk (6, 12)" assumes an edge of 16; MOVEMENT_DEFAULT_CHUNK_SIZE is 32).

    // No movement-settings singleton is needed any more: PlayerStMovComponent was a
    // per-entity copy of the MOVEMENT_DEFAULT_* constants and was removed 2026-08-15.
    // The systems read types.hpp directly.

    // Spawn player via lifecycle system
    auto entity = do_spawn_request(registry, "spawn_test", 100.0f, 200.0f);
    REQUIRE(entity != ase::ecs::NullEntity);

    // Find player via view query
    auto found = do_find_player(registry, "spawn_test");
    CHECK(found == entity);

    // Verify components
    auto* identity = registry.try_get<PlayerStaIdntComponent>(entity);
    REQUIRE(identity != nullptr);
    CHECK(std::strcmp(identity->player_id, "spawn_test") == 0);

    auto* pos = registry.try_get<PlayerStaPosComponent>(entity);
    REQUIRE(pos != nullptr);

    /**
     * THE POSITION IS CHUNK-RELATIVE, and the two lines that stood here were not.
     *
     * They read `pos→x` and `pos→z` - fields PlayerStaPosComponent lost in the S2b split of
     * 2026-08-11. Under NDEBUG the expression inside assert() is never compiled, so the dead
     * check hid a compile error for nine days. Measured 2026-08-20.
     *
     * The replacement states the invariant the producer actually establishes
     * (player_life_spwn_sys.cpp:364-367): the split is LOSSLESS, so chunk and local recombine
     * to the requested world coordinate. It uses the module's own constant rather than a chunk
     * number worked out by hand — the comment further up saying "(100, 200) → chunk (6, 12)"
     * assumed an edge of 16 and is itself stale; MOVEMENT_DEFAULT_CHUNK_SIZE is 32.
     */
    CHECK(static_cast<float>(pos->chunk_x) * MOVEMENT_DEFAULT_CHUNK_SIZE + pos->local_x
          == 100.0f);
    CHECK(static_cast<float>(pos->chunk_z) * MOVEMENT_DEFAULT_CHUNK_SIZE + pos->local_z
          == 200.0f);

    /**
     * THE HEIGHT IS 0, NOT 10 — and that is a finding, not a weakened check.
     *
     * The line here claimed `pos→y == 10.0f  // From terrain height`. The spawn system does
     * not read terrain: player_life_spwn_sys.cpp:334-337 takes the ground from the Hub key
     * TRN_HGT_AT_POS and falls back to 0.0f when the key is absent. This test sets no Hub
     * value, so 0.0f is what the producer writes.
     *
     * Asserting 10.0f would have been red the moment the checks were switched on. Setting the
     * Hub key here to make it green would mean recomputing the producer's pos_hash in the test
     * — a logic duplicate, which the house form of the sibling suites forbids by name. The
     * height path therefore stays UNCOVERED, and that is reported rather than papered over.
     */
    CHECK(pos->y == 0.0f);

    auto* physics = registry.try_get<PlayerStaPhysComponent>(entity);
    REQUIRE(physics != nullptr);
    CHECK(physics->on_ground == true);

    // Cannot spawn duplicate
    auto dup = do_spawn_request(registry, "spawn_test", 0.0f, 0.0f);
    CHECK(dup == ase::ecs::NullEntity);

    // Find non-existent
    auto notfound = do_find_player(registry, "nonexistent");
    CHECK(notfound == ase::ecs::NullEntity);
}

TEST_CASE("despawn via PlayerLifeSpwnSystem") {
    ase::ecs::Registry registry;

    // No terrain setup — inert, see the tombstone.

    // Spawn and despawn
    do_spawn_request(registry, "despawn_test", 0.0f, 0.0f);
    REQUIRE(do_find_player(registry, "despawn_test") != ase::ecs::NullEntity);

    bool result = do_despawn_request(registry, "despawn_test");
    CHECK(result == true);
    CHECK(do_find_player(registry, "despawn_test") == ase::ecs::NullEntity);

    // Cannot despawn again
    result = do_despawn_request(registry, "despawn_test");
    CHECK(result == false);
}

TEST_CASE("get_all_players via view") {
    ase::ecs::Registry registry;

    // No terrain setup — inert, see the tombstone. Note that two of the three calls that
    // stood here were identical (0,0,0.0f); even as scaffolding it had stopped meaning anything.

    // Spawn multiple players
    do_spawn_request(registry, "player_a", 0.0f, 0.0f);
    do_spawn_request(registry, "player_b", 10.0f, 10.0f);
    do_spawn_request(registry, "player_c", 20.0f, 20.0f);

    auto all = do_get_all_players(registry);
    CHECK(all.size() == 3);
}

TEST_CASE("lifecycle chain: run_lifecycle holds the family the module registers, in its order") {
    // Neither list is written down here: the module's run is sorted out of its registration with
    // the sorter App::startup runs, the driver's run is what run_lifecycle() ticks. The family is
    // every PlayerLife* system of the module's Dynamics run. A stage cut into the chain turns
    // THIS case red, instead of leaving the cases above to ask a system that no longer answers.
    ase::ecs::App module_app;
    module_app.add_module<PlayerModule>();
    const auto cycles =
        ase::ecs::internal::sort_systems_by_dependencies(module_app.system_registry());
    REQUIRE(cycles.empty());
    const auto& module_run = module_app.systems_for(ase::ecs::Schedule::Dynamics);

    constexpr uint32_t FAMILY_CAP = 16u;
    const char* family[FAMILY_CAP] = {};
    uint32_t family_n = 0u;
    for (size_t i = 0; i < module_run.size(); ++i) {
        if (module_run[i] == nullptr) continue;
        const char* name = module_run[i]->name();
        if (std::strncmp(name, "PlayerLife", 10) != 0) continue;
        REQUIRE(family_n < FAMILY_CAP);
        family[family_n++] = name;
    }

    ase::ecs::Registry registry;
    const char* ran[LIFECYCLE_STAGE_COUNT] = {};
    run_lifecycle(registry, ran);

    for (uint32_t i = 0; i < family_n; ++i) {
        bool driven = false;
        for (uint32_t k = 0; k < LIFECYCLE_STAGE_COUNT; ++k) {
            if (ran[k] != nullptr && std::strcmp(family[i], ran[k]) == 0) driven = true;
        }
        INFO("registered by the module, not driven by run_lifecycle: " << std::string(family[i]));
        CHECK(driven);
    }
    REQUIRE(family_n == LIFECYCLE_STAGE_COUNT);
    for (uint32_t i = 0; i < LIFECYCLE_STAGE_COUNT; ++i) {
        INFO("position " << i << ": the module runs " << std::string(family[i])
                         << ", run_lifecycle runs " << std::string(ran[i]));
        CHECK(std::strcmp(family[i], ran[i]) == 0);
    }
}

TEST_CASE("teleport lever moves the position, never the velocity, and the teleport authority sees it") {
    using namespace entt::literals;
    ase::ecs::Registry registry;
    auto entity = do_spawn_request(registry, "tele_test", 10.0f, 0.0f);
    REQUIRE(entity != ase::ecs::NullEntity);
    // The physics step integrates only players with a terrain bridge; the sync system that fills
    // it in the server is not under test here.
    registry.emplace_or_replace<PlayerInpTrnComponent>(entity);
    const uint32_t lever_owner = entt::hashed_string{"tele_test"}.value();
    const uint32_t owner = static_cast<uint32_t>(entity);

    PlayerSyncTlptSystem sync;
    PlayerSimTlptSystem tlpt;
    PlayerSimPhysSystem phys;
    PlayerAccMovSystem acc;
    acc.tick(registry, 0.0f);  // the detector's first tick only records the baseline

    // RED when: the jump goes through the velocity (the speed authority would fire instead) or the
    // carry across the cell edge is lost (40 m from x = 10 crosses the 32 m edge).
    hub::set(registry, lever_owner, "PLR_CHEAT_TELEPORT"_hs, 40.0f);
    sync.tick(registry, 0.0f);
    REQUIRE(registry.all_of<PlayerInpTlptComponent>(entity));
    CHECK(registry.get<PlayerInpTlptComponent>(entity).jump_m == doctest::Approx(40.0f));
    tlpt.tick(registry, 0.0f);
    phys.tick(registry, 0.0f);
    const auto& pos = registry.get<PlayerStaPosComponent>(entity);
    CHECK(static_cast<float>(pos.chunk_x) * MOVEMENT_DEFAULT_CHUNK_SIZE + pos.local_x ==
          doctest::Approx(50.0f));
    CHECK(pos.local_x >= 0.0f);
    CHECK(pos.local_x < MOVEMENT_DEFAULT_CHUNK_SIZE);
    const auto& vel = registry.get<PlayerStaVelComponent>(entity);
    CHECK(vel.vx == doctest::Approx(0.0f));
    CHECK(vel.vz == doctest::Approx(0.0f));
    acc.tick(registry, 0.0f);
    CHECK(hub::get(registry, owner, "PLAYER_MOVEMENT_SUSPICIOUS"_hs) == doctest::Approx(1.0f));

    // A cleared lever stops the jumps; the next detector pass sees a still player and drops the flag.
    hub::set(registry, lever_owner, "PLR_CHEAT_TELEPORT"_hs, 0.0f);
    sync.tick(registry, 0.0f);
    tlpt.tick(registry, 0.0f);
    phys.tick(registry, 0.0f);
    CHECK(static_cast<float>(pos.chunk_x) * MOVEMENT_DEFAULT_CHUNK_SIZE + pos.local_x ==
          doctest::Approx(50.0f));
    acc.tick(registry, 0.0f);
    CHECK(hub::get(registry, owner, "PLAYER_MOVEMENT_SUSPICIOUS"_hs) == doctest::Approx(0.0f));
}

TEST_CASE("session start is a pulse: up for one pass, down in the next, never announced twice") {
    using namespace entt::literals;
    ase::ecs::Registry registry;
    auto entity = do_spawn_request(registry, "sess_test", 0.0f, 0.0f);
    REQUIRE(entity != ase::ecs::NullEntity);
    const uint32_t owner = static_cast<uint32_t>(entity);
    PlayerHubSessStrtSystem sess;

    sess.tick(registry, 0.0f);
    CHECK(hub::get(registry, owner, "PLAYER_SESSION_START"_hs) == doctest::Approx(1.0f));
    CHECK(registry.all_of<PlayerSessStrtTag>(entity));
    CHECK_FALSE(registry.all_of<PlayerSessDoneTag>(entity));

    // RED when: the flag is held — the Replica would re-fire CoordinationDetector every heartbeat.
    sess.tick(registry, 0.0f);
    CHECK(hub::get(registry, owner, "PLAYER_SESSION_START"_hs) == doctest::Approx(0.0f));
    CHECK(registry.all_of<PlayerSessDoneTag>(entity));

    // RED when: a finished player is announced again.
    sess.tick(registry, 0.0f);
    CHECK(hub::get(registry, owner, "PLAYER_SESSION_START"_hs) == doctest::Approx(0.0f));
}
