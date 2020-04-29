# EFTracker
## What's it do?
Scans your loot screenshots and outputs all detected items in text format.

## How's it work?
The work is divided into two parts: data collection and image processing.

Data collection is handled by the python scripts, requiring the `requests`
and `lxml` modules. The `scraper` script scrapes the EscapeFromTarkov wiki
page for a list of lootable containers, caching the associated wiki page
for each loot item. Item names are then formatted to (mostly) respect POSIX
naming conventions and then packed into three JSON files: `dat/droptable.json`,
`dat/iconcache.json`, and `dat/items.json`. Once the `dat` directory has been
constructed, the `saver` script loads the iconcache and downloads each item
icon to the `img` directory, naming each image file the aformentioned formatted
item name and saving them all as PNGs.

Now for the image processing part. I decided to use OpenCV's template
matching for identifying the item icons in the loot scene. At the moment,
the user has to specify which container is being looted (case sensitive).
Eventually I'd like to incorporate some form of OCR to remove as much human
input as possible. The input screenshot is scaled to 1920x1080 (since 1x1
size items' icons are 64x64px) and then cropped to focus only on the loot
container (top-right quarter of screenshot). Finally, the cropped screenshot
is iteratively searched for each item icon specified for the container in
`dat/droptable.json`. Depending on your system, enabling multithreading with
the '-m' flag can drastically improve the performance (especially for containers
with a ton of possible drops).

## How do I use it?
At the moment I've only tested this program on Arch Linux, requiring the
`extra/opencv` pacman package. However, this should work for any Linux system
with OpenCV4 installed and accessable via `pkg-config`. A Windows version is
currently in the works and will (hopefully) be maintained alongside the Linux
version.

## Dependencies
C++ Libraries:
- OpenCV

Python modules:
-   requests
-   lxml

## Known Issues
- Rotated item icons aren't matched
- Chainlet icon on wiki out-of-date
