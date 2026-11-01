<div align="center">
  <h1>picomon</h1>
  <p><em>An RP2350 library for Raspberry Pi Pico 2 boards.</em></p>
  <p><sub>For personal use, you may do what you want, but outside contributions to this repo will be dismissed.</sub></p>
</div>

<div align="center">
  <sub><em>Written in <a href="https://github.com/exoad/kira">Kira</a>; needs the Kira compiler to build.</em></sub>
</div>

## Layout

- `rp2350/`: target-neutral modules (pins, ADC, PWM duty, I2C addresses, a ticker, the board profiles), with host tests in `tests/`.
- `rp2350fw/`: firmware-only bindings to the Pico SDK and arduino-pico (`@_extern`, drift-checked against the SDK headers), with no hand-written C++: pins, PWM, UART and I2C go through arduino-pico (pinMode, digitalWrite, analogWrite, Serial1/2, Wire/Wire1).
- `src/app.kira`: the smoke firmware, which blinks the board's LED.
- `tests/`: host tests and the mutation runner, all in Kira.

## Boards

`PICOMON_BOARD` (set by each PlatformIO env) is an index into `rp2350.board.PROFILES`:

| index | env | PlatformIO board | LED | flash |
|---|---|---|---|---|
| 0 | `pico2` | `rpipico2` | GP25 | 4 MB |
| 1 | `pico2_w` | `rpipico2w` | wireless chip (CYW43) | 4 MB |
| 2 | `beetle` | `rpipico2` | GP25 | 2 MB |

## Toolchain

- PlatformIO in `.venv` (`pip install platformio`). The platform is `maxgerhardt/platform-raspberrypi` pinned to `5d4561a0`, which provides the `rpipico2` and `rpipico2w` boards and the earlephilhower arduino-pico framework.
- Kira compiler: `vendor/kira` (submodule, `exoad/kira` at `e73b7f07`).

## Build and test

Build the Kira compiler once with `cd vendor/kira && ./gradlew.bat installDist --offline -q`. Then, from the repo root:

    cd tests && ..\vendor\kira\build\install\kira\bin\kira.bat --target py && cd ..
    .venv\Scripts\python.exe tests\rp2350_test_kira.py
    .venv\Scripts\python.exe -m platformio run -e pico2
    .venv\Scripts\python.exe tests\check_kira.py --mutants

`check_kira.py` compiles the library, runs the host tests, regenerates the firmware, and builds every env. With `--mutants` it also runs the mutation tests in `tests/mutants.kira`. Nothing here flashes a board.
