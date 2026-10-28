#pragma once

#include <memory>

#include "config/config.hpp"
#include "mtl/backend.hpp"

namespace mxlgw::media
{
    std::unique_ptr<MediaBackend> createRivermaxBackend(config::Config const& config);
}