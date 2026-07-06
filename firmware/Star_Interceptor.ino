/*
███████╗████████╗ █████╗ ██████╗     ██╗███╗   ██╗████████╗███████╗██████╗  ██████╗███████╗██████╗ ████████╗ ██████╗ ██████╗ 
██╔════╝╚══██╔══╝██╔══██╗██╔══██╗    ██║████╗  ██║╚══██╔══╝██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗╚══██╔══╝██╔═══██╗██╔══██╗
███████╗   ██║   ███████║██████╔╝    ██║██╔██╗ ██║   ██║   █████╗  ██████╔╝██║     █████╗  ██████╔╝   ██║   ██║   ██║██████╔╝
╚════██║   ██║   ██╔══██║██╔══██╗    ██║██║╚██╗██║   ██║   ██╔══╝  ██╔══██╗██║     ██╔══╝  ██╔═══╝    ██║   ██║   ██║██╔══██╗
███████║   ██║   ██║  ██║██║  ██║    ██║██║ ╚████║   ██║   ███████╗██║  ██║╚██████╗███████╗██║        ██║   ╚██████╔╝██║  ██║
╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝    ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝╚═╝  ╚═╝ ╚═════╝╚══════╝╚═╝        ╚═╝    ╚═════╝ ╚═╝  ╚═╝                                                                                                                           
*/





//MS1 repin
#include <BLEDevice.h>
#define SERVICE_UUID        "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

#include <thread>
#include <future>
#include <atomic>
#include <ESP32Servo.h>
#define SERVO_PIN 4
Servo servoMotor;

#include <FastAccelStepper.h>
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;



int led = 2;

// = 0,5n
float stepAngle = 0.016071428571875;//0.5142857143;
int   speed     =     1;

float azimuth   = 0.0;
float elevation = 90.0;

//Stepper
int dir       = 26;
int step      = 18;
int sleepPin  = 33;
int reset     = 19;
int enablePin = 22;

int MS1       = 14;
int MS2       = 16;
int MS3       = 23;

static int n = 2; 
static float* floatArray;  

float azimuthal = 30.0;
float elevational  = 31.0;

float planet = 34.0;


/*
  _____                    __  .__                      
_/ ____\_ __  ____   _____/  |_|__| ____   ____   ______
\   __\  |  \/    \_/ ___\   __\  |/  _ \ /    \ /  ___/
 |  | |  |  /   |  \  \___|  | |  (  <_> )   |  \\___ \ 
 |__| |____/|___|  /\___  >__| |__|\____/|___|  /____  >
                 \/     \/                    \/     \/
*/
std::atomic<bool> keepRunning(true);

float calculateClosestApproach(float start_angle, float target_angle, float step_size) {
    float raw_diff = target_angle - start_angle;
    
    if (raw_diff > 180.0) {
        raw_diff -= 360.0;
    } else if (raw_diff < -180.0) {
        raw_diff += 360.0;
    }
    
    float nearest_step = round(raw_diff / step_size) * step_size;
    
    return nearest_step;
}

float* stringToFloatArray(const std::string& value) {
    if (value.empty()) {
        return nullptr;
    }

    const uint8_t* data = reinterpret_cast<const uint8_t*>(value.c_str());
    size_t dataSize = value.length();
    
    size_t numFloats = dataSize / sizeof(float);
    
    if (numFloats == 0) {
        return nullptr;
    }


    float* floatArray = new float[numFloats];
    memcpy(floatArray, data, numFloats * sizeof(float));
  
    for(size_t i = 0; i < numFloats; i++) {
        Serial.println(floatArray[i]);
    }
    
    return floatArray;
}

void turn(float angle, bool elevate){

  Serial.println("Turn Signal ====================================================");
  //Serial.println("angle: " + String(angle,4));
  //Serial.println("elevate: " + String(elevate,4));
  if (elevate)
  {
    if (angle > 90 && angle < 150){
      servoMotor.write(angle);
      elevation = angle;
      Serial.println("Elevation: " + String(elevation, 3));
    }
  } 
  else
  { 
    float clossestAngle = calculateClosestApproach(azimuth, angle, stepAngle);
    int steps = int(clossestAngle/stepAngle);
    azimuth = azimuth + clossestAngle;
    Serial.println("steps: " + String(steps) + "| angle: " + String(azimuth, 6) + "Azimuth");

    //digitalWrite(enablePin, LOW);
    stepper->move(steps);
    while(stepper->isRunning()) {
      yield();
    }

    //digitalWrite(enablePin, HIGH);

    digitalWrite(dir, LOW);
    digitalWrite(led, LOW);
  }
}

void trackObject(float* Angles, int speed) {
    auto nextTimePoint = std::chrono::steady_clock::now();
    int index = 1;
    while (keepRunning) {
        nextTimePoint += std::chrono::seconds(speed); 
        //subFunction(Angles[index], Angles[index+1]);
        Serial.println("Azimuth" + String(Angles[index + 1]));
        turn(Angles[index + 1], false);  // Elevation
        Serial.println("Elevation" + String(Angles[index]));
        turn(Angles[index], true);  
        
        index += 2;
        std::future<void> result = std::async(std::launch::async, [nextTimePoint]() {
            std::this_thread::sleep_until(nextTimePoint); 
        });
        result.get(); 
    }
}

/*
 _______   ___       _______  
|   _  "\ |"  |     /"     "| 
(. |_)  :)||  |    (: ______) 
|:     \/ |:  |     \/    |   
(|  _  \\  \  |___  // ___)_  
|: |_)  :)( \_|:  \(:      "| 
(_______/  \_______)\_______) 
                              
*/
BLECharacteristic* pCharacteristic;

void handleAsyncTask(float* Data, int speed) {
    auto asyncFuture = std::async(std::launch::async, trackObject, Data, speed); // Start the asynchronous function
}
class MyCallbacks : public BLECharacteristicCallbacks {
public:
    int speed = 10;
    float* Data;

    void onWrite(BLECharacteristic* characteristic) override {
        keepRunning = false;
        String arduinoString = pCharacteristic->getValue();
        std::string value(arduinoString.begin(), arduinoString.end());

        Data = stringToFloatArray(value);

        switch (static_cast<int>(Data[0])) {
    case 30:
        turn(azimuth + Data[1], false);
        Serial.println("Azimuth: HERE!!!!");
        break;
    case 31:{
        float finalElevation = elevation + Data[1];
        if (finalElevation < 170 && finalElevation > 90) {
            servoMotor.write(finalElevation);
            elevation = finalElevation;
            Serial.println("elevation: " + String(elevation, 6));
        }
        else {
            Serial.println("Elevation out of bounds!!!");
        }
        break;
    }
    case 32:{
      azimuth = Data[1];
      elevation = Data[2];
    }
    default: {
        if (Data[0] != planet) {
            planet = Data[0];
            keepRunning = true;

            if (Data != nullptr) {
                std::thread taskThread(handleAsyncTask, Data, speed);
                taskThread.detach(); 
            } else {
                Serial.println("Failed to parse data.");
            }
        }
    }   
    break;
}

    }
};

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        Serial.println("Device connected");
    }

    void onDisconnect(BLEServer* pServer) override {
        Serial.println("Device disconnected");
        pServer->startAdvertising();
    }
};

/*
               __                
  ______ _____/  |_ __ ________  
 /  ___// __ \   __\  |  \____ \ 
 \___ \\  ___/|  | |  |  /  |_> >
/____  >\___  >__| |____/|   __/ 
     \/     \/           |__|   
*/

void setup() {
    Serial.begin(115200);
    floatArray = (float*)malloc(n * sizeof(float));
    if (floatArray == NULL) {
        Serial.println("Memory allocation failed!");
        return;
    }

    BLEDevice::init("noscope");
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService* pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );

    pCharacteristic->setCallbacks(new MyCallbacks());
    pService->start();
    BLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();

    Serial.println("Characteristic defined! Now you can write data to it.");

    pinMode(led, OUTPUT);

    //Stepper
    pinMode(dir, OUTPUT);
    pinMode(step, OUTPUT);
    pinMode(sleepPin,OUTPUT);
    pinMode(reset, OUTPUT);
    pinMode(enablePin, OUTPUT);

    pinMode(MS1, OUTPUT);
    pinMode(MS2, OUTPUT);
    pinMode(MS3, OUTPUT);
    digitalWrite(MS1, HIGH);
    digitalWrite(MS2, HIGH);
    digitalWrite(MS3, HIGH);
    digitalWrite(sleepPin, HIGH);
    digitalWrite(reset, HIGH);


    engine.init();
    stepper = engine.stepperConnectToPin(step);
    stepper->setDirectionPin(dir);
    stepper->setSpeedInHz(5000);     
    stepper->setAcceleration(800);     
    stepper->enableOutputs();

  
    servoMotor.attach(SERVO_PIN);
    servoMotor.write(90);

}

void loop() {
}