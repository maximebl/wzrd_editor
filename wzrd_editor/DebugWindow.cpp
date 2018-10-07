#include "pch.h"
#include "App.h"
#include "DebugWindow.h"
#include "GraphicsResources.h"

class ViewProvider final : public winrt::implements<ViewProvider, winrt::Windows::ApplicationModel::Core::IFrameworkView>
{
public:
	ViewProvider() :
		m_visible(true)
	{
	}

	// IFrameworkView methods
	void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const & applicationView)
	{
		applicationView.Activated({ this, &ViewProvider::OnActivated });
		m_graphics_resources.enable_debug_layer();
		m_graphics_resources.create_factory();
		m_graphics_resources.create_device();
		m_graphics_resources.create_fence();
		m_graphics_resources.create_cmd_objects();
		m_graphics_resources.create_descriptor_heaps();
		m_graphics_resources.create_depthstencil_buffer();
	}

	void Uninitialize()
	{
	}

	void SetWindow(winrt::Windows::UI::Core::CoreWindow const & window)
	{
		m_window_debug = window;
		m_graphics_resources.create_vertex_colored_box_geometry();
		m_graphics_resources.create_swapchain_corewindow(m_window_debug);
		m_graphics_resources.create_render_targets();
		m_graphics_resources.create_constant_buffers();
		m_graphics_resources.create_rootsignature();
		m_running = true;
	}

	void Run()
	{
		m_timer.Reset();
		while (m_running)
		{
			if (m_visible)
			{
				m_window_debug.Dispatcher().ProcessEvents(winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);
				m_timer.Tick();

				//m_graphics_resources.update();
				//m_graphics_resources.render();
			}
			else
			{
				m_window_debug.Dispatcher().ProcessEvents(winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessOneAndAllPending);
			}
		}
	}

	void Load(winrt::hstring const &)
	{
	}
protected:
	void OnActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const & /*applicationView*/, winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const & args)
	{
		winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Activate();
	}

private:
	bool m_visible = false;
	bool m_running = false;
	GameTimer m_timer;
	GraphicsResources m_graphics_resources;
	winrt::Windows::UI::Core::CoreWindow m_window_debug = nullptr;
};

class ViewProviderFactory final : public winrt::implements<ViewProviderFactory, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource>
{
public:
	winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView()
	{
		return winrt::make<ViewProvider>();
	}
};

DebugWindow::DebugWindow()
{
	ViewProviderFactory viewProviderFactory;
	winrt::Windows::ApplicationModel::Core::CoreApplication::Run(viewProviderFactory);
}

DebugWindow::~DebugWindow()
{
}

void print_context()
{
	APTTYPE type;
	APTTYPEQUALIFIER qualifier;
	HRESULT const result = CoGetApartmentType(&type, &qualifier);

	auto res = type == APTTYPE_MTA ? "MTA" : "STA";
	std::string apptType(res);
	auto rez = apptType + "\n";
	OutputDebugStringA(rez.c_str());
}

winrt::Windows::Foundation::IAsyncAction init()
{
	co_await winrt::resume_background();
	DebugWindow debugWindow;
	//winrt::Windows::UI::Xaml::Application::Start(
	//	[](auto &&)
	//{
	//	print_context();
	//	winrt::make<winrt::wzrd_editor::implementation::App>();
	//});
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	//init();
	winrt::Windows::UI::Xaml::Application::Start(
		[](auto &&)
	{
		winrt::make<winrt::wzrd_editor::implementation::App>();
	});
	return 0;
}