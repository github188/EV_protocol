#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<pthread.h>


#include<jni.h>
#include "EV_com.h"
#include "json.h"
#include "ev_bento.h"
#include "ev_config.h"
#include "timer.h"

#if _WIN32
#define LOGI(...) printf(__VA_ARGS__)
#define LOGW(...) printf(__VA_ARGS__)
#define LOGE(...) printf(__VA_ARGS__)
#else
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "EV_thread", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "EV_thread", __VA_ARGS__))
#endif
// 全局变量
JavaVM* g_jvm = NULL;
jobject g_obj = NULL;
JNIEnv *g_env = NULL;
static pthread_t pid;
static volatile int g_threadStop = 0;
static jmethodID methodID_EV_callBack = NULL;

#define JSON_HEAD		"EV_json"
#define JSON_TYPE		"EV_type"

void JNI_json_insert_str(json_t *json,char *label,char *value)
{
	json_t *j_label,*j_value;
	if(label == NULL || value == NULL || json == NULL )
		return;
	j_label = json_new_string(label);
	j_value = json_new_string(value);
	json_insert_child(j_label,j_value);
	json_insert_child(json,j_label);
}


void JNI_json_insert_int(json_t *json,char *label,long value)
{
	json_t *j_label,*j_value;
	char buf[10] = {0};
	if(label == NULL || json == NULL )
		return;
	sprintf(buf,"%ld",value);
	j_label = json_new_string(label);
	j_value = json_new_number(buf);
	json_insert_child(j_label,j_value);
	json_insert_child(json,j_label);
}



//回调函数
void JNI_callBack(const int type,const void *ptr)
{
	jstring msg;
	char *text;
	unsigned int temp;
    quint8 *data;
	ST_VM_DATA *vm_ptr;
    json_t *root = NULL, *entry = NULL, *label;

	switch(type)
	{
		case EV_SETUP_REQ:
			break;
		case EV_SETUP_RPT:
			
			break;
		case EV_TRADE_RPT:
			root = json_new_object();
    		entry = json_new_object();
            data = (quint8 *)ptr;
			JNI_json_insert_str(entry,JSON_TYPE,"EV_TRADE_RPT");				
			JNI_json_insert_int(entry,"cabinet",data[MT + 1]);		
			JNI_json_insert_int(entry,"column",data[MT + 3]);
			JNI_json_insert_int(entry,"result",data[MT + 2]);
			JNI_json_insert_int(entry,"type",data[MT + 4]);
			temp = INTEG16(data[MT + 5],data[MT + 6]);
			temp = EV_amountFromVM(temp);
			
			JNI_json_insert_int(entry,"cost",temp);
			temp = INTEG16(data[MT + 7],data[MT + 8]);
			temp = EV_amountFromVM(temp);			
			JNI_json_insert_int(entry,"remainAmount",temp);
			JNI_json_insert_int(entry,"remainCount",data[MT + 9]);

			
			label = json_new_string(JSON_HEAD);			
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_PAYIN_RPT:
			vm_ptr = (ST_VM_DATA *)ptr;
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_PAYIN_RPT");
			JNI_json_insert_int(entry,"remainAmount",(long)vm_ptr->remainAmount);
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_PAYOUT_RPT:
			vm_ptr = (ST_VM_DATA *)ptr;
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_PAYOUT_RPT");
			JNI_json_insert_int(entry,"remainAmount",vm_ptr->remainAmount);
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_ENTER_MANTAIN:
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_ENTER_MANTAIN");
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_EXIT_MANTAIN:
			root = json_new_object();
    		entry = json_new_object();

			JNI_json_insert_str(entry,JSON_TYPE,"EV_EXIT_MANTAIN");
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_OFFLINE:
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_OFFLINE");
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_ONLINE:
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_ONLINE");
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_RESTART:
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_RESTART");
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_INITING:
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_INITING");
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_TIMEOUT:
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_TIMEOUT");
            data = (quint8 *)ptr;
			JNI_json_insert_int(entry,"cmd",*data);
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_NA:
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_NA");
            data = (quint8 *)ptr;
			JNI_json_insert_int(entry,"cmd",*data);			
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_STATE_RPT:
            data = (quint8 *)ptr;
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_STATE_RPT");
			JNI_json_insert_int(entry,"state",(int)(*data));
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_BUTTON_RPT:// type 表示 按键类型 0 游戏 1货道  2退币  cabinet 柜号 value 具体值(只有货道按键有意义)
            data = (quint8 *)ptr;
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_BUTTON_RPT");
			JNI_json_insert_int(entry,"type",data[0]);
			JNI_json_insert_int(entry,"cabinet",data[1]);
			JNI_json_insert_int(entry,"value",data[2]);
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			
			break;
		default:
			break;
	}

    if(root != NULL)
    {
        json_tree_to_string(root, &text);
        msg = (*g_env)->NewStringUTF(g_env,text);
        if(methodID_EV_callBack)// 最后调用类中“成员”方法
        {
            (*g_env)->CallVoidMethod(g_env, g_obj, methodID_EV_callBack,msg);
        }
        free(text);
        json_free_value(&root);
    }
}


void *JNI_run(void* arg)
{ 
    jclass cls;
    int ret;
    EV_register(JNI_callBack);//注册回调
    // Attach主线程
    ret  = (*g_jvm)->AttachCurrentThread(g_jvm,(void **)&g_env, NULL);
    if(ret != JNI_OK)
    {
        LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
        pthread_exit(0);
        return NULL;
    }

    // 找到对应的类
    cls = (*g_env)->GetObjectClass(g_env, g_obj);
    if(cls == NULL)
    {
        LOGE("FindClass() Error ......");
        pthread_exit(0);
        return NULL;
    }
    jstring msg = (*g_env)->NewStringUTF(g_env,"JNI CallBack OK...");
    //获得类中的“成员”方法
    methodID_EV_callBack = (*g_env)->GetMethodID(g_env,cls,"EV_callBack","(Ljava/lang/String;)V");
    if(methodID_EV_callBack == NULL)
    {
        LOGE("GetMethodID() Error ......");
    }
    if(methodID_EV_callBack)// 最后调用类中“成员”方法
    {
        (*g_env)->CallVoidMethod(g_env, g_obj, methodID_EV_callBack,msg);
    }

    LOGI("thread_fun ready...\n");
	while(!g_threadStop)
	{
       EV_task();
	}

    EV_release();
    methodID_EV_callBack = NULL;
    //Detach主线程
    if((*g_jvm)->DetachCurrentThread(g_jvm) != JNI_OK)
    {
        LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
    }
	LOGI("JNI Thread stopped....");
    pid = 0;
	pthread_exit(0);

    return NULL;
}


/*
* Set some test stuff up.
*
* Returns the JNI version on success, -1 on failure.
*/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
    reserved = reserved;
	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("GetEnv failed!");
		return -1;
	}
	return JNI_VERSION_1_4;
}


/*
 * Class:     com_easivend_ev_vmc_EV_vmc
 * Method:    vmcStart
 * Signature: ()V
 */
JNIEXPORT jint JNICALL
Java_com_easivend_evprotocol_EVprotocol_vmcStart
  (JNIEnv *env, jobject obj,jstring jport)
{
    void *ret;
	const char *portName = (*env)->GetStringUTFChars(env,jport, NULL);
    EV_closeSerialPort();
	int fd = EV_openSerialPort((char *)portName,9600,8,'N',1);
	(*env)->ReleaseStringUTFChars(env,jport,portName);
    if (fd < 0){
			LOGE("Can't Open Serial Port:%s!",portName);
			return -1;
	}
	LOGI("EV_openSerialPort suc.....!\n");
    //串口打开成功  开启线程
    if(pid)//线程已经开启了  关闭线程
    {
        LOGI("The serialport thread has runing!!!!");
        g_threadStop = 1;
        pthread_join(pid,&ret);
    }
    (*env)->GetJavaVM(env, &g_jvm);// 保存全局JVM以便在子线程中使用
    g_obj = (*env)->NewGlobalRef(env, obj);// 不能直接赋值(g_obj = ojb)
    g_threadStop = 0;
    pthread_create(&pid, NULL, JNI_run, NULL);

	return 1;


}

/*
 * Class:     com_easivend_ev_vmc_EV_vmc
 * Method:    vmcStop
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_easivend_evprotocol_EVprotocol_vmcStop
  (JNIEnv *env, jobject obj)
{
    env = env; obj = obj;
	LOGI("Java_com_easivend_evprotocol_EVprotocol_vmcStop....");
	g_threadStop = 1;
}

/*
 * Class:     com_easivend_ev_vmc_EV_vmc
 * Method:    trade
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_com_easivend_evprotocol_EVprotocol_trade
  (JNIEnv *env, jobject obj,jint cabinet, jint column, jint type, jint cost)
{
	jint ret;
    env = env; obj = obj;
	ret = EV_pcTrade(cabinet & 0xFF,column & 0xFF,type & 0xFF,cost & 0xFFFF);
	return ret;
}


JNIEXPORT jint JNICALL 
Java_com_easivend_evprotocol_EVprotocol_payout
  (JNIEnv *env, jobject cls,jlong value)
{
	jint ret;
    env = env; cls = cls;
	ret = EV_pcPayout(value & 0xFFFF);
	return ret;
}




JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_getStatus
  (JNIEnv *env, jobject obj)
{
    env = env; obj = obj;
	return EV_pcReqSend(GET_STATUS,0,NULL,0);
}


JNIEXPORT jlong JNICALL Java_com_easivend_evprotocol_EVprotocol_getRemainAmount
  (JNIEnv *env, jobject obj)
{
	jlong value = 0;
    env = env; obj = obj;
	value = EV_vmGetAmount();
	return value;
}




JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_bentoRegister
  (JNIEnv *env, jobject obj, jstring jport)
{
	const char *portName = (*env)->GetStringUTFChars(env,jport, NULL);
    env = env; obj = obj;
    EV_bento_closeSerial();
	int fd = EV_bento_openSerial((char *)portName,9600,8,'N',1);
	(*env)->ReleaseStringUTFChars(env,jport,portName);
    if (fd < 0){
			LOGE("Can't Open Serial Port:%s!",portName);
			return -1;
	}
	return 1;
}


JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_bentoOpen
  (JNIEnv *env, jobject obj, jint cabinet, jint box)
{
	jint ret = 0;
    env = env; obj = obj;
    ret = EV_bento_open(cabinet,box);

	return ret;
}

JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_bentoRelease
  (JNIEnv *env, jobject obj)
{

	jint ret = 1;
    env = env; obj = obj;
    EV_bento_closeSerial();
	return ret;
}



JNIEXPORT jint JNICALL 
Java_com_easivend_evprotocol_EVprotocol_bentoLight
	(JNIEnv *env, jobject obj, jint cabinet, jint flag)
{
	jint ret = 0;
    env = env; obj = obj;
    ret = EV_bento_light(cabinet,flag);
	return ret;
}



JNIEXPORT jstring JNICALL 
Java_com_easivend_evprotocol_EVprotocol_bentoCheck
  (JNIEnv *env, jobject obj, jint cabinet)
{
	jstring str;
	jint ret;
    json_t *root = NULL, *entry = NULL, *label;
    char *text,id[10] = {0},i;

	ST_BENTO_FEATURE st_bento;

    env = env; obj = obj;
	EV_LOGI5("EV_bento_check:start\n");
	ret = EV_bento_check(cabinet, &st_bento);

	if(ret == 1)
	{
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_BENTO_FEATURE");
			JNI_json_insert_int(entry,"boxNum",st_bento.boxNum);
			JNI_json_insert_int(entry,"HotSupport",st_bento.ishot);
			JNI_json_insert_int(entry,"CoolSupport",st_bento.iscool);
			JNI_json_insert_int(entry,"LightSupport",st_bento.islight);

			for(i = 0;i < 7;i++)
			{
                sprintf(&id[i * 2],"%02x",st_bento.id[i]);
			}
			
			JNI_json_insert_str(entry,"ID",id);
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			json_tree_to_string(root, &text);
			//memcpy(bentdata,text,sizeof());
			//free(text);
			

			str = (*g_env)->NewStringUTF(g_env,text);
			json_free_value(&root);
			return str;
	}
	str = (*g_env)->NewStringUTF(g_env,"");
	return str;
}
