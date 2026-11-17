# ESP32 Robot - Web-Controlled Steppers + Servo

Turn motors on and off from any browser on your network. Current hardware:

| # | Motor | Driver | Notes |
|---|-------|--------|-------|
| 0-3 | Unipolar steppers 1-4 | 4-input ULN2003-style boards, driven by an **SX1509 I2C GPIO expander** | half-step drive |
| 4 | Bipolar stepper | SparkFun Easy Driver | STEP / DIR / ENABLE, 1/8 microstep default |
| - | Hobby servo | direct from GPIO | 0-180 degrees |

The four unipolar motors share one SX1509 (16 outputs = 4 motors x IN1-IN4), so they only use the ESP32's two I2C pins instead of 16 GPIOs. Set `UNI_COUNT` in `config.h` if you have fewer than four connected.

See `hookup-diagram.svg` (open it in a browser) for the full wiring.

## Files

```
platformio.ini          PlatformIO project (ESP32 + ESP32Servo library)
src/config.h            Wi-Fi credentials, pins, speed limits  <-- edit this
src/main.cpp            setup()/loop()
src/StepperBase.*       non-blocking run/stop/direction/speed logic
src/Sx1509.*            SX1509 I2C expander (16 outputs, one I2C write per update)
src/UnipolarStepper.*   4-wire half-step driver on SX1509 pins
src/EasyDriverStepper.* STEP/DIR/ENABLE driver
src/ServoMotor.*        servo wrapper (signal only while enabled)
src/WebApp.*            Wi-Fi, HTTP server, JSON API
src/WebUI.h             the browser page (HTML/JS in flash)
```

## Wiring (pin map)

| Function | ESP32 GPIO | Connects to |
|----------|-----------|-------------|
| I2C SDA | 21 | SX1509 SDA |
| I2C SCL | 22 | SX1509 SCL |
| 3.3 V | 3V3 | SX1509 3.3V |
| Easy Driver STEP | 14 | STEP |
| Easy Driver DIR | 27 | DIR |
| Easy Driver ENABLE | 33 | ENABLE (active low) |
| Servo signal | 32 | signal wire (orange/white) |
| Ground | GND | **common ground to everything** |

| SX1509 pins | Connects to |
|-------------|-------------|
| 0, 1, 2, 3 | unipolar board 1 IN1, IN2, IN3, IN4 |
| 4, 5, 6, 7 | unipolar board 2 IN1, IN2, IN3, IN4 |
| 8, 9, 10, 11 | unipolar board 3 IN1, IN2, IN3, IN4 |
| 12, 13, 14, 15 | unipolar board 4 IN1, IN2, IN3, IN4 |

SX1509 notes:
- Power it from the ESP32's **3.3 V**, not 5 V. Its outputs then drive the ULN2003 inputs at 3.3 V, which is enough.
- The SparkFun SX1509 breakout has I2C pull-up resistors on board. Other boards may need 4.7 kOhm pull-ups from SDA and SCL to 3.3 V.
- Default address is `0x3E` (both ADDR jumpers open). If you change the jumpers, set `SX1509_I2C_ADDR` in `config.h`.
- The serial monitor prints `SX1509 found at 0x3E` at boot. If it says *NOT found*, check SDA/SCL, 3.3 V, GND and the address.

Change any pin in `src/config.h`.

### Power - read before powering up

- **ESP32**: powered over USB.
- **Unipolar driver boards** (28BYJ-48 style): a separate **5 V** supply to each board's +/- pins. Each running 28BYJ-48 draws about 250 mA, so four motors plus the servo need 3 A or more. Leave the board's power jumper on.
- **Servo**: red wire to the same 5 V supply, black/brown to GND. Servos can pull an amp when stalled; don't power them from the ESP32.
- **Easy Driver**: motor supply on `M+` / `GND` (6-12 V is typical; match your motor's ratings). Add a 100 uF capacitor across M+ and GND. Set the current pot for your motor before running it for long periods.
- **Common ground**: the ESP32 GND, the SX1509, every driver, the servo and the negative of every supply must be tied together. Missing grounds are the #1 cause of erratic motors.
- Never connect or disconnect the bipolar motor while the Easy Driver is powered.
- **Logic level**: the ESP32 outputs 3.3 V. Easy Driver boards generally respond to this, but some revisions have a 3.3 V/5 V logic solder jumper; check yours if steps are missed.

### Bipolar motor coils

A bipolar motor has two windings (4 wires). Find each pair with a multimeter (continuity between two wires = one winding). Connect one pair to the Easy Driver's two `A` pins and the other to the two `B` pins. If the motor just vibrates, swap the wires of one pair. Swapping a pair also reverses its direction.

## Build and flash

### PlatformIO (recommended)
1. Install VS Code + PlatformIO.
2. Open this folder, copy `src/secrets.example.h` to `src/secrets.h` and put your Wi-Fi name/password in it (`secrets.h` is git-ignored).
3. Click **Upload**, then open the **Serial Monitor** at 115200 baud.

### Arduino IDE
1. Install the ESP32 board package and the **ESP32Servo** library (Library Manager). The SX1509 driver is included in `src/`, so no extra library is needed.
2. Create a sketch folder named `ESP32_Robot`; copy everything from `src/` into it.
3. Rename `main.cpp` to `ESP32_Robot.ino`.
4. Select your ESP32 board and upload.

## Usage

1. Power the ESP32. The serial monitor prints the address, e.g. `Open http://192.168.1.50/`.
2. On a phone or computer on the same Wi-Fi, open that address (or `http://esp32-robot.local`).
3. **If the router can't be reached** within 15 s, the ESP32 starts its own Wi-Fi network (`ESP32-Robot`, password `robot12345`, both changeable in `config.h`) and the page is at `http://192.168.4.1/`.

On the page:

- **Stepper cards**: tap the big button to start/stop, pick Clockwise or Counter-CW, drag the slider for speed (steps/s). The position counter shows steps taken since boot. Stopping de-energizes the coils so motors and drivers don't heat up (they will free-wheel, with no holding torque).
- **Servo card**: tap *Enable* to start sending the signal (the servo moves to the last angle, 90 degrees at boot), then use the slider or 0/90/180 buttons. *Disable* releases the servo.
- **STOP ALL**: stops all steppers and releases the servo.

Motors always start **off** after a reset.

## HTTP API

All are `GET` and return the current status as JSON.

| URL | Effect |
|-----|--------|
| `/api/status` | read status |
| `/api/stepper?id=0&run=1` | start (`run=0` stops); `id` 0-3 = unipolar 1-4, 4 = Easy Driver (with `UNI_COUNT` 4) |
| `/api/stepper?id=4&dir=ccw` | direction `cw` / `ccw` |
| `/api/stepper?id=1&speed=300` | steps per second (clamped to the motor's max) |
| `/api/servo?angle=45` | set angle 0-180 |
| `/api/servo?enabled=1` | enable (`0` disables) |
| `/api/stopall` | stop everything |

Parameters can be combined, e.g. `/api/stepper?id=0&dir=cw&speed=400&run=1`.

## Tuning and troubleshooting

| Symptom | Try |
|---------|-----|
| Unipolar motor hums but doesn't turn | lower speed; check wiring order IN1-IN4; confirm 5 V supply and common ground |
| No unipolar motor moves at all | serial monitor says *SX1509 NOT found*: check SDA/SCL wiring, 3.3 V, pull-ups and `SX1509_I2C_ADDR` |
| Unipolar motor stalls at high speed | reduce `UNI_MAX_SPS` in `config.h` (28BYJ-48: about 500 steps/s is safe) |
| Easy Driver motor vibrates only | swap one coil pair; check coil pairing with a multimeter |
| Easy Driver motor skips steps | lower speed, raise current pot slightly, check 3.3 V logic jumper |
| Servo jitters or buzzes at the ends | adjust `SERVO_MIN_US` / `SERVO_MAX_US` in `config.h`; check supply |
| Page won't load | confirm same network; use the IP from the serial monitor; try AP mode |
| Motors run slowly or unevenly while using the page | normal at very high speeds with all 5 steppers running; lower speeds |

Steps per revolution: 28BYJ-48 half-step = 4096; a 200-step bipolar motor at 1/8 microstepping = 1600.

## Adding more motors

Create another `StepperBase` subclass (or reuse one), construct it in `main.cpp`, and add it to the `steppers[]` array. The web page builds its cards automatically from `/api/status`.
