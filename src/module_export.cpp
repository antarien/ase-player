/**
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 */

#include <ase/ecs/plugin_interface.hpp>
#include <ase/player/player_module.hpp>

namespace {

AseModuleInfo get_info() {
    return {"ase-player", ase::player::PlayerModule::version(), ASE_API_VERSION, 3};
}

uint32_t on_load(AseLoadContext* ctx) {
    ase::player::PlayerModule instance;
    instance.build(*ctx->app);
    return ASE_LOAD_OK;
}

uint32_t on_unload(AseLoadContext*) { return ASE_LOAD_OK; }

void on_config_changed(AseLoadContext*, const char*, const char*) {}

void cleanup() {}

}  // anonymous namespace

ASE_MODULE_EXPORT(get_info, on_load, on_unload, on_config_changed, cleanup)
