#include <kernel/device_module.h>
#include <kernel/error.h>

namespace kernel {

device_module::~device_module()
{
    if (_module_handle != NULL)
    {
        while (_device_ptr.use_count() != 1);
        _device_ptr.reset();
        FreeLibrary(_module_handle);
        _module_handle = NULL;
    }
}

device_module::device_module(const char* module_name) throw(...):
_device_ptr(nullptr),
_module_name("null"),
_module_handle(NULL),
_key(0)
{
    HMODULE module_handle = NULL;
    module_handle = LoadLibraryA(module_name);
    if (module_handle == NULL)
    {
        throw vee::exception("Load dll failure!", (int)error_code::load_dll_failure);
    }
    using entry_point = opennui_device_ptr(__stdcall*)();
    entry_point ep = (entry_point)GetProcAddress(module_handle, "on_load");
    if (ep == NULL)
    {
        throw vee::exception("Could not call the module entry point!", (int)error_code::could_not_call_module_entry_point);
    }
    _device_ptr = ep();
    _module_name = module_name;
    _module_handle = module_handle;
    ::std::string name(module_name);
    ::std::hash<::std::string> hash;
    _key = hash(name);

    _device_ptr->get_color_frame_info(_color_frame_info);
    _device_ptr->get_depth_frame_info(_depth_frame_info);
    _device_ptr->get_body_tracking_info(_body_frame_info);
}

device_module::device_module(device_module&& other):
_device_ptr(std::move(other._device_ptr)),
_module_name(std::move(other._module_name)),
_module_handle(std::move(other._module_handle))
{
    other._module_handle = NULL;
}

device_module& device_module::operator=(device_module&& other)
{
    _device_ptr = std::move(other._device_ptr);
    _module_name = std::move(other._module_name);
    _module_handle = std::move(other._module_handle);
    other._module_handle = NULL;
    return *this;
}

} // namespace kernel