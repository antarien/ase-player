#include <ase/player/systems/network/player_bct_snd_system.hpp>
#include <ase/player/components/buffer/player_buf_bct_spn_component.hpp>
#include <ase/player/components/buffer/player_buf_bct_sta_component.hpp>
#include <ase/player/components/tag/player_tag_bct_spn_pnd_component.hpp>
#include <ase/player/components/tag/player_tag_bct_sta_pnd_component.hpp>
#include <ase/player/types.hpp>

#include <ase/replication/replication.hpp>
#include <cstring>

namespace ase::player {

namespace {

struct PlayerBctSpnSntTag {};
struct PlayerBctStaSntTag {};

}  // namespace

void PlayerBctSndSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Process spawn broadcasts
    {
        auto view = registry.view<serial::SerialJsnFinTag,
                                  serial::SerialBufJsnComponent,
                                  PlayerBufBctSpnComponent,
                                  PlayerBctSpnPndTag>(
            entt::exclude<PlayerBctSpnSntTag>
        );

        for (auto [entity, jsn_buf, spn_buf] : view.each()) {
            if (jsn_buf.st != serial::SERIAL_ST_FIN || jsn_buf.jsn_ptr == 0) {
                registry.remove<serial::SerialJsnFinTag>(entity);
                registry.remove<serial::SerialBufJsnComponent>(entity);
                registry.remove<PlayerBctSpnPndTag>(entity);
                continue;
            }

            auto& msg = registry.emplace<replication::ReplicationMsgDatComponent>(entity);
            std::strncpy(msg.chn.data(), CHANNEL_PLR_SPN, msg.chn.size() - 1);
            msg.chn[msg.chn.size() - 1] = '\0';
            msg.pay_ptr = jsn_buf.jsn_ptr;
            msg.pay_len = jsn_buf.jsn_len;

            registry.emplace<replication::ReplicationMsgPndBctTag>(entity);
            registry.emplace<PlayerBctSpnSntTag>(entity);

            registry.remove<serial::SerialJsnFinTag>(entity);
            registry.remove<PlayerBctSpnPndTag>(entity);
        }
    }

    // Process state broadcasts
    {
        auto view = registry.view<serial::SerialJsnFinTag,
                                  serial::SerialBufJsnComponent,
                                  PlayerBufBctStaComponent,
                                  PlayerBctStaPndTag>(
            entt::exclude<PlayerBctStaSntTag>
        );

        for (auto [entity, jsn_buf, sta_buf] : view.each()) {
            if (jsn_buf.st != serial::SERIAL_ST_FIN || jsn_buf.jsn_ptr == 0) {
                registry.remove<serial::SerialJsnFinTag>(entity);
                registry.remove<serial::SerialBufJsnComponent>(entity);
                registry.remove<PlayerBctStaPndTag>(entity);
                continue;
            }

            auto& msg = registry.emplace<replication::ReplicationMsgDatComponent>(entity);
            std::strncpy(msg.chn.data(), CHANNEL_PLR_STA, msg.chn.size() - 1);
            msg.chn[msg.chn.size() - 1] = '\0';
            msg.pay_ptr = jsn_buf.jsn_ptr;
            msg.pay_len = jsn_buf.jsn_len;

            registry.emplace<replication::ReplicationMsgPndBctTag>(entity);
            registry.emplace<PlayerBctStaSntTag>(entity);

            registry.remove<serial::SerialJsnFinTag>(entity);
            registry.remove<PlayerBctStaPndTag>(entity);
        }
    }
}

}  // namespace ase::player
