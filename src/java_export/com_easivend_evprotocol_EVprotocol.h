/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_easivend_evprotocol_EVprotocol */

#ifndef _Included_com_easivend_evprotocol_EVprotocol
#define _Included_com_easivend_evprotocol_EVprotocol
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    vmcStart
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_vmcStart
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    vmcStop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_easivend_evprotocol_EVprotocol_vmcStop
  (JNIEnv *, jobject);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    trade
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_trade
  (JNIEnv *, jobject, jint, jint, jint, jint);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    payout
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_payout
  (JNIEnv *, jobject, jlong);


/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    payout
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_payback
  (JNIEnv *, jobject);


JNIEXPORT jint JNICALL
Java_com_easivend_evprotocol_EVprotocol_getColumn
  (JNIEnv *env, jobject cls,jint cabinet);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    getStatus
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_getStatus
  (JNIEnv *, jobject);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    getRemainAmount
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_easivend_evprotocol_EVprotocol_getRemainAmount
  (JNIEnv *, jobject);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    cashControl
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_cashControl
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    setDate
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_setDate
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    cabinetControl
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_cabinetControl
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    bentoRegister
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_bentoRegister
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    bentoRelease
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_bentoRelease
  (JNIEnv *, jobject);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    bentoOpen
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_bentoOpen
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    bentoLight
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_bentoLight
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    bentoCheck
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_easivend_evprotocol_EVprotocol_bentoCheck
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
