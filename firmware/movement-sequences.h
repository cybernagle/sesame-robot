#pragma once

#include <Arduino.h>

enum ServoName : uint8_t {
  R1 = 0,
  R2 = 1,
  L1 = 2,
  L2 = 3,
  R4 = 4,
  R3 = 5,
  L3 = 6,
  L4 = 7
};

const String ServoNames[]={"R1","R2","L1","L2","R4","R3","L3","L4"};

// ====== SEMANTIC TYPES (declarations only) ======
enum Leg : uint8_t { FL, FR, RL, RR };
enum Joint : uint8_t { HIP, KNEE };

struct ServoConfig {
  uint8_t channel;
  int stand;
  int safeMin;
  int safeMax;
  int8_t direction;
};

// Direction table:
//   HIP  +offset: FL=backward, FR=forward, RL=backward, RR=forward
//   KNEE +offset: FL=extend,   FR=retract, RL=retract,  RR=extend
// moveJoint constrains to safeMin/safeMax automatically.
const ServoConfig legConfig[4][2] = {
  {{0, 52, 0, 90, -1},  {1, 70, 10, 180, +1}},
  {{2, 128, 100, 180, +1}, {3, 110, 0, 170, -1}},
  {{4, 128, 100, 180, -1}, {5, 110, 0, 170, -1}},
  {{7, 52, 0, 90, +1},  {6, 70, 70, 70, +1}},   // RR KNEE locked (L3 defective)
};

void moveJoint(Leg leg, Joint joint, int offset);
// ====== END SEMANTIC TYPES ======

inline int servoNameToIndex(const String& servo) {
  if (servo == "L1") return L1;
  if (servo == "L2") return L2;
  if (servo == "L3") return L3;
  if (servo == "L4") return L4;
  if (servo == "R1") return R1;
  if (servo == "R2") return R2;
  if (servo == "R3") return R3;
  if (servo == "R4") return R4;
  return -1;
}

enum FaceAnimMode : uint8_t {
  FACE_ANIM_LOOP = 0,
  FACE_ANIM_ONCE = 1,
  FACE_ANIM_BOOMERANG = 2
};

// External globals and helpers used by movement/pose sequences
extern int frameDelay;
extern int walkCycles;
extern String currentCommand;

extern void setServoAngle(uint8_t channel, int angle);
extern void setFace(const String& faceName);
extern void setFaceMode(FaceAnimMode mode);
extern void setFaceWithMode(const String& faceName, FaceAnimMode mode);
extern void delayWithFace(unsigned long ms);
extern void enterIdle();
extern bool pressingCheck(String cmd, int ms);

// Pose/animation prototypes
void runRestPose();
void runStandPose(int face = 1);
void runWavePose();
void runDancePose();
void runSwimPose();
void runPointPose();
void runPushupPose();
void runBowPose();
void runCutePose();
void runFreakyPose();
void runWormPose();
void runShakePose();
void runShrugPose();
void runDeadPose();
void runCrabPose();
void runWalkPose();
void runWalkBackward();
void runTurnLeft();
void runTurnRight();

// ====== POSES ======

inline void runRestPose() {
  Serial.println(F("REST"));
  setFaceWithMode("rest", FACE_ANIM_BOOMERANG);
  for (int i = 0; i < 8; i++) setServoAngle(i, 90);
}

inline void runStandPose(int face) {
  Serial.println(F("STAND"));
  if (face == 1) setFaceWithMode("stand", FACE_ANIM_ONCE);
  moveJoint(FL, HIP, 0); moveJoint(FL, KNEE, 0);
  moveJoint(FR, HIP, 0); moveJoint(FR, KNEE, 0);
  moveJoint(RL, HIP, 0); moveJoint(RL, KNEE, 0);
  moveJoint(RR, HIP, 0); moveJoint(RR, KNEE, 0);
  if (face == 1) enterIdle();
}

inline void runWavePose() {
  Serial.println(F("WAVE"));
  setFaceWithMode("wave", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  moveJoint(FL, KNEE, +100);
  delayWithFace(300);
  for (int i = 0; i < 4; i++) {
    moveJoint(FL, HIP, +20); delayWithFace(300);
    moveJoint(FL, HIP, -20); delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "wave") currentCommand = "";
}

inline void runBowPose() {
  Serial.println(F("BOW"));
  setFaceWithMode("bow", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  // Head down: front hips forward, front knees retract
  moveJoint(FL, HIP, -30); moveJoint(FR, HIP, +30);
  moveJoint(FL, KNEE, -40); moveJoint(FR, KNEE, +40);
  delayWithFace(600);
  moveJoint(FL, HIP, -38); moveJoint(FR, HIP, +52);
  moveJoint(FL, KNEE, -60); moveJoint(FR, KNEE, +60);
  delayWithFace(3000);
  runStandPose(1);
  if (currentCommand == "bow") currentCommand = "";
}

// Dance offsets (converted from raw angles):
//   R1=90→FL HIP -38, R2=90→FL KNEE +20
//   L1=90→FR HIP -38, L2=90→FR KNEE +20
//   R4=160→RL HIP -32, R3=160→RL KNEE -50
//   L3=10→RR KNEE -60, L4=10→RR HIP -42
inline void runDancePose() {
  Serial.println(F("DANCE"));
  setFaceWithMode("dance", FACE_ANIM_LOOP);
  moveJoint(FL, HIP, -38); moveJoint(FL, KNEE, +20);
  moveJoint(FR, HIP, -38); moveJoint(FR, KNEE, +20);
  moveJoint(RL, HIP, -32); moveJoint(RL, KNEE, -50);
  moveJoint(RR, HIP, -42); moveJoint(RR, KNEE, -60);
  delayWithFace(300);
  for (int i = 0; i < 5; i++) {
    moveJoint(RL, HIP, +13); moveJoint(RL, KNEE, -5);
    moveJoint(RR, HIP, -42); moveJoint(RR, KNEE, -60);
    delayWithFace(300);
    moveJoint(RL, HIP, -32); moveJoint(RL, KNEE, -50);
    moveJoint(RR, HIP, +13); moveJoint(RR, KNEE, -5);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "dance") currentCommand = "";
}

// Swim: front legs paddle, rear stays centered
//   R1=45→FL HIP +7, R2=135→FL KNEE +65
//   L1=135→FR HIP +7, L2=135→FR KNEE -25
inline void runSwimPose() {
  Serial.println(F("SWIM"));
  setFaceWithMode("swim", FACE_ANIM_ONCE);
  for (int i = 0; i < 8; i++) setServoAngle(i, 90);
  for (int i = 0; i < 4; i++) {
    moveJoint(FL, HIP, +7); moveJoint(FL, KNEE, +65);
    moveJoint(FR, HIP, +7); moveJoint(FR, KNEE, -25);
    delayWithFace(400);
    setServoAngle(R1, 90); setServoAngle(R2, 90);
    setServoAngle(L1, 90); setServoAngle(L2, 90);
    delayWithFace(400);
  }
  runStandPose(1);
  if (currentCommand == "swim") currentCommand = "";
}

// Point offsets:
//   L2=90→FR KNEE +20, R1=45→FL HIP +7, R2=80→FL KNEE +10
//   L4=180→RR HIP clamped +38, L1=155→FR HIP +27
//   L3=145→RR KNEE +75, R4=80→RL HIP +48, R3=170→RL KNEE -60
inline void runPointPose() {
  Serial.println(F("POINT"));
  setFaceWithMode("point", FACE_ANIM_BOOMERANG);
  moveJoint(FR, KNEE, +20);
  moveJoint(FL, HIP, +7); moveJoint(FL, KNEE, +10);
  moveJoint(RR, HIP, +38); moveJoint(FR, HIP, +27);
  moveJoint(RR, KNEE, +75); moveJoint(RL, HIP, +48);
  moveJoint(RL, KNEE, -60);
  delayWithFace(2000);
  runStandPose(1);
  if (currentCommand == "point") currentCommand = "";
}

inline void runPushupPose() {
  Serial.println(F("PUSHUP"));
  setFaceWithMode("pushup", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  for (int i = 0; i < 4; i++) {
    // All knees retract → body down
    moveJoint(FL, KNEE, -40); moveJoint(FR, KNEE, +40);
    moveJoint(RL, KNEE, +40); moveJoint(RR, KNEE, -40);
    delayWithFace(600);
    // All knees extend → body up
    moveJoint(FL, KNEE, +40); moveJoint(FR, KNEE, -40);
    moveJoint(RL, KNEE, -40); moveJoint(RR, KNEE, +40);
    delayWithFace(500);
  }
  runStandPose(1);
  if (currentCommand == "pushup") currentCommand = "";
}

// Cute offsets:
//   L2=160→FR KNEE -50, R2=160→FL KNEE +90
//   R4=180→RL HIP -52, L4=0→RR HIP -52
//   L1=180→FR HIP +52, R1=0→FL HIP +52
//   L3=180→RR KNEE +110, R3=0→RL KNEE +110
inline void runCutePose() {
  Serial.println(F("CUTE"));
  setFaceWithMode("cute", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  moveJoint(FR, KNEE, -50); moveJoint(FL, KNEE, +90);
  moveJoint(RL, HIP, -52); moveJoint(RR, HIP, -52);
  moveJoint(FR, HIP, +52); moveJoint(FL, HIP, +52);
  moveJoint(RR, KNEE, +110); moveJoint(RL, KNEE, +110);
  delayWithFace(200);
  for (int i = 0; i < 5; i++) {
    moveJoint(RL, HIP, -52); moveJoint(RR, HIP, -7);
    delayWithFace(300);
    moveJoint(RL, HIP, -7); moveJoint(RR, HIP, -52);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "cute") currentCommand = "";
}

// Freaky offsets:
//   L1=180→FR HIP +52, R1=0→FL HIP +52
//   L2=180→FR KNEE -70(clamped), R2=180→FL KNEE +110
//   R4=90→RL HIP +38, R3=0→RL KNEE +110
//   R3=25→RL KNEE +85
inline void runFreakyPose() {
  Serial.println(F("FREAKY"));
  setFaceWithMode("freaky", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  moveJoint(FR, HIP, +52); moveJoint(FL, HIP, +52);
  moveJoint(FR, KNEE, -70); moveJoint(FL, KNEE, +110);
  moveJoint(RL, HIP, +38); moveJoint(RL, KNEE, +110);
  delayWithFace(200);
  for (int i = 0; i < 3; i++) {
    moveJoint(RL, KNEE, +85); delayWithFace(400);
    moveJoint(RL, KNEE, +110); delayWithFace(400);
  }
  runStandPose(1);
  if (currentCommand == "freaky") currentCommand = "";
}

// Worm offsets:
//   R1=0→FL HIP +52, R2=180→FL KNEE +110
//   L1=180→FR HIP +52, L2=180→FR KNEE -70(clamped)
//   R4=90→RL HIP +38, R3=90→RL KNEE +20
//   L3=90→RR KNEE +20, L4=90→RR HIP +38
inline void runWormPose() {
  Serial.println(F("WORM"));
  setFaceWithMode("worm", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  moveJoint(FL, HIP, +52); moveJoint(FL, KNEE, +110);
  moveJoint(FR, HIP, +52); moveJoint(FR, KNEE, -60);
  moveJoint(RL, HIP, +38); moveJoint(RL, KNEE, +20);
  moveJoint(RR, HIP, +38); moveJoint(RR, KNEE, +20);
  delayWithFace(200);
  for(int i=0; i<5; i++) {
    moveJoint(RL, HIP, -30); moveJoint(RR, HIP, +30);
    moveJoint(RL, KNEE, +40); moveJoint(RR, KNEE, -40);
    delayWithFace(300);
    moveJoint(RL, HIP, +20); moveJoint(RR, HIP, -30);
    moveJoint(RL, KNEE, -40); moveJoint(RR, KNEE, +40);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "worm") currentCommand = "";
}

// Shake offsets:
//   R1=45→FL HIP +7, L1=135→FR HIP +7
//   L3=90→RR KNEE +20, R3=90→RL KNEE +20
//   L2=90→FR KNEE +20, R2=90→FL KNEE +20
//   R4=45→RL HIP -7, L4=135→RR HIP +83(clamped→+38)
//   R4=0→RL HIP +128(clamped→+28), L4=180→RR HIP +128(clamped→+38)
inline void runShakePose() {
  Serial.println(F("SHAKE"));
  setFaceWithMode("shake", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  moveJoint(FL, HIP, +7); moveJoint(FR, HIP, +7);
  moveJoint(RR, KNEE, +20); moveJoint(RL, KNEE, +20);
  moveJoint(FR, KNEE, +20); moveJoint(FL, KNEE, +20);
  delayWithFace(200);
  for(int i=0; i<5; i++) {
    moveJoint(RL, HIP, -7); moveJoint(RR, HIP, +38);
    delayWithFace(300);
    moveJoint(RL, HIP, +28); moveJoint(RR, HIP, +38);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "shake") currentCommand = "";
}

// Shrug offsets:
//   R3=90,R4=90,L3=90,L4=90 → all rear at ~center
//   R3=0→RL KNEE +110, R4=180→RL HIP -52(clamped→-52)
//   L3=180→RR KNEE +110, L4=0→RR HIP -52
inline void runShrugPose() {
  Serial.println(F("SHRUG"));
  runStandPose(0);
  setFaceWithMode("dead", FACE_ANIM_ONCE);
  delayWithFace(200);
  moveJoint(RL, KNEE, +20); moveJoint(RL, HIP, +38);
  moveJoint(RR, KNEE, +20); moveJoint(RR, HIP, +38);
  delayWithFace(1000);
  setFaceWithMode("shrug", FACE_ANIM_ONCE);
  moveJoint(RL, KNEE, +110); moveJoint(RL, HIP, -52);
  moveJoint(RR, KNEE, +110); moveJoint(RR, HIP, -52);
  delayWithFace(1500);
  runStandPose(1);
  if (currentCommand == "shrug") currentCommand = "";
}

inline void runDeadPose() {
  Serial.println(F("DEAD"));
  runStandPose(0);
  setFaceWithMode("dead", FACE_ANIM_BOOMERANG);
  delayWithFace(200);
  moveJoint(RL, KNEE, +20); moveJoint(RL, HIP, +38);
  moveJoint(RR, KNEE, +20); moveJoint(RR, HIP, +38);
  if (currentCommand == "dead") currentCommand = "";
}

// Crab offsets:
//   R1=90→FL HIP -38, R2=90→FL KNEE +20
//   L1=90→FR HIP -38, L2=90→FR KNEE +20
//   R4=0→RL HIP +128(clamped→+28), R3=180→RL KNEE -70(clamped→-60)
//   L3=45→RR KNEE -25, L4=135→RR HIP +83(clamped→+38)
inline void runCrabPose() {
  Serial.println(F("CRAB"));
  setFaceWithMode("crab", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  moveJoint(FL, HIP, -38); moveJoint(FL, KNEE, +20);
  moveJoint(FR, HIP, -38); moveJoint(FR, KNEE, +20);
  moveJoint(RL, HIP, +28); moveJoint(RL, KNEE, -60);
  moveJoint(RR, KNEE, -25); moveJoint(RR, HIP, +38);
  for(int i=0; i<5; i++) {
    moveJoint(RL, HIP, -7); moveJoint(RL, KNEE, -40);
    moveJoint(RR, KNEE, -60); moveJoint(RR, HIP, +38);
    delayWithFace(300);
    moveJoint(RL, HIP, +28); moveJoint(RL, KNEE, -60);
    moveJoint(RR, KNEE, -25); moveJoint(RR, HIP, +38);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "crab") currentCommand = "";
}

// --- WALKING ANIMATIONS (use raw angles, need re-tuning for new config) ---
inline void runWalkPose() {
  Serial.println(F("WALK FWD"));
  setFaceWithMode("walk", FACE_ANIM_ONCE);
  // Initial Step
  setServoAngle(R3, 135); setServoAngle(L3, 70);
  setServoAngle(R2, 80); setServoAngle(L1, 155);
  if (!pressingCheck("forward", frameDelay)) return;

  for (int i = 0; i < walkCycles; i++) {
    setServoAngle(R3, 135); setServoAngle(L3, 70);
    if (!pressingCheck("forward", frameDelay)) return;
    setServoAngle(L4, 135); setServoAngle(L2, 90);
    setServoAngle(R4, 0); setServoAngle(R1, 0);
    if (!pressingCheck("forward", frameDelay)) return;
    setServoAngle(R2, 135); setServoAngle(L1, 90);
    if (!pressingCheck("forward", frameDelay)) return;
    setServoAngle(R4, 45); setServoAngle(L4, 180);
    if (!pressingCheck("forward", frameDelay)) return;
    setServoAngle(R3, 180); setServoAngle(L3, 70);
    setServoAngle(R2, 90); setServoAngle(L1, 180);
    if (!pressingCheck("forward", frameDelay)) return;
    setServoAngle(L2, 135); setServoAngle(R1, 90);
    if (!pressingCheck("forward", frameDelay)) return;
  }
  runStandPose(1);
}

// Logic reversed from Walk
inline void runWalkBackward() {
  Serial.println(F("WALK BACK"));
  setFaceWithMode("walk", FACE_ANIM_ONCE);
  if (!pressingCheck("backward", frameDelay)) return;

  for (int i = 0; i < walkCycles; i++) {
    setServoAngle(R3, 135); setServoAngle(L3, 70);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(L4, 135); setServoAngle(L2, 135);
    setServoAngle(R4, 0); setServoAngle(R1, 90);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(R2, 90); setServoAngle(L1, 180);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(R4, 45); setServoAngle(L4, 180);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(R3, 180); setServoAngle(L3, 70);
    setServoAngle(R2, 135); setServoAngle(L1, 90);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(L2, 90); setServoAngle(R1, 0);
    if (!pressingCheck("backward", frameDelay)) return;
  }
  runStandPose(1);
}

// Simple turn logic
inline void runTurnLeft() {
  Serial.println(F("TURN LEFT"));
  setFaceWithMode("walk", FACE_ANIM_ONCE);
  for (int i = 0; i < walkCycles; i++) {
    setServoAngle(R3, 135); setServoAngle(L4, 135);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R1, 0); setServoAngle(L2, 180);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R3, 180); setServoAngle(L4, 180);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R1, 45); setServoAngle(L2, 135);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R4, 45); setServoAngle(L3, 70);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R2, 90); setServoAngle(L1, 90);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R4, 0); setServoAngle(L3, 70);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R2, 135); setServoAngle(L1, 135);
    if (!pressingCheck("left", frameDelay)) return;
  }
  runStandPose(1);
}

inline void runTurnRight() {
  Serial.println(F("TURN RIGHT"));
  setFaceWithMode("walk", FACE_ANIM_ONCE);
  for (int i = 0; i < walkCycles; i++) {
    setServoAngle(R4, 45); setServoAngle(L3, 70);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R2, 180); setServoAngle(L1, 180);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R4, 0); setServoAngle(L3, 70);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R2, 135); setServoAngle(L1, 135);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R3, 135); setServoAngle(L4, 135);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R1, 90); setServoAngle(L2, 90);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R3, 180); setServoAngle(L4, 180);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R1, 45); setServoAngle(L2, 135);
    if (!pressingCheck("right", frameDelay)) return;
  }
  runStandPose(1);
}
