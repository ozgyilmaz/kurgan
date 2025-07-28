# Kurgan MUD

**Kurgan MUD** is a modernized, classless, color-enhanced fork of the classic ROM 2.4b6 MUD engine.  
Built with a focus on performance, clarity, and immersion, Kurgan MUD simplifies old systems, introduces dynamic object generation and more immersive gameplay experience.

> ⚔️ Version: `1.0b1`  
> 📅 Release Date: July 2025  
> 🧪 Development Branch: `dev`  
> ✅ Stable Branch: `main`



## Features

- **Based on ROM 2.4b6**, cleaned and modernized
- **Classless system**: All players follow race-based progression
- **Dynamic area & object system** with JSON-based area support
- **Automated quest system** from Vassago and Anatolia MUD
- **Book-based skill learning** system with rarity mechanics
- **Randomized objects** Weapons, armor, scrolls, potions, and more with randomized stats
- **Improved XP progression formula** scales better for low levels
- **Expanded gender options**: male, female, nonbinary, androgynous, agender
- **Full 8-bit RGB color support** (Lope's Color 2.0 & W3C palette)



## Public Server

You can connect to the official public Kurgan MUD server via any Telnet client:

**Host:** `37.48.78.239`  
**Port:** `4000`

Example (Linux/macOS terminal):
```bash
telnet 37.48.78.239 4000
```



## Installation

```bash
git clone https://github.com/ozgyilmaz/kurganmud.git
cd kurganmud/src
make
../startup.sh 4000
```

You can change port number in startup.sh.
Recommended compiler: gcc on Linux.



## Tools & Scripts

area-to-json.py: Converts .are files to .json

area-to-json.sh: Batch converter for entire /area folder

JSON parser: Parses .json areas at boot time



## Changelog Highlights

See CHANGELOG.md for full details.



## License

This project is released under the MIT License.
Although based on the ROM 2.4b6 engine, all new code is original and open-sourced.



## Author & Contact

Created and maintained by ozgyilmaz

Feedback, contributions, and forks are welcome!



## Website

GitHub Pages site coming soon
Until then, follow development on the GitHub repository