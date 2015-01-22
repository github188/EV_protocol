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



#ifdef EV_ANDROID
#include<android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "EV_thread", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "EV_thread", __VA_ARGS__))
#else
#define LOGI(...) printf(__VA_ARGS__)
#define LOGW(...) printf(__VA_ARGS__)
#define LOGE(...) printf(__VA_ARGS__)
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



json_t *JNI_setup_rpt(const void *ptr)
{
    json_t *root,*entry,*label,*sub;
    ST_SETUP *setup;
    root = json_new_object();
    entry = json_new_object();
    sub = json_new_object();
    setup = (ST_SETUP *)ptr;
    JNI_json_insert_str(entry,JSON_TYPE,"EV_SETUP_RPT");

    //语言版本
    if(setup->language == 0)
        JNI_json_insert_str(entry,"language","Chinese");
    else if(setup->language == 1)
        JNI_json_insert_str(entry,"language","English");
    else
        JNI_json_insert_str(entry,"language","Other");


    //超时退币
    JNI_json_insert_int(entry,"payoutTime",setup->payoutTime);
    JNI_json_insert_int(entry,"ratio",setup->vmRatio);
    JNI_json_insert_int(entry,"multBuy",setup->multBuy);
    JNI_json_insert_int(entry,"forceBuy",setup->forceBuy);

    JNI_json_insert_int(sub,"Recvtype",setup->bill.recv_type);
    JNI_json_insert_int(sub,"Recvch1",setup->bill.recv_ch[0]);
    JNI_json_insert_int(sub,"Recvch2",setup->bill.recv_ch[1]);
    JNI_json_insert_int(sub,"Recvch3",setup->bill.recv_ch[2]);
    JNI_json_insert_int(sub,"Recvch4",setup->bill.recv_ch[3]);
    JNI_json_insert_int(sub,"Recvch5",setup->bill.recv_ch[4]);
    JNI_json_insert_int(sub,"Recvch6",setup->bill.recv_ch[5]);
    JNI_json_insert_int(sub,"Recvch7",setup->bill.recv_ch[6]);
    JNI_json_insert_int(sub,"Recvch8",setup->bill.recv_ch[7]);

    JNI_json_insert_int(sub,"Changertype",setup->bill.change_type);
    JNI_json_insert_int(sub,"Changerch1",setup->bill.change_ch[0]);
    JNI_json_insert_int(sub,"Changerch2",setup->bill.change_ch[1]);
    JNI_json_insert_int(sub,"Changerch3",setup->bill.change_ch[2]);
    JNI_json_insert_int(sub,"Changerch4",setup->bill.change_ch[3]);
    JNI_json_insert_int(sub,"Changerch5",setup->bill.change_ch[4]);
    JNI_json_insert_int(sub,"Changerch6",setup->bill.change_ch[5]);
    JNI_json_insert_int(sub,"Changerch7",setup->bill.change_ch[6]);
    JNI_json_insert_int(sub,"Changerch8",setup->bill.change_ch[7]);

    label = json_new_string("Bill");
    json_insert_child(label,sub);
    json_insert_child(entry,label);


    sub = json_new_object();
    JNI_json_insert_int(sub,"Recvtype",setup->coin.recv_type);
    JNI_json_insert_int(sub,"Recvch1",setup->coin.recv_ch[0]);
    JNI_json_insert_int(sub,"Recvch2",setup->coin.recv_ch[1]);
    JNI_json_insert_int(sub,"Recvch3",setup->coin.recv_ch[2]);
    JNI_json_insert_int(sub,"Recvch4",setup->coin.recv_ch[3]);
    JNI_json_insert_int(sub,"Recvch5",setup->coin.recv_ch[4]);
    JNI_json_insert_int(sub,"Recvch6",setup->coin.recv_ch[5]);
    JNI_json_insert_int(sub,"Recvch7",setup->coin.recv_ch[6]);
    JNI_json_insert_int(sub,"Recvch8",setup->coin.recv_ch[7]);
    JNI_json_insert_int(sub,"Changertype",setup->coin.change_type);
    JNI_json_insert_int(sub,"Changerch1",setup->coin.change_ch[0]);
    JNI_json_insert_int(sub,"Changerch2",setup->coin.change_ch[1]);
    JNI_json_insert_int(sub,"Changerch3",setup->coin.change_ch[2]);
    JNI_json_insert_int(sub,"Changerch4",setup->coin.change_ch[3]);
    JNI_json_insert_int(sub,"Changerch5",setup->coin.change_ch[4]);
    JNI_json_insert_int(sub,"Changerch6",setup->coin.change_ch[5]);
    JNI_json_insert_int(sub,"Changerch7",setup->coin.change_ch[6]);
    JNI_json_insert_int(sub,"Changerch8",setup->coin.change_ch[7]);
    label = json_new_string("Coin");
    json_insert_child(label,sub);
    json_insert_child(entry,label);

    sub = json_new_object();
    JNI_json_insert_int(sub,"type",setup->bin.type);
    JNI_json_insert_int(sub,"goc",setup->bin.goc);

    label = json_new_string("Cabinet");
    json_insert_child(label,sub);
    json_insert_child(entry,label);


    sub = json_new_object();
    JNI_json_insert_int(sub,"type",setup->subBin.type);
    JNI_json_insert_int(sub,"goc",setup->subBin.goc);
    label = json_new_string("SubCabinet");
    json_insert_child(label,sub);
    json_insert_child(entry,label);


    label = json_new_string(JSON_HEAD);
    json_insert_child(label,entry);
    json_insert_child(root,label);

    return root;
}



json_t *JNI_trade_rpt(const void *ptr)
{
    json_t *root,*entry,*label;
    ST_TRADE *trade = (ST_TRADE *)ptr;
    root = json_new_object();
    entry = json_new_object();
    JNI_json_insert_str(entry,JSON_TYPE,"EV_TRADE_RPT");
    JNI_json_insert_int(entry,"cabinet",trade->cabinet);
    JNI_json_insert_int(entry,"column",trade->column);
    JNI_json_insert_int(entry,"result",trade->result);
    JNI_json_insert_int(entry,"type",trade->type);
    JNI_json_insert_int(entry,"cost",trade->cost);
    JNI_json_insert_int(entry,"remainAmount",trade->remainAmount);
    JNI_json_insert_int(entry,"remainCount",trade->remainCount);
    label = json_new_string(JSON_HEAD);
    json_insert_child(label,entry);
    json_insert_child(root,label);
    return root;
}

//回调函数
static void JNI_callBack(const int type,const void *ptr)
{
	jstring msg;
	char *text;
    uint8 *data;
	ST_VM_DATA *vm_ptr;
    json_t *root = NULL, *entry = NULL, *label;

	switch(type)
	{
		case EV_SETUP_REQ:
			break;
		case EV_SETUP_RPT:
            root = JNI_setup_rpt(ptr);
			break;
		case EV_TRADE_RPT:
            root = JNI_trade_rpt(ptr);
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
            data = (uint8 *)ptr;
			JNI_json_insert_int(entry,"cmd",*data);
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_NA:
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_NA");
            data = (uint8 *)ptr;
			JNI_json_insert_int(entry,"cmd",*data);			
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_STATE_RPT:
            data = (uint8 *)ptr;
			root = json_new_object();
    		entry = json_new_object();
			JNI_json_insert_str(entry,JSON_TYPE,"EV_STATE_RPT");
			JNI_json_insert_int(entry,"state",(int)(*data));
			label = json_new_string(JSON_HEAD);
			json_insert_child(label,entry);
			json_insert_child(root,label);
			break;
		case EV_BUTTON_RPT:// type 表示 按键类型 0 游戏 1货道  2退币  cabinet 柜号 value 具体值(只有货道按键有意义)
            data = (uint8 *)ptr;
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
    ret  = (*g_jvm)->AttachCurrentThread(g_jvm,&g_env, NULL);
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
   // LOGI("cabinet=%d coulmn=%d type =%d  cost=%d",cabinet,column,type,cost);
    ret = EV_pcTrade(cabinet,column,type,cost);
	return ret;
}


JNIEXPORT jint JNICALL 
Java_com_easivend_evprotocol_EVprotocol_payout
  (JNIEnv *env, jobject cls,jlong value)
{
	jint ret;
    //uint32 temp = (uint32)value;
    ret = EV_pcPayout(1);
	return ret;
}




JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_getStatus
  (JNIEnv *env, jobject obj)
{
	return EV_pcReqSend(GET_STATUS,0,NULL,0);
}


JNIEXPORT jlong JNICALL Java_com_easivend_evprotocol_EVprotocol_getRemainAmount
  (JNIEnv *env, jobject obj)
{
	jlong value = 0;
	value = EV_vmGetAmount();
	return value;
}


/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    cashControl
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_cashControl
  (JNIEnv *env, jobject obj, jint flag)
{
    jint ret;
    ret = EV_cash_control(flag);
    return ret;

}


/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    cabinetControl
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_cabinetControl
  (JNIEnv *env, jobject obj, jint cabinet, jint dev, jint flag)
{
    jint ret;
    ret = EV_cabinet_control(cabinet,dev,flag);
    return ret;
}

/*
 * Class:     com_easivend_evprotocol_EVprotocol
 * Method:    setDate
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_easivend_evprotocol_EVprotocol_setDate
  (JNIEnv *env, jobject obj, jstring dateStr)
{
    ST_DATE date;
    jint ret;
    char *data[20] = {0};
    //date 日期 格式"2014-10-10 12:24:24"
    char *buf = (char *)((*env)->GetStringUTFChars(env,dateStr, NULL));
    memcpy(data,buf,4);
    date.year = atoi((const char *)data);

    memset(data,0,sizeof(data));
    memcpy(data,&buf[5],2);
    date.moth = atoi((const char *)data);

    memset(data,0,sizeof(data));
    memcpy(data,&buf[8],2);
    date.day = atoi((const char *)data);

    memset(data,0,sizeof(data));
    memcpy(data,&buf[11],2);
    date.hour = atoi((const char *)data);

    memset(data,0,sizeof(data));
    memcpy(data,&buf[14],2);
    date.min = atoi((const char *)data);

    memset(data,0,sizeof(data));
    memcpy(data,&buf[17],2);
    date.sec = atoi((const char *)data);


    date.week = 1;

    (*env)->ReleaseStringUTFChars(env,dateStr,buf);
    ret = EV_set_date(&date);
    return ret;

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
