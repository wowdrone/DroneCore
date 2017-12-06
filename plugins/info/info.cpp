#include "info.h"
#include "info_impl.h"

namespace dronecore {

Info::Info(Device *device) :
    PluginBase()
{
    _impl = new InfoImpl(device);
    _impl->init();
    _impl->enable();
}

Info::~Info()
{
    _impl->disable();
    _impl->deinit();
    delete _impl;
}

uint64_t Info::uuid() const
{
    return _impl->get_uuid();
}

bool Info::is_complete() const
{
    return _impl->is_complete();
}

Info::Version Info::get_version() const
{
    return _impl->get_version();
}

Info::Product Info::get_product() const
{
    return _impl->get_product();
}

} // namespace dronecore
