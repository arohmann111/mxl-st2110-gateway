
#include "rivermax/rivermax_backend.hpp"
#include <rivermax_api.h>

#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace mxlgw::media
{
    namespace
    {
        class RivermaxBackend final : public MediaBackend
        {
        public:
            explicit RivermaxBackend(config::Config const& config)
                : _config(config)
            {
                if (_config.nic.portPairs.empty())
                {
                    throw std::runtime_error("nic.portPairs is empty");
                }
                if (_config.ptp.mode != config::PtpMode::External)
                {
                    throw std::runtime_error("nic.backend = rivermax requires "
                                             "ptp.mode = external");
                }

                auto const status = rmx_init();
                if (status != RMX_OK)
                {
                    std::ostringstream error;
                    error << "Rivermax init failed: " << rmx_status_to_string(status) << " (status " << status << ")";
                    throw std::runtime_error(error.str());
                }
                _rivermaxInitialized = true;

                auto const& primary = _config.nic.portPairs.front().primary;
                _status.backend = "rivermax";
                _status.ptpAvailable = false;
                _status.ports.push_back({.name = primary.name, .pci = primary.pci, .ifname = primary.ifname, .ip = primary.ip, .bindMode = "rivermax"});
            }

            ~RivermaxBackend() override
            {
                if (_rivermaxInitialized)
                {
                    (void)rmx_cleanup();
                }
            }

            std::string name() const override { return "rivermax"; }

            std::int64_t ptpTimeNs() const override { return hostTaiNs(); }

            BackendStatus status() const override { return _status; }

            std::unique_ptr<VideoRxSession> createVideoRx(VideoRxParams const&, VideoRxHandler&) override
            {
                throw std::runtime_error("Rivermax video receive is not implemented");
            }

            std::unique_ptr<VideoTxSession> createVideoTx(VideoTxParams const&) override
            {
                throw std::runtime_error("Rivermax video transmit is not implemented");
            }

            std::unique_ptr<AudioRxSession> createAudioRx(AudioParams const&) override
            {
                throw std::runtime_error("Rivermax audio receive is not implemented");
            }

            std::unique_ptr<AudioTxSession> createAudioTx(AudioParams const&) override
            {
                throw std::runtime_error("Rivermax audio transmit is not implemented");
            }

            std::unique_ptr<AncRxSession> createAncRx(AncParams const&) override { throw std::runtime_error("Rivermax ANC receive is not implemented"); }

            std::unique_ptr<AncTxSession> createAncTx(AncParams const&) override { throw std::runtime_error("Rivermax ANC transmit is not implemented"); }

        private:
            config::Config _config;
            BackendStatus _status;
            bool _rivermaxInitialized = false;
        };
    }

    std::unique_ptr<MediaBackend> createRivermaxBackend(config::Config const& config)
    {
        return std::make_unique<RivermaxBackend>(config);
    }
}