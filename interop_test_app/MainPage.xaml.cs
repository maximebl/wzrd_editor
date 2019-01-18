using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using graphics;
using System.Threading.Tasks;

namespace interop_test_app
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            //renderer.enable_debug_layer();
            //renderer.initialize(swapChainPanel);
            //renderer.start_render_loop();
            test_abi_calls();
        }

        async Task test_abi_calls()
        {
            graphics.renderer renderer = new graphics.renderer();

            graphics.shader new_shader = new graphics.shader("default_vs", shader_type.vertex);
            new_shader.is_loading = true;
            var result = await renderer.pick_and_compile_shader(new_shader.shader_name, "VS", "vs_5_0");

            graphics.buffer new_buffer = new graphics.buffer();

        }

    }
}
