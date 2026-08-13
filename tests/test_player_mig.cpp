#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

/**
 * ASE Player Migrate-Serializer Tests (PLAN_ASE_COMPUTE_PHASE_06_WORLD WS-H.4)
 *
 * Drives the REAL PlayerMigSerSystem: the armed PlayerReqMigComponent must yield exactly one
 * staged PlayerBufMigComponent carrying the frozen 42-byte PlayerSnap layout
 * (ase/types/region_wire.hpp) encoded from the live player_st_* components, with the per-player
 * monotonic epoch bumped exactly once per armed migrate (the WS-H.4 idempotency source).
 * doctest CHECK/REQUIRE only - never assert() (Release/NDEBUG no-op).
 */

#include <ase/player/player_module.hpp>
#include <ase/player/systems/migration/player_mig_ser_sys.hpp>
#include <ase/player/components/request/player_req_mig_comp.hpp>
#include <ase/player/components/state/player_sta_pos_comp.hpp>
#include <ase/player/components/state/player_sta_yaw_comp.hpp>
#include <ase/player/components/state/player_sta_vel_comp.hpp>
#include <ase/player/components/state/player_sta_sts_comp.hpp>
#include <ase/player/components/state/player_sta_idnt_comp.hpp>
#include <ase/player/components/state/player_sta_epch_comp.hpp>
#include <ase/player/components/buffer/player_buf_mig_comp.hpp>
#include <ase/types/region_wire.hpp>
#include <ase/player/types.hpp>
#include <ase/math/math.hpp>
#include <ase/ecs/system.hpp>

#include <entt/core/hashed_string.hpp>

#include <cstdint>
#include <cstring>

using namespace ase;
using namespace entt::literals;

namespace {

// One live player with the real state components an armed migrate serializes from.
ecs::Entity add_player(ecs::Registry& reg, const char* uuid, float x, float y, float z,
                       float yaw, float vx, float vy, float vz, uint8_t sts) {
    auto e = reg.create();
    auto& idc = reg.emplace<player::PlayerStaIdntComponent>(e);
    uint32_t n = 0;
    while (uuid[n] != '\0' && n < 63u) {
        idc.player_id[n] = uuid[n];
        ++n;
    }
    // Chunk-relativ (S2b 2026-08-11): der Helfer nimmt weiter Weltmeter und zerlegt per
    // floor/Rest - exakt wie der produktive Spawn (player_life_spwn_sys.cpp).
    auto& pos = reg.emplace<player::PlayerStaPosComponent>(e);
    pos.chunk_x = static_cast<int32_t>(math::floor(x / player::MOVEMENT_DEFAULT_CHUNK_SIZE));
    pos.chunk_z = static_cast<int32_t>(math::floor(z / player::MOVEMENT_DEFAULT_CHUNK_SIZE));
    pos.local_x = x - static_cast<float>(pos.chunk_x) * player::MOVEMENT_DEFAULT_CHUNK_SIZE;
    pos.local_z = z - static_cast<float>(pos.chunk_z) * player::MOVEMENT_DEFAULT_CHUNK_SIZE;
    pos.y = y;
    auto& yaw_row = reg.emplace<player::PlayerStaYawComponent>(e);
    yaw_row.yaw = yaw;
    auto& vel = reg.emplace<player::PlayerStaVelComponent>(e);
    vel.vx = vx;
    vel.vy = vy;
    vel.vz = vz;
    auto& sta = reg.emplace<player::PlayerStaStsComponent>(e);
    sta.sts = sts;
    return e;
}

}  // namespace

// WS-H.4 serializer: the frozen PlayerSnap layout byte for byte, epoch bumped exactly once per
// armed migrate, identity hashed from the UUID string (FNV-1a32 - the u32 the wire carries).
TEST_CASE("PlayerSnap serializer: frozen 42-byte layout + monotonic epoch bump") {
    REQUIRE(ase::player::PlayerModule::name() != nullptr);

    ecs::Registry reg;
    player::PlayerMigSerSystem ser;
    auto plr = add_player(reg, "uuid-mig-1", 70.0f, 1.5f, 5.0f, 0.5f, 0.1f, -0.2f, 0.3f, 2u);
    auto& req = reg.emplace<player::PlayerReqMigComponent>(plr);
    req.dst_region = types::REGION_ID_NONE;
    req.proj_hash = 4242u;

    ser.tick(reg, 1.0f);

    REQUIRE(reg.all_of<player::PlayerBufMigComponent>(plr));
    const auto& snap_buf = reg.get<player::PlayerBufMigComponent>(plr);
    CHECK(snap_buf.snap_len == types::PLAYER_SNAP_SZ);

    uint16_t schema = 0;
    uint32_t wire_id = 0;
    uint32_t wire_epoch = 0;
    float x = 0.0f;
    float yaw = 0.0f;
    float vy = 0.0f;
    uint32_t status = 0;
    std::memcpy(&schema, snap_buf.snap + types::PLAYER_SNAP_OFF_SCHEMA, 2);
    std::memcpy(&wire_id, snap_buf.snap + types::PLAYER_SNAP_OFF_ID, 4);
    std::memcpy(&wire_epoch, snap_buf.snap + types::PLAYER_SNAP_OFF_EPOCH, 4);
    std::memcpy(&x, snap_buf.snap + types::PLAYER_SNAP_OFF_POS, 4);
    std::memcpy(&yaw, snap_buf.snap + types::PLAYER_SNAP_OFF_POS + 12u, 4);
    std::memcpy(&vy, snap_buf.snap + types::PLAYER_SNAP_OFF_VEL + 4u, 4);
    std::memcpy(&status, snap_buf.snap + types::PLAYER_SNAP_OFF_STATUS, 4);
    CHECK(schema == types::PLAYER_SNAP_SCHEMA_VER);
    CHECK(wire_id == entt::hashed_string::value("uuid-mig-1"));
    CHECK(wire_epoch == 1u);  // first armed migrate of this player
    CHECK(x == doctest::Approx(70.0f));
    CHECK(yaw == doctest::Approx(0.5f));
    CHECK(vy == doctest::Approx(-0.2f));
    CHECK(status == 2u);  // u8 player state widened to the u32 wire field

    SUBCASE("staged buffer blocks re-entry: no double encode, no double epoch bump") {
        ser.tick(reg, 1.0f);
        const auto& epch = reg.get<player::PlayerStaEpchComponent>(plr);
        CHECK(epch.player_epoch == 1u);
    }

    SUBCASE("a later re-arm bumps the epoch monotonically (WS-H.4 idempotency source)") {
        // The World send system consumed request + buffer; a fresh boundary crossing re-arms.
        reg.remove<player::PlayerBufMigComponent>(plr);
        ser.tick(reg, 1.0f);  // request still present → second encode
        const auto& epch = reg.get<player::PlayerStaEpchComponent>(plr);
        CHECK(epch.player_epoch == 2u);
        const auto& again = reg.get<player::PlayerBufMigComponent>(plr);
        uint32_t epoch_bytes = 0;
        std::memcpy(&epoch_bytes, again.snap + types::PLAYER_SNAP_OFF_EPOCH, 4);
        CHECK(epoch_bytes == 2u);
    }

    SUBCASE("a player without an armed request is never serialized") {
        auto idle = add_player(reg, "uuid-idle", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0u);
        ser.tick(reg, 1.0f);
        CHECK(!reg.all_of<player::PlayerBufMigComponent>(idle));
        CHECK(!reg.all_of<player::PlayerStaEpchComponent>(idle));
    }
}
