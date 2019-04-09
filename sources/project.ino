#include <Arduino.h>

#include <WMHead.h>
#include <WMBoard.h>

#define DISTANCE_ARR_LEN 5            // Chuỗi khoảng cách thu vào để khử nhiễu
#define STABLE_DISTANCE_MAX_DIFF 50   // Độ khác biệt lớn nhất cho phép giữa các giá trị khoảng cách để được coi là chuỗi ổn định
#define MIN_DISTANCE_RANGE_MIN 9      // Khoảng cách nhỏ nhất để xe bắt đầu đi lùi lại
#define MIN_DISTANCE_RANGE_MAX 11     // Khoảng cách nhỏ nhất để xe bắt đầu đi tiến lên
#define MAX_DETECTABLE_DISTANCE 50    // Khoảng cách lớn nhất mà xe có thể nhận diện vật thể

WMDCMotor motor1(8);                  // Bánh xe 1
WMDCMotor motor2(7);                  // Bánh xe 2

boolean doDetectObject = 1;

void setup()
{
  pinMode(TRIG, OUTPUT);  // chân trig sẽ phát tín hiệu
  pinMode(ECHO, INPUT);   // chân echo sẽ nhận tín hiệu
}

void loop()
{
  unsigned long distance = measureDistance();     // Đo khoảng cách tới vật phía trước
  
  if (distance > MIN_DISTANCE_RANGE_MIN && distance < MIN_DISTANCE_RANGE_MAX) {   // Nếu vật cách 1 khoảng vừa phải
    stopMoving();
  } else if (distance < MIN_DISTANCE_RANGE_MIN) {                                 // Nếu vật quá gần
    moveBackward(30);
  } else if (distance > MIN_DISTANCE_RANGE_MAX && distance < MAX_DETECTABLE_DISTANCE) {   // Nếu vật trong khoảng nhìn thấy nhưng cách quá xa
    moveForward(30);
    doDetectObject = 1;
  } else if (distance > MAX_DETECTABLE_DISTANCE) {                                // Nếu không tìm thấy vật
    if(!detectObject()) {           // Thử tìm vật
      doDetectObject = 0;
      stopMoving();
    }
  }
}

// Trả về khoảng cách tới vật gần nhất
unsigned long measureDistance() {
  unsigned long duration; // biến đo thời gian
  unsigned long distances[DISTANCE_ARR_LEN];
  unsigned long distance;           // biến lưu khoảng cách
 
  for(int i = 0; i < DISTANCE_ARR_LEN; i++){
    /* Phát xung từ chân trig */
    digitalWrite(TRIG, LOW);   // tắt chân trig
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);   // phát xung từ chân trig
    delayMicroseconds(10);   // xung có độ dài 5 microSeconds
    digitalWrite(TRIG, LOW);   // tắt chân trig
 
    /* Tính toán thời gian */
    // Đo độ rộng xung HIGH ở chân echo.
    duration = pulseIn(ECHO, HIGH);
    // Tính khoảng cách đến vật.
    distance = duration / 59;
 
    distances[i] = distance;
  }
 
  if(!checkStableDistance(distances))  // Khử nhiễu
    return measureDistance();
 
  distance = distances[DISTANCE_ARR_LEN - 1];
  matrixLed.showNum(0, distance);
  return distance;
}

// Dừng xe
void stopMoving() {
  motor1.reverseRun(0);
  motor2.reverseRun(0);
}
 
// Tiến lên trước
// params: speed tốc độ quay của bánh xe (vòng/phút)
void moveForward(int speed) {
  motor1.reverseRun(speed);
  motor2.reverseRun(speed);
}
 
// Lùi lại
// params: speed tốc độ quay của bánh xe (vòng/phút)
void moveBackward(int speed) {
  motor1.reverseRun(-speed);
  motor2.reverseRun(-speed);
}

// Xoay xe +-TURN_ANGLE độ để tìm kiếm vật
boolean detectObject() {  
  if(!doDetectObject) return 0;
  
  return 1;
}
