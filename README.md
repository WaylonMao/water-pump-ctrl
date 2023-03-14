The Automatic Water Pump Controller is an Arduino project designed to automate the control of a water pump which I built for my home in 2020.

The system uses a pressure sensor to measure the water pressure in the pipeline and turns on or off the water pump based on the desired pressure set by the user. The project involves reading the analog output from the pressure sensor, calculating the pressure value, and controlling the pump's on/off state based on the calculated pressure value. The system also includes a flow switch to prevent the pump from running when there is no water flowing in the pipeline. The project is useful in areas where water supply is limited or when manual control of the water pump is not practical.

Source Code: [https://github.com/WaylonMao/water-pump-ctrl](https://github.com/WaylonMao/water-pump-ctrl)

# Automatic Water Pump Controller

Released Date: 2020-04-15

## Background

When I was in China, the building where my condo was located had 33 floors. Although the water pressure was up to par, it was always unstable when it was peak hours. This is badly affecting my gas furnace water heater. This kind of water heater is mainly used for showering, and there is no hot water tank. Therefore, the unstable water pressure will cause the water temperature to be hot and cold.

So I first added an electric water pump. It really does work, the water flow and temperature are steady when activated. But this pump is kind of silly because it runs as long as there is water flowing. It's slightly noisier, and the frequent start-stops are very annoying. I want it to only activate when the water pressure gets too low.

That's why I decided to use Arduino to solve this problem, because I have a lot of Arduino-related components at home. But I lack water pressure sensor, so I bought this 3-pin linear sensor online. It's very simple, as simple as a temperature sensor.

## Getting Started

### Hardware

To build this project, you will need the following components:

- Arduino board (Uno or Nano)
- Water pressure sensor (Main is 5V for 1.2MPa)
- Relay module (Since controling AC pump directly)
- Four-digit display module (74HC595. You can use other modules)
- Jumper wires
- 5V power supply(I used a USB adapter)

### Wiring

![avatar](https://raw.githubusercontent.com/WaylonMao/water-pump-ctrl/main/pics/Sketch_bb.png)

This wiring diagram draw by Fritzing. There is no 5-pin 4-digit module in Fritzing, so I used a LED module and wrote a note. This part can be replaced by any display module. Just need to change the code and the wiring.

The Arduino I used is a Nano, which is more tiny and good for persistent installation and using.

The water flow switch on my pump is actually a reed switch. There is a flap with a magnet that cocks up when the water flows. The greater the water flow, the closer the magnet is to the reed switch. There is a screw to adjust the strength of the spring, that is to say, its sensitivity can be adjusted. I used a 10K resistor to pull down the reed switch. When the water flows, the reed switch will be pulled up to 5V. When the water stops flowing, the reed switch will be pulled down to 0V. I know this pull-down method is not necessary, you may directly connect the reed switch to the digital pin of the Arduino. Since there is an internal pull-down resistor on the Arduino, it will be pulled down to 0V when the water stops flowing. But I think it's better to use an external pull-down resistor to avoid the internal pull-down resistor from being damaged.

The optocoupler relay module is used to control the AC pump. When the Arduino sends a high signal, the relay module will switch on, until the Arduino sends a low signal, the relay module will switch off.

There is also no water pressure sensor in Fritzing. I used a temperature sensor which is also a 3-pin linear sensor. The output voltage is proportional to the pressure. The greater the pressure, the greater the output voltage. The output voltage is 0.5V-4.5V. The pressure range is 0-1.2MPa.

### Software

This project was developed using the Arduino IDE. You may download the source code from the GitHub repository. Don't forget to change the pin numbers in the code to match your wiring.

This project used a 4-digit module which is controlled by 74HC595. The code is a little bit different from the normal 4-digit module. You can find the library in the GitHub repository. If you use other modules, you can change the code to match your module.

## Code Explanation

There are two thresholds in this project. One is the pressure of turning on the pump, and the other is the pressure of turning off the pump. You can change the values to meet your needs. And I found that these two values should be set to a certain distance. If the two values are too close, the pump will turn on and off frequently.

```c
const float HIGH_PRESSURE = 0.27;  // The pressure of turning off the pump.
const float LOW_PRESSURE = 0.17;   // The pressure of turning on the pump.
```

The pressure sensor is a 3-pin linear sensor. The output voltage is proportional to the pressure. The following is the formula for calculating the pressure value. Why divide by 1023? Because the analogRead() function returns a value between 0 and 1023. I noticed there are many arguments about this. But based on my test, the value returned by analogRead() is between 0 and 1023. So I think it's OK to divide by 1023.

```c
  v_out = analogRead(SENSOR) / 1023. * 5.0;

  // Calculate pressure from sensor output voltage
  pressure = ((v_out - V_MIN) / (V_MAX - V_MIN)) * P_MAX;

  // If pressure has a minimum value, use the following formula instead:
  // pressure = ((v_out - V_MIN) / (V_MAX - V_MIN)) * (P_MAX - P_MIN) + P_MIN;

```

I did comment detial in the code. I will not explain it here.

## My Prototype

![avatar](https://raw.githubusercontent.com/WaylonMao/water-pump-ctrl/main/pics/PIC01.JPG)

![avatar](https://raw.githubusercontent.com/WaylonMao/water-pump-ctrl/main/pics/PIC02.JPG)

## Summary

This project is very simple. It's just a simple control system. But it's very useful. And I did learnd a lot from this project especially how to use a 74HC595 4-digit module.

As I said before, this porject was built on 2020-04-15. I have been using it for more than two year. It works very well. I haven't consider to improve its performance, because it's very simple and it works well. I think if you are good at math, you can rewrite the pressure calculation formula. My original formula was hardcoded after all the constants were computed. I write to this version because I think it's more readable.

The other part can be improved is the UI. I think it's better to use a LCD screen to display the pressure value. And provied some buttons to set the thresholds.

Hope you like this project. If you have any questions, please feel free to contact me.

## Reference

4-bit Display Module:

![avatar](https://raw.githubusercontent.com/WaylonMao/water-pump-ctrl/main/pics/4-bit-7-segment-module.jpg)

## Authors

### Weilong Mao

- Linkedin[https://www.linkedin.com/in/weilong-mao/]
- Github[https://github.com/WaylonMao]
