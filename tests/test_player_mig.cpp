// The doctest entry point is a compile definition of this target, set in tests/CMakeLists.txt —
// it reaches the translation unit before every #include, so doctest still generates exactly one
// main() here. A #define in the source would be a macro definition in C++, which is forbidden.
#include <doctest/doctest.h>

/**
 * ASE Player Migrate-Serializer Tests (PLAN_ASE_COMPUTE_PHASE_06_WORLD WS-H.4)
 *
 * @usecase     usecase:100692 - migrate figure between regions
 *
 * doctest CHECK/REQUIRE only - never assert() (all five servers compile tests with
 * -O3 -DNDEBUG, where assert(expr) expands to ((void)0) and the expression is never
 * compiled).
 *
 * WHAT IS DRIVEN HERE: the REAL PlayerMigSerSystem, not a re-implementation. The encoder
 * lives in an anonymous namespace inside the .cpp and is unreachable from a test, which is
 * correct - the contract that matters is the BYTES of the staged row, and those are produced by
 * ticking the system.
 *
 * THE FOUR CASES:
 *   1. the frozen 42-byte PlayerSnap layout, read back at the L0 offsets from HUB_PLR_MIG_STG
 *   2. the per-player epoch bump is monotonic - the source of migrate idempotency
 *   3. a standing HUB_PLR_MIG_STG row bars re-entry across ticks
 *   4. two figures staged in one tick keep their own identity
 *
 * The arm is the hub row HUB_PLR_MIG_ARM on the figure, as WorldPlrBndSystem adopts it; the
 * staged image is a row of its own with the figure as its FIG field.
 */

#include <ase/player/player.hpp>
#include <ase/player/systems/migration/player_mig_ser_sys.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_sts_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_epch_comp.hpp>
#include <ase/player/types.hpp>
// The frozen wire layout is an L0 contract: offsets and size come from there, never from a
// number repeated here. A test that hard-codes 42 would still pass after a contract change.
#include <ase/types/region_wire.hpp>
// Arm and staged image are hub rows.
#include <ase/hub/api.hpp>
#include <ase/ecs/system.hpp>
#include <ase/utils/strops.hpp>   // Layer 0 — str_copy kennt die Zielgroesse

#include <cstdint>
#include <cstring>

#include <entt/core/hashed_string.hpp>

namespace {

using namespace ase;
using namespace entt::literals;

// Reads a little-endian u32 back out of the staged image. memcpy, not a cast: the snap is a
// char buffer and an aliased load through uint32_t* is undefined behaviour.
uint32_t read_u32(const char* snap, uint32_t offset) {
    uint32_t out = 0;
    std::memcpy(&out, snap + offset, 4);
    return out;
}

uint16_t read_u16(const char* snap, uint32_t offset) {
    uint16_t out = 0;
    std::memcpy(&out, snap + offset, 2);
    return out;
}

float read_f32(const char* snap, uint32_t offset) {
    float out = 0.0f;
    std::memcpy(&out, snap + offset, 4);
    return out;
}

// Builds one player carrying every component the serialize walk requires, plus the armed
// migrate as the boundary watch files it. Values are distinct per field so a swapped offset
// cannot pass unnoticed.
ecs::Entity make_armed_player(ecs::Registry& registry, const char* uuid, uint32_t dst_region) {
    const auto plr = registry.create();

    auto& pos = registry.emplace<player::PlayerStaPosComponent>(plr);
    pos.chunk_x = 3;
    pos.chunk_z = -2;
    pos.local_x = 5.5f;
    pos.local_z = 7.25f;
    pos.y = 64.5f;

    registry.emplace<player::PlayerStaYawComponent>(plr).yaw = 1.75f;

    auto& vel = registry.emplace<player::PlayerStaVelComponent>(plr);
    vel.vx = 1.5f;
    vel.vy = -9.5f;
    vel.vz = 2.25f;

    registry.emplace<player::PlayerStaStsComponent>(plr).sts = 7u;

    auto& idnt = registry.emplace<player::PlayerStaIdntComponent>(plr);
    // str_copy gets the target size and always terminates; the cast states the narrowing.
    utils::str_copy(idnt.player_id, static_cast<uint32_t>(sizeof(idnt.player_id)), uuid);

    const uint32_t fig = static_cast<uint32_t>(plr);
    hub::row_adopt(registry, plr, "HUB_PLR_MIG_ARM"_hs);
    hub::set_u32(registry, fig, "HUB_PLR_MIG_ARM_DST_REGION"_hs, dst_region);
    hub::set_u32(registry, fig, "HUB_PLR_MIG_ARM_PROJ_HASH"_hs, 0xABCD1234u);

    return plr;
}

// The staged row of one figure, or entt::null when none stands. Test-side lookup over the
// kind; the system under test never searches like this.
ecs::Entity staged_row_of(ecs::Registry& registry, ecs::Entity plr) {
    ecs::Entity found = entt::null;
    hub::row_each(registry, "HUB_PLR_MIG_STG"_hs, [&registry, plr, &found](ecs::Entity row) {
        uint32_t fig = 0u;
        if (hub::try_get_u32(registry, static_cast<uint32_t>(row), "HUB_PLR_MIG_STG_FIG"_hs, fig) &&
            fig == static_cast<uint32_t>(plr)) {
            found = row;
        }
    });
    return found;
}

}  // namespace

TEST_CASE("PlayerMigSer: the staged row carries the frozen 42-byte PlayerSnap layout") {
    ecs::Registry registry;
    player::PlayerMigSerSystem system;

    const char* uuid = "11111111-2222-3333-4444-555555555555";
    const auto plr = make_armed_player(registry, uuid, 42u);

    system.tick(registry, 0.0f);

    REQUIRE(hub::row_count(registry, "HUB_PLR_MIG_STG"_hs) == 1u);
    const auto row = staged_row_of(registry, plr);
    REQUIRE(registry.valid(row));
    const uint32_t owner = static_cast<uint32_t>(row);
    char snap[types::PLAYER_SNAP_SZ] = {};
    REQUIRE(hub::try_get_bytes(registry, owner, "HUB_PLR_MIG_STG_SNAP"_hs, snap,
                               types::PLAYER_SNAP_SZ));

    SUBCASE("the stored length is the contract size, not a local constant") {
        CHECK(hub::bytes_len(registry, owner, "HUB_PLR_MIG_STG_SNAP"_hs) == types::PLAYER_SNAP_SZ);
    }

    SUBCASE("schema, id and epoch sit at their contract offsets") {
        CHECK(read_u16(snap, types::PLAYER_SNAP_OFF_SCHEMA) == types::PLAYER_SNAP_SCHEMA_VER);
        // player_ref is the FNV-1a32 of the UUID string, derived by the system itself.
        CHECK(read_u32(snap, types::PLAYER_SNAP_OFF_ID) == entt::hashed_string::value(uuid));
        CHECK(read_u32(snap, types::PLAYER_SNAP_OFF_EPOCH) == 1u);
    }

    SUBCASE("position is the world-metre sum chunk * edge + local, and y passes through") {
        // THE WIRE STAYS IN WORLD METRES (frozen PlayerSnap, 4x f32).
        const float expect_x = 3.0f * player::MOVEMENT_DEFAULT_CHUNK_SIZE + 5.5f;
        const float expect_z = -2.0f * player::MOVEMENT_DEFAULT_CHUNK_SIZE + 7.25f;
        CHECK(read_f32(snap, types::PLAYER_SNAP_OFF_POS) == doctest::Approx(expect_x));
        CHECK(read_f32(snap, types::PLAYER_SNAP_OFF_POS + 4u) == doctest::Approx(64.5f));
        CHECK(read_f32(snap, types::PLAYER_SNAP_OFF_POS + 8u) == doctest::Approx(expect_z));
        CHECK(read_f32(snap, types::PLAYER_SNAP_OFF_POS + 12u) == doctest::Approx(1.75f));
    }

    SUBCASE("velocity keeps its three axes apart") {
        CHECK(read_f32(snap, types::PLAYER_SNAP_OFF_VEL) == doctest::Approx(1.5f));
        CHECK(read_f32(snap, types::PLAYER_SNAP_OFF_VEL + 4u) == doctest::Approx(-9.5f));
        CHECK(read_f32(snap, types::PLAYER_SNAP_OFF_VEL + 8u) == doctest::Approx(2.25f));
    }

    SUBCASE("the u8 status widens into the u32 wire field") {
        CHECK(read_u32(snap, types::PLAYER_SNAP_OFF_STATUS) == 7u);
    }

    SUBCASE("the instruction stays on the figure for the sender") {
        // The sender takes destination and project from the arm and withdraws it after the ship.
        CHECK(hub::row_has(registry, plr, "HUB_PLR_MIG_ARM"_hs));
    }
}

TEST_CASE("PlayerMigSer: the epoch bump is monotonic and happens once per armed migrate") {
    ecs::Registry registry;
    player::PlayerMigSerSystem system;

    const auto plr = make_armed_player(registry, "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee", 9u);

    system.tick(registry, 0.0f);
    const auto epoch_after_first = registry.get<player::PlayerStaEpchComponent>(plr).player_epoch;
    CHECK(epoch_after_first == 1u);

    SUBCASE("a second tick does NOT bump again - the standing row bars re-entry") {
        // Without that barrier the receiver would see two different epochs for one migrate and
        // could not tell a duplicate from a fresh move.
        system.tick(registry, 0.0f);
        CHECK(registry.get<player::PlayerStaEpchComponent>(plr).player_epoch == epoch_after_first);
        CHECK(hub::row_count(registry, "HUB_PLR_MIG_STG"_hs) == 1u);
    }

    SUBCASE("releasing the staged row re-arms the walk and the epoch grows, never resets") {
        const auto first_row = staged_row_of(registry, plr);
        REQUIRE(registry.valid(first_row));
        hub::row_release(registry, first_row);
        system.tick(registry, 0.0f);
        const auto second = registry.get<player::PlayerStaEpchComponent>(plr).player_epoch;
        CHECK(second == 2u);
        CHECK(second > epoch_after_first);
        const auto second_row = staged_row_of(registry, plr);
        REQUIRE(registry.valid(second_row));
        char snap[types::PLAYER_SNAP_SZ] = {};
        REQUIRE(hub::try_get_bytes(registry, static_cast<uint32_t>(second_row),
                                   "HUB_PLR_MIG_STG_SNAP"_hs, snap, types::PLAYER_SNAP_SZ));
        // Monotonic means the number never goes backwards, which is what makes it usable as a
        // duplicate discriminator on the receiving side.
        CHECK(read_u32(snap, types::PLAYER_SNAP_OFF_EPOCH) == second);
    }
}

TEST_CASE("PlayerMigSer: an unarmed player is never staged") {
    ecs::Registry registry;
    player::PlayerMigSerSystem system;

    const auto plr = make_armed_player(registry, "00000000-0000-0000-0000-000000000000", 1u);
    // Take the arm away: without it the walk must not touch the entity at all.
    hub::row_withdraw(registry, plr, "HUB_PLR_MIG_ARM"_hs);

    system.tick(registry, 0.0f);

    CHECK(hub::row_count(registry, "HUB_PLR_MIG_STG"_hs) == 0u);
    // And no epoch was invented for a migrate that was never armed.
    const auto* epch = registry.try_get<player::PlayerStaEpchComponent>(plr);
    CHECK((epch == nullptr || epch->player_epoch == 0u));
}

TEST_CASE("PlayerMigSer: two players staged in one tick keep their own ref and epoch") {
    ecs::Registry registry;
    player::PlayerMigSerSystem system;

    const char* uuid_a = "aaaa1111-0000-0000-0000-000000000001";
    const char* uuid_b = "bbbb2222-0000-0000-0000-000000000002";
    const auto plr_a = make_armed_player(registry, uuid_a, 11u);
    const auto plr_b = make_armed_player(registry, uuid_b, 22u);

    system.tick(registry, 0.0f);

    REQUIRE(hub::row_count(registry, "HUB_PLR_MIG_STG"_hs) == 2u);
    const auto row_a = staged_row_of(registry, plr_a);
    const auto row_b = staged_row_of(registry, plr_b);
    REQUIRE(registry.valid(row_a));
    REQUIRE(registry.valid(row_b));
    char snap_a[types::PLAYER_SNAP_SZ] = {};
    char snap_b[types::PLAYER_SNAP_SZ] = {};
    REQUIRE(hub::try_get_bytes(registry, static_cast<uint32_t>(row_a), "HUB_PLR_MIG_STG_SNAP"_hs,
                               snap_a, types::PLAYER_SNAP_SZ));
    REQUIRE(hub::try_get_bytes(registry, static_cast<uint32_t>(row_b), "HUB_PLR_MIG_STG_SNAP"_hs,
                               snap_b, types::PLAYER_SNAP_SZ));

    // row_each walks a storage from its back, so the staging ORDER among simultaneous migrates
    // is not fixed and is deliberately not checked here. What must hold is that neither image
    // picked up the other's identity.
    CHECK(read_u32(snap_a, types::PLAYER_SNAP_OFF_ID) == entt::hashed_string::value(uuid_a));
    CHECK(read_u32(snap_b, types::PLAYER_SNAP_OFF_ID) == entt::hashed_string::value(uuid_b));
    CHECK(read_u32(snap_a, types::PLAYER_SNAP_OFF_EPOCH) == 1u);
    CHECK(read_u32(snap_b, types::PLAYER_SNAP_OFF_EPOCH) == 1u);
}
