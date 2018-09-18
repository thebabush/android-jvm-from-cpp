#include <iostream>
#include <unistd.h>

#include <jni.h>

#include "hack.h"


using namespace std;


const char *TAG = "HACK";
const char *MSG = "HELLO HACK!";


int main(int argc, char *argv[]) {
  JavaVM* vm = NULL;
  JNIEnv* env = NULL;

  // Create an Android JVM
  int err = JvmCreate(&vm, &env);
  if (err != 0) {
    cout << "init_jvm: " << err << endl;
  }

  // Get a reference to android.util.Log
  jclass Log_class = env->FindClass("android/util/Log");
  if (Log_class == 0) {
    cout << "Error finding android.util.Log" << endl;
    return -10;
  }

  // Get Log.d(String, String)
  jmethodID Log_d = env->GetStaticMethodID(Log_class, "d", "(Ljava/lang/String;Ljava/lang/String;)I");
  if (Log_d == 0) {
    cout << "Error getting android.util.Log.d(String, String)" << endl;
    return -11;
  }

  // There might be some race-condition :/
  // Sometimes the message doesn't show up in the logcat
  sleep(1);

  // Convert native strings to java objects
  jstring tag = env->NewStringUTF(TAG);
  jstring msg = env->NewStringUTF(MSG);

  // Log.d(tag, msg)
  env->CallStaticIntMethod(Log_class, Log_d, tag, msg);

  // Print to console
  cout << "\"adb logcat -s HACK\" should now show something :)" << endl;

  // Free the memory
  env->ReleaseStringUTFChars(tag, NULL);
  env->ReleaseStringUTFChars(msg, NULL);

  return 0;
}

