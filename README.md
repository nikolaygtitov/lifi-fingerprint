# lifi-fingerprint
HW requirements:

    - UNO R3 Board ATmega328P ATMEGA16U2

    - Photoresistor: eBoot 5mm GM5539 5539

Circuit setup:

    ----------------------------------------------------
                     PhotoR     10K
          +5    o---/|/|/--.--/|/|/---o GND
                           |
          PinA0 o-----------
          Pin9  o-----buzzer---o GND
    ----------------------------------------------------

SW requirements:

    - Download and install Arduino IDE https://www.arduino.cc/en/Main/Software

    - The program needs to be running and executing in Arduino IDE.

Program fingerprint.ino:

    This program is Fingerprint technique using light intensity picked up by
    photoresistor, measured in resistance values (Ohms), and converted into a
    distance measurements using various Regression models.

    It consists of 2 phases:

        1) Off-line Calibration (training) phase:

            During this phase fingerprint table is constructed using measurement
            samples and stored in database. Each resistance value is stored
            with associated distance value (meters) in the table during
            training. Training is performed by placing LED light at certain
            distances from photoresistor. By default, the distance is sampled
            every 0.1 meters from photoresistor.
            Based on the measurements stored in database the Statistical
            analysis is performed to construct the curve-fitting Regression
            models.

            Algorithm builds 3 (three) regression models:

                a) Linear Regression model:
                   y = a + bx

                b) Polynomial (Quadratic) Regression model:
                   y = a + bx + cx^2

                c) Exponential Regression model:
                   y = a e^(bx)

            In addition, calculates R^2 (R-Squared) value for each of the
            regression model. Since R-Squared value cannot be used for
            statistical evaluation of NON-Linear regression models, all three
            models are used to determine the distance during On-line
            Positioning phase.

         2) On-line Positioning phase:

            During this phase resistance (x) measurements picked up by
            photoresistor are used for distance (y) calculations utilizing each
            regression model. Distance values are displayed on the serial
            monitor.
