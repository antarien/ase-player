#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

/**
 * ASE Player Migrate-Serializer Tests (PLAN_ASE_COMPUTE_PHASE_06_WORLD WS-H.4)
 *
 * doctest CHECK/REQUIRE only - never assert() (all five servers compile tests with
 * -O3 -DNDEBUG, where assert(expr) expands to ((void)0) and the expression is never
 * compiled; test_player.cpp documents how that hid a compile error for nine days).
 *
 * WHY THIS FILE EXISTS, written 2026-08-23:
 *
 *   The target `ase-player-mig-test` was declared in tests/CMakeLists.txt with an EMPTY
 *   source list. Nothing was compiled into it, so the linker found no main and the build
 *   failed at 750/832 with "undefined reference to `main'". The target was intentional -
 *   its purpose is spelled out above the declaration - but the file was never written.
 *   This is that file. The target is NOT removed and NOT excluded from `all`: deleting it
 *   would make the link error disappear together with the test (Rule 13, Rule 19).
 *
 *   It went unnoticed because ctest never ran: 105 add_test() calls existed tree-wide
 *   against zero enable_testing(), so every test binary was built by every build and
 *   executed by nobody. A target with no sources is exactly the defect a first ctest run
 *   reports immediately.
 *
 * WHAT IS DRIVEN HERE: the REAL PlayerMigSerSystem, not a re-implementation. The encoder
 * lives in an anonymous namespace inside the .cpp and is unreachable from a test, which is
 * correct - the contract that matters is the BYTES on the entity, and those are produced by
 * ticking the system.
 *
 * THE FOUR CASES, in the order the target comment names them:
 *   1. the frozen 42-byte PlayerSnap layout, read back at the L0 offsets
 *   2. the per-player epoch bump is monotonic - the source of migrate idempotency
 *   3. staged-buffer re-entry is barred across ticks
 *   4. all of it through PlayerMigSerSystem::tick
 *
 * IF A CHECK TURNS RED ON THE FIRST REAL RUN, THAT IS A FINDING, NOT A REGRESSION - the
 * same caveat test_player.cpp carries. These checks have never been executed.
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
// Arm and delivery ride the hub star on the figure itself since 2026-08-19.
#include <ase/hub/api.hpp>
#include <ase/ecs/system.hpp>

#include <cstdint>
#include <cstring>

#include <entt/core/hashed_string.hpp>

namespace {

using namespace ase;

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
// migrate. Values are distinct per field so a swapped offset cannot pass unnoticed.
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
    std::memcpy(idnt.player_id, uuid, std::strlen(uuid) + 1u);

    auto& arm = registry.emplace<hub::HubPlrMigArmComponent>(plr);
    arm.dst_region = dst_region;
    arm.proj_hash = 0xABCD1234u;

    return plr;
}

}  // namespace

TEST_CASE("PlayerMigSer: the staged image carries the frozen 42-byte PlayerSnap layout") {
    ecs::Registry registry;
    player::PlayerMigSerSystem system;

    const char* uuid = "11111111-2222-3333-4444-555555555555";
    const auto plr = make_armed_player(registry, uuid, 42u);

    system.tick(registry, 0.0f);

    const auto* snap = registry.try_get<hub::HubPlrMigSnapComponent>(plr);
    REQUIRE(snap != nullptr);

    SUBCASE("the encoded length is the contract size, not a local constant") {
        CHECK(snap->snap_len == types::PLAYER_SNAP_SZ);
    }

    SUBCASE("schema, id and epoch sit at their contract offsets") {
        CHECK(read_u16(snap->snap, types::PLAYER_SNAP_OFF_SCHEMA) == types::PLAYER_SNAP_SCHEMA_VER);
        // player_ref is the FNV-1a32 of the UUID string, derived by the system itself.
        CHECK(read_u32(snap->snap, types::PLAYER_SNAP_OFF_ID) == entt::hashed_string::value(uuid));
        CHECK(read_u32(snap->snap, types::PLAYER_SNAP_OFF_EPOCH) == 1u);
    }

    SUBCASE("position is the world-metre sum chunk * edge + local, and y passes through") {
        // THE WIRE STAYS IN WORLD METRES (frozen PlayerSnap, 4x f32). The sum is exact on
        // cell edges; a (chunk:i32, local:f32) wire v2 is an open operator decision.
        const float expect_x = 3.0f * player::MOVEMENT_DEFAULT_CHUNK_SIZE + 5.5f;
        const float expect_z = -2.0f * player::MOVEMENT_DEFAULT_CHUNK_SIZE + 7.25f;
        CHECK(read_f32(snap->snap, types::PLAYER_SNAP_OFF_POS) == doctest::Approx(expect_x));
        CHECK(read_f32(snap->snap, types::PLAYER_SNAP_OFF_POS + 4u) == doctest::Approx(64.5f));
        CHECK(read_f32(snap->snap, types::PLAYER_SNAP_OFF_POS + 8u) == doctest::Approx(expect_z));
        CHECK(read_f32(snap->snap, types::PLAYER_SNAP_OFF_POS + 12u) == doctest::Approx(1.75f));
    }

    SUBCASE("velocity keeps its three axes apart") {
        CHECK(read_f32(snap->snap, types::PLAYER_SNAP_OFF_VEL) == doctest::Approx(1.5f));
        CHECK(read_f32(snap->snap, types::PLAYER_SNAP_OFF_VEL + 4u) == doctest::Approx(-9.5f));
        CHECK(read_f32(snap->snap, types::PLAYER_SNAP_OFF_VEL + 8u) == doctest::Approx(2.25f));
    }

    SUBCASE("the u8 status widens into the u32 wire field") {
        CHECK(read_u32(snap->snap, types::PLAYER_SNAP_OFF_STATUS) == 7u);
    }

    SUBCASE("the delivery repeats ref and destination beside the image") {
        CHECK(snap->player_ref == entt::hashed_string::value(uuid));
        CHECK(snap->dst_region == 42u);
    }

    SUBCASE("the send-side enumeration marker is set in the same pass") {
        // The sender counts over this marker, never over the hub rows themselves.
        CHECK(registry.all_of<hub::HubPlrMigSndPndTag>(plr));
    }
}

TEST_CASE("PlayerMigSer: the epoch bump is monotonic and happens once per armed migrate") {
    ecs::Registry registry;
    player::PlayerMigSerSystem system;

    const auto plr = make_armed_player(registry, "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee", 9u);

    system.tick(registry, 0.0f);
    const auto epoch_after_first = registry.get<player::PlayerStaEpchComponent>(plr).player_epoch;
    CHECK(epoch_after_first == 1u);

    SUBCASE("a second tick does NOT bump again - the staged delivery bars re-entry") {
        // This is the idempotency source: the serialize walk excludes HubPlrMigSnapComponent,
        // so an entity that already carries a staged image never re-enters the bump. Without
        // that barrier the receiver would see two different epochs for one migrate and could
        // not tell a duplicate from a fresh move.
        system.tick(registry, 0.0f);
        CHECK(registry.get<player::PlayerStaEpchComponent>(plr).player_epoch == epoch_after_first);
    }

    SUBCASE("clearing the staged delivery re-arms the walk and the epoch grows, never resets") {
        registry.remove<hub::HubPlrMigSnapComponent>(plr);
        system.tick(registry, 0.0f);
        const auto second = registry.get<player::PlayerStaEpchComponent>(plr).player_epoch;
        CHECK(second == 2u);
        CHECK(second > epoch_after_first);
        // Monotonic means the number never goes backwards, which is what makes it usable as
        // a duplicate discriminator on the receiving side.
        CHECK(read_u32(registry.get<hub::HubPlrMigSnapComponent>(plr).snap,
                       types::PLAYER_SNAP_OFF_EPOCH) == second);
    }
}

TEST_CASE("PlayerMigSer: an unarmed player is never staged") {
    ecs::Registry registry;
    player::PlayerMigSerSystem system;

    const auto plr = make_armed_player(registry, "00000000-0000-0000-0000-000000000000", 1u);
    // Take the arm away: without it the walk must not touch the entity at all.
    registry.remove<hub::HubPlrMigArmComponent>(plr);

    system.tick(registry, 0.0f);

    CHECK(registry.try_get<hub::HubPlrMigSnapComponent>(plr) == nullptr);
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

    const auto& snap_a = registry.get<hub::HubPlrMigSnapComponent>(plr_a);
    const auto& snap_b = registry.get<hub::HubPlrMigSnapComponent>(plr_b);

    // EnTT iterates a pool backwards, so the staging ORDER among simultaneous migrates is not
    // fixed and is deliberately not checked here. What must hold is that neither image picked
    // up the other's identity or destination.
    CHECK(snap_a.player_ref == entt::hashed_string::value(uuid_a));
    CHECK(snap_b.player_ref == entt::hashed_string::value(uuid_b));
    CHECK(snap_a.player_ref != snap_b.player_ref);
    CHECK(snap_a.dst_region == 11u);
    CHECK(snap_b.dst_region == 22u);
    CHECK(read_u32(snap_a.snap, types::PLAYER_SNAP_OFF_EPOCH) == 1u);
    CHECK(read_u32(snap_b.snap, types::PLAYER_SNAP_OFF_EPOCH) == 1u);
}
