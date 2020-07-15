/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_southernstars_sscore_JSSObjectArray */

#ifndef _Included_com_southernstars_sscore_JSSObjectArray
#define _Included_com_southernstars_sscore_JSSObjectArray
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    create
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_southernstars_sscore_JSSObjectArray_create
  (JNIEnv *, jobject);

/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_southernstars_sscore_JSSObjectArray_destroy
  (JNIEnv *, jobject);

/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    size
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_southernstars_sscore_JSSObjectArray_size
  (JNIEnv *, jobject);

/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    getObject
 * Signature: (I)Lcom/southernstars/sscore/JSSObject;
 */
JNIEXPORT jobject JNICALL Java_com_southernstars_sscore_JSSObjectArray_getObject
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    importFromCSV
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_southernstars_sscore_JSSObjectArray_importFromCSV
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    exportToCSV
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_southernstars_sscore_JSSObjectArray_exportToCSV
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    importFromTLE
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_southernstars_sscore_JSSObjectArray_importFromTLE
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    importMcNames
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_southernstars_sscore_JSSObjectArray_importMcNames
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_southernstars_sscore_JSSObjectArray
 * Method:    initAssetManager
 * Signature: (Landroid/content/res/AssetManager;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_southernstars_sscore_JSSObjectArray_initAssetManager
  (JNIEnv *, jclass, jobject);

#ifdef __cplusplus
}
#endif
#endif
