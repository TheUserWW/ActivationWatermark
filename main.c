#include <gtk/gtk.h>
#include <glib.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
// 在 GTK 的 windows 后端头中定义了 GDK_SURFACE_HWND 等宏
// 有些安装把头放在 <gdk/gdkwin32.h> 或 <gdk/win32/gdkwin32.h>
#include <gdk/win32/gdkwin32.h>

#endif

// ---------------------------------------------------------
// 数据结构与多语言定义
// ---------------------------------------------------------

typedef struct {
    const char *line1;
    const char *line2;
} WatermarkText;

static WatermarkText translations[] = {
    {"Activate Windows", "Go to Settings to activate Windows"},
    {"激活 Windows", "转到“设置”以激活 Windows"},
    {"Windows aktivieren", "Gehen Sie zu Einstellungen, um Windows zu aktivieren"},
    {"Windows を有効化", "設定に移動して Windows を有効化してください"},
    {"Activer Windows", "Accédez aux paramètres pour activer Windows"},
    {"Activar Windows", "Ve a configuración para activar Windows"},
    {"Активируйте Windows", "Перейдите в раздел \"Параметры\", чтобы активировать Windows"}
};

static WatermarkText* get_current_text() {
    const char *lang = getenv("LANG");
    int index = 0;
    if (lang) {
        if (strstr(lang, "zh_")) index = 1;
        else if (strstr(lang, "de_")) index = 2;
        else if (strstr(lang, "ja_")) index = 3;
        else if (strstr(lang, "fr_")) index = 4;
        else if (strstr(lang, "es_")) index = 5;
        else if (strstr(lang, "ru_")) index = 6;
    }
    return &translations[index];
}

// ---------------------------------------------------------
// 绘图逻辑
// ---------------------------------------------------------

static void on_draw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    WatermarkText *text = (WatermarkText *)user_data;

    cairo_select_font_face(cr, "Microsoft YaHei", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 20);

    cairo_text_extents_t ext1, ext2;
    cairo_text_extents(cr, text->line1, &ext1);
    cairo_text_extents(cr, text->line2, &ext2);

    double max_text_width = (ext1.width > ext2.width) ? ext1.width : ext2.width;

    double margin_right = 50.0;
    double margin_bottom = 100.0;
    double line_spacing = 26.0;

    double x = width - max_text_width - margin_right;
    double y = height - (line_spacing * 2) - margin_bottom;

    cairo_set_source_rgba(cr, 0.47, 0.47, 0.47, 0.63);

    cairo_move_to(cr, x, y + ext1.height);
    cairo_show_text(cr, text->line1);

    cairo_move_to(cr, x, y + line_spacing + ext2.height);
    cairo_show_text(cr, text->line2);
}

// ---------------------------------------------------------
// 核心：设置鼠标穿透
// ---------------------------------------------------------

static void make_window_transparent(GtkWidget *window) {
    // 1. GTK 级别：禁止该窗口及其子组件成为鼠标事件目标
    gtk_widget_set_can_target(window, FALSE);

    // 2. GDK 级别：设置输入区域为空（这会让 GTK 层不接收事件）
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(window));
    if (surface) {
        cairo_region_t *region = cairo_region_create();
        gdk_surface_set_input_region(surface, region);
        cairo_region_destroy(region);

#ifdef _WIN32
        // 3. Windows 平台：使用公开宏获取 HWND（不要调用内部函数）
        // GDK_SURFACE_HWND(surface) —— 在 gdk 的 win32 头里定义并返回 HWND
        HWND hwnd = GDK_SURFACE_HWND(surface);
        if (hwnd) {
            LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
            // 注入 WS_EX_TRANSPARENT（穿透）与 WS_EX_LAYERED（分层）
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED);
        }
#endif
    }
}

static void on_surface_mapped(GtkWidget *widget, gpointer data) {
    make_window_transparent(widget);
}

// ---------------------------------------------------------
// 应用初始化
// ---------------------------------------------------------

static void activate(GtkApplication *app, gpointer user_data) {
    WatermarkText *text = (WatermarkText *)user_data;
    GdkDisplay *display = gdk_display_get_default();

    GtkCssProvider *provider = gtk_css_provider_new();
    // 注意：gtk_css_provider_load_from_data 已被弃用，改用 load_from_string
    const char *css = "window.watermark-window { background-color: transparent; box-shadow: none; border: none; }\n"
                      "drawingarea { background-color: transparent; }";
    gtk_css_provider_load_from_string(provider, css);

    gtk_style_context_add_provider_for_display(display,
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);

    GListModel *monitors = gdk_display_get_monitors(display);
    guint n_monitors = g_list_model_get_n_items(monitors);

    for (guint i = 0; i < n_monitors; i++) {
        GtkWidget *window = gtk_application_window_new(app);

        gtk_widget_set_focusable(window, FALSE);
        gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
        gtk_widget_add_css_class(window, "watermark-window");

        // 全屏透明窗口
        gtk_window_fullscreen(GTK_WINDOW(window));

        GtkWidget *drawing_area = gtk_drawing_area_new();
        gtk_widget_set_hexpand(drawing_area, TRUE);
        gtk_widget_set_vexpand(drawing_area, TRUE);

        // 关键点：让 DrawingArea 也不接收输入
        gtk_widget_set_can_target(drawing_area, FALSE);

        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), on_draw, text, NULL);
        gtk_window_set_child(GTK_WINDOW(window), drawing_area);

        // 窗口映射时应用穿透逻辑
        g_signal_connect(window, "map", G_CALLBACK(on_surface_mapped), NULL);

        gtk_window_present(GTK_WINDOW(window));
    }
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "");
    WatermarkText *text = get_current_text();

    GtkApplication *app = gtk_application_new("com.example.watermark", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), text);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
