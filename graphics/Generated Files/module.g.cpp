﻿// WARNING: Please don't edit this file. It was generated by C++/WinRT v1.0.180821.2

#include "pch.h"
#include "buffer.h"
#include "renderer.h"
#include "shader.h"
#include "vertex.h"

int32_t WINRT_CALL WINRT_CanUnloadNow() noexcept
{
#ifdef _WRL_MODULE_H_
    if (!::Microsoft::WRL::Module<::Microsoft::WRL::InProc>::GetModule().Terminate())
    {
        return 1; // S_FALSE
    }
#endif

    if (winrt::get_module_lock())
    {
        return 1; // S_FALSE
    }

    winrt::clear_factory_cache();
    return 0; // S_OK
}

int32_t WINRT_CALL WINRT_GetActivationFactory(void* classId, void** factory) noexcept
{
    try
    {
        *factory = nullptr;
        uint32_t length{};
        wchar_t const* const buffer = WINRT_WindowsGetStringRawBuffer(classId, &length);
        std::wstring_view const name{ buffer, length };

        auto requal = [](std::wstring_view const& left, std::wstring_view const& right) noexcept
        {
            return std::equal(left.rbegin(), left.rend(), right.rbegin(), right.rend());
        };

        if (requal(name, L"graphics.buffer"))
        {
            *factory = winrt::detach_abi(winrt::make<winrt::graphics::factory_implementation::buffer>());
            return 0;
        }

        if (requal(name, L"graphics.renderer"))
        {
            *factory = winrt::detach_abi(winrt::make<winrt::graphics::factory_implementation::renderer>());
            return 0;
        }

        if (requal(name, L"graphics.shader"))
        {
            *factory = winrt::detach_abi(winrt::make<winrt::graphics::factory_implementation::shader>());
            return 0;
        }

        if (requal(name, L"graphics.vertex"))
        {
            *factory = winrt::detach_abi(winrt::make<winrt::graphics::factory_implementation::vertex>());
            return 0;
        }

#ifdef _WRL_MODULE_H_
        return ::Microsoft::WRL::Module<::Microsoft::WRL::InProc>::GetModule().GetActivationFactory(static_cast<HSTRING>(classId), reinterpret_cast<::IActivationFactory**>(factory));
#else
        return winrt::hresult_class_not_available(name).to_abi();
#endif
    }
    catch (...) { return winrt::to_hresult(); }
}
