# Image Map Generator

A visual tool for creating HTML image maps. Draw clickable hotspots on any image and generate the corresponding HTML code automatically.

![Version](https://img.shields.io/badge/version-1.1.0-blue)
![Platform](https://img.shields.io/badge/platform-Windows%2064--bit-lightgrey)
![Qt](https://img.shields.io/badge/Qt-6.10-green)

---

## Table of Contents

- [What is an Image Map?](#what-is-an-image-map)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [User Interface](#user-interface)
- [Tools](#tools)
- [Modes](#modes)
- [Working with Hotspots](#working-with-hotspots)
- [Generating HTML Code](#generating-html-code)
- [Project Files](#project-files)
- [Keyboard Shortcuts](#keyboard-shortcuts)
- [Building from Source](#building-from-source)

---

## What is an Image Map?

An image map is an HTML feature that allows different areas of an image to link to different destinations. When a user clicks on a defined region (hotspot), they are directed to the associated URL.

**Example HTML output:**
```html
<img src="navigation.png" width="800" height="600" usemap="#navmap" alt="Image Map">
<map name="navmap">
  <area shape="rect" coords="0,0,100,50" href="/home" alt="Home">
  <area shape="circle" coords="200,150,40" href="/about" alt="About">
  <area shape="poly" coords="300,100,350,150,300,200,250,150" href="/contact" alt="Contact">
</map>
```

---

## Installation

### Option 1: Installer (Recommended)

1. Download `ImageMapGenerator-v1.1.0-Setup-Win64.exe`
2. Run the installer
3. Follow the installation wizard
4. Launch from Start Menu or Desktop shortcut

### Option 2: Portable Version

1. Download `ImageMapGenerator-v1.1.0-Portable-Win64.zip`
2. Extract to any folder
3. Run `ImageMapGenerator.exe`

> **Note:** No additional dependencies required. All necessary files are included.

---

## Quick Start

1. **Open an image:** Click `Open` or press `Ctrl+O`
2. **Select a tool:** Choose Rectangle, Circle, or Polygon from the toolbar
3. **Draw hotspots:** Click and drag on the image to create hotspot areas
4. **Set URLs:** Select a hotspot and enter the URL in the Properties panel
5. **Copy HTML:** Click `Copy to Clipboard` to get your image map code

---

## User Interface

### Main Window Layout

```
┌─────────────────────────────────────────────────────────────────┐
│  Menu Bar                                                        │
├─────────────────────────────────────────────────────────────────┤
│  Toolbar: Open | Select | Rect | Circle | Poly | Clip | 1080p   │
├───────────────────────────────────────┬─────────────────────────┤
│                                       │  Hotspot Properties     │
│                                       │  ─────────────────────  │
│         Image Canvas                  │  Shape: Rectangle       │
│                                       │  Coords: 10,20,100,150  │
│    (Draw hotspots here)               │                         │
│                                       │  URL: [____________]    │
│                                       │  Alt:  [____________]   │
│                                       │  Title:[____________]   │
│                                       │                         │
│                                       │  [Delete Hotspot]       │
├───────────────────────────────────────┴─────────────────────────┤
│  HTML Code Preview                                               │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │ <img src="image.png" usemap="#imagemap">                    ││
│  │ <map name="imagemap">                                       ││
│  │   <area shape="rect" coords="10,20,100,150" href="...">    ││
│  │ </map>                                                      ││
│  └─────────────────────────────────────────────────────────────┘│
│  [Copy to Clipboard]  [Export HTML]                              │
├─────────────────────────────────────────────────────────────────┤
│  Status Bar: X: 150, Y: 200                     800 × 600 px    │
└─────────────────────────────────────────────────────────────────┘
```

---

## Tools

### Select Tool
- **Purpose:** Select and move existing hotspots
- **Usage:** Click on a hotspot to select it, then drag to move
- **Tip:** Use this to reposition hotspots after creation

### Rectangle Tool
- **Purpose:** Create rectangular hotspots
- **Usage:** Click and drag to define the rectangle area
- **HTML Output:** `<area shape="rect" coords="x1,y1,x2,y2">`

### Circle Tool
- **Purpose:** Create circular hotspots
- **Usage:** Click at the center, drag outward to set the radius
- **HTML Output:** `<area shape="circle" coords="x,y,radius">`

### Polygon Tool
- **Purpose:** Create irregular polygon hotspots
- **Usage:**
  1. Click to add each vertex point
  2. Double-click or right-click to close the polygon
  3. Minimum 3 points required
- **HTML Output:** `<area shape="poly" coords="x1,y1,x2,y2,x3,y3,...">`

---

## Modes

### Clipboard Mode
- **Toggle:** Click the `Clipboard` button in the toolbar
- **Purpose:** Quickly copy coordinates of any point you click
- **Usage:** 
  1. Enable Clipboard Mode
  2. Click anywhere on the image
  3. Coordinates are automatically copied (e.g., `150,200`)
- **Use Case:** When you need precise coordinates for manual HTML editing

### Screen Standard Mode (1080p)
- **Toggle:** Click the `1080p` button in the toolbar
- **Purpose:** Scale all coordinates to 1920×1080 resolution
- **Usage:**
  1. Enable 1080p Mode
  2. All coordinates are automatically scaled
  3. Generated HTML includes `width="1920" height="1080"`
- **Use Case:** When your source image is high-resolution but you want the output to match a standard 1080p display

**Example:**
| Image Size | Click Position | Normal Coords | 1080p Mode Coords |
|------------|----------------|---------------|-------------------|
| 3840×2160 | (1920, 1080) | 1920,1080 | 960,540 |
| 960×540 | (480, 270) | 480,270 | 960,540 |

---

## Working with Hotspots

### Creating Hotspots

1. Select the appropriate tool (Rectangle, Circle, or Polygon)
2. Draw on the image canvas
3. The hotspot appears with a semi-transparent overlay

### Selecting Hotspots

- **Click** on a hotspot with the Select tool
- Or **click** on an item in the Hotspots panel list

### Editing Hotspot Properties

1. Select a hotspot
2. In the Properties panel, enter:
   - **URL:** The link destination (e.g., `https://example.com/page`)
   - **Alt Text:** Accessibility text (required for valid HTML)
   - **Title:** Tooltip text shown on hover

### Moving Hotspots

1. Select the hotspot
2. Drag to the new position

### Deleting Hotspots

- Select and press `Delete` or `Backspace`
- Or click `Delete Hotspot` in the Properties panel
- Or use `Edit → Delete Hotspot`

### Clearing All Hotspots

- Use `Edit → Clear All Hotspots`
- Or click `Clear All` in the Hotspots panel

---

## Generating HTML Code

### Live Preview

The HTML Code panel shows a real-time preview of your image map code. It updates automatically as you:
- Add or remove hotspots
- Edit hotspot properties
- Enable/disable Screen Standard Mode

### Customizing the Map Name

Change the map name in the `Map Name` field. This affects:
- The `name` attribute of the `<map>` element
- The `usemap` attribute of the `<img>` element

### Copying to Clipboard

Click `Copy to Clipboard` to copy the generated HTML code.

### Exporting to File

1. Click `Export HTML` or use `File → Export HTML`
2. Choose a location and filename
3. A complete HTML document is saved with:
   - DOCTYPE declaration
   - HTML structure
   - Your image map code

---

## Project Files

### Saving Projects

1. Use `File → Save Project` or `Ctrl+S`
2. Projects are saved as `.imap` files (JSON format)
3. Includes:
   - Image path
   - All hotspots with their shapes and properties
   - Map name

### Loading Projects

1. Use `File → Load Project` or `Ctrl+Shift+O`
2. Select a `.imap` file
3. The image and all hotspots are restored

> **Note:** If the original image has moved, you may need to reopen it manually.

---

## Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| Open Image | `Ctrl+O` |
| Save Project | `Ctrl+S` |
| Load Project | `Ctrl+Shift+O` |
| Export HTML | `Ctrl+E` |
| Delete Hotspot | `Delete` or `Backspace` |
| Cancel Drawing | `Escape` |
| Deselect | `Escape` |
| Finish Polygon | `Enter` or `Double-click` or `Right-click` |
| Zoom In | `Ctrl++` or `Ctrl+Scroll Up` |
| Zoom Out | `Ctrl+-` or `Ctrl+Scroll Down` |
| Zoom to Fit | `Ctrl+0` |
| Reset Zoom (100%) | `Ctrl+1` |

---

## Building from Source

### Requirements

- Qt 6.10 or later (with Widgets module)
- CMake 3.16 or later
- MinGW 13.1 or compatible compiler

### Build Steps

```bash
# Clone the repository
git clone https://github.com/your-repo/image-coord.git
cd image-coord

# Create build directory
mkdir build && cd build

# Configure
cmake .. -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.10.1/mingw_64"

# Build
cmake --build .
```

### Creating Distribution Packages

```bash
# Deploy Qt dependencies
windeployqt image-coord.exe --release

# Create installer (requires Inno Setup)
iscc installer.iss
```

---

## Support

For issues and feature requests, please open an issue on the GitHub repository.

---

## License

This project is provided as-is for educational and personal use.

---

*Image Map Generator v1.1.0*
