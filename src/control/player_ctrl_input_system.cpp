#include <ase/player/systems/control/player_ctrl_input_system.hpp>
#include <ase/player/components/state/player_st_id_component.hpp>
#include <ase/player/components/state/player_st_pos_component.hpp>
#include <ase/player/components/state/player_st_mov_component.hpp>
#include <ase/player/types.hpp>
#include <ase/input/input.hpp>
#include <ase/camera/camera.hpp>
#include <ase/network/components/player/state/network_plr_state_input_component.hpp>
#include <ase/math/spherical.hpp>
#include <ase/log/log.hpp>

#include <cmath>
#include <vector>

namespace ase::player {

using math::PI;
using math::TWO_PI;

namespace {

ecs::Entity find_player_by_id(ecs::Registry& registry, const std::string& player_id) {
    auto view = registry.view<PlayerStIdComponent>();
    for (auto [entity, identity] : view.each()) {
        if (identity.player_id == player_id) {
            return entity;
        }
    }
    return ecs::NullEntity;
}

void process_network_input(ecs::Registry& registry) {
    auto view = registry.view<network::NetworkPlrStateInputComponent>();
    std::vector<ecs::Entity> to_destroy;

    for (auto [input_entity, net_input] : view.each()) {
        std::string player_id(net_input.player_id.data());
        auto player_entity = find_player_by_id(registry, player_id);

        if (player_entity == ecs::NullEntity) {
            to_destroy.push_back(input_entity);
            continue;
        }

        auto* movement = registry.try_get<input::InputStateMoveComponent>(player_entity);
        if (movement) {
            movement->forward = net_input.forward;
            movement->strafe = net_input.strafe;
            movement->sprint = net_input.sprint;
            movement->jump = net_input.jump;
        }

        auto* inp_cam = registry.try_get<input::InputStateCameraComponent>(player_entity);
        if (inp_cam) {
            inp_cam->yaw_delta = net_input.camera_yaw_delta;
            inp_cam->pitch_delta = net_input.camera_pitch_delta;
            inp_cam->zoom_delta = net_input.camera_zoom_delta;
            inp_cam->orbit_mode = net_input.camera_orbit_only;
            inp_cam->mode_toggle = net_input.camera_mode_toggle;
        }

        auto* cam_input = registry.try_get<camera::CameraStInpComponent>(player_entity);
        if (cam_input) {
            cam_input->orbit_mode = net_input.camera_orbit_only;
        }

        auto* inp_meta = registry.try_get<input::InputStateMetaComponent>(player_entity);
        if (inp_meta) {
            inp_meta->camera_changed = (net_input.camera_yaw_delta != 0.0f ||
                                        net_input.camera_pitch_delta != 0.0f ||
                                        net_input.camera_zoom_delta != 0.0f ||
                                        net_input.camera_mode_toggle);
            inp_meta->movement_changed = (net_input.forward != 0.0f ||
                                          net_input.strafe != 0.0f);
            inp_meta->last_update = std::chrono::steady_clock::now();
        }

        to_destroy.push_back(input_entity);
    }

    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }
}

}  // anonymous namespace

void PlayerCtrlInputSystem::on_start(ecs::Registry& /*registry*/) {
}

void PlayerCtrlInputSystem::on_stop(ecs::Registry& registry) {
    auto view = registry.view<network::NetworkPlrStateInputComponent>();
    std::vector<ecs::Entity> to_destroy;

    for (auto entity : view) {
        to_destroy.push_back(entity);
    }

    for (auto entity : to_destroy) {
        registry.destroy(entity);
    }

    if (!to_destroy.empty()) {
        log::info("[PlayerCtrlInputSystem] Cleaned up {} pending network inputs on shutdown", to_destroy.size());
    }
}

void PlayerCtrlInputSystem::tick(ecs::Registry& registry, float dt) {
    process_network_input(registry);

    float turn_speed = MOVEMENT_DEFAULT_TURN_SPEED;

    auto mov_view = registry.view<PlayerStMovComponent>();
    for (auto [e, mov] : mov_view.each()) {
        turn_speed = mov.turn_speed;
        break;
    }

    auto view = registry.view<
        PlayerStIdComponent,
        PlayerStPosComponent,
        input::InputStateMoveComponent
    >();

    for (auto [entity, identity, pos, movement] : view.each()) {
        auto* cam_orient = registry.try_get<camera::CameraStOrtComponent>(entity);
        auto* cam_input = registry.try_get<camera::CameraStInpComponent>(entity);

        float movement_yaw = cam_orient ? cam_orient->movement_yaw : pos.yaw;
        bool is_moving = movement.forward != 0.0f || movement.strafe != 0.0f;
        bool orbit_mode = cam_input ? cam_input->orbit_mode : false;

        if (is_moving && cam_orient && !orbit_mode) {
            float delta = movement_yaw - pos.yaw;
            while (delta > PI) delta -= TWO_PI;
            while (delta < -PI) delta += TWO_PI;

            float max_turn = turn_speed * dt;
            if (std::abs(delta) < max_turn) {
                pos.yaw = movement_yaw;
            } else {
                pos.yaw += (delta > 0 ? max_turn : -max_turn);
            }

            while (pos.yaw < 0.0f) pos.yaw += TWO_PI;
            while (pos.yaw >= TWO_PI) pos.yaw -= TWO_PI;
        }

        identity.last_input = std::chrono::steady_clock::now();
    }
}

}  // namespace ase::player
