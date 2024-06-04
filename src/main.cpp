#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>

#include <math.h>

const int thermistorPin = 4; // Thermistor pin (analog input)
const int relayPin = 35; // Relay control pin
const int seriesResistor = 10000; // 10kΩ resistor
const int nominalResistance = 10000; // Resistance of thermistor at 25°C
const int nominalTemperature = 25; // Nominal temperature (usually 25°C)
const int betaCoefficient = 3950; // Beta coefficient of the thermistor
const int adcMax = 4095; // Maximum ADC value (12-bit ADC for ESP32)

const float relayOnTemperatureF = 195.0; // Relay ON temperature in Fahrenheit
const float relayOffTemperatureF = 175.0; // Relay OFF temperature in Fahrenheit
const float vcc = 5.0; // Supply voltage (5V)

// Adjustment factor for temperature offset (add 10°F)
const float temperatureOffset = 20.0;

const int numReadings = 5; // Number of readings to average
float readings[numReadings]; // Array to store readings
int readingIndex = 0; // Index for storing readings
float total = 0; // Total of readings

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT); // Set relay pin as output
  digitalWrite(relayPin, LOW); // Ensure the relay is off initially
}

void loop() {
  // Read the ADC value from the thermistor
  int adcValue = analogRead(thermistorPin);
  
  // Convert the ADC value to voltage (assuming Vref = 5V)
  float voltage = adcValue * (vcc / adcMax);

  // Convert the voltage to resistance using voltage divider formula
  float resistance = seriesResistor * ((vcc / voltage) - 1);

  // Calculate temperature in Kelvin using the Beta parameter equation
  float temperatureKelvin = 1.0 / (1.0 / (nominalTemperature + 273.15) + log(resistance / nominalResistance) / betaCoefficient);

  // Convert temperature to Celsius
  float temperatureCelsius = temperatureKelvin - 273.15;
  
  // Convert temperature to Fahrenheit and add offset
  float temperatureFahrenheit = (temperatureCelsius * 9.0 / 5.0 + 32.0) + temperatureOffset;

  // Add the new reading to the total
  total = total - readings[readingIndex] + temperatureFahrenheit;
  
  // Store the new reading
  readings[readingIndex] = temperatureFahrenheit;
  
  // Increment the index
  readingIndex = (readingIndex + 1) % numReadings;
  
  // Calculate the average temperature
  float averageTemperature = total / numReadings;

  // Print the temperature in Celsius and Fahrenheit
  Serial.print("Temperature: ");
  Serial.print(temperatureCelsius);
  Serial.print(" °C, ");
  Serial.print(averageTemperature);
  Serial.println(" °F");

  // Relay control logic
  if (averageTemperature >= relayOnTemperatureF) {
    digitalWrite(relayPin, HIGH); // Turn relay on
  } else if (averageTemperature <= relayOffTemperatureF) {
    digitalWrite(relayPin, LOW); // Turn relay off
  }

  delay(1000); // Delay for readability
}