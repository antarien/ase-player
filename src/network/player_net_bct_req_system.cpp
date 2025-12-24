#include <ase/player/systems/network/player_net_bct_req_system.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_vel_component.hpp>
#include <ase/player/components/state/player_st_sts_component.hpp>
#include <ase/player/components/buffer/player_buf_bct_spn_component.hpp>
#include <ase/player/components/buffer/player_buf_bct_sta_component.hpp>
#include <ase/player/components/tag/player_tag_dirty_component.hpp>
#include <ase/player/components/tag/player_tag_spawned_component.hpp>
#include <ase/player/components/tag/player_tag_chunk_changed_component.hpp>
#include <ase/player/components/tag/player_tag_bct_spn_pnd_component.hpp>
#include <ase/player/components/tag/player_tag_bct_sta_pnd_component.hpp>
#include <ase/player/types.hpp>
#include <ase/serial/serial.hpp>
#include <ase/log/log.hpp>

#include <nlohmann/json.hpp>
#include <chrono>
#include <cstring>

namespace ase::player {

namespace {

// =============================================================================
// SERIALIZATION FUNCTIONS (registered with ase-serial)
// =============================================================================

void ser_plr_spn(const void* src, uint32_t /*siz*/, void* out) {
    const auto* buf = static_cast<const PlayerBufBctSpnComponent*>(src);
    auto* j = static_cast<nlohmann::json*>(out);

    *j = {
        {"playerStId", {
            {"player_id", buf->player_id.data()},
            {"spawned_at", static_cast<double>(buf->spawned_at_ms)},
            {"last_input", static_cast<double>(buf->last_input_ms)}
        }},
        {"playerStPos", {
            {"x", buf->x},
            {"y", buf->y},
            {"z", buf->z},
            {"yaw", buf->yaw}
        }}
    };
}

void ser_plr_sta(const void* src, uint32_t /*siz*/, void* out) {
    const auto* buf = static_cast<const PlayerBufBctStaComponent*>(src);
    auto* j = static_cast<nlohmann::json*>(out);

    *j = {
        {"playerStId", {
            {"player_id", buf->player_id.data()},
            {"spawned_at", static_cast<double>(buf->spawned_at_ms)},
            {"last_input", static_cast<double>(buf->last_input_ms)}
        }},
        {"playerStPos", {
            {"x", buf->x},
            {"y", buf->y},
            {"z", buf->z},
            {"yaw", buf->yaw}
        }},
        {"playerStVel", {
            {"vx", buf->vx},
            {"vy", buf->vy},
            {"vz", buf->vz}
        }},
        {"playerStSts", {
            {"state", buf->state}
        }}
    };
}

}  // anonymous namespace

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerNetBctReqSystem::on_start(ecs::Registry& registry) {
    // Register spawn serializer
    {
        auto reg = registry.create();
        auto& buf = registry.emplace<serial::SerialBufRegComponent>(reg);
        buf.typ_id = SERIAL_TYP_PLR_SPN;
        buf.fn_ptr = reinterpret_cast<uint64_t>(&ser_plr_spn);
        registry.emplace<serial::SerialRegPndTag>(reg);
    }

    // Register state serializer
    {
        auto reg = registry.create();
        auto& buf = registry.emplace<serial::SerialBufRegComponent>(reg);
        buf.typ_id = SERIAL_TYP_PLR_STA;
        buf.fn_ptr = reinterpret_cast<uint64_t>(&ser_plr_sta);
        registry.emplace<serial::SerialRegPndTag>(reg);
    }

    log::info("[PlayerNetBctReqSystem] Registered serialization types");
}

void PlayerNetBctReqSystem::on_stop(ecs::Registry& /*registry*/) {
    // Serializers are cleaned up by SerialJsnSystem
}

// =============================================================================
// TICK
// =============================================================================

void PlayerNetBctReqSystem::tick(ecs::Registry& registry, float /*dt*/) {
    using namespace std::chrono;

    // =========================================================================
    // 1. CREATE SPAWN BROADCAST REQUESTS
    // =========================================================================
    {
        auto view = registry.view<
            PlayerStIdComponent,
            PlayerStPosComponent,
            PlayerSpawnedTag
        >();

        std::vector<ecs::Entity> to_remove;
        for (auto [entity, id, pos] : view.each()) {
            // Create serialization request entity
            auto ser = registry.create();

            // Create buffer with combined data
            auto& spn_buf = registry.emplace<PlayerBufBctSpnComponent>(ser);
            std::strncpy(spn_buf.player_id.data(), id.player_id.data(), spn_buf.player_id.size() - 1);
            spn_buf.spawned_at_ms = duration_cast<milliseconds>(id.spawned_at.time_since_epoch()).count();
            spn_buf.last_input_ms = duration_cast<milliseconds>(id.last_input.time_since_epoch()).count();
            spn_buf.x = pos.x;
            spn_buf.y = pos.y;
            spn_buf.z = pos.z;
            spn_buf.yaw = pos.yaw;

            // Create serialization request
            auto& jsn_buf = registry.emplace<serial::SerialBufJsnComponent>(ser);
            jsn_buf.src_ptr = reinterpret_cast<uint64_t>(&registry.get<PlayerBufBctSpnComponent>(ser));
            jsn_buf.src_typ = SERIAL_TYP_PLR_SPN;
            jsn_buf.src_siz = sizeof(PlayerBufBctSpnComponent);

            // Add tags
            registry.emplace<serial::SerialJsnPndTag>(ser);
            registry.emplace<PlayerBctSpnPndTag>(ser);

            log::debug("[PlayerNetBctReqSystem] Created spawn request for player {}", id.player_id.data());
            to_remove.push_back(entity);
        }

        for (auto entity : to_remove) {
            registry.remove<PlayerSpawnedTag>(entity);
        }
    }

    // =========================================================================
    // 2. CREATE STATE BROADCAST REQUESTS (dirty players)
    // =========================================================================
    {
        auto view = registry.view<
            PlayerStIdComponent,
            PlayerStPosComponent,
            PlayerStVelComponent,
            PlayerStStsComponent,
            PlayerDirtyTag
        >();

        std::vector<ecs::Entity> to_remove;
        for (auto [entity, id, pos, vel, sts] : view.each()) {
            // Create serialization request entity
            auto ser = registry.create();

            // Create buffer with combined data
            auto& sta_buf = registry.emplace<PlayerBufBctStaComponent>(ser);
            std::strncpy(sta_buf.player_id.data(), id.player_id.data(), sta_buf.player_id.size() - 1);
            sta_buf.spawned_at_ms = duration_cast<milliseconds>(id.spawned_at.time_since_epoch()).count();
            sta_buf.last_input_ms = duration_cast<milliseconds>(id.last_input.time_since_epoch()).count();
            sta_buf.x = pos.x;
            sta_buf.y = pos.y;
            sta_buf.z = pos.z;
            sta_buf.yaw = pos.yaw;
            sta_buf.vx = vel.vx;
            sta_buf.vy = vel.vy;
            sta_buf.vz = vel.vz;
            sta_buf.state = sts.state;

            // Create serialization request
            auto& jsn_buf = registry.emplace<serial::SerialBufJsnComponent>(ser);
            jsn_buf.src_ptr = reinterpret_cast<uint64_t>(&registry.get<PlayerBufBctStaComponent>(ser));
            jsn_buf.src_typ = SERIAL_TYP_PLR_STA;
            jsn_buf.src_siz = sizeof(PlayerBufBctStaComponent);

            // Add tags
            registry.emplace<serial::SerialJsnPndTag>(ser);
            registry.emplace<PlayerBctStaPndTag>(ser);

            to_remove.push_back(entity);
        }

        for (auto entity : to_remove) {
            registry.remove<PlayerDirtyTag>(entity);
        }
    }

    // =========================================================================
    // 3. CLEAR CHUNK CHANGED TAGS
    // =========================================================================
    {
        auto view = registry.view<PlayerChunkChangedTag>();
        std::vector<ecs::Entity> to_remove;
        for (auto entity : view) {
            to_remove.push_back(entity);
        }
        for (auto entity : to_remove) {
            registry.remove<PlayerChunkChangedTag>(entity);
        }
    }
}

}  // namespace ase::player
