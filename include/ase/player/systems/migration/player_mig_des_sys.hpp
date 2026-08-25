#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        player_mig_des_sys.hpp
 * @brief       PlayerMigDesSystem - deserialize migrated-in PlayerSnaps and honour migrate acks
 * @description WS-H.4 deserializer half, spiegelbildlich zu PlayerMigSerSystem — und der
 *              Schlussstein der Sterntrennung ase-world/ase-player: bis 2026-08-19 packte
 *              ase-world das eingefrorene 42-Byte-Bild SELBST aus und schrieb sieben fremde
 *              Zeilen (serialisieren tat der Eigentuemer, deserialisieren der Fremde). Jetzt
 *              stellt ase-world nur noch Zustellungen auf den Stern (HubPlrMigSnapComponent /
 *              HubPlrMigAckComponent + HubPlrMigPndTag), und DIESES System baut die Figur mit
 *              den eigenen Typen: epoch-idempotentes Anlegen bzw. In-Place-Update aus dem
 *              Snap, Abmeldebitte (PlayerReqDespComponent) aus der Quittung. Verbrauchte
 *              Zustellungen tragen HubPlrMigFinTag; die leere Huelle raeumt ihr Erzeuger ab.
 *
 * @module      ase-player
 * @layer       3 (Module)
 * @category    entity/actor/player
 * @schedule    Reception
 * @created     2026-08-19
 * @modified    2026-08-19
 * @version     1.0.0
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants → types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::player {

/**
 * @brief Drains the star deliveries of the migrate seam: instantiates or updates the figure
 *        from a snap delivery, files the module-own despawn request from an ack delivery.
 *
 * Processes (Reception, one delivery = one figure event):
 *   each (HubPlrMigSnapComponent, HubPlrMigPndTag) → validate snap_len + schema →
 *     O(1) player_ref lookup → epoch-idempotent instantiate/update → HubPlrMigFinTag
 *   each (HubPlrMigAckComponent, HubPlrMigPndTag) → O(1) player_ref lookup →
 *     PlayerReqDespComponent from the OWN identity row → HubPlrMigFinTag
 *   each (HubPlrMigFinTag) → consume the delivery rows (remove), hull left for its creator
 *
 * @schedule Reception
 * @reads    hub::HubPlrMigSnapComponent, hub::HubPlrMigAckComponent, hub::HubPlrMigPndTag,
 *           PlayerStaEpchComponent (lookup), PlayerStaIdntComponent (despawn identity)
 * @writes   PlayerSta* rows of the instantiated figure, PlayerSpndTag, PlayerReqDespComponent,
 *           hub::HubPlrMigFinTag (receipt), delivery rows consumed
 */
class PlayerMigDesSystem : public ecs::System {
public:
    const char* name() const override { return "PlayerMigDesSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::player
