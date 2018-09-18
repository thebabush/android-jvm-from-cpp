#include <dlfcn.h>
#include <jni.h>


typedef int (*JNI_CreateJavaVM_t)(void *, void *, void *);
typedef jint (*registerNatives_t)(JNIEnv* env);


int JvmCreate(JavaVM **p_vm, JNIEnv **p_env) {
  /*
   * Modified from: https://calebfenton.github.io/2017/04/05/creating_java_vm_from_android_native_code/
   *
   * Differences:
   *
   * 1. registerFrameworkNatives got changed
   * 2. libsigchain must be linked statically otherwise libsigchain_dummy.so
   *    aborts at runtime
   *
   * So, if you really want to use this you should probably figure out which
   * version of libsigchain you need to download.
   * The latest version uses C++17 kung-fu and doesn't compile with the g++
   * bundled on the NDK I'm using.
   *
   * P.S.: Google is still using "abort" in programs? I feel so threatened and
   *       not included ![sadface.jpg](sadface.jpg "sadface.jpg")
   */

  JavaVMOption opt[4];
  opt[0].optionString = "-Djava.class.path=/data/local/tmp/shim_app.apk";
  opt[1].optionString = "-agentlib:jdwp=transport=dt_android_adb,suspend=n,server=y";
  opt[2].optionString = "-Djava.library.path=/data/local/tmp";
  opt[3].optionString = "-verbose:jni"; // may want to remove this, it's noisy

  JavaVMInitArgs args;
  args.version = JNI_VERSION_1_6;
  args.options = opt;
  args.nOptions = 4;
  args.ignoreUnrecognized = JNI_FALSE;

  void *libandroid_runtime_dso = dlopen("libandroid_runtime.so", RTLD_NOW);
  void *libdvm_dso = dlopen("libart.so", RTLD_NOW);
  if (!libdvm_dso || !libandroid_runtime_dso) {
    return -1;
  }
  
  JNI_CreateJavaVM_t JNI_CreateJavaVM;
  JNI_CreateJavaVM = (JNI_CreateJavaVM_t) dlsym(libdvm_dso, "JNI_CreateJavaVM");
  if (!JNI_CreateJavaVM) {
    return -2;
  }

  if (JNI_CreateJavaVM(&(*p_vm), &(*p_env), &args)) {
    return -4;
  }

  registerNatives_t registerNatives;
  registerNatives = (registerNatives_t) dlsym(libandroid_runtime_dso, "registerFrameworkNatives");
  
  if (!registerNatives) {
    return -3;
  }
  
  if (registerNatives(*p_env)) {
    return -5;
  }

  return 0;
}

