﻿// WARNING: Please don't edit this file. It was generated by C++/WinRT v1.0.180821.2

#pragma once

WINRT_EXPORT namespace winrt::Windows::UI::Xaml::Controls {

struct SwapChainPanel;

}

WINRT_EXPORT namespace winrt::Windows::UI::Xaml::Data {

struct PropertyChangedEventHandler;

}

WINRT_EXPORT namespace winrt::graphics {

enum class buffer_type : int32_t
{
    dynamic_buffer = 0,
    static_buffer = 1,
};

enum class primitive_types : int32_t
{
    points = 0,
    triangle_list = 1,
    triangle_strips = 2,
    line_lists = 3,
    line_strips = 4,
};

enum class shader_type : int32_t
{
    vertex = 0,
    pixel = 1,
};

struct Ibuffer;
struct IbufferFactory;
struct Irenderer;
struct Ishader;
struct IshaderFactory;
struct Ivertex;
struct IvertexFactory;
struct buffer;
struct renderer;
struct shader;
struct vertex;
struct compilation_result;
struct view;

}

namespace winrt::impl {

template <> struct category<graphics::Ibuffer>{ using type = interface_category; };
template <> struct category<graphics::IbufferFactory>{ using type = interface_category; };
template <> struct category<graphics::Irenderer>{ using type = interface_category; };
template <> struct category<graphics::Ishader>{ using type = interface_category; };
template <> struct category<graphics::IshaderFactory>{ using type = interface_category; };
template <> struct category<graphics::Ivertex>{ using type = interface_category; };
template <> struct category<graphics::IvertexFactory>{ using type = interface_category; };
template <> struct category<graphics::buffer>{ using type = class_category; };
template <> struct category<graphics::renderer>{ using type = class_category; };
template <> struct category<graphics::shader>{ using type = class_category; };
template <> struct category<graphics::vertex>{ using type = class_category; };
template <> struct category<graphics::buffer_type>{ using type = enum_category; };
template <> struct category<graphics::primitive_types>{ using type = enum_category; };
template <> struct category<graphics::shader_type>{ using type = enum_category; };
template <> struct category<graphics::compilation_result>{ using type = struct_category<bool,hstring>; };
template <> struct category<graphics::view>{ using type = struct_category<uint64_t,uint32_t,uint32_t>; };
template <> struct name<graphics::Ibuffer>{ static constexpr auto & value{ L"graphics.Ibuffer" }; };
template <> struct name<graphics::IbufferFactory>{ static constexpr auto & value{ L"graphics.IbufferFactory" }; };
template <> struct name<graphics::Irenderer>{ static constexpr auto & value{ L"graphics.Irenderer" }; };
template <> struct name<graphics::Ishader>{ static constexpr auto & value{ L"graphics.Ishader" }; };
template <> struct name<graphics::IshaderFactory>{ static constexpr auto & value{ L"graphics.IshaderFactory" }; };
template <> struct name<graphics::Ivertex>{ static constexpr auto & value{ L"graphics.Ivertex" }; };
template <> struct name<graphics::IvertexFactory>{ static constexpr auto & value{ L"graphics.IvertexFactory" }; };
template <> struct name<graphics::buffer>{ static constexpr auto & value{ L"graphics.buffer" }; };
template <> struct name<graphics::renderer>{ static constexpr auto & value{ L"graphics.renderer" }; };
template <> struct name<graphics::shader>{ static constexpr auto & value{ L"graphics.shader" }; };
template <> struct name<graphics::vertex>{ static constexpr auto & value{ L"graphics.vertex" }; };
template <> struct name<graphics::buffer_type>{ static constexpr auto & value{ L"graphics.buffer_type" }; };
template <> struct name<graphics::primitive_types>{ static constexpr auto & value{ L"graphics.primitive_types" }; };
template <> struct name<graphics::shader_type>{ static constexpr auto & value{ L"graphics.shader_type" }; };
template <> struct name<graphics::compilation_result>{ static constexpr auto & value{ L"graphics.compilation_result" }; };
template <> struct name<graphics::view>{ static constexpr auto & value{ L"graphics.view" }; };
template <> struct guid_storage<graphics::Ibuffer>{ static constexpr guid value{ 0xC40F195C,0xDB97,0x5E9E,{ 0xAD,0x2C,0x90,0x5B,0x20,0x31,0x2D,0xFA } }; };
template <> struct guid_storage<graphics::IbufferFactory>{ static constexpr guid value{ 0xF3D669E1,0x5F70,0x5F2C,{ 0xA1,0x52,0x82,0xF3,0x19,0x89,0xC5,0x66 } }; };
template <> struct guid_storage<graphics::Irenderer>{ static constexpr guid value{ 0xCD38E630,0xDB6F,0x5277,{ 0x85,0x2D,0xDB,0x51,0xDC,0xF4,0x4C,0x71 } }; };
template <> struct guid_storage<graphics::Ishader>{ static constexpr guid value{ 0xD5907D07,0x2DE4,0x537C,{ 0x92,0xD8,0xEA,0x3C,0x66,0xA0,0x35,0xC7 } }; };
template <> struct guid_storage<graphics::IshaderFactory>{ static constexpr guid value{ 0xE76F4BFC,0x6A2E,0x5F53,{ 0xAA,0x4E,0xF0,0xA8,0x9F,0x1D,0x37,0x75 } }; };
template <> struct guid_storage<graphics::Ivertex>{ static constexpr guid value{ 0x46A070D6,0x07F9,0x5B89,{ 0xA9,0x3E,0xCD,0x8D,0x8D,0xD0,0x43,0xC3 } }; };
template <> struct guid_storage<graphics::IvertexFactory>{ static constexpr guid value{ 0x3D0E8F40,0x3967,0x507D,{ 0xBE,0x98,0xBE,0x44,0xE9,0x03,0x29,0x4C } }; };
template <> struct default_interface<graphics::buffer>{ using type = graphics::Ibuffer; };
template <> struct default_interface<graphics::renderer>{ using type = graphics::Irenderer; };
template <> struct default_interface<graphics::shader>{ using type = graphics::Ishader; };
template <> struct default_interface<graphics::vertex>{ using type = graphics::Ivertex; };

template <> struct abi<graphics::Ibuffer>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL get_view(struct struct_graphics_view* result) noexcept = 0;
    virtual int32_t WINRT_CALL get_capacity_percentage(int32_t* result) noexcept = 0;
    virtual int32_t WINRT_CALL add_to_view(void* new_vertex) noexcept = 0;
    virtual int32_t WINRT_CALL clear() noexcept = 0;
    virtual int32_t WINRT_CALL get_max_size(int32_t* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_max_size(int32_t value) noexcept = 0;
    virtual int32_t WINRT_CALL get_current_size(int32_t* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_current_size(int32_t value) noexcept = 0;
    virtual int32_t WINRT_CALL get_resize_increment(int32_t* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_resize_increment(int32_t value) noexcept = 0;
    virtual int32_t WINRT_CALL get_is_auto_resize(bool* value) noexcept = 0;
    virtual int32_t WINRT_CALL get_is_buffer_full(bool* value) noexcept = 0;
};};

template <> struct abi<graphics::IbufferFactory>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL CreateInstance(graphics::buffer_type type, void* initial_data, int32_t max_size, int32_t resize_increment, bool is_auto_resize, void** value) noexcept = 0;
};};

template <> struct abi<graphics::Irenderer>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL enable_debug_layer() noexcept = 0;
    virtual int32_t WINRT_CALL initialize(void* target_swapchain) noexcept = 0;
    virtual int32_t WINRT_CALL start_render_loop() noexcept = 0;
    virtual int32_t WINRT_CALL stop_render_loop() noexcept = 0;
    virtual int32_t WINRT_CALL get_is_rendering(bool* value) noexcept = 0;
    virtual int32_t WINRT_CALL get_current_topology(graphics::primitive_types* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_current_topology(graphics::primitive_types value) noexcept = 0;
    virtual int32_t WINRT_CALL pick_and_compile_shader(void* shader_name, void* entry_point, void* version, void** operation) noexcept = 0;
    virtual int32_t WINRT_CALL get_current_buffer(void** value) noexcept = 0;
    virtual int32_t WINRT_CALL put_current_buffer(void* value) noexcept = 0;
};};

template <> struct abi<graphics::Ishader>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL get_shader_name(void** value) noexcept = 0;
    virtual int32_t WINRT_CALL put_shader_name(void* value) noexcept = 0;
    virtual int32_t WINRT_CALL get_type_glyph(void** value) noexcept = 0;
    virtual int32_t WINRT_CALL put_type_glyph(void* value) noexcept = 0;
    virtual int32_t WINRT_CALL get_is_loading(bool* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_is_loading(bool value) noexcept = 0;
    virtual int32_t WINRT_CALL get_is_error(bool* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_is_error(bool value) noexcept = 0;
    virtual int32_t WINRT_CALL get_shader_type(graphics::shader_type* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_shader_type(graphics::shader_type value) noexcept = 0;
};};

template <> struct abi<graphics::IshaderFactory>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL CreateInstance(void* name, graphics::shader_type type, void** value) noexcept = 0;
};};

template <> struct abi<graphics::Ivertex>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL get_x(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_x(float value) noexcept = 0;
    virtual int32_t WINRT_CALL get_y(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_y(float value) noexcept = 0;
    virtual int32_t WINRT_CALL get_z(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_z(float value) noexcept = 0;
    virtual int32_t WINRT_CALL get_r(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_r(float value) noexcept = 0;
    virtual int32_t WINRT_CALL get_g(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_g(float value) noexcept = 0;
    virtual int32_t WINRT_CALL get_b(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_b(float value) noexcept = 0;
    virtual int32_t WINRT_CALL get_a(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_a(float value) noexcept = 0;
    virtual int32_t WINRT_CALL get_u(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_u(float value) noexcept = 0;
    virtual int32_t WINRT_CALL get_v(float* value) noexcept = 0;
    virtual int32_t WINRT_CALL put_v(float value) noexcept = 0;
};};

template <> struct abi<graphics::IvertexFactory>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL CreateInstance(float x, float y, float z, float r, float g, float b, float a, float u, float v, void** value) noexcept = 0;
};};

template <typename D>
struct consume_graphics_Ibuffer
{
    graphics::view get_view() const;
    int32_t get_capacity_percentage() const;
    void add_to_view(graphics::vertex const& new_vertex) const;
    void clear() const;
    int32_t max_size() const;
    void max_size(int32_t value) const;
    int32_t current_size() const;
    void current_size(int32_t value) const;
    int32_t resize_increment() const;
    void resize_increment(int32_t value) const;
    bool is_auto_resize() const;
    bool is_buffer_full() const;
};
template <> struct consume<graphics::Ibuffer> { template <typename D> using type = consume_graphics_Ibuffer<D>; };

template <typename D>
struct consume_graphics_IbufferFactory
{
    graphics::buffer CreateInstance(graphics::buffer_type const& type, Windows::Foundation::Collections::IObservableVector<graphics::vertex> const& initial_data, int32_t max_size, int32_t resize_increment, bool is_auto_resize) const;
};
template <> struct consume<graphics::IbufferFactory> { template <typename D> using type = consume_graphics_IbufferFactory<D>; };

template <typename D>
struct consume_graphics_Irenderer
{
    void enable_debug_layer() const;
    void initialize(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain) const;
    void start_render_loop() const;
    void stop_render_loop() const;
    bool is_rendering() const;
    graphics::primitive_types current_topology() const;
    void current_topology(graphics::primitive_types const& value) const;
    Windows::Foundation::IAsyncOperation<graphics::compilation_result> pick_and_compile_shader(param::hstring const& shader_name, param::hstring const& entry_point, param::hstring const& version) const;
    graphics::buffer current_buffer() const;
    void current_buffer(graphics::buffer const& value) const;
};
template <> struct consume<graphics::Irenderer> { template <typename D> using type = consume_graphics_Irenderer<D>; };

template <typename D>
struct consume_graphics_Ishader
{
    hstring shader_name() const;
    void shader_name(param::hstring const& value) const;
    hstring type_glyph() const;
    void type_glyph(param::hstring const& value) const;
    bool is_loading() const;
    void is_loading(bool value) const;
    bool is_error() const;
    void is_error(bool value) const;
    graphics::shader_type shader_type() const;
    void shader_type(graphics::shader_type const& value) const;
};
template <> struct consume<graphics::Ishader> { template <typename D> using type = consume_graphics_Ishader<D>; };

template <typename D>
struct consume_graphics_IshaderFactory
{
    graphics::shader CreateInstance(param::hstring const& name, graphics::shader_type const& type) const;
};
template <> struct consume<graphics::IshaderFactory> { template <typename D> using type = consume_graphics_IshaderFactory<D>; };

template <typename D>
struct consume_graphics_Ivertex
{
    float x() const;
    void x(float value) const;
    float y() const;
    void y(float value) const;
    float z() const;
    void z(float value) const;
    float r() const;
    void r(float value) const;
    float g() const;
    void g(float value) const;
    float b() const;
    void b(float value) const;
    float a() const;
    void a(float value) const;
    float u() const;
    void u(float value) const;
    float v() const;
    void v(float value) const;
};
template <> struct consume<graphics::Ivertex> { template <typename D> using type = consume_graphics_Ivertex<D>; };

template <typename D>
struct consume_graphics_IvertexFactory
{
    graphics::vertex CreateInstance(float x, float y, float z, float r, float g, float b, float a, float u, float v) const;
};
template <> struct consume<graphics::IvertexFactory> { template <typename D> using type = consume_graphics_IvertexFactory<D>; };

struct struct_graphics_compilation_result
{
    bool is_success;
    void* error_message;
};
template <> struct abi<graphics::compilation_result>{ using type = struct_graphics_compilation_result; };


struct struct_graphics_view
{
    uint64_t buffer_location;
    uint32_t size_in_bytes;
    uint32_t stride_in_bytes;
};
template <> struct abi<graphics::view>{ using type = struct_graphics_view; };


}
