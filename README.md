ARCameraHeadRom
===============

Firmware for computer controlled stepper motor based stereoscopic camera head with pan and tilt functionality. For PC drivers used to cotrol the head see [ARCameraHeadAPI](https://github.com/mrozo/ARCameraHeadAPI) and for the documentation check [ARCameraHeadThesis](https://github.com/mrozo/ARCameraHeadThesis).

The firmware was developed for ATMega8 and is intended to be used together with ARCameraHeadAPI that is a PC software developed to control the device over USB. This code is a parto of university thesis "Stereoscopic camera system with pan and tilt functionality controled in real time by oculus rift". The project has been created together with Jędrzej D. and Maciej R. . I (mrozo) was responsible for creating the mechanics, electronics, firmware for the elctronics and drivers exposing API used to control the camera head.

The code
====

The firmware is a kind of real time scheduler with a custom stepper motor controller and uart based communication protocol with framing and feedback.


