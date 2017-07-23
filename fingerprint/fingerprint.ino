/* 
   Author: Nikolay Titov
   Email: ngtitov@ncsu.edu
          nikolay.titov@nokia.com
   
   SW requirements: 
   Download and install Arduino IDE https://www.arduino.cc/en/Main/Software
   The program needs to be running and executing in Arduino IDE.
   
   Program:
   This program is Fingerprint technique using light intensity picked up
   by photoresistor and measured in resistance values (Ohms).
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
        c) Exponential regression model:
           y = a e^(bx)
        In addition calculates R^2 (R-Squared) value for each of the regression
        model and based on this R^2 value identifies among three the best
        curve-fitting model that is used for online position phase.
   2) On-line Positioning phase
        During this phase resistance measurements picked up by photoresistor are
        sent to the best-fitting regression model algorithm to compute the
        distance and displays it to the serial monitor.
*/

#include<math.h>

#define a 0
#define b 1
#define c 2
#define r_2 3

//Constants
const int pResistor = A0; // Photoresistor at Arduino analog pin A0
const int samples = 10;
const int relations = 2;

//Variables
double fingerprint [samples] [relations] = {
  {0, 0.1},
  {0, 0.2},
  {0, 0.3},
  {0, 0.4},
  {0, 0.5},
  {0, 0.6},
  {0, 0.7},
  {0, 0.8},
  {0, 0.9},
  {0, 1}
};
double *linear_values;
double *polynomial_valaues;
double *exponential_valaues;
bool offlinePhase;
char regressionModel;
double resistance, distance, sum_x, sum_x_2, sum_y, sum_log_y, sum_xy, sum_log_xy, sum_x_3, sum_x_4, sum_x_2_y;

void setup(){
 pinMode(9, OUTPUT);
 Serial.begin(9600);
 linear_values = (double*) calloc(4, sizeof(double));
 polynomial_valaues = (double*) calloc(8, sizeof(double));
 exponential_valaues = (double*) calloc(8, sizeof(double));
 offlinePhase = true;
 sum_x = 0;
 sum_x_2 = 0;
 sum_y = 0;
 sum_log_y = 0;
 sum_xy = 0;
 sum_log_xy = 0;
 sum_x_3 = 0;
 sum_x_4 = 0;
 sum_x_2_y =0;
 regressionModel = 'N';
// pinMode(ledPin, OUTPUT); // Set lepPin - 9 pin as an output
 pinMode(pResistor, INPUT); // Set pResistor - A0 pin as an input (optional)
}

void loop(){
  // Get offline calibration phase done
  if (offlinePhase) {
    runOfflinePhase();
    getSumMatrixValues();
    getLinearRegression();
    getPolynomialRegression();
    getExponentialRegression();
    getBestRSquaredValue();
    delay(10000); //Delay
    Serial.println("On-Line Positioning starting ...");
    delay(3000); //Delay
  }
  else{
    resistance = analogRead(pResistor);
    switch(regressionModel) {
      case 'L':
        distance = linear_values[a] + linear_values[b] * resistance;
        break;
      case 'P':
        distance = polynomial_valaues[a] + polynomial_valaues[b] * resistance + polynomial_valaues[c] * pow(resistance, 2);
        break;
      case 'E':
        distance = exponential_valaues[a] * exp(exponential_valaues[b] * resistance);
        break;
     default:
        distance = -1;
    }
    Serial.print(distance); //Write the distance to the serial monitor
    Serial.println("  meters");
    delay(10000); //Delay
  }
}

void runOfflinePhase() {
  Serial.println("The Offline Calibration Phase is starting...");
  delay(3000); // Delay
  Serial.println("Get ready to take some measurements ...");
  delay(3000); // Delay
  for(int i = 0; i < samples; i++){
    Serial.print("Please place your LED light at ");
    Serial.print(fingerprint[i][1]);
    Serial.print(" meters");
    tone(9, 2000, 100);    
    delay(5000); // Delay
    Serial.println(" Measurements are taking ... Wait ...");
    delay(1000); // Delay
    resistance = getResistance();
    fingerprint[i][0] = resistance;
  }
  tone(9, 2000, 50);  
  delay(15);
  tone(9, 2000, 50);  
  delay(50);
  tone(9, 2000, 500);  
  Serial.println("Done!!!");
  // Print fingerprint database
  Serial.println("Fingerprint table is following:");
  Serial.println("Resistance (ohms)          Distance (meters");
  for(int i = 0; i < samples; i++){
    for(int j = 0; j < relations; j++){
      Serial.print(fingerprint[i][j]);
      Serial.print("               ");
    }
    Serial.println();
  }
  offlinePhase = false;
}

double getResistance(){
  double r = 0;
  for(int i = 0; i < samples; i++){
    r += analogRead(pResistor);
  }
  return (r / samples);
}

void getSumMatrixValues(){
  for(int i = 0; i < samples; i++){
    sum_x += fingerprint[i][0];
    sum_x_2 += pow(fingerprint[i][0], 2);
    sum_y += fingerprint[i][1];
    sum_log_y += log(fingerprint[i][1]);
    sum_xy += fingerprint[i][0] * fingerprint[i][1];
    sum_log_xy += fingerprint[i][0] * log(fingerprint[i][1]);
    sum_x_3 += pow(fingerprint[i][0], 3);
    sum_x_4 += pow(fingerprint[i][0], 4);
    sum_x_2_y += pow(fingerprint[i][0], 2) * fingerprint[i][1];
  }
  return;
}

void getLinearRegression(){
  linear_values[a] = ((sum_x_2 * sum_y - sum_x * sum_xy) * 1.0 / (samples * sum_x_2 - sum_x * sum_x) * 1.0);
  linear_values[b] = ((samples * sum_xy - sum_x * sum_y) * 1.0 / (samples * sum_x_2 - sum_x * sum_x) * 1.0);
  getRSquaredValue(linear_values, 'L');
  Serial.println("Linear Regression model is: ");
  Serial.print("y = ");
  Serial.print(linear_values[a]);
  Serial.print(" + ");
  Serial.print(linear_values[b]);
  Serial.println("*x");
  Serial.println("R squared value is: ");
  Serial.print(linear_values[r_2]);
  Serial.println();
  return;
}

void getPolynomialRegression(){
  double calculation_array[3][4]= {0};
  double temp = 0;
  calculation_array[0][0] = samples;
  calculation_array[0][1] = sum_x;
  calculation_array[0][2] = sum_x_2;
  calculation_array[0][3] = sum_y;
  calculation_array[1][0] = sum_x;
  calculation_array[1][1] = sum_x_2;
  calculation_array[1][2] = sum_x_3;
  calculation_array[1][3] = sum_xy;
  calculation_array[2][0] = sum_x_2;
  calculation_array[2][1] = sum_x_3;
  calculation_array[2][2] = sum_x_4;
  calculation_array[2][3] = sum_x_2_y;
  for(int k = 0; k < 3; k++){
    for(int i = 0; i < 3; i++){
      if(k != i){
        temp = calculation_array[i][k]/calculation_array[k][k];
      }
      for(int j = k; j < 4; j++){
        calculation_array[i][j] = calculation_array[i][j] - temp * calculation_array[k][j];
      }
    }
  }
  polynomial_valaues[a] = calculation_array[a][3]/calculation_array[a][a];
  polynomial_valaues[b] = calculation_array[b][3]/calculation_array[b][b];
  polynomial_valaues[c] = calculation_array[c][3]/calculation_array[c][c];
  getRSquaredValue(polynomial_valaues, 'P');
  Serial.println("Polynomial Regression model is: ");
  Serial.print("y = ");
  Serial.print(polynomial_valaues[a]);
  Serial.print(" + ");
  Serial.print(polynomial_valaues[b]);
  Serial.print("*x + ");
  Serial.print(polynomial_valaues[c]);
  Serial.println("*x^2");
  Serial.println("R squared value is: ");
  Serial.print(polynomial_valaues[r_2]);
  Serial.println();
  return;
}

void getExponentialRegression(){
  exponential_valaues[a] = exp(((sum_x_2 * sum_log_y - sum_x * sum_log_xy) *1.0 / (samples * sum_x_2 - sum_x * sum_x) * 1.0));
  exponential_valaues[b] = ((samples * sum_log_xy - sum_x * sum_log_y) * 1.0 / (samples * sum_x_2 - sum_x * sum_x) * 1.0);
  getRSquaredValue(exponential_valaues, 'E');
  Serial.println("Exponential Regression model is: ");
  Serial.print("y = ");
  Serial.print(exponential_valaues[a]);
  Serial.print(" * e^(");
  Serial.print(exponential_valaues[b]);
  Serial.println("*x)");
  Serial.println("R squared value is: ");
  Serial.print(exponential_valaues[r_2]);
  Serial.println();
  return;
}

void getRSquaredValue(double* array_values, char regression){
  double y_hat = 0;
  double sum_y_hat = 0;
  double sum_error_sq_value = 0;
  double sum_sq_from_mean_y = 0;
  double mean_y = 0;
  for(int i = 0; i < samples; i++){
    mean_y += fingerprint[i][1];
  }
  mean_y = mean_y / samples;
  for(int i = 0; i < samples; i++){
    switch(regression) {
      case 'L':
        y_hat = array_values[a] + array_values[b] * fingerprint[i][0];
        break;
      case 'P':
        y_hat = array_values[a] + array_values[b] * fingerprint[i][0] + array_values[c] * pow(fingerprint[i][0], 2);
        break;
      case 'E':
        y_hat = array_values[a] * exp(array_values[b] * fingerprint[i][0]);
        break;
     default:
        y_hat = 0;
    }
    sum_y_hat += y_hat;
    sum_error_sq_value += pow((fingerprint[i][1] - y_hat), 2);
    sum_sq_from_mean_y += pow((fingerprint[i][1] - mean_y), 2);
  }
  array_values[r_2] = 1 - (sum_error_sq_value / sum_sq_from_mean_y);
  return;
}

void getBestRSquaredValue(){
  if(linear_values[r_2] >= polynomial_valaues[r_2] && linear_values[r_2] >= exponential_valaues[r_2]){
    Serial.println("The Best Regression model is Linear");
    regressionModel = 'L';
  }
  else if(polynomial_valaues[r_2] >= linear_values[r_2] && polynomial_valaues[r_2] >= exponential_valaues[r_2]){
    Serial.println("The Best Regression model is Polynomial");
    regressionModel = 'P';
  }
  else{
    Serial.println("The Best Regression model is Exponential");
    regressionModel = 'E';
  }
  return;
}
