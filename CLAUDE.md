# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Static personal portfolio website for Troy Nachtigall (researcher in fashion, digital fabrication, and computational design). Hosted on GitHub Pages at troykyo.net.

## Technology Stack

- **HTML5 static site** using Pixelarity's Phantom theme
- **SASS/SCSS** for styling (source in `/assets/sass/`, compiled to `/assets/css/main.css`)
- **jQuery** for interactivity
- **Font Awesome 4.x** for icons
- **Google Fonts** (Source Sans Pro)

## Development Commands

**SASS Compilation** (if modifying styles):
```bash
sass assets/sass/main.scss assets/css/main.css
```

**Local Preview** (any static server works):
```bash
python3 -m http.server 8000
```

**Deployment**: Push to master branch - GitHub Pages deploys automatically.

## Architecture

### Key Pages
- `index.html` - Homepage with project tile grid
- `bio.html` - Author biography
- `publications.html` - Academic publications list
- `cv.html` - Curriculum vitae
- `projectJ.html` - Featured project showcase

### Project Subdirectories
- `/FabAcademy/` - Fab Academy course documentation (46 HTML pages)
- `/solemaker/`, `/circular/`, `/magenta/`, `/generic/` - Individual project showcases
- `/DataMaterial/` - Research presentation PDFs

### Asset Organization
- `/assets/sass/` - SCSS source files organized by base/components/layout/libs
- `/assets/css/` - Compiled CSS (main.css is 3300+ lines)
- `/assets/js/` - jQuery, utilities, breakpoint handling
- `/images/` - Portfolio images (150+ files)

## Important Notes

- No build system - edit HTML files directly
- SASS changes require manual compilation to update CSS
- Pixelarity theme has commercial license - preserve attribution
- Domain configured via CNAME file (troykyo.net)
- When adding projects, update both the project page and the tile on index.html
