#include <ase/player/systems/network/player_net_bct_snd_system.hpp>
#include <ase/player/components/buffer/player_buf_bct_spn_component.hpp>
#include <ase/player/components/buffer/player_buf_bct_sta_component.hpp>
#include <ase/player/components/tag/player_tag_bct_spn_pnd_component.hpp>
#include <ase/player/components/tag/player_tag_bct_sta_pnd_component.hpp>
#include <ase/serial/serial.hpp>
#include <ase/replication/replication.hpp>
#include <ase/log/log.hpp>

#include <cstring>

namespace ase::player {

namespace {

// =============================================================================
// HELPER: Create broadcast message
// =============================================================================

void create_broadcast_message(
    ecs::Registry& registry,
    const char* channel,
    const char* payload,
    uint32_t payload_len
) {
    auto msg = registry.create();
    auto& dat = registry.emplace<replication::ReplicationMsgDatComponent>(msg);

    std::strncpy(dat.chn.data(), channel, dat.chn.size() - 1);
    dat.chn[dat.chn.size() - 1] = '\0';

    auto* pay = new char[payload_len];
    std::memcpy(pay, payload, payload_len);
    dat.pay_ptr = reinterpret_cast<uint64_t>(pay);
    dat.pay_len = payload_len;

    registry.emplace<replication::ReplicationMsgPndBctTag>(msg);
}

}  // anonymous namespace

// =============================================================================
// LIFECYCLE
// =============================================================================

void PlayerNetBctSndSystem::on_start(ecs::Registry& /*registry*/) {
    // No initialization needed
}

void PlayerNetBctSndSystem::on_stop(ecs::Registry& registry) {
    // Cleanup any pending serialization entities
    {
        auto view = registry.view<serial::SerialBufJsnComponent, PlayerBctSpnPndTag>();
        std::vector<ecs::Entity> to_destroy;

        for (auto [entity, buf] : view.each()) {
            if (buf.jsn_ptr != 0) {
                delete[] reinterpret_cast<char*>(buf.jsn_ptr);
            }
            if (buf.err_ptr != 0) {
                delete[] reinterpret_cast<char*>(buf.err_ptr);
            }
            to_destroy.push_back(entity);
        }

        for (auto entity : to_destroy) {
            registry.destroy(entity);
        }
    }

    {
        auto view = registry.view<serial::SerialBufJsnComponent, PlayerBctStaPndTag>();
        std::vector<ecs::Entity> to_destroy;

        for (auto [entity, buf] : view.each()) {
            if (buf.jsn_ptr != 0) {
                delete[] reinterpret_cast<char*>(buf.jsn_ptr);
            }
            if (buf.err_ptr != 0) {
                delete[] reinterpret_cast<char*>(buf.err_ptr);
            }
            to_destroy.push_back(entity);
        }

        for (auto entity : to_destroy) {
            registry.destroy(entity);
        }
    }
}

// =============================================================================
// TICK
// =============================================================================

void PlayerNetBctSndSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // =========================================================================
    // 1. PROCESS COMPLETED SPAWN BROADCASTS
    // =========================================================================
    {
        auto view = registry.view<
            serial::SerialJsnFinTag,
            serial::SerialBufJsnComponent,
            PlayerBctSpnPndTag
        >();

        std::vector<ecs::Entity> to_destroy;

        for (auto [entity, buf] : view.each()) {
            if (buf.jsn_ptr != 0 && buf.jsn_len > 0) {
                const char* jsn = reinterpret_cast<const char*>(buf.jsn_ptr);
                create_broadcast_message(registry, "player_spawn", jsn, buf.jsn_len);
                log::debug("[PlayerNetBctSndSystem] Broadcast spawn message ({} bytes)", buf.jsn_len);

                // Cleanup JSON buffer
                delete[] jsn;
            }

            to_destroy.push_back(entity);
        }

        for (auto entity : to_destroy) {
            registry.destroy(entity);
        }
    }

    // =========================================================================
    // 2. PROCESS COMPLETED STATE BROADCASTS
    // =========================================================================
    {
        auto view = registry.view<
            serial::SerialJsnFinTag,
            serial::SerialBufJsnComponent,
            PlayerBctStaPndTag
        >();

        std::vector<ecs::Entity> to_destroy;

        for (auto [entity, buf] : view.each()) {
            if (buf.jsn_ptr != 0 && buf.jsn_len > 0) {
                const char* jsn = reinterpret_cast<const char*>(buf.jsn_ptr);
                create_broadcast_message(registry, "player_state", jsn, buf.jsn_len);

                // Cleanup JSON buffer
                delete[] jsn;
            }

            to_destroy.push_back(entity);
        }

        for (auto entity : to_destroy) {
            registry.destroy(entity);
        }
    }

    // =========================================================================
    // 3. HANDLE SERIALIZATION ERRORS
    // =========================================================================
    {
        auto view = registry.view<
            serial::SerialErrTag,
            serial::SerialBufJsnComponent
        >();

        std::vector<ecs::Entity> to_destroy;

        for (auto [entity, buf] : view.each()) {
            if (buf.err_ptr != 0) {
                const char* err = reinterpret_cast<const char*>(buf.err_ptr);
                log::error("[PlayerNetBctSndSystem] Serialization error: {}", err);
                delete[] err;
            }
            to_destroy.push_back(entity);
        }

        for (auto entity : to_destroy) {
            registry.destroy(entity);
        }
    }
}

}  // namespace ase::player
