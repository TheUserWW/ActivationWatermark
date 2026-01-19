# ActivationWatermark

A cross-platform desktop application that displays an "Activate Windows" style watermark on your screen. Built with GTK4 and Cairo, this application creates transparent, click-through windows that show localized activation messages across all monitors.

## Features

- **Multi-monitor support**: Automatically creates watermark windows for all connected displays
- **Multi-language localization**: Automatically displays text in your system language (English, Chinese, German, Japanese, French, Spanish, Russian)
- **Click-through capability**: Watermark windows allow mouse events to pass through to underlying applications
- **Transparent rendering**: Uses Cairo for smooth, anti-aliased text rendering
- **Cross-platform design**: Built with GTK4, supporting Windows and other platforms

## Requirements

- **Windows**: GTK4 runtime libraries, MSYS2 or similar development environment
- **Build tools**: CMake, GCC/MinGW, GTK4 development packages
- **Dependencies**:
  - GTK4
  - Cairo
  - GLib

## Building

### Windows (MSYS2/MinGW)

1. Install required packages using MSYS2:
```bash
pacman -S mingw-w64-x86_64-gtk4 mingw-w64-x86_64-cairo mingw-w64-x86_64-glib2 mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc
```

2. Create build directory and compile:
```bash
mkdir build
cd build
cmake ..
make
```

3. Run the application:
```bash
./ActivationWatermark
```

### Linux (optional)

1. Install dependencies:
```bash
# Debian/Ubuntu
sudo apt install libgtk-4-dev libcairo2-dev cmake build-essential

# Fedora
sudo dnf install gtk4-devel cairo-devel cmake gcc
```

2. Build and run:
```bash
mkdir build && cd build
cmake ..
make
./ActivationWatermark
```

## Supported Languages

The application automatically detects your system locale and displays the appropriate language:

| Language | Locale Codes | Translation |
|----------|--------------|-------------|
| English  | en_*         | "Activate Windows, Go to Settings to activate Windows" |
| Chinese  | zh_*         | "激活 Windows, 转到“设置”以激活 Windows" |
| German   | de_*         | "Windows aktivieren, Gehen Sie zu Einstellungen, um Windows zu aktivieren" |
| Japanese | ja_*         | "Windows を有効化, 設定に移動して Windows を有効化してください" |
| French   | fr_*         | "Activer Windows, Accédez aux paramètres pour activer Windows" |
| Spanish  | es_*         | "Activar Windows, Ve a configuración para activar Windows" |
| Russian  | ru_*         | "Активируйте Windows, Перейдите в раздел \"Параметры\", чтобы активировать Windows" |

## How It Works

1. **Window Creation**: The application creates a fullscreen, transparent GTK window on each monitor
2. **Text Rendering**: Uses Cairo to render the activation message with semi-transparency (63% opacity)
3. **Click-through**: Applies `WS_EX_TRANSPARENT` and `WS_EX_LAYERED` extended styles on Windows to allow mouse events to pass through
4. **Positioning**: Text is positioned in the bottom-right corner of each monitor with consistent margins

## Architecture

```
main.c
├── Data Structures & Translations
│   └── WatermarkText array with 7 language variants
├── Drawing Logic (on_draw)
│   └── Cairo text rendering with positioning
├── Mouse穿透 (make_window_transparent)
│   └── GTK widget settings + GDK surface + Win32 API
└── Application Initialization (activate)
    └── Multi-monitor window creation
```

## Technical Details

- **GTK Widgets**: Uses `GtkApplication`, `GtkWindow`, and `GtkDrawingArea`
- **Cairo Rendering**: `cairo_show_text()` for text display with RGBA colors (0.47, 0.47, 0.47, 0.63)
- **Windows Integration**: Uses `GDK_SURFACE_HWND()` macro to get HWND for native Windows API calls
- **CSS Styling**: Applies custom CSS for transparent backgrounds and border removal

## License

This project is open source and available for personal and educational use.

## Contributing

Contributions are welcome! Areas for improvement include:

- Additional language support
- macOS support (using NSWindowLevel)
- Configuration file for custom messages
- System tray icon for easy exit
- Display position configuration

## Troubleshooting

**Watermark not visible?**
- Ensure GTK4 runtime is properly installed
- Check that all monitors are detected

**Mouse clicks not passing through?**
- Run as administrator on Windows
- Check Windows firewall settings
- Verify no conflicting overlay software

**Text not in your language?**
- Check `LANG` environment variable
- Ensure proper UTF-8 locale is set
- Restart the application after changing system language