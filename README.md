# lifi-fingerprint
HW requirements:

    - Elegoo UNO R3 Board ATmega328P ATMEGA16U2

    - TBD

Circuit setup:

    TDB 

SW requirements:

    - Download and install Arduino IDE https://www.arduino.cc/en/Main/Software

    - The program needs to be running and executing in Arduino IDE.

Program fingerprint.ino:

    This program is Fingerprint technique using light intensity picked up by
    photoresistor and measured in resistance values (Ohms).

    It consists of 2 phases:

        1) Off-line Calibration (training) phase:

            During this phase fingerprint table is constructed using measurement
            samples and stored in database. Each measurement of resistor is stored
            with associated distance value (meters) in the table. By default the
            distance is sampled every 0.1 m from photoresistor.
            Based on the measurements stored in database the Statistical analysis 
            is performed to construct the curve-fitting or best-fitting Regression
            models.

            Algorithm builds 3 (three) regression models:

                a) Linear Regression model:
                   y = a + bx

                b) Polynomial (Quadratic) Regression model:
                   y = a + bx + cx^2

                c) Exponential Regression model:
                   y = a e^(bx)

            In addition calculates R^2 (R-Squared) value for each of the regression
            model and based on this R^2 value identifies among three the best
            curve-fitting model that is used for online position phase.

         2) On-line Positioning phase:

            During this phase resistance measurements picked up by photoresistor are
            sent to the best-fitting regression model algorithm to compute the
            distance and displays it to the serial monitor.
