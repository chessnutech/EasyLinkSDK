//
// Created by nightrider on 31.12.2024.
//

#include "EasyLink.h"
#include "EasyLink_java.h"
#include <assert.h>

jstring JNICALL Java_com_chessnut_EasyLink_version
  (JNIEnv *env, jclass)
{
    return env->NewStringUTF(ChessLink::CL_VERSION.c_str());
}

jint JNICALL Java_com_chessnut_EasyLink_connect (JNIEnv *, jclass)
{
  return ChessLink::instance()->connect();
}

void JNICALL Java_com_chessnut_EasyLink_disconnect (JNIEnv *, jclass)
{
  ChessLink::instance()->disconnect();
}

jint JNICALL Java_com_chessnut_EasyLink_switchRealTimeMode (JNIEnv *, jclass)
{
  return ChessLink::instance()->switchRealTimeMode();
}

jint JNICALL Java_com_chessnut_EasyLink_switchUploadMode (JNIEnv *, jclass)
{
  return ChessLink::instance()->switchUploadMode();
}

std::string current_fen;

void JNICALL Java_com_chessnut_EasyLink_setRealtimeCallback(JNIEnv *env, jclass, jobject callback)
{
  static JavaVM *jvm;
  env->GetJavaVM(&jvm);
  assert(jvm!=NULL);

  static JNIEnv* thread_env = NULL;
  static jclass callBackInterface = env->FindClass("com/chessnut/EasyLink$IRealTimeCallback");
  assert(callBackInterface);
  static jmethodID callBackMethod = env->GetMethodID(callBackInterface, "realTimeCallback", "(Ljava/lang/String;)V");
  assert(callBackMethod);
  static jobject callbackObject = env->NewGlobalRef(callback);
  assert(callbackObject);

  ChessLink::instance()->setRealTimeCallback([](string fen) {
    current_fen = fen;
    //printf("current fen: %s\n", current_fen.c_str());

    //  worker thread must be known to the JVM
    if (thread_env==NULL) {
      jint attach = jvm->AttachCurrentThreadAsDaemon((void**)&thread_env, NULL);
      assert(attach==JNI_OK);
      assert(thread_env!=NULL);
    }

    jstring arg = thread_env->NewStringUTF(current_fen.c_str());
    thread_env->CallVoidMethod(callbackObject, callBackMethod, arg);
  });
}

jint JNICALL Java_com_chessnut_EasyLink_beep (JNIEnv *, jclass, jint frequency, jint ms)
{
  return ChessLink::instance()->beep(frequency, ms);
}

jint JNICALL Java_com_chessnut_EasyLink_led
  (JNIEnv *env, jclass, jstring map)
{
    const char* chars = env->GetStringUTFChars(map,NULL);
    bool result = ChessLink::instance()->setLed(chars);
    env->ReleaseStringUTFChars(map,chars);
    return result;
}

jstring JNICALL Java_com_chessnut_EasyLink_getMcuVersion (JNIEnv *env, jclass)
{
  string ver = ChessLink::instance()->getMcuVersion();
  return env->NewStringUTF(ver.c_str());
}


jstring JNICALL Java_com_chessnut_EasyLink_getBleVersion (JNIEnv *env, jclass)
{
  string ver = ChessLink::instance()->getBleVersion();
  return env->NewStringUTF(ver.c_str());
}

jint JNICALL Java_com_chessnut_EasyLink_getBattery (JNIEnv *, jclass)
{
  return ChessLink::instance()->getBattery();
}

jint JNICALL Java_com_chessnut_EasyLink_getFileCount
  (JNIEnv *, jclass)
{
  return ChessLink::instance()->getFileCount();
}

jobjectArray JNICALL Java_com_chessnut_EasyLink_getFile
  (JNIEnv *env, jclass, jboolean andDelete)
{
  static jclass stringClass = env->FindClass("java/lang/String");
  assert(stringClass);
  const vector<string> file = ChessLink::instance()->getFile(andDelete);
  jobjectArray array = env->NewObjectArray(file.size(),stringClass,NULL);
  for (int i = 0; i < file.size(); i++)
    env->SetObjectArrayElement(array,i,env->NewStringUTF(file[i].c_str()));
  return array;
}
