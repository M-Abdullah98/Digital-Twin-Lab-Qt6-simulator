# Digital Twin Lab

A C++17 / Qt 6 desktop app that simulates a small fleet of drones and environmental sensors. You log in, add and remove devices, take drones off and land them, and run a simulation while the app logs what happens.

I built it module by module for my OOP lab, so the folders follow the lab's structure.

## What's in it

| Folder | What it does |
|---|---|
| `Module1` | Small utility classes: `Battery`, `Coordinate`, `TelemetryData` |
| `Module2` | Class hierarchy: `DigitalObject` -> `Device` -> `AutonomousDevice` -> `Drone` -> `SurveillanceDrone`, and `Sensor` -> `EnvironmentalSensor` -> `ClimateSensor` |
| `Module3` | `SimulationEngine`, which steps the devices forward |
| `Module4` | Controllers for devices, sensors and the simulation |
| `Module5` | Custom exceptions (invalid input, low or dead battery, device not found, and so on) |
| `Module6` | The Qt GUI: login window, dashboard, drone control panel, simulation window |
| `Module7` | `DataManager`, which saves and loads devices, sensors and a simulation log as text files |

## Building

You need Qt 6 (Widgets), CMake 3.16 or newer and a C++17 compiler. I built it on Windows with MSYS2 UCRT64 and Ninja.

```
cd Module6
cmake --preset ucrt64-qt6
cmake --build build
```

The preset in `CMakeUserPresets.json` points at `C:/msys64/ucrt64`. If Qt lives somewhere else, change `CMAKE_PREFIX_PATH` there. `CMakeLists.txt` is in `Module6` but pulls in the other modules through `../ModuleN`, so always build from inside `Module6`.

## Running

Run `DigitalTwinLab` from the `Module6/build` folder. The login is a demo account that is hardcoded in the source:

- username: `admin`
- password: `1234`

It's only there for the lab and isn't real security.

## Saved data

`DataManager` (Module 7) can write `devices.dat`, `sensors.dat` and `simulation_logs.txt`. The two `.dat` files are pipe-separated text, one record per line, with a version header on the first line (for example `DTL_DEVICES_V1`). By default the files go next to the executable. The GUI doesn't call `DataManager` yet, so the app doesn't save anything on its own right now. Those file names are in `.gitignore` so test data stays out of the repo.

## Known limits

- The login is hardcoded and not checked against any user store.
- Only tested on Windows.
- The GUI isn't connected to `DataManager` yet, so nothing persists between runs.
- No automated tests yet.
