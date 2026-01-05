/**
 * PlayerHubPosSystem - Write player positions to Hub
 *
 * PLAN_ASE_SDK_V2: L4 plugins MUST read via Hub, not direct components
 * Uses owner = entity ID, so plugins can iterate and find positions
 */

#include <ase/player/systems/hub/player_hub_pos_system.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/hub/hub.hpp>

namespace ase::player {
using namespace entt::literals;

void PlayerHubPosSystem::on_start(ecs::Registry& /*registry*/) {}
void PlayerHubPosSystem::on_stop(ecs::Registry& /*registry*/) {}

void PlayerHubPosSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // PlayerStIdComponent identifies player entities
    auto view = registry.view<PlayerStIdComponent, PlayerStPosComponent>();

    for (auto [entity, id, pos] : view.each()) {
        uint32_t owner = static_cast<uint32_t>(entity);

        // Write position to hub (create or update)
        hub::set_or_create_hub_value(registry, owner, "PLR_POS_X"_hs, pos.x);
        hub::set_or_create_hub_value(registry, owner, "PLR_POS_Y"_hs, pos.y);
        hub::set_or_create_hub_value(registry, owner, "PLR_POS_Z"_hs, pos.z);
        hub::set_or_create_hub_value(registry, owner, "PLR_ENTITY_ID"_hs, static_cast<float>(owner));
        hub::set_or_create_hub_value(registry, owner, "PLR_IS_PLAYER"_hs, 1.0f);  // Mark as player
    }
}

}  // namespace ase::player
