// The doctest entry point is a compile definition of this target, set in tests/CMakeLists.txt —
// it reaches the translation unit before every #include, so doctest still generates exactly one
// main() here. A #define in the source would be a macro definition in C++, which is forbidden.
#include <doctest/doctest.h>

/**
 * ASE Player Module Tests (ECS)
 *
 * @usecase     usecase:101557 - receive a spawn request
 *
 * doctest CHECK/REQUIRE only - never assert(). All five compile_commands.json of the servers
 * carry -O3 -DNDEBUG, and under NDEBUG assert(expr) expands to ((void)0): the expression is never
 * compiled, so an assert may name fields that no longer exist and the build stays green.
 *
 * WHAT IS DRIVEN HERE: the REAL systems of the module. The lifecycle chain answers spawn and
 * despawn requests, the teleport lever moves a figure, the session start pulses once. Every
 * system that splits world metres into chunk and local reads the chunk edge from the manager
 * row; the test lays TER_CONST_CHUNK_SIZE on the hub and lets PlayerChnkCnstSynSystem carry it
 * there, the way the server does in Ingestion.
 *
 * THE GROUND IS NOT COMPUTED HERE. ase-player holds no gravity and no ground (CAUSA_ASE_PHYSICS,
 * DoD 07-K): the spawn lays the figure at y = 0, PlayerSimWalkSystem files the step, ase-physics
 * answers the place on HUB_PHYS_STAND, and PlayerSimStndSystem takes that answer back. This suite
 * stands in for ase-physics with answer_as_physics(): it lays exactly the PlayerInpStndComponent
 * that PlayerHubStndSyncSystem lays from the row, for a body that stands where it was filed. The
 * fall into a dug hole is covered by the host test of ase-physics.
 *
 * A CHECK THAT TURNS RED ON A REAL RUN IS A FINDING, NOT A REGRESSION. Do not weaken the check.
 */

#include <ase/player/player.hpp>
// The lifecycle chain: birth, errand, death - three systems in Dynamics.
#include <ase/player/systems/lifecycle/player_life_spwn_sys.hpp>
#include <ase/player/systems/lifecycle/player_life_roam_ini_sys.hpp>
#include <ase/player/systems/lifecycle/player_life_desp_sys.hpp>
// The teleport lever and the session-start pulse (register D0703)
#include <ase/player/systems/sync/player_sync_tlpt_sys.hpp>
#include <ase/player/systems/simulation/player_sim_tlpt_sys.hpp>
// The step filed to ase-physics and its answer taken back (07.13)
#include <ase/player/systems/simulation/player_sim_walk_sys.hpp>
#include <ase/player/systems/simulation/player_sim_stnd_sys.hpp>
#include <ase/player/components/input/player_inp_stnd_comp.hpp>
#include <ase/player/components/state/player_sta_frm_comp.hpp>
#include <ase/player/components/state/player_sta_plcm_comp.hpp>
#include <ase/player/components/buffer/player_buf_walk_comp.hpp>
#include <ase/player/components/tag/player_stnd_grnd_tag.hpp>
#include <ase/player/components/tag/player_walk_rplc_tag.hpp>
#include <ase/player/systems/anticheat/player_acc_mov_sys.hpp>
#include <ase/player/systems/hub/player_hub_sess_strt_sys.hpp>
#include <ase/player/systems/state/player_sta_gait_sys.hpp>
#include <ase/player/components/tag/player_gait_stnd_tag.hpp>
#include <ase/player/components/tag/player_gait_walk_tag.hpp>
#include <ase/player/components/tag/player_gait_run_tag.hpp>
// The motion row of the star (07.8): course, horizontal velocity and gait mark per figure
#include <ase/player/systems/hub/player_hub_motn_sys.hpp>
#include <ase/player/components/tag/player_spnd_tag.hpp>
// The loaded area of a figure (07.7): the disc of its plate, and the rim that keeps it inside
#include <ase/player/systems/sync/player_area_syn_sys.hpp>
#include <ase/player/systems/simulation/player_sim_bndy_sys.hpp>
#include <ase/player/components/input/player_inp_area_comp.hpp>
// The chunk edge of the terrain protocol, laid on the manager row by its sync system
#include <ase/player/systems/sync/player_chnk_cnst_syn_sys.hpp>
#include <ase/player/components/input/player_inp_chnk_cnst_comp.hpp>
#include <ase/player/components/tag/player_mgr_tag.hpp>
#include <ase/player/components/input/player_inp_tlpt_comp.hpp>
#include <ase/player/components/tag/player_sess_strt_tag.hpp>
#include <ase/player/components/tag/player_sess_done_tag.hpp>
// The turn on the spot (fig.turn): the host row laid out, then turned out at the turn rate
#include <ase/player/systems/hub/player_hub_turn_sys.hpp>
#include <ase/player/systems/control/player_ctrl_turn_sys.hpp>
#include <ase/player/components/input/player_inp_turn_comp.hpp>
#include <ase/player/components/input/player_inp_ext_comp.hpp>
#include <ase/player/components/request/player_req_turn_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/tag/player_drty_tag.hpp>
// Water under the figure (usecase:101588, CAUSA_ASE_HYDRO item 6, B0358)
#include <ase/player/systems/sync/player_hub_watr_sync_sys.hpp>
#include <ase/player/systems/state/player_sta_sts_sys.hpp>
#include <ase/player/components/input/player_inp_watr_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/tag/player_stnd_watr_tag.hpp>
#include <ase/math/math.hpp>
#include <ase/hub/api.hpp>
#include <entt/core/hashed_string.hpp>
#include <ase/ecs/system.hpp>
#include <ase/ecs/app.hpp>
#include <ase/ecs/internal/dependency_sorter.hpp>
#include <ase/containers/vector.hpp>
#include <ase/containers/hash_map.hpp>
#include <cstring>
// A NaN yaw of the host (std::nanf): the turn row that carries it must turn nothing
#include <cmath>
// Names in INFO lines as text: doctest prints a raw `const char*` as an ADDRESS unless
// DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING is set, and the message would name no system.
#include <string>

using namespace ase;
using namespace ase::player;

namespace {

constexpr uint32_t LIFECYCLE_STAGE_COUNT = 3u;

/**
 * The chunk edge the terrain protocol sets (TER_CONST_CHUNK_SIZE in hub_constants.json). The test
 * lays it on the hub; every recombination of chunk and local below multiplies with this value.
 */
constexpr float TEST_CHUNK_EDGE_M = 32.0f;

/**
 * Lays the chunk edge on the hub and lets the REAL PlayerChnkCnstSynSystem fill the input row of
 * the manager. Without it every player system that splits world metres finds no edge and leaves
 * the pass, and the spawn request stays unanswered.
 */
void lay_chunk_edge(ase::ecs::Registry& registry) {
    using namespace entt::literals;
    bool has_mgr = false;
    for (auto mgr : registry.view<PlayerMgrTag>()) {
        (void)mgr;
        has_mgr = true;
    }
    if (!has_mgr) {
        registry.emplace<PlayerMgrTag>(registry.create());
    }
    hub::set(registry, hub::GLOBAL, "TER_CONST_CHUNK_SIZE"_hs, TEST_CHUNK_EDGE_M);
    PlayerChnkCnstSynSystem edge_sync;
    edge_sync.tick(registry, 0.0f);
}

/**
 * The lifecycle chain as player_module.hpp registers it in Dynamics: PlayerLifeSpwnSystem answers
 * spawn requests, PlayerLifeRoamIniSystem sends a newborn walker on its errand, and
 * PlayerLifeDespSystem answers despawn requests. A helper that ticked only the first one would
 * leave every despawn request unanswered.
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

    lay_chunk_edge(registry);
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

/**
 * Stands in for ase-physics and PlayerHubStndSyncSystem: the body stands exactly where the figure
 * filed it in its last PlayerBufWalkComponent, on its ground, after the placement it follows from.
 * `vel_y` and `grounded` let a case answer a falling body. No view: one named figure.
 */
void answer_as_physics(ase::ecs::Registry& registry, ase::ecs::Entity figure, float vel_y, bool grounded) {
    const auto& walk = registry.get<PlayerBufWalkComponent>(figure);
    const auto& placement = registry.get<PlayerStaPlcmComponent>(figure);
    auto& stand = registry.get_or_emplace<PlayerInpStndComponent>(figure);
    stand.x = walk.x;
    stand.y = walk.y;
    stand.z = walk.z;
    stand.vel_y = vel_y;
    stand.seq = placement.seq;
    if (grounded) {
        registry.emplace_or_replace<PlayerStndGrndTag>(figure);
        return;
    }
    registry.remove<PlayerStndGrndTag>(figure);
}

}  // anonymous namespace

/**
 * CONSTANTS, DECIDED WHILE BUILDING
 *
 * Every value on both sides is constexpr, so a changed constant breaks the build at the line that
 * names it, instead of at a run nobody may make.
 */

static_assert(MOVEMENT_DEFAULT_WALK_SPEED > 0.0f);
static_assert(MOVEMENT_DEFAULT_RUN_SPEED > MOVEMENT_DEFAULT_WALK_SPEED);
static_assert(MOVEMENT_DEFAULT_JUMP_IMPULSE > 0.0f);

static_assert(PLAYER_STATE_GROUND == 0);
static_assert(PLAYER_STATE_JUMPING == 3);
static_assert(PLAYER_STATE_FALLING == 4);

/**
 * COMPONENT TESTS, AT RUNTIME
 */

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

    // The position is chunk-relative: 10 m lies in cell 0 at local 10
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

    // The view direction lives in its own component
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
    CHECK(!physics.on_ground);

    // Add PlayerStaStsComponent
    auto& state = registry.emplace<PlayerStaStsComponent>(entity);
    state.sts = PLAYER_STATE_FALLING;
    CHECK(state.sts == PLAYER_STATE_FALLING);

    // Add PlayerStaChkComponent
    auto& chunk = registry.emplace<PlayerStaChkComponent>(entity);
    chunk.chunk_x = 2;
    chunk.chunk_y = 3;
    CHECK(chunk.chunk_x == 2);
    CHECK(chunk.chunk_y == 3);
}

TEST_CASE("spawn via PlayerLifeSpwnSystem") {
    ase::ecs::Registry registry;

    // No terrain setup: the spawn path reads its ground height from the hub, not from a terrain
    // chunk. The movement values come from types.hpp, the chunk edge from the manager row.

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
     * THE POSITION IS CHUNK-RELATIVE, AND THE SPLIT IS LOSSLESS.
     *
     * The producer splits the requested world coordinate into chunk and local with the edge it
     * reads from the manager row, so chunk and local recombine to the request. The check uses the
     * edge the test laid on the hub rather than a chunk number worked out by hand.
     */
    CHECK(static_cast<float>(pos->chunk_x) * TEST_CHUNK_EDGE_M + pos->local_x
          == 100.0f);
    CHECK(static_cast<float>(pos->chunk_z) * TEST_CHUNK_EDGE_M + pos->local_z
          == 200.0f);

    /**
     * THE HEIGHT IS 0, AND THAT IS A FINDING, NOT A WEAKENED CHECK.
     *
     * The spawn system reads no ground: it lays the figure at y = 0, and the height arrives with
     * the first stand answer of ase-physics (see the head of this file and the stand case below).
     */
    CHECK(pos->y == 0.0f);

    auto* physics = registry.try_get<PlayerStaPhysComponent>(entity);
    REQUIRE(physics != nullptr);
    CHECK(physics->on_ground == true);

    // A second spawn under the same id is refused
    auto again = do_spawn_request(registry, "spawn_test", 0.0f, 0.0f);
    CHECK(again == ase::ecs::NullEntity);

    // Find non-existent
    auto notfound = do_find_player(registry, "nonexistent");
    CHECK(notfound == ase::ecs::NullEntity);
}

TEST_CASE("despawn via PlayerLifeSpwnSystem") {
    ase::ecs::Registry registry;

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
    const uint32_t lever_owner = entt::hashed_string{"tele_test"}.value();
    const uint32_t owner = static_cast<uint32_t>(entity);

    PlayerSyncTlptSystem sync;
    PlayerSimTlptSystem tlpt;
    PlayerSimWalkSystem walk;
    PlayerSimStndSystem stnd;
    PlayerAccMovSystem acc;

    // The born figure is placed once: PlayerSimWalkSystem lays its frame, ase-physics answers.
    walk.tick(registry, 0.0f);
    answer_as_physics(registry, entity, 0.0f, true);
    stnd.tick(registry, 0.0f);
    const uint32_t first_placement = registry.get<PlayerStaPlcmComponent>(entity).seq;
    acc.tick(registry, 0.0f);  // the detector's first tick only records the baseline

    // RED when: the jump goes through the velocity (the speed authority would fire instead) or the
    // carry across the cell edge is lost (40 m from x = 10 crosses the 32 m edge).
    hub::set(registry, lever_owner, "PLR_CHEAT_TELEPORT"_hs, 40.0f);
    sync.tick(registry, 0.0f);
    REQUIRE(registry.all_of<PlayerInpTlptComponent>(entity));
    CHECK(registry.get<PlayerInpTlptComponent>(entity).jump_m == doctest::Approx(40.0f));
    tlpt.tick(registry, 0.0f);

    // RED when: the jump is not a new placement - the next answer of ase-physics would walk the
    // figure back to where it stood.
    CHECK(registry.all_of<PlayerWalkRplcTag>(entity));
    walk.tick(registry, 0.0f);
    CHECK_FALSE(registry.all_of<PlayerWalkRplcTag>(entity));
    CHECK(registry.get<PlayerStaPlcmComponent>(entity).seq == first_placement + 1u);
    answer_as_physics(registry, entity, 0.0f, true);
    stnd.tick(registry, 0.0f);

    // The carry over the chunk edge is done by PlayerSimStndSystem when it takes the answer back.
    const auto& pos = registry.get<PlayerStaPosComponent>(entity);
    CHECK(static_cast<float>(pos.chunk_x) * TEST_CHUNK_EDGE_M + pos.local_x ==
          doctest::Approx(50.0f));
    CHECK(pos.local_x >= 0.0f);
    CHECK(pos.local_x < TEST_CHUNK_EDGE_M);
    const auto& vel = registry.get<PlayerStaVelComponent>(entity);
    CHECK(vel.vx == doctest::Approx(0.0f));
    CHECK(vel.vz == doctest::Approx(0.0f));
    acc.tick(registry, 0.0f);
    CHECK(hub::get(registry, owner, "PLAYER_MOVEMENT_SUSPICIOUS"_hs) == doctest::Approx(1.0f));

    // A cleared lever stops the jumps; the next detector pass sees a still player and drops the flag.
    hub::set(registry, lever_owner, "PLR_CHEAT_TELEPORT"_hs, 0.0f);
    sync.tick(registry, 0.0f);
    tlpt.tick(registry, 0.0f);
    walk.tick(registry, 0.0f);
    answer_as_physics(registry, entity, 0.0f, true);
    stnd.tick(registry, 0.0f);
    CHECK(static_cast<float>(pos.chunk_x) * TEST_CHUNK_EDGE_M + pos.local_x ==
          doctest::Approx(50.0f));
    acc.tick(registry, 0.0f);
    CHECK(hub::get(registry, owner, "PLAYER_MOVEMENT_SUSPICIOUS"_hs) == doctest::Approx(0.0f));
}

TEST_CASE("PlayerSimStndSystem takes the stand answer of ase-physics back, and only the current one") {
    ase::ecs::Registry registry;
    auto entity = do_spawn_request(registry, "stand_test", 20.0f, 5.0f);
    REQUIRE(entity != ase::ecs::NullEntity);
    PlayerSimWalkSystem walk;
    PlayerSimStndSystem stnd;

    // A born figure has no frame: PASS A of PlayerSimWalkSystem places it on its chunk corner.
    CHECK_FALSE(registry.all_of<PlayerStaFrmComponent>(entity));
    walk.tick(registry, 0.0f);
    REQUIRE(registry.all_of<PlayerStaFrmComponent, PlayerStaPlcmComponent, PlayerBufWalkComponent>(entity));
    CHECK(registry.get<PlayerStaPlcmComponent>(entity).seq == 1u);
    CHECK(registry.get<PlayerBufWalkComponent>(entity).x == doctest::Approx(20.0f));

    // RED when: the height or the fall of the body is not taken back. A body 1.5 m above its frame
    // level, falling, off its ground.
    auto& stand = registry.get_or_emplace<PlayerInpStndComponent>(entity);
    stand.x = 20.0f;
    stand.y = 1.5f;
    stand.z = 5.0f;
    stand.vel_y = -3.0f;
    stand.seq = registry.get<PlayerStaPlcmComponent>(entity).seq;
    stnd.tick(registry, 0.0f);
    const auto& pos = registry.get<PlayerStaPosComponent>(entity);
    CHECK(pos.y == doctest::Approx(1.5f));
    CHECK(registry.get<PlayerStaVelComponent>(entity).vy == doctest::Approx(-3.0f));
    CHECK_FALSE(registry.get<PlayerStaPhysComponent>(entity).on_ground);

    // RED when: the ground contact is computed instead of taken: only the mark of the answer sets it.
    answer_as_physics(registry, entity, 0.0f, true);
    stnd.tick(registry, 0.0f);
    CHECK(registry.get<PlayerStaPhysComponent>(entity).on_ground);

    // RED when: an answer to an older placement moves the figure. The stand follows placement 1, the
    // figure was put somewhere else since.
    registry.get<PlayerStaPlcmComponent>(entity).seq += 1u;
    registry.get<PlayerInpStndComponent>(entity).x = 90.0f;
    stnd.tick(registry, 0.0f);
    CHECK(static_cast<float>(pos.chunk_x) * TEST_CHUNK_EDGE_M + pos.local_x == doctest::Approx(20.0f));

    // RED when: the answer overwrites a figure that was put somewhere in this frame. The tag stands
    // until PlayerSimWalkSystem lays the frame again.
    registry.get<PlayerInpStndComponent>(entity).seq = registry.get<PlayerStaPlcmComponent>(entity).seq;
    registry.emplace_or_replace<PlayerWalkRplcTag>(entity);
    stnd.tick(registry, 0.0f);
    CHECK(static_cast<float>(pos.chunk_x) * TEST_CHUNK_EDGE_M + pos.local_x == doctest::Approx(20.0f));

    // The same answer once the tag is gone: 90 m from the frame corner of chunk 0 is chunk 2, 26 m.
    registry.remove<PlayerWalkRplcTag>(entity);
    stnd.tick(registry, 0.0f);
    CHECK(pos.chunk_x == 2);
    CHECK(pos.local_x == doctest::Approx(26.0f));
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

TEST_CASE("the gait follows the horizontal speed: one tag per figure, moved when the band changes") {
    ase::ecs::Registry registry;
    auto entity = registry.create();
    registry.emplace<PlayerStaIdntComponent>(entity);
    auto& vel = registry.emplace<PlayerStaVelComponent>(entity);
    PlayerStaGaitSystem gait;

    // RED when: a resting figure gets no gait tag, or two.
    vel.vx = 0.0f;
    vel.vz = 0.0f;
    gait.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerGaitStndTag>(entity));
    CHECK_FALSE(registry.any_of<PlayerGaitWalkTag, PlayerGaitRunTag>(entity));

    // |v| = 3 m/s lies in the walking band.
    vel.vx = 3.0f;
    gait.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerGaitWalkTag>(entity));
    CHECK_FALSE(registry.any_of<PlayerGaitStndTag, PlayerGaitRunTag>(entity));

    // RED when: the vertical speed counts - a fall at walk pace stays a walk.
    vel.vy = -20.0f;
    gait.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerGaitWalkTag>(entity));

    // |(5, 5)| = 7.07 m/s lies above the walk bound (midway between the gears).
    vel.vx = 5.0f;
    vel.vz = 5.0f;
    gait.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerGaitRunTag>(entity));
    CHECK_FALSE(registry.any_of<PlayerGaitStndTag, PlayerGaitWalkTag>(entity));

    // RED when: a bound belongs to two bands - the figure would flip tags tick after tick.
    vel.vx = PLAYER_GAIT_WALK_SPEED_MAX;
    vel.vz = 0.0f;
    gait.tick(registry, 0.0f);
    gait.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerGaitWalkTag>(entity));
    CHECK_FALSE(registry.any_of<PlayerGaitStndTag, PlayerGaitRunTag>(entity));

    vel.vx = PLAYER_GAIT_STND_SPEED_MAX;
    gait.tick(registry, 0.0f);
    gait.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerGaitWalkTag>(entity));
    CHECK_FALSE(registry.any_of<PlayerGaitStndTag, PlayerGaitRunTag>(entity));

    // Back to rest: the walk tag goes, the stand tag comes.
    vel.vx = 0.0f;
    gait.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerGaitStndTag>(entity));
    CHECK_FALSE(registry.any_of<PlayerGaitWalkTag, PlayerGaitRunTag>(entity));
}

TEST_CASE("the motion row states course, horizontal velocity and exactly one gait mark of a figure") {
    using namespace entt::literals;
    ase::ecs::Registry registry;
    auto figure = registry.create();
    registry.emplace<PlayerSpndTag>(figure);
    registry.emplace<PlayerStaIdntComponent>(figure);
    auto& yaw = registry.emplace<PlayerStaYawComponent>(figure);
    auto& vel = registry.emplace<PlayerStaVelComponent>(figure);
    const uint32_t owner = static_cast<uint32_t>(figure);
    PlayerStaGaitSystem gait;
    PlayerHubMotnSystem motion;

    // A figure that stands: the row exists, the fields carry the state, the stand mark is up.
    yaw.yaw = 1.25f;
    gait.tick(registry, 0.0f);
    motion.tick(registry, 0.0f);
    CHECK(hub::row_has(registry, figure, "HUB_PLR_MOTN"_hs));
    CHECK(hub::get_f64(registry, owner, "HUB_PLR_MOTN_YAW"_hs, -1.0) == doctest::Approx(1.25));
    CHECK(hub::row_has(registry, figure, "HUB_PLR_GAIT_STND"_hs));
    CHECK_FALSE(hub::row_has(registry, figure, "HUB_PLR_GAIT_WALK"_hs));
    CHECK_FALSE(hub::row_has(registry, figure, "HUB_PLR_GAIT_RUN"_hs));

    // RED when: the axes are not exchanged - the star's horizontal pair is (vx, vz), vy is height.
    vel.vx = 2.0f;
    vel.vy = -9.0f;
    vel.vz = 1.5f;
    gait.tick(registry, 0.0f);
    motion.tick(registry, 0.0f);
    CHECK(hub::get_f64(registry, owner, "HUB_PLR_MOTN_VEL_X"_hs, 0.0) == doctest::Approx(2.0));
    CHECK(hub::get_f64(registry, owner, "HUB_PLR_MOTN_VEL_Y"_hs, 0.0) == doctest::Approx(1.5));

    // RED when: the previous gait mark stays - the host would see two gaits on one figure.
    CHECK(hub::row_has(registry, figure, "HUB_PLR_GAIT_WALK"_hs));
    CHECK_FALSE(hub::row_has(registry, figure, "HUB_PLR_GAIT_STND"_hs));
    CHECK_FALSE(hub::row_has(registry, figure, "HUB_PLR_GAIT_RUN"_hs));

    vel.vx = 5.0f;
    vel.vz = 5.0f;
    gait.tick(registry, 0.0f);
    motion.tick(registry, 0.0f);
    CHECK(hub::row_has(registry, figure, "HUB_PLR_GAIT_RUN"_hs));
    CHECK_FALSE(hub::row_has(registry, figure, "HUB_PLR_GAIT_WALK"_hs));

    // RED when: an entity without PlayerSpndTag reaches the star - it is no figure of the roster.
    auto stranger = registry.create();
    registry.emplace<PlayerStaYawComponent>(stranger);
    registry.emplace<PlayerStaVelComponent>(stranger);
    motion.tick(registry, 0.0f);
    CHECK_FALSE(hub::row_has(registry, stranger, "HUB_PLR_MOTN"_hs));
}

TEST_CASE("the area of a figure is the measured disc of its plate, and an unmeasured disc is no area") {
    using namespace entt::literals;
    ase::ecs::Registry registry;
    auto figure = registry.create();
    registry.emplace<PlayerSpndTag>(figure);
    const uint32_t figure_id = static_cast<uint32_t>(figure);
    auto plate = registry.create();
    const uint32_t plate_id = static_cast<uint32_t>(plate);
    PlayerAreaSynSystem area_sync;

    // The levelled disc ase-terrain wrote under the plate.
    hub::set(registry, plate_id, "TRN_FLAT_HGT"_hs, 1.5f);
    hub::set(registry, plate_id, "TRN_FLAT_RAD"_hs, 12.0f);
    hub::set(registry, plate_id, "TRN_FLAT_X"_hs, -4.0f);
    hub::set(registry, plate_id, "TRN_FLAT_Z"_hs, 7.0f);

    // RED when: a figure that nobody bound to a plate walks bounded.
    area_sync.tick(registry, 0.0f);
    CHECK_FALSE(registry.all_of<PlayerInpAreaComponent>(figure));

    // The binding as ase-pl-avatar files it from the host port avatar.area.v1: the plate field first,
    // the kind last.
    hub::set_u32(registry, figure_id, "HUB_PLR_AREA_PLATE"_hs, plate_id);
    hub::row_tag(registry, figure, "HUB_PLR_AREA"_hs);
    area_sync.tick(registry, 0.0f);
    REQUIRE(registry.all_of<PlayerInpAreaComponent>(figure));
    const auto& bound = registry.get<PlayerInpAreaComponent>(figure);
    CHECK(bound.level == doctest::Approx(1.5f));
    CHECK(bound.radius_m == doctest::Approx(12.0f));
    CHECK(bound.x == doctest::Approx(-4.0f));
    CHECK(bound.z == doctest::Approx(7.0f));

    // RED when: the area keeps last pass's edge after the disc lost its measurement.
    hub::remove(registry, plate_id, "TRN_FLAT_RAD"_hs);
    area_sync.tick(registry, 0.0f);
    CHECK_FALSE(registry.all_of<PlayerInpAreaComponent>(figure));

    // RED when: a radius of zero passes as an area - the figure would be held on the centre point.
    hub::set(registry, plate_id, "TRN_FLAT_RAD"_hs, 0.0f);
    area_sync.tick(registry, 0.0f);
    CHECK_FALSE(registry.all_of<PlayerInpAreaComponent>(figure));

    // RED when: an entity without PlayerSpndTag gets an area - it is no figure of the roster.
    auto stranger = registry.create();
    hub::set_u32(registry, static_cast<uint32_t>(stranger), "HUB_PLR_AREA_PLATE"_hs, plate_id);
    hub::row_tag(registry, stranger, "HUB_PLR_AREA"_hs);
    hub::set(registry, plate_id, "TRN_FLAT_RAD"_hs, 12.0f);
    area_sync.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerInpAreaComponent>(figure));
    CHECK_FALSE(registry.all_of<PlayerInpAreaComponent>(stranger));
}

TEST_CASE("the rim sets a figure back onto the circle of its area and takes only the outward velocity") {
    ase::ecs::Registry registry;
    lay_chunk_edge(registry);
    auto figure = registry.create();
    auto& area = registry.emplace<PlayerInpAreaComponent>(figure);
    auto& pos = registry.emplace<PlayerStaPosComponent>(figure);
    auto& vel = registry.emplace<PlayerStaVelComponent>(figure);
    registry.emplace<PlayerStaPhysComponent>(figure);
    area.radius_m = 10.0f;
    area.x = 5.0f;
    area.z = 5.0f;
    const float limit = area.radius_m - PLR_AREA_EDGE_M;
    PlayerSimBndySystem rim;

    // Inside the circle the figure walks free: place and velocity stay as they are.
    pos.chunk_x = 0;
    pos.local_x = 6.0f;
    pos.chunk_z = 0;
    pos.local_z = 5.0f;
    vel.vx = 3.0f;
    vel.vz = 1.0f;
    rim.tick(registry, 0.0f);
    CHECK(pos.local_x == doctest::Approx(6.0f));
    CHECK(vel.vx == doctest::Approx(3.0f));

    // World place (40, 5) lies 35 m east of the centre: set back onto the circle, carried back into the
    // chunk that holds it. RED when the chunk address keeps the old cell or the local metres leave [0, edge).
    pos.chunk_x = 1;
    pos.local_x = 8.0f;
    rim.tick(registry, 0.0f);
    CHECK(pos.chunk_x == 0);
    CHECK(pos.local_x == doctest::Approx(5.0f + limit));
    CHECK(pos.chunk_z == 0);
    CHECK(pos.local_z == doctest::Approx(5.0f));
    // RED when: the whole velocity goes - the figure would stick to the rim instead of sliding along it.
    CHECK(vel.vx == doctest::Approx(0.0f));
    CHECK(vel.vz == doctest::Approx(1.0f));

    // RED when: an inward velocity is taken - the figure could not walk back into its area.
    pos.chunk_x = 1;
    pos.local_x = 8.0f;
    vel.vx = -2.0f;
    rim.tick(registry, 0.0f);
    CHECK(vel.vx == doctest::Approx(-2.0f));

    // West of a centre below zero: the bounded place lies in chunk -1. RED when the cell is truncated
    // towards zero instead of floored.
    area.x = -3.0f;
    area.z = 0.0f;
    pos.chunk_x = -1;
    pos.local_x = 2.0f;
    pos.chunk_z = 0;
    pos.local_z = 0.0f;
    rim.tick(registry, 0.0f);
    CHECK(pos.chunk_x == -1);
    CHECK(pos.local_x == doctest::Approx(TEST_CHUNK_EDGE_M - 3.0f - limit));
    CHECK(pos.local_x >= 0.0f);
    CHECK(pos.local_x < TEST_CHUNK_EDGE_M);

    // RED when: a figure without an area is bounded - the radius is a value of the area, never of the module.
    auto free_walker = registry.create();
    auto& free_pos = registry.emplace<PlayerStaPosComponent>(free_walker);
    registry.emplace<PlayerStaVelComponent>(free_walker);
    free_pos.chunk_x = 9;
    free_pos.local_x = 1.0f;
    rim.tick(registry, 0.0f);
    CHECK(free_pos.chunk_x == 9);
    CHECK(free_pos.local_x == doctest::Approx(1.0f));
}

TEST_CASE("the area bounds the place only: height, fall and ground contact stay as ase-physics left them") {
    // The floor of the area is the plane ase-physics stands the walking body on (PhysicsGrndCalcSystem,
    // PhysicsCntcSystem, tested in modules/ase-physics/tests/test_physics_ground.cpp); the rim of the area only
    // bounds the horizontal place. RED when the rim lifts a figure onto the level, stops its fall or sets its
    // ground contact - a second floor that disagrees with the one the host draws.
    ase::ecs::Registry registry;
    lay_chunk_edge(registry);
    auto figure = registry.create();
    auto& area = registry.emplace<PlayerInpAreaComponent>(figure);
    auto& pos = registry.emplace<PlayerStaPosComponent>(figure);
    auto& vel = registry.emplace<PlayerStaVelComponent>(figure);
    auto& physics = registry.emplace<PlayerStaPhysComponent>(figure);
    area.level = 12.0f;
    area.radius_m = 10.0f;
    area.x = 5.0f;
    area.z = 5.0f;
    pos.local_x = 5.0f;
    pos.local_z = 5.0f;
    PlayerSimBndySystem rim;

    // Below the level, falling: the rim leaves the height and the fall to the physics.
    pos.y = 11.0f;
    vel.vy = -4.0f;
    physics.on_ground = false;
    rim.tick(registry, 0.0f);
    CHECK(pos.y == doctest::Approx(11.0f));
    CHECK(vel.vy == doctest::Approx(-4.0f));
    CHECK_FALSE(physics.on_ground);

    // Rising from below: the rise stays.
    pos.y = 11.5f;
    vel.vy = 3.0f;
    rim.tick(registry, 0.0f);
    CHECK(pos.y == doctest::Approx(11.5f));
    CHECK(vel.vy == doctest::Approx(3.0f));

    // Standing above the level with ground contact: the contact stays.
    pos.y = 14.0f;
    vel.vy = 0.0f;
    physics.on_ground = true;
    rim.tick(registry, 0.0f);
    CHECK(pos.y == doctest::Approx(14.0f));
    CHECK(vel.vy == doctest::Approx(0.0f));
    CHECK(physics.on_ground);

    // Inside the circle the horizontal place stays too.
    CHECK(pos.local_x == doctest::Approx(5.0f));
    CHECK(pos.local_z == doctest::Approx(5.0f));
}

TEST_CASE("a turn row of the host becomes one turn input per row and is released whatever its outcome") {
    using namespace entt::literals;
    ecs::Registry registry;
    PlayerHubTurnSystem hub_turn;

    const ecs::Entity figure = registry.create();
    registry.emplace<PlayerStaYawComponent>(figure);
    const uint32_t figure_id = static_cast<uint32_t>(figure);

    auto file_turn = [&registry](uint32_t named, float yaw) {
        const ecs::Entity row = hub::row_create(registry, "HUB_PLR_TURN_REQ"_hs);
        hub::set_u32(registry, static_cast<uint32_t>(row), "HUB_PLR_TURN_FIGURE"_hs, named);
        hub::set_f32(registry, static_cast<uint32_t>(row), "HUB_PLR_TURN_YAW"_hs, yaw);
        hub::row_tag(registry, row, "HUB_PLR_TURN"_hs);
    };
    auto inputs_of = [&registry](ecs::Entity named, float& sum) {
        uint32_t count = 0u;
        sum = 0.0f;
        for (auto [request, input] : registry.view<PlayerInpTurnComponent>().each()) {
            (void)request;
            if (input.figure == named) {
                ++count;
                sum += input.yaw;
            }
        }
        return count;
    };
    auto clear_inputs = [&registry]() {
        auto view = registry.view<PlayerInpTurnComponent>();
        registry.destroy(view.begin(), view.end());
    };

    // Two rows: two inputs, both rows released, nothing laid on the figure itself.
    file_turn(figure_id, 0.5f);
    file_turn(figure_id, -0.25f);
    REQUIRE(hub::row_count(registry, "HUB_PLR_TURN_REQ"_hs) == 2u);
    hub_turn.tick(registry, 0.0f);
    float sum = 0.0f;
    CHECK(inputs_of(figure, sum) == 2u);
    CHECK(sum == doctest::Approx(0.25f));
    CHECK(hub::row_count(registry, "HUB_PLR_TURN_REQ"_hs) == 0u);
    CHECK_FALSE(registry.all_of<PlayerInpTurnComponent>(figure));
    CHECK_FALSE(registry.all_of<PlayerReqTurnComponent>(figure));
    clear_inputs();

    // A row without its yaw turns nothing and is released.
    const ecs::Entity bare = hub::row_create(registry, "HUB_PLR_TURN_REQ"_hs);
    hub::set_u32(registry, static_cast<uint32_t>(bare), "HUB_PLR_TURN_FIGURE"_hs, figure_id);
    hub::row_tag(registry, bare, "HUB_PLR_TURN"_hs);
    hub_turn.tick(registry, 0.0f);
    CHECK(inputs_of(figure, sum) == 0u);
    CHECK(hub::row_count(registry, "HUB_PLR_TURN_REQ"_hs) == 0u);

    // A NaN yaw turns nothing and is released.
    file_turn(figure_id, std::nanf(""));
    hub_turn.tick(registry, 0.0f);
    CHECK(inputs_of(figure, sum) == 0u);
    CHECK(hub::row_count(registry, "HUB_PLR_TURN_REQ"_hs) == 0u);

    // A figure that is gone turns nothing and its row is released.
    const ecs::Entity gone = registry.create();
    const uint32_t gone_id = static_cast<uint32_t>(gone);
    registry.destroy(gone);
    file_turn(gone_id, 0.5f);
    hub_turn.tick(registry, 0.0f);
    CHECK(registry.view<PlayerInpTurnComponent>().size() == 0u);
    CHECK(hub::row_count(registry, "HUB_PLR_TURN_REQ"_hs) == 0u);
}

TEST_CASE("a standing figure turns out its open course at the turn speed and wraps the heading") {
    ecs::Registry registry;
    PlayerCtrlTurnSystem turn;
    // One tick turns at most half a radian.
    const float dt = 0.5f / MOVEMENT_DEFAULT_TURN_SPEED;

    const ecs::Entity figure = registry.create();
    auto& heading = registry.emplace<PlayerStaYawComponent>(figure);
    heading.yaw = 1.0f;

    auto file_input = [&registry, figure](float yaw) {
        const ecs::Entity request = registry.create();
        auto& input = registry.emplace<PlayerInpTurnComponent>(request);
        input.figure = figure;
        input.yaw = yaw;
    };

    // A turn inside one tick: turned at once, the request gone, the figure marked dirty.
    file_input(0.25f);
    turn.tick(registry, dt);
    CHECK(registry.get<PlayerStaYawComponent>(figure).yaw == doctest::Approx(1.25f));
    CHECK_FALSE(registry.all_of<PlayerReqTurnComponent>(figure));
    CHECK(registry.all_of<PlayerDrtyTag>(figure));
    CHECK(registry.view<PlayerInpTurnComponent>().size() == 0u);

    // A turn beyond one tick: half a radian now, the rest stays open for the next tick.
    registry.remove<PlayerDrtyTag>(figure);
    file_input(1.0f);
    turn.tick(registry, dt);
    CHECK(registry.get<PlayerStaYawComponent>(figure).yaw == doctest::Approx(1.75f));
    REQUIRE(registry.all_of<PlayerReqTurnComponent>(figure));
    CHECK(registry.get<PlayerReqTurnComponent>(figure).open_yaw == doctest::Approx(0.5f));
    CHECK(registry.all_of<PlayerDrtyTag>(figure));
    turn.tick(registry, dt);
    CHECK(registry.get<PlayerStaYawComponent>(figure).yaw == doctest::Approx(2.25f));
    CHECK_FALSE(registry.all_of<PlayerReqTurnComponent>(figure));

    // Two inputs in one tick add up onto one course.
    file_input(0.125f);
    file_input(0.25f);
    turn.tick(registry, dt);
    CHECK(registry.get<PlayerStaYawComponent>(figure).yaw == doctest::Approx(2.625f));
    CHECK_FALSE(registry.all_of<PlayerReqTurnComponent>(figure));

    // Across the full circle upward: the heading wraps back into [0, 2π).
    registry.get<PlayerStaYawComponent>(figure).yaw = math::TWO_PI - 0.125f;
    file_input(0.25f);
    turn.tick(registry, dt);
    CHECK(registry.get<PlayerStaYawComponent>(figure).yaw == doctest::Approx(0.125f));

    // Across zero downward: the heading wraps below the full circle.
    file_input(-0.25f);
    turn.tick(registry, dt);
    const float wrapped = registry.get<PlayerStaYawComponent>(figure).yaw;
    CHECK(wrapped == doctest::Approx(math::TWO_PI - 0.125f));
    CHECK(wrapped < math::TWO_PI);
    CHECK(wrapped >= 0.0f);
}

TEST_CASE("a walking figure keeps the heading of the camera and drops its open course") {
    ecs::Registry registry;
    PlayerCtrlTurnSystem turn;
    const float dt = 0.5f / MOVEMENT_DEFAULT_TURN_SPEED;

    const ecs::Entity figure = registry.create();
    registry.emplace<PlayerStaYawComponent>(figure).yaw = 1.0f;
    registry.emplace<PlayerInpExtComponent>(figure).inp_fwd = 1.0f;

    const ecs::Entity request = registry.create();
    auto& input = registry.emplace<PlayerInpTurnComponent>(request);
    input.figure = figure;
    input.yaw = 0.25f;

    turn.tick(registry, dt);
    CHECK(registry.get<PlayerStaYawComponent>(figure).yaw == doctest::Approx(1.0f));
    CHECK_FALSE(registry.all_of<PlayerReqTurnComponent>(figure));
    CHECK_FALSE(registry.valid(request));

    // A figure without a heading takes no course, and an input for a figure that is gone goes too.
    const ecs::Entity headless = registry.create();
    const ecs::Entity to_headless = registry.create();
    auto& headless_input = registry.emplace<PlayerInpTurnComponent>(to_headless);
    headless_input.figure = headless;
    headless_input.yaw = 0.25f;
    const ecs::Entity gone = registry.create();
    registry.destroy(gone);
    const ecs::Entity to_gone = registry.create();
    auto& gone_input = registry.emplace<PlayerInpTurnComponent>(to_gone);
    gone_input.figure = gone;
    gone_input.yaw = 0.25f;

    turn.tick(registry, dt);
    CHECK_FALSE(registry.all_of<PlayerReqTurnComponent>(headless));
    CHECK(registry.view<PlayerInpTurnComponent>().size() == 0u);
}

TEST_CASE("fig.turn end to end: a turn row of the host turns the figure through both systems") {
    using namespace entt::literals;
    ecs::Registry registry;
    PlayerHubTurnSystem hub_turn;
    PlayerCtrlTurnSystem turn;
    const float dt = 0.5f / MOVEMENT_DEFAULT_TURN_SPEED;

    const ecs::Entity figure = registry.create();
    registry.emplace<PlayerStaYawComponent>(figure).yaw = 0.5f;

    const ecs::Entity row = hub::row_create(registry, "HUB_PLR_TURN_REQ"_hs);
    hub::set_u32(registry, static_cast<uint32_t>(row), "HUB_PLR_TURN_FIGURE"_hs, static_cast<uint32_t>(figure));
    hub::set_f32(registry, static_cast<uint32_t>(row), "HUB_PLR_TURN_YAW"_hs, 0.75f);
    hub::row_tag(registry, row, "HUB_PLR_TURN"_hs);

    // Reception lays the row out, Dynamics turns the first half radian of it.
    hub_turn.tick(registry, dt);
    turn.tick(registry, dt);
    CHECK(registry.get<PlayerStaYawComponent>(figure).yaw == doctest::Approx(1.0f));
    CHECK(registry.all_of<PlayerDrtyTag>(figure));
    CHECK(registry.all_of<PlayerReqTurnComponent>(figure));
    CHECK(hub::row_count(registry, "HUB_PLR_TURN_REQ"_hs) == 0u);

    // The next pass turns the rest; no row is left and no course is open.
    hub_turn.tick(registry, dt);
    turn.tick(registry, dt);
    CHECK(registry.get<PlayerStaYawComponent>(figure).yaw == doctest::Approx(1.25f));
    CHECK_FALSE(registry.all_of<PlayerReqTurnComponent>(figure));
    CHECK(hub::row_count(registry, "HUB_PLR_TURN_REQ"_hs) == 0u);
}

TEST_CASE("usecase:101588 water under the figure: the measured cell depth lays the water tag, its loss takes it") {
    using namespace entt::literals;
    ecs::Registry registry;
    PlayerHubWatrSyncSystem water_sync;
    PlayerStaStsSystem status;

    const ecs::Entity figure = registry.create();
    registry.emplace<PlayerSpndTag>(figure);
    registry.emplace<PlayerStaIdntComponent>(figure);
    const uint32_t figure_id = static_cast<uint32_t>(figure);

    // RED when: a figure whose cell ase-gis has not mapped yet stands in water.
    water_sync.tick(registry, 0.0f);
    status.tick(registry, 0.0f);
    CHECK_FALSE(registry.all_of<PlayerInpWatrComponent>(figure));
    CHECK_FALSE(registry.all_of<PlayerStndWatrTag>(figure));

    // GisQryHydHubWrtSystem writes the depth of the chunk cell under the figure.
    hub::set(registry, figure_id, "GIS_HYD_CELL_DEPTH"_hs, 0.8f);
    water_sync.tick(registry, 0.0f);
    status.tick(registry, 0.0f);
    REQUIRE(registry.all_of<PlayerInpWatrComponent>(figure));
    CHECK(registry.get<PlayerInpWatrComponent>(figure).depth_m == doctest::Approx(0.8f));
    CHECK(registry.all_of<PlayerStndWatrTag>(figure));

    // RED when: the depth is read once and frozen - the next measurement must reach the figure.
    hub::set(registry, figure_id, "GIS_HYD_CELL_DEPTH"_hs, PLAYER_WATR_DRY_M);
    water_sync.tick(registry, 0.0f);
    status.tick(registry, 0.0f);
    CHECK(registry.get<PlayerInpWatrComponent>(figure).depth_m == doctest::Approx(PLAYER_WATR_DRY_M));
    CHECK_FALSE(registry.all_of<PlayerStndWatrTag>(figure));

    // RED when: a figure keeps the water of a measurement that no longer stands.
    hub::set(registry, figure_id, "GIS_HYD_CELL_DEPTH"_hs, 1.2f);
    water_sync.tick(registry, 0.0f);
    status.tick(registry, 0.0f);
    CHECK(registry.all_of<PlayerStndWatrTag>(figure));
    hub::remove(registry, figure_id, "GIS_HYD_CELL_DEPTH"_hs);
    water_sync.tick(registry, 0.0f);
    status.tick(registry, 0.0f);
    CHECK_FALSE(registry.all_of<PlayerInpWatrComponent>(figure));
    CHECK_FALSE(registry.all_of<PlayerStndWatrTag>(figure));

    // RED when: a negative depth passes the range check and lays a component.
    hub::set(registry, figure_id, "GIS_HYD_CELL_DEPTH"_hs, -1.0f);
    water_sync.tick(registry, 0.0f);
    CHECK_FALSE(registry.all_of<PlayerInpWatrComponent>(figure));

    // RED when: an entity outside the roster (no PlayerSpndTag) gets a depth.
    const ecs::Entity stranger = registry.create();
    hub::set(registry, static_cast<uint32_t>(stranger), "GIS_HYD_CELL_DEPTH"_hs, 2.0f);
    water_sync.tick(registry, 0.0f);
    CHECK_FALSE(registry.all_of<PlayerInpWatrComponent>(stranger));
}
