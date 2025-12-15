#pragma once

#include <cstdint>
#include <string>
#include <cmath>

namespace ase::player {

/**
 * 3D Vector for position, velocity, etc.
 */
struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    [[nodiscard]] float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    [[nodiscard]] float length_xz() const {
        return std::sqrt(x * x + z * z);
    }

    [[nodiscard]] Vec3 normalized() const {
        float len = length();
        if (len < 0.0001f) return {0, 0, 0};
        return {x / len, y / len, z / len};
    }

    [[nodiscard]] Vec3 normalized_xz() const {
        float len = length_xz();
        if (len < 0.0001f) return {0, 0, 0};
        return {x / len, 0, z / len};
    }
};

/**
 * Movement input state from client
 */
struct MovementInput {
    float forward = 0.0f;   // -1 to 1 (W/S)
    float strafe = 0.0f;    // -1 to 1 (A/D)
    float yaw = 0.0f;       // Camera rotation in radians
    bool sprint = false;    // Shift held
    bool jump = false;      // Space pressed
};

/**
 * Player state flags
 */
enum class PlayerState : uint8_t {
    Idle = 0,
    Walking = 1,
    Running = 2,
    Jumping = 3,
    Falling = 4,
    Swimming = 5,
    Dead = 6
};

/**
 * Movement constants
 */
struct MovementConfig {
    float walk_speed = 4.0f;        // m/s
    float run_speed = 8.0f;         // m/s
    float jump_velocity = 5.0f;     // m/s
    float gravity = 9.81f;          // m/s^2
    float ground_friction = 10.0f;  // Deceleration factor
    float air_control = 0.3f;       // Air movement multiplier
};

}  // namespace ase::player
