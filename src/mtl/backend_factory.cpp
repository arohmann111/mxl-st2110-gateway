// SPDX-License-Identifier: MIT
#include "mtl/backend_factory.hpp"

#include <stdexcept>

#include "mtl/mock_backend.hpp"

#if defined(MXLGW_HAVE_MTL)
#include "mtl/mtl_backend.hpp"
#endif
#if defined(MXLGW_HAVE_RIVERMAX)
#include "rivermax/rivermax_backend.hpp"
#endif

namespace mxlgw::media
{
    bool mtlAvailable()
    {
#if defined(MXLGW_HAVE_MTL)
        return true;
#else
        return false;
#endif
    }

    std::unique_ptr<MediaBackend> createBackend(config::Config const& config)
    {
        if (config.nic.backend == config::Backend::Mock)
        {
            return createMockBackend(config);
        }
        if (config.nic.backend == config::Backend::Rivermax)
        {
#if defined(MXLGW_HAVE_RIVERMAX)
                return createRivermaxBackend(config);
#else
                throw std::runtime_error(
                        "nic.backend = rivermax needs Rivermax support, "
                        "but this build has no Rivermax support");
#endif
        }
#if defined(MXLGW_HAVE_MTL)
        return createMtlBackend(config);
#else
        throw std::runtime_error(std::string("nic.backend = ") + config::toName(config.nic.backend) + " needs MTL, but this build has no MTL support");
#endif
    }
}
